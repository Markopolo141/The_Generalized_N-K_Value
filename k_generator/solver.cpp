#include <stdbool.h>
#include <float.h>

#include "table.cpp"
#include "memory.cpp"
#include "reference_memory.cpp"
#include "table_memory.cpp"
#include "sorted_list.cpp"



Table* prev_max_table = NULL;

double* master_head = NULL;
double* temporary_head = NULL;
double* results = NULL;
int players;


void free_memory() {
	if (results != NULL)
		free(results);
	if (prev_max_table != NULL) {
		prev_max_table->free_data();
		free(prev_max_table);
	}
	if (master_head != NULL)
		free(master_head);
	if (temporary_head != NULL)
		free(temporary_head);
}

void setup_memory(Table* t, int table_players, bool rs_enable) {
	if (rs_enable==true) {
		#if DEBUG==1
			printf("ALLOCATING RESULTS SPACE size %i bytes\n",((unsigned long)1)<<(t->w));
		#endif
		results = (double*)calloc(sizeof(double),((unsigned long)1)<<(t->w));
		for (unsigned long i=0; i<(((unsigned long)1)<<(t->w)); i++)
			results[i] = DBL_MAX;
	}
	players = table_players;
	prev_max_table = (Table*)malloc(sizeof(Table));
	prev_max_table->initialise_and_load(t);
	master_head = (double*)calloc(sizeof(double),t->w);
	temporary_head = (double*)calloc(sizeof(double),t->w);
}


int analyse_coalition(unsigned long *coalition, unsigned long *anticoalition, unsigned long *mask, int* inverting) {
	*mask = ((((unsigned long)1)<<players)-1);
	if ((*coalition) - ((*mask)&(*coalition)) != 0)
		return -1;
	long coalition2 = *coalition;
	int member_no = 0;
	while (coalition2 > 0) {
		member_no += coalition2&1;
		coalition2 >>= 1;
	}
	*anticoalition = (*mask)^(*coalition);
	#if DEBUG==1
		printf("member number %i %i\n",member_no,players>>1);
	#endif
	if (member_no < (players>>1)) {
		*inverting = -1;
	} else {
		*inverting = 1;
	}
	return 0;
}


void apply_coalition(unsigned long coalition) {
	for (int i=0; i< prev_max_table->w; i++)
		temporary_head[i] = 0;
	for (int i=0; i< players; i++) {
		if (coalition&1) {
			temporary_head[i] = -master_head[i];
		} else {
			temporary_head[i] = master_head[i];
		}
		coalition >>= 1;
	}
}

double simplex(Table* t, double* head, bool maximising) {
	if (t->table_pivot_column_number != t->h)
		printf("WARNING: simplex method on potentially badly formed table.\n");
	//printf("SIMPLEX INITIALISE\n");
	t->calculate_pivots();
	t->apply_to_head(head,head);
	while (true) {
		double best_improvement=0;
		int best_improvement_index = -1;
		if (maximising==true) {
			for (int pivot_index=0; pivot_index < t->pivotable_number; pivot_index++) {
				double head_value = head[t->pivotable_columns[pivot_index]];
				if (head_value < 0) {
					double ratio = head_value*t->pivotable_ratios[pivot_index];
					if (ratio<=best_improvement) {
						//printf("nonbland ");
						best_improvement = ratio;
						best_improvement_index = pivot_index;
					} else if ((ratio==0) && (best_improvement_index==-1)) { //bland's rule
						//printf("bland \n");
						best_improvement_index = pivot_index;
					}
				}
			}
		} else {
			for (int pivot_index=0; pivot_index < t->pivotable_number; pivot_index++) {
				double head_value = head[t->pivotable_columns[pivot_index]];
				if (head_value > 0) {
					double ratio = head_value*t->pivotable_ratios[pivot_index];
					if (ratio>=best_improvement) {
						//printf("nonbland ");
						best_improvement = ratio;
						best_improvement_index = pivot_index;
					} else if ((ratio==0) && (best_improvement_index==-1)) { //bland's rule
						//printf("bland \n");
						best_improvement_index = pivot_index;
					}
				}
			}
		}
		if (best_improvement_index==-1) { // destinct optima attained
			break;
		}
		t->pivot(t, best_improvement_index);
		t->apply_to_head(head,head);
		//printf("pivoting\n");
	}
	prev_max_table = t;
	//printf("SIMPLEX DONE\n");
	return head[t->w-1];
}



