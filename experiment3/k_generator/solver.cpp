#include <stdbool.h>
#include <float.h>

#include "mask_memory.cpp"
#include "row_memory.cpp"
#include "table.cpp"
#include "table_memory.cpp"

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


double bilevel_solve(Table* t, Mask* anti_subset_mask, double* head, bool maximising) {
	#if DEBUG==1
		printf("BILEVEL SOLVE: anti_coalition: ");
		anti_subset_mask->print();
		printhead(head, t->w);
		t->table_pivot_column_mask->print();
		t->print();
		int iterations = 0;
	#endif
	double* temp_head = (double*)malloc(sizeof(double)*(t->w+1));
	double* temp_head2 = (double*)malloc(sizeof(double)*(t->w+1));
	Table_Memory* table_refs = (Table_Memory*)malloc(sizeof(Table_Memory));
	Mask_Memory* plus_masks = (Mask_Memory*)malloc(sizeof(Mask_Memory));
	Mask_Memory* neutral_masks = (Mask_Memory*)malloc(sizeof(Mask_Memory));
	table_refs->setup(MEMORY_INITIAL_SIZE);
	plus_masks->setup(MEMORY_INITIAL_SIZE);
	neutral_masks->setup(MEMORY_INITIAL_SIZE);

	t->apply_to_head(head,temp_head);
	int max_int = maximising==true ? 1 : -1;
	t->simplex(temp_head, !maximising);

	Table* new_t = (Table*)calloc(sizeof(Table),1);
	new_t->initialise_and_load(t);
	new_t->data = (double*)realloc(new_t->data, sizeof(double)*(t->w+1)*(t->h+1));
	new_t->w = t->w+1;
	new_t->h = t->h+1;
	for (int j=0; j<t->h; j++) {
		for (int i=0; i<t->w-1; i++)
			new_t->set(i,j,t->get(i,j));
		new_t->set(t->w-1,j,0);
		new_t->set(t->w,j,t->get(t->w-1,j));
	}
	for (int i=0; i<t->w-1; i++)
		new_t->set(i,t->h,max_int*temp_head[i]);
	new_t->set(t->w,t->h,temp_head[t->w-1]);
	new_t->set(t->w-1,t->h,1.0);
	new_t->table_pivot_columns[t->h]=t->w-1;
	new_t->table_pivot_column_number += 1;
	new_t->table_pivot_column_mask->set_bit(t->w-1,1);
	new_t->calculate_pivots();
	temp_head[t->w] = temp_head[t->w-1];
	temp_head[t->w-1]=0;
	
	t=new_t;
	Mask new_mask;
	bool refresh;

	neutral_masks->add(t->table_pivot_column_mask);
	
	/*printhead(temp_head,t->w);
	t->print();
	t->print_pivot_info();
	t->print_pivotable_info();*/

	while (true) {
		refresh=false;
		for (int i=0; i<t->pivotable_number; i++)
			if (-max_int * t->pivotable_ratios[i] * temp_head[t->pivotable_columns[i]] > 0) {
				//printf("pivot %i is improvement\n",i);
				new_mask.set(t->table_pivot_column_mask);
				new_mask.flip_bit(t->pivotable_columns[i]);
				if (t->pivotable_rows[i] != -1)
					new_mask.flip_bit(t->table_pivot_columns[t->pivotable_rows[i]]);
				//new_mask.print();
				if (plus_masks->search(&new_mask)==false) {
					Table* tt = (Table*)malloc(sizeof(Table));
					tt->initialise(t->w,t->h);
					tt->pivot(t,i);
					tt->apply_to_head(temp_head,temp_head2);

					/*printf("temporarily pivoted to:\n");
					printhead(temp_head2,tt->w);
					tt->print();*/
					if (tt->check_subset_improvable(anti_subset_mask, temp_head2, !maximising) == false) {
						//printf("pivot %i is verified improvement\n",i);
						table_refs->free_all();
						t->free_data();
						free(t);
						neutral_masks->clear();
						neutral_masks->add(&new_mask);
						tt->apply_to_head(temp_head,temp_head);
						tt->set(tt->w-1,tt->h-1,0);
						tt->calculate_pivots();
						t = tt;
						refresh = true;

						/*printf("Stepping to table:\n");
						printhead(temp_head,t->w);
						t->print();
						t->print_pivot_info();
						t->print_pivotable_info();
						neutral_masks->print_all();
						plus_masks->print_all();*/
						break;
					} else {
						//printf("pivot %i not verified\n",i);
						tt->free_data();
						free(tt);
						plus_masks->add(&new_mask);
					}
				}
			}// else
			//	printf("pivot %i is fail improvement\n",i);
		if (refresh==false) {
			for (int i=0; i<t->pivotable_number; i++) {
				//printf("checking %i\n",i);
				if (t->pivotable_ratios[i] * temp_head[t->pivotable_columns[i]]==0) {
					//printf("pivot %i is not improvement\n",i);
					new_mask.set(t->table_pivot_column_mask);
					new_mask.flip_bit(t->pivotable_columns[i]);
					if (t->pivotable_rows[i] != -1)
						new_mask.flip_bit(t->table_pivot_columns[t->pivotable_rows[i]]);
					//new_mask.print();
					if (neutral_masks->search(&new_mask)==false) {
						//printf("adding table\n");
						neutral_masks->add(&new_mask);
						Table* tt = (Table*)malloc(sizeof(Table));
						tt->initialise(t->w,t->h);
						tt->pivot(t,i);
						//tt->print();
						table_refs->add(tt);
					}
				}
			}
			if (table_refs->length==0) {
				//printf("end of run\n");
				break;
			}
			t->free_data();
			free(t);
			t = table_refs->memory[table_refs->length-1];
			t->calculate_pivots();
			table_refs->length -= 1;
			t->apply_to_head(temp_head,temp_head);
			
			/*printf("Raising table:\n");
			printhead(temp_head,t->w);
			t->print();
			t->print_pivot_info();
			t->print_pivotable_info();*/
		}
	}
	table_refs->destroy();
	plus_masks->destroy();
	neutral_masks->destroy();
	free(table_refs);
	free(plus_masks);
	free(neutral_masks);
	//t->apply_to_head(head,temp_head);
	double ret = temp_head[t->w-1];
	t->free_data();
	free(t);
	free(temp_head);
	free(temp_head2);
	return ret;
}



