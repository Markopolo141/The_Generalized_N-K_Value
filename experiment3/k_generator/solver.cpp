#include <stdbool.h>
#include <float.h>

#include "row_memory.cpp"
Row_Memory* row_memory = NULL;

#include "mask_memory.cpp"
Mask_Memory* masks = NULL;

#include "table.cpp"
#include "reference_memory.cpp"
#include "table_memory.cpp"
#include "sorted_list.cpp"
#include "value_memory.cpp"



Table* t = NULL;
Table* prev_max_table = NULL;
Table* prev_min_table = NULL;

double* master_head = NULL;
double* temporary_head = NULL;
int players;

Reference_Memory* refs = NULL;
Table_Memory* table_refs = NULL;
SortedList* pivot_list = NULL;
Value_Memory* value_memory = NULL;


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
	
	if (masks != NULL) {
		masks->destroy();
		free(masks);
		masks = NULL;
	}
	if (refs != NULL) {
		refs->destroy();
		free(refs);
		refs = NULL;
	}
	if (table_refs != NULL) {
		table_refs->destroy();
		free(table_refs);
		table_refs = NULL;
	}
	if (pivot_list != NULL) {
		free(pivot_list);
		pivot_list = NULL;
	}
	if (row_memory != NULL) {
		row_memory->destroy();
		free(row_memory);
		row_memory = NULL;
	}
	if (value_memory != NULL) {
		value_memory->destroy();
		free(value_memory);
		value_memory = NULL;
	}
	if (t!=NULL) {
		t->free_data();
		free(t);
		t = NULL;
	}
}

void primary_setup_memory() {
	masks = (Mask_Memory*)calloc(sizeof(Mask_Memory),1);
	refs = (Reference_Memory*)calloc(sizeof(Reference_Memory),1);
	table_refs = (Table_Memory*)calloc(sizeof(Table_Memory),1);
	pivot_list = (SortedList*)calloc(sizeof(SortedList),1);
	row_memory = (Row_Memory*)calloc(sizeof(Row_Memory),1);
	value_memory = (Value_Memory*)calloc(sizeof(Value_Memory),1);
	
	masks->setup(MEMORY_INITIAL_SIZE);
	refs->setup(MEMORY_INITIAL_SIZE);
	table_refs->setup(MEMORY_INITIAL_SIZE);
	pivot_list->setup();
	row_memory->setup(MEMORY_INITIAL_SIZE);
	value_memory->setup(MEMORY_INITIAL_SIZE);
}

void setup_memory(Table* t, int table_players) {
	players = table_players;
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
		if (coalition&1) {
			temporary_head[i] = -master_head[i];
		} else {
			temporary_head[i] = master_head[i];
		}
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
	for (int i=0; i<artificial_variables; i++) {
		t->delete_column(t->w-2);
	}
	return true;
}

void equation_pruning(Table* t, int* slackness_columns) {
	//printf("EQUATION PRUNING\n");
	double* head = (double*)calloc(sizeof(double),t->w);
	int original_h = t->h;
	int jj=0;
	for (int j=0; j<original_h; j++) {
		if (slackness_columns[j]!=-1) {
			for (int i=0;i<t->w;i++) {
				if (i==slackness_columns[j]) {
					head[i]=-1;
				} else {
					head[i]=0;
				}
			}
			if (t->simplex(head,false)>TINY) {
				t->delete_row(jj);
				t->delete_column(slackness_columns[j]);
				for (int jjj=0;jjj<original_h;jjj++)
					if (slackness_columns[jjj]>slackness_columns[j])
						slackness_columns[jjj] -= 1;
			} else {
				jj++;
			}
		} else {
			jj++;
		}
	}
	free(head);
}

struct WalkBackLinked : ValueLinked {
	Table* t;
	int pivot_index;
	double* head;
};


double walk_back(Table* t, Mask* coalition, double* head) {
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
}