bool artificial_variables_simplex(Table* t, int artificial_variables) {
	double* head = (double*)calloc(sizeof(double),t->w);
	for (int i=t->w-1-artificial_variables; i<t->w-1; i++)
		head[i] = -1.0;
	if (simplex(t,head,false)>TINY) {
		printf("ERROR: INFEASIBIILTY\n");
		return false;
	}
	free(head);
	for (int i=0; i<artificial_variables; i++) {
		t->delete_column(t->w-2);
	}
	return true;
}

unsigned long equation_pruning(Table* t, int* slackness_columns) {
	//printf("EQUATION PRUNING\n");
	double* head = (double*)calloc(sizeof(double),t->w);
	int original_h = t->h;
	int deletions = 0;
	unsigned long deleted_columns=0;
	int jj = 0;
	for (int j=0; j<original_h; j++) {
		if (slackness_columns[j]!=-1) {
			//printf("selecting slackness column %i\n",slackness_columns[j]);
			for (int i=0;i<t->w;i++) {
				if (i==slackness_columns[j]) {
					head[i]=-1;
				} else {
					head[i]=0;
				}
			}
			if (simplex(t,head,false)>TINY) {
				t->delete_row(jj);
				t->delete_column(slackness_columns[j]);
				deleted_columns |= ((unsigned long)1)<<(slackness_columns[j]);
				deletions++;
				for (int jj=0;jj<original_h;jj++)
					if (slackness_columns[jj]>slackness_columns[j])
						slackness_columns[jj] -= 1;
				//printf("DELETING\n");
			} else {
				jj++;
			}
		}
	}
	free(head);
	return deleted_columns;
}

struct WalkBackLinked : ValueLinked {
	Table* t;
	int pivot_index;
	double* head;
};

double walk_back(Table* t, unsigned long coalition, unsigned long anticoalition, double* head) {
	#if DEBUG==1
		printf("WALKBACK: coalition: ");
		printbin(coalition);
		printf("   anticoalition: ");
		printbin(anticoalition);
		printf("\n");
		printhead(head, t->w);
		t->print();
		int walkbacks = 0;
	#endif
	double* original_head = head;
	int w = t->w;
	
	Memory* masks = (Memory*)calloc(sizeof(Memory),1);
	Reference_Memory* refs = (Reference_Memory*)calloc(sizeof(Reference_Memory),1);
	Table_Memory* table_refs = (Table_Memory*)calloc(sizeof(Table_Memory),1);
	SortedList* pivot_list = (SortedList*)calloc(sizeof(SortedList),1);
	
	WalkBackLinked* link;
	
	masks->setup(MEMORY_INITIAL_SIZE);
	refs->setup(MEMORY_INITIAL_SIZE);
	table_refs->setup(MEMORY_INITIAL_SIZE);
	masks->add(t->table_pivot_column_mask);
	
	while (t->check_subset_improvable(anticoalition, coalition, head, false)) {
		#if DEBUG==1
			printf("ITERATING %i\n",walkbacks);
			walkbacks++;
		#endif
		for (int i=0; i < t->pivotable_number; i++) {
			unsigned long new_mask = t->table_pivot_column_mask ^ ((((unsigned long)1)<<(t->pivotable_columns[i]))|(((unsigned long)1)<<(t->table_pivot_columns[t->pivotable_rows[i]])));
			if (masks->search(new_mask)==false) {
				link = (WalkBackLinked*)malloc(sizeof(WalkBackLinked));
				link->t = t;
				link->head = head;
				link->pivot_index = i;
				link->v = -(head[w-1] - t->pivotable_ratios[i]*head[t->pivotable_columns[i]]);
				pivot_list->add(link);
				masks->add(new_mask);
			}
		}
		#if DEBUG==1
			masks->print_all();
			pivot_list->print_all();
		#endif
		
		link = (WalkBackLinked*)(pivot_list->pop());
		head = (double*)malloc(sizeof(double)*(w));
		refs->add(head);
		t = (Table*)malloc(sizeof(Table));
		t->initialise(w,link->t->h);
		table_refs->add(t);
		t->pivot(link->t,link->pivot_index);
		t->apply_to_head(link->head,head);
		free(link);
	}
	#if DEBUG==1
		printf("finished walkback: %i walkbacks\n",walkbacks);
		printhead(head, w);
		t->print();
	#endif
	
	memcpy(original_head,head,sizeof(double)*w);
	refs->free_all();
	table_refs->free_all();
	
	table_refs->destroy();
	refs->destroy();
	masks->destroy();
	pivot_list->destroy();
	
	free(refs);
	free(masks);
	free(pivot_list);
	return original_head[w-1];
}






