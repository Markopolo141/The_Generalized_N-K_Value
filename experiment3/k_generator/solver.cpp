#include <stdbool.h>
#include <float.h>

#include "mask_memory.cpp"
#include "row_memory.cpp"
#include "table.cpp"
#include "table_memory.cpp"
#include "table_pivot_memory.cpp"
#include "reference_memory.cpp"



Table* t = NULL;
Table* prev_max_table = NULL;
Table* prev_min_table = NULL;

double* master_head = NULL;
double* temporary_head = NULL;
int players;



void free_memory() {
	if (prev_max_table != NULL) {
		prev_max_table->free_data();
		free(prev_max_table);
		prev_max_table = NULL;
	}
	if (prev_min_table != NULL) {
		prev_min_table->free_data();
		free(prev_min_table);
		prev_min_table = NULL;
	}
	if (master_head != NULL) {
		free(master_head);
		master_head = NULL;
	}
	if (temporary_head != NULL) {
		free(temporary_head);
		temporary_head = NULL;
	}
	if (t!=NULL) {
		t->free_data();
		free(t);
		t = NULL;
	}
}


void setup_memory(Table* t) {
	prev_max_table = (Table*)malloc(sizeof(Table));
	prev_max_table->initialise_and_load(t);
	prev_min_table = (Table*)malloc(sizeof(Table));
	prev_min_table->initialise_and_load(t);
	master_head = (double*)calloc(sizeof(double),t->w);
	temporary_head = (double*)calloc(sizeof(double),t->w);
}

int analyse_coalition(unsigned long *coalition, unsigned long *anticoalition, unsigned long *mask) {
	*mask = ((((unsigned long)1)<<players)-1);
	if ((*coalition) - ((*mask)&(*coalition)) != 0)
		return -1;
	*anticoalition = (*mask)^(*coalition);
	return 0;
}

void apply_coalition(unsigned long coalition) {
	for (int i=0; i< prev_min_table->w; i++)
		temporary_head[i] = 0;
	for (int i=0; i< players; i++) {
		if (coalition&1)
			temporary_head[i] = -master_head[i];
		else
			temporary_head[i] = master_head[i];
		coalition >>= 1;
	}
}


bool artificial_variables_simplex(Table* t, int artificial_variables) {
	double* head = (double*)calloc(sizeof(double),t->w);
	for (int i=t->w-1-artificial_variables; i<t->w-1; i++)
		head[i] = -1.0;
	if (t->simplex(head,false)>TINY) {
		printf("ERROR: INFEASIBIILTY\n");
		return false;
	}
	free(head);
	for (int i=0; i<artificial_variables; i++)
		t->delete_column(t->w-2,NULL);
	return true;
}

void equation_pruning(Table* t, int* slackness_columns) {
	double* head = (double*)calloc(sizeof(double),t->w);
	int original_h = t->h;
	int jj=0;
	for (int j=0; j<original_h; j++) {
		if (slackness_columns[j]!=-1) {
			for (int i=0;i<t->w;i++) {
				if (i==slackness_columns[j])
					head[i]=-1;
				else
					head[i]=0;
			}
			if (t->simplex(head,false)>TINY) {
				t->delete_row(jj);
				t->delete_column(slackness_columns[j],NULL);
				for (int jjj=0;jjj<original_h;jjj++)
					if (slackness_columns[jjj]>slackness_columns[j])
						slackness_columns[jjj] -= 1;
			} else
				jj++;
		} else
			jj++;
	}
	free(head);
}


void iteration(Table* t, Mask* anti_subset_mask, double* head, bool maximising, Table_Memory* table_refs, Reference_Memory* refs, Mask_Memory* masks, Table_Pivot_Memory* table_pivots) {
	table_pivots->free_all();
	masks->clear();
	masks->add(t->table_pivot_column_mask);
	Table_Pivot* tp
	while (true) {
		for (int i=0; i<t->pivotable_number; i++) {
			if (t->pivotable_ratios[i]>=0) {
				Table* tt = (Table*)malloc(sizeof(Table));
				tt->pivot(t,i);
				masks->add(tt->table_pivot_column_mask);
				double* new_head = (double*)malloc(sizeof(double)*(w));
				tt->apply_to_head(new_head,head);
				if (t->pivotable_ratios[i]>0) {
					if (tt->check_subset_improvable(Mask* anti_subset_mask, double* head, bool maximising)) {
						tt->free_data();
						free(tt);
						free(new_head);
					} else {
						tt->calculate_pivots();
						table_refs->add(tt);
						refs->add(new_head);
						//TODO
					}
				} else {
					table_refs->add(tt);
					refs->add(new_head);
					tp = (Table_Pivot*)malloc(sizeof(Table_Pivot));
					tp->t = tt;
					tp->head = new_head;
					table_pivots->add(tp);
				}
			}
		}
		tp = table_pivots[table_pivots->length-1];
		table_pivots->length--;
		t = tp->t;
		head = tp->head;
		free(tp);
	}
}