/*int simplex_while_unimprovable(Table* t, double* head, Mask* coalition, Mask* anticoalition, bool maximising) {
	if (t->table_pivot_column_number != t->h) {
		printf("ERROR: simplex_while_unimprovable method on badly formed table.\n");
		return -1;
	}
	if (t->check_subset_improvable(anticoalition, coalition, head, false)) {
		printf("ERROR: simplex_while_unimprovable given bad table\n");
		return -1;
	}
	//printf("SIMPLEX_WHILE_UNIMPROVABLE INITIALISE\n");
	Table* t2 = (Table*)malloc(sizeof(Table));
	t2->initialise(t->w,t->h);
	t->calculate_pivots();
	t->apply_to_head(head,head);
	masks->add(t->table_pivot_column_mask);
	while (true) {
		double best_improvement=0;
		int best_improvement_index = -1;
		if (maximising==true) {
			for (int pivot_index=0; pivot_index < t->pivotable_number; pivot_index++) {
				double head_value = head[t->pivotable_columns[pivot_index]];
				if (head_value < 0) {
					double ratio = head_value*t->pivotable_ratios[pivot_index];
					if (ratio<best_improvement) {
						best_improvement = ratio;
						best_improvement_index = pivot_index;
					} else if ((ratio==0) && (best_improvement_index==-1)) { //bland's rule
						best_improvement_index = pivot_index;
					}
				}
			}
		} else {
			for (int pivot_index=0; pivot_index < t->pivotable_number; pivot_index++) {
				double head_value = head[t->pivotable_columns[pivot_index]];
				if (head_value > 0) {
					double ratio = head_value*t->pivotable_ratios[pivot_index];
					if (ratio>best_improvement) {
						best_improvement = ratio;
						best_improvement_index = pivot_index;
					} else if ((ratio==0) && (best_improvement_index==-1)) { //bland's rule
						best_improvement_index = pivot_index;
					}
				}
			}
		}
		if (best_improvement_index==-1) { // destinct optima attained
			masks->clear();
			t2->free_data();
			free(t2);
			return 0;
		}
		//printf("best pivot index %i at %f improvement\n",best_improvement_index,best_improvement);
		t2->pivot(t, best_improvement_index);
		t2->apply_to_head(head,head);
		if (t->check_subset_improvable(anticoalition, coalition, head, false)) {
			
		}
		t2->calculate_pivots();
		if (best_improvement==0) {
			if (masks->search(t->table_pivot_column_mask)==true) {// if degenerate cycling is occuring in context of bland's rule
				masks->clear();
				t2->free_data();
				free(t2);
				return 0;
			}
			masks->add(t->table_pivot_column_mask);
		} else {
			masks->clear();
		}
	}
}*/



/*double walk_jagged(Table* t, Mask* coalition, Mask* anticoalition, double* head) {
	#if DEBUG==1
		printf("WALKFORWARD: coalition: ");
		coalition->print();
		printf("   anticoalition: ");
		anticoalition->print();
		printhead(head, t->w);
		t->table_pivot_column_mask->print();
		t->print();
		int walkbacks = 0;
	#endif
	
	double* original_head = head;
	int w = t->w;
	
	
}*/

//struct WalkBackLinked : ValueLinked {
//	Table* t;
//	int pivot_index;
//	double* head;
//};

double walk_forward(Table* t, Mask* coalition, double* head) {
	#if DEBUG==1
		printf("WALKFORWARD: coalition: ");
		coalition->print();
		printf("   anticoalition: ");
		anticoalition->print();
		printhead(head, t->w);
		t->table_pivot_column_mask->print();
		t->print();
		int walkbacks = 0;
	#endif
	if (t->check_subset_improvable(coalition, head, true)) {
		printf("ERROR: simplex_while_unimprovable given bad table\n");
		return -1;
	}
	int w = t->w;
	t->calculate_pivots();
	t->apply_to_head(head,head);
	
	WalkBackLinked* link;
	Mask* new_mask = (Mask*)malloc(sizeof(Mask));
	
	link = (WalkBackLinked*)malloc(sizeof(WalkBackLinked));
	link->t = t;
	link->head = head;
	link->v = 0;
	pivot_list->add(link);
	masks->add(t->table_pivot_column_mask);
	value_memory->add(head[w-1]);
	
	double* new_head = (double*)malloc(sizeof(double)*(w));
	Table* new_table = (Table*)malloc(sizeof(Table));
	new_table->initialise(w,t->h);
	table_refs->add(new_table);
	refs->add(new_head);
	
	while (pivot_list->root != NULL) {
		#if DEBUG==1
			printf("ITERATING %i\n",walkbacks);
			walkbacks++;
		#endif
		link = (WalkBackLinked*)(pivot_list->pop());
		t = link->t;
		head = link->head;
		free(link);
		for (int i=0; i < t->pivotable_number; i++) {
			new_mask->set(t->table_pivot_column_mask);
			new_mask->flip_bit(t->pivotable_columns[i]);
			new_mask->flip_bit(t->table_pivot_columns[t->pivotable_rows[i]]);
			if (masks->search(new_mask)==false) {
				masks->add(new_mask);
				new_table->pivot(t,i);
				new_table->apply_to_head(head,new_head);
				if (new_table->check_subset_improvable(coalition, new_head, true)==false) {
					new_table->calculate_pivots();
					//printf("Mask not improvable: ");
					//new_mask->print();
					value_memory->add(new_head[w-1]);
					link = (WalkBackLinked*)malloc(sizeof(WalkBackLinked));
					link->t = new_table;
					link->head = new_head;
					link->v = 0;
					pivot_list->add(link);
					new_head = (double*)malloc(sizeof(double)*(w));
					new_table = (Table*)malloc(sizeof(Table));
					new_table->initialise(w,t->h);
					table_refs->add(new_table);
					refs->add(new_head);
				}
			}
		}
	}
	printf("finished walkforward: %i walks\n",refs->length);
	#if DEBUG==1
		printf("finished walkforward: %i walkforwards\n",walkbacks);
	#endif
	refs->free_all();
	table_refs->free_all();
	masks->clear();
	double max_val = value_memory->max();
	value_memory->clear();
	free(new_mask);
	
	#if DEBUG==1
		printf("RETURNING: %f\n", max_val);
	#endif
	return max_val;
}