void run(Table* t, Mask* anti_subset_mask, double* head, bool maximising) {
	#if DEBUG==1
		printf("SOLVER RUN: anti_coalition: ");
		anti_subset_mask->print();
		printhead(head, t->w);
		t->table_pivot_column_mask->print();
		t->print();
		int iterations = 0;
	#endif
	t->calculate_pivots();
	t->apply_to_head(head,head);
	Mask_Memory* masks;
	masks = (Mask_Memory*)calloc(sizeof(Mask_Memory),1);
	masks->setup(MEMORY_INITIAL_SIZE);

	//TODO
}

/*double walk_back(Table* t, Mask* coalition, double* head) {
	#if DEBUG==1
		printf("WALKBACK: coalition: ");
		coalition->print();
		printhead(head, t->w);
		t->table_pivot_column_mask->print();
		t->print();
		int walkbacks = 0;
	#endif
	int walkbacks = 0;
	
	t->calculate_pivots();
	t->apply_to_head(head,head);
	double* original_head = head;
	int w = t->w;
	WalkBackLinked* link;
	masks->add(t->table_pivot_column_mask);
	
	Mask* new_mask = (Mask*)malloc(sizeof(Mask));
	while (t->check_subset_improvable(coalition, head, false)) {
		#if DEBUG==1
			printf("ITERATING %i\n",walkbacks);
			walkbacks++;
		#endif
		walkbacks++;
		for (int i=0; i < t->pivotable_number; i++) {
			new_mask->set(t->table_pivot_column_mask);
			new_mask->flip_bit(t->pivotable_columns[i]);
			new_mask->flip_bit(t->table_pivot_columns[t->pivotable_rows[i]]);
			if (masks->search(new_mask)==false) {
				link = (WalkBackLinked*)malloc(sizeof(WalkBackLinked));
				link->t = t;
				link->head = head;
				link->pivot_index = i;
				link->v = (head[w-1] - t->pivotable_ratios[i]*head[t->pivotable_columns[i]]);
				pivot_list->add(link);
				masks->add(new_mask);
			}
		}
		
		#if DEBUG==1
			WalkBackLinked *ww;
			ww = (WalkBackLinked*)pivot_list->root;
			printf("SORTEDLIST contents:\n");
			if (ww!=NULL)
				while (ww!= NULL) {
					printf("\t%f\t%i,%i\t%li\n",ww->v,ww->t->pivotable_rows[ww->pivot_index],ww->t->pivotable_columns[ww->pivot_index],((long)(ww->t)));
					ww = (WalkBackLinked*)(ww->next);
				}
			masks->print_all();
		#endif
		
		link = (WalkBackLinked*)(pivot_list->pop());
		head = (double*)malloc(sizeof(double)*(w));
		refs->add(head);
		t = (Table*)malloc(sizeof(Table));
		t->initialise(w,link->t->h);
		table_refs->add(t);
		t->pivot(link->t,link->pivot_index);
		t->calculate_pivots();
		t->apply_to_head(link->head,head);
		free(link);
		
		#if DEBUG==1
			printf("TABLE:\n");
			printhead(head,t->w);
			t->table_pivot_column_mask->print();
			t->print();
		#endif
	}
	printf("finished walkback: %i walkbacks\n",walkbacks);
	#if DEBUG==1
		printf("finished walkback: %i walkbacks\n",walkbacks);
		printhead(head, w);
		t->print();
	#endif
	
	memcpy(original_head,head,sizeof(double)*w);
	refs->free_all();
	table_refs->free_all();
	pivot_list->destroy();
	masks->clear();
	free(new_mask);
	
	#if DEBUG==1
		printf("RETURNING: %f\n", original_head[w-1]);
	#endif
	return original_head[w-1];
}*/





