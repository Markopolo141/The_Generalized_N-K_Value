#include <stdbool.h>
#include <float.h>

#include "mask_memory.cpp"
#include "row_memory.cpp"
#include "table.cpp"
#include "table_memory.cpp"
#include "sorted_list.cpp"

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

struct WalkBackLinked : ValueLinked {
	Table* t;
	int pivot_index;
};





double alt_bilevel_solve(Table* t, Mask* coalition_mask, double* head, bool maximising) {
	Mask field_mask;
	field_mask.set_ones(t->w-1);
	~field_mask;
	Mask working_mask;
	double extreme_value;

	Table_Memory* table_refs = (Table_Memory*)malloc(sizeof(Table_Memory));
	Table_Memory* unique_table_refs = (Table_Memory*)malloc(sizeof(Table_Memory));
	Mask_Memory* minus_masks = (Mask_Memory*)malloc(sizeof(Mask_Memory));
	Mask_Memory* masks = (Mask_Memory*)malloc(sizeof(Mask_Memory));
	Mask_Memory* intermediary_masks = (Mask_Memory*)malloc(sizeof(Mask_Memory));
	Mask_Memory* face_masks = (Mask_Memory*)malloc(sizeof(Mask_Memory));
	Table* new_t = (Table*)calloc(sizeof(Table),1);
	double* temp_head = (double*)malloc(sizeof(double)*t->w);

	t->apply_to_head(head,temp_head);
	new_t->initialise_and_load(t);
	new_t->calculate_pivots(false);

	table_refs->setup(MEMORY_INITIAL_SIZE);
	unique_table_refs->setup(MEMORY_INITIAL_SIZE);
	minus_masks->setup(MEMORY_INITIAL_SIZE);
	masks->setup(MEMORY_INITIAL_SIZE);
	intermediary_masks->setup(MEMORY_INITIAL_SIZE);
	face_masks->setup(MEMORY_INITIAL_SIZE);

	unique_table_refs->add(new_t);
	
	bool improvable = new_t->check_subset_improvable(coalition_mask, temp_head, !maximising);
	double best_improvement;
	int best_improvement_index;
	while (improvable==true) {
		new_t->simplex_improve(temp_head, -max_int, masks, &best_improvement, &best_improvement_index);
		new_t->pivot(new_t, best_improvement_index);
		new_t->calculate_pivots(false);
		new_t->apply_to_head(temp_head,temp_head);
		improvable = new_t->check_subset_improvable(coalition_mask, temp_head, !maximising);
	}
	masks->add(new_t->table_pivot_column_mask);
	extreme_value = temp_head[new_t->w-1];
	while (improvable==false) {
		minus_masks->clear();
		new_t->add_degenerate_pivot_masks(minus_masks);
		new_t->simplex_step(temp_head, max_int, &best_improvement, &best_improvement_index);
		if (*best_improvement_index==-1) { // destinct optima attained
			return temp_head[new_t->w-1];
		}
		new_t->pivot(new_t, best_improvement_index);
		new_t->calculate_pivots(false);
		new_t->apply_to_head(temp_head,temp_head);
		extreme_value = temp_head[new_t->w-1];
		if (masks->search(new_t->table_pivot_column_mask)==true) { // if degenerate cycling is occuring in context of bland's rule
			return temp_head[new_t->w-1];
		}
		masks->add(new_t->table_pivot_column_mask);
		improvable = new_t->check_subset_improvable(coalition_mask, temp_head, !maximising);
	}
	Mask new_mask;
	new_mask.set(masks->memory[masks->length-2]);
	masks->clear();
	masks->add(new_mask);
	table_refs->add(new_t);

	int no_ones;
	while(table_refs->length > 0) {
		t = table_refs->memory[table_refs->length-1];
		new_mask->set(masks->memory[masks->length-1];
		table_refs->length -= 1;
		masks->length -= 1;

		working_mask.set(new_mask);
		working_mask = working_mask | t->table_pivot_column_mask;
		working_mask = working_mask | field_mask;
		working_mask = ~working_mask;
		no_ones = working_mask.count_bits();
		
		new_t = (Table*)calloc(sizeof(Table),1);
		new_t->initialise_and_load(t);
		for (int i=0; i<no_ones; i++) {
			int ii = 0;
			int j;
			for (j=0; j<length; j++) {
				if (working_mask.get_bit(j) == 1)
					ii++;
				if (ii>i)
					break;
			}
			working_mask.flip_bit(j);
			if (face_masks->search(working_mask) == false) {
				intermediary_masks->clear();
				intermediary_masks->add(new_t->table_pivot_column_mask);
				face_masks->add(working_mask);
				while() {
					for (ii=0; ii<new_t->pivotable_number; ii++) {
						if (working_mask->get_bit(new_t->pivotable_columns[ii])==0) {
							new_mask.set(this->table_pivot_column_mask);
							new_mask.flip_bit(this->pivotable_columns[i]);
							if (this->pivotable_rows[i] != -1)
								new_mask.flip_bit(this->table_pivot_columns[this->pivotable_rows[i]]);
							if ((intermediary_masks->search(new_mask)==false) && (minus_masks->search(new_mask)==false)) {
								intermediary_masks->add(new_mask);
								new_t->pivot(new_t,ii);
								break; ///TODO: RAWR!
							}
						}
					}
				}
			}
			working_mask.flip_bit(j);
		}
	}
}


/*double super_bilevel_solve(Table* t, Mask* coalition_mask, double* head, bool maximising) {
	#if DEBUG==1
		printf("SUPER BILEVEL SOLVE: coalition: ");
		coalition_mask->print();
		printhead(head, t->w);
		t->table_pivot_column_mask->print();
		t->print();
	#endif
	int max_int = maximising==true ? 1 : -1;
	double* temp_head = (double*)malloc(sizeof(double)*t->w);
	t->apply_to_head(head,temp_head);
	Table* new_t = (Table*)calloc(sizeof(Table),1);
	new_t->initialise_and_load(t);
	new_t->calculate_pivots(false);
	
	Table_Memory* plus_table_refs = (Table_Memory*)malloc(sizeof(Table_Memory));
	Table_Memory* minus_table_refs = (Table_Memory*)malloc(sizeof(Table_Memory));
	Mask_Memory* plus_masks = (Mask_Memory*)malloc(sizeof(Mask_Memory));
	Mask_Memory* minus_masks = (Mask_Memory*)malloc(sizeof(Mask_Memory));

	plus_table_refs->setup(MEMORY_INITIAL_SIZE);
	minus_table_refs->setup(MEMORY_INITIAL_SIZE);
	plus_masks->setup(MEMORY_INITIAL_SIZE);
	minus_masks->setup(MEMORY_INITIAL_SIZE);

	bool improvable = new_t->check_subset_improvable(coalition_mask, temp_head, !maximising);
	while (improvable==false) {
		double best_improvement;
		int best_improvement_index;
		if (new_t->simplex_improve(temp_head, max_int, minus_masks, &best_improvement, &best_improvement_index) == false)
			break;
		new_t->pivot(new_t, best_improvement_index);
		new_t->calculate_pivots(false);
		new_t->apply_to_head(temp_head,temp_head);
		improvable = new_t->check_subset_improvable(coalition_mask, temp_head, !maximising);
	}
	minus_masks->clear();
	while (improvable==true) {
		double best_improvement;
		int best_improvement_index;
		if (new_t->simplex_improve(temp_head, -max_int, plus_masks, &best_improvement, &best_improvement_index) == false)
			break;
		new_t->pivot(new_t, best_improvement_index);
		new_t->calculate_pivots(false);
		new_t->apply_to_head(temp_head,temp_head);
		improvable = new_t->check_subset_improvable(coalition_mask, temp_head, !maximising);
	}
	plus_masks->clear();

	minus_table_refs->add(new_t);
	Mask new_mask;
	double extreme_value = temp_head[new_t->w-1];
	#if DEBUG==1
		printf("EQUALIZED TABLE: ");
		new_t->table_pivot_column_mask->print();
		printhead(temp_head, t->w);
		new_t->print();
		printf("table id: %li\n",(long)new_t);
		printf("extreme value: %f\n",extreme_value);
	#endif
	new_t = (Table*)calloc(sizeof(Table),1);
	new_t->initialise(t->w,t->h);
	while (true) {
		//printf("ZOOS: %i\n",new_t->h);
		if (minus_table_refs->length>0) {
			#if DEBUG==1
				printf("SOURCING TABLE MINUS\n");
			#endif
			t = minus_table_refs->memory[minus_table_refs->length-1];
			minus_table_refs->length -= 1;
			t->apply_to_head(head,temp_head);
			#if DEBUG==1
				printf("loading table id: %li\n",(long)t);
			#endif
			if (max_int*temp_head[t->w-1] > max_int*extreme_value) {
				extreme_value = temp_head[t->w-1];
				#if DEBUG==1
					printf("extreme value set to: %f\n",extreme_value);
				#endif
			}
			for (int i=0; i<t->pivotable_number; i++) {
				new_mask.set(t->table_pivot_column_mask);
				new_mask.flip_bit(t->pivotable_columns[i]);
				if (t->pivotable_rows[i] != -1)
					new_mask.flip_bit(t->table_pivot_columns[t->pivotable_rows[i]]);
				#if DEBUG==1
					printf("pivot mask check: ");
					new_mask.print();
				#endif
				if ((plus_masks->search(&new_mask)==false) && (minus_masks->search(&new_mask)==false)) {
					new_t->pivot(t,i);
					new_t->apply_to_head(head,temp_head);
					#if DEBUG==1
						printf("pivot improvability check\n");
						printhead(temp_head, t->w);
						new_t->print();
					#endif
					if (new_t->check_subset_improvable(coalition_mask, temp_head, !maximising) == true) {
						#if DEBUG==1
							printf("NEW PLUS TABLE: \n");
							printhead(temp_head, t->w);
							new_t->print();
							printf("table id: %li\n",(long)new_t);
							new_t->print_pivot_info();
						#endif
						new_t->calculate_pivots(false);
						plus_table_refs->add(new_t);
						new_t = (Table*)calloc(sizeof(Table),1);
						new_t->initialise(t->w,t->h);
					}
				}
			}
			minus_masks->add(t->table_pivot_column_mask);
			t->free_data();
			free(t);
		} else if (plus_table_refs->length>0) {
			#if DEBUG==1
				printf("SOURCING TABLE PLUS\n");
			#endif
			t = plus_table_refs->memory[plus_table_refs->length-1];
			plus_table_refs->length -= 1;
			#if DEBUG==1
				printf("loading table id: %li\n",(long)t);
			#endif
			for (int i=0; i<t->pivotable_number; i++) {
				new_mask.set(t->table_pivot_column_mask);
				new_mask.flip_bit(t->pivotable_columns[i]);
				if (t->pivotable_rows[i] != -1)
					new_mask.flip_bit(t->table_pivot_columns[t->pivotable_rows[i]]);
				#if DEBUG==1
					printf("pivot mask check: ");
					new_mask.print();
				#endif
				if ((plus_masks->search(&new_mask)==false) && (minus_masks->search(&new_mask)==false)) {
					new_t->pivot(t,i);
					new_t->apply_to_head(head,temp_head);
					#if DEBUG==1
						printf("pivot improvability check\n");
						printhead(temp_head, t->w);
						new_t->print();
					#endif
					if (new_t->check_subset_improvable(coalition_mask, temp_head, !maximising) == false) {
						#if DEBUG==1
							printf("NEW MINUS TABLE: \n");
							printhead(temp_head, t->w);
							new_t->print();
							printf("table id: %li\n",(long)new_t);
							new_t->print_pivot_info();
						#endif
						minus_table_refs->add(new_t);
						new_t->calculate_pivots(false);
						new_t = (Table*)calloc(sizeof(Table),1);
						new_t->initialise(t->w,t->h);
					}
				}
			}
			plus_masks->add(t->table_pivot_column_mask);
			t->free_data();
			free(t);
		} else {
			break;
		}
	}
	new_t->free_data();
	free(new_t);
	free(temp_head);
	plus_table_refs->destroy();
	minus_table_refs->destroy();
	plus_masks->destroy();
	minus_masks->destroy();
	free(plus_table_refs);
	free(minus_table_refs);
	free(plus_masks);
	free(minus_masks);
	return extreme_value;
}*/

double bilevel_solve(Table* t, Mask* coalition_mask, double* head, bool maximising) {
	#if DEBUG==1
		printf("BILEVEL SOLVE: coalition: ");
		coalition_mask->print();
		printhead(head, t->w);
		t->table_pivot_column_mask->print();
		t->print();
		//int iterations = 0;
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
	new_t->resize_table(t->w+1, t->h+1);
	for (int j=0; j<t->h; j++) {
		for (int i=0; i<t->w-1; i++)
			new_t->set(i,j,t->get(i,j));
		new_t->set(t->w-1,j,0);
		new_t->set(t->w,j,t->get(t->w-1,j));
	}
	for (int i=0; i<t->w; i++)
		new_t->set(i,t->h,max_int*temp_head[i]);
	//new_t->set(t->w,t->h,temp_head[t->w-1]);
	new_t->set(t->w-1,t->h,1.0);
	new_t->set(t->w,t->h,0.0);
	new_t->table_pivot_columns[t->h]=t->w-1;
	new_t->table_pivot_column_number += 1;
	new_t->table_pivot_column_mask->set_bit(t->w-1,1);
	new_t->calculate_pivots(false);
	temp_head[t->w] = temp_head[t->w-1];
	temp_head[t->w-1]=0;

	t=new_t;
	Mask new_mask;
	bool refresh;

	neutral_masks->add(t->table_pivot_column_mask);
	
	#if DEBUG==1
		printf("BILEVEL SOLVE BEGIN\n");
		printhead(temp_head,t->w);
		t->print();
		t->print_pivot_info();
		t->print_pivotable_info();
	#endif

	while (true) {
		refresh=false;
		for (int i=0; i<t->pivotable_number; i++)
			if (-max_int * t->pivotable_ratios[i] * temp_head[t->pivotable_columns[i]] > 0) {
				#if DEBUG==1
					printf("pivot %i is improvement\n",i);
				#endif
				new_mask.set(t->table_pivot_column_mask);
				new_mask.flip_bit(t->pivotable_columns[i]);
				if (t->pivotable_rows[i] != -1)
					new_mask.flip_bit(t->table_pivot_columns[t->pivotable_rows[i]]);
				#if DEBUG==1
					new_mask.print();
				#endif
				if (plus_masks->search(&new_mask)==false) {
					Table* tt = (Table*)malloc(sizeof(Table));
					tt->initialise(t->w,t->h);
					tt->pivot(t,i);
					tt->apply_to_head(head,temp_head2);

					#if DEBUG==1
						printf("temporarily pivoted to:\n");
						printhead(temp_head2,tt->w);
						tt->print();
					#endif
					if (tt->check_subset_improvable(coalition_mask, temp_head2, !maximising) == false) {
						#if DEBUG==1
							printf("pivot %i is verified improvement\n",i);
						#endif
						table_refs->free_all();
						t->free_data();
						free(t);
						neutral_masks->clear();
						neutral_masks->add(&new_mask);
						tt->apply_to_head(head,temp_head);
						for (int k=0;k<(tt->h);k++) {
							if (tt->table_pivot_columns[k]==(tt->w)-2) {
								tt->set(tt->w-1,k,0);
								break;
							}
						}
						tt->calculate_pivots(false);
						t = tt;
						refresh = true;

						#if DEBUG==1
							printf("Stepping to table:\n");
							printhead(temp_head,t->w);
							t->print();
							t->print_pivot_info();
							t->print_pivotable_info();
							neutral_masks->print_all();
							plus_masks->print_all();
						#endif
						//printf("S ");
						//t->table_pivot_column_mask->print();
						break;
					} else {
						#if DEBUG==1
							printf("pivot %i not verified\n",i);
						#endif
						tt->free_data();
						free(tt);
						plus_masks->add(&new_mask);
					}
				}
			}
			#if DEBUG==1
			else
				printf("pivot %i is fail improvement\n",i);
			#endif
		if (refresh==false) {
			for (int i=0; i<t->pivotable_number; i++) {
				#if DEBUG==1
					printf("checking %i\n",i);
				#endif
				if (t->pivotable_ratios[i] * temp_head[t->pivotable_columns[i]]==0) {
					#if DEBUG==1
						printf("pivot %i is not improvement\n",i);
					#endif
					new_mask.set(t->table_pivot_column_mask);
					new_mask.flip_bit(t->pivotable_columns[i]);
					if (t->pivotable_rows[i] != -1)
						new_mask.flip_bit(t->table_pivot_columns[t->pivotable_rows[i]]);
					#if DEBUG==1
						new_mask.print();
					#endif
					if (neutral_masks->search(&new_mask)==false) {
						#if DEBUG==1
							printf("adding table\n");
						#endif
						neutral_masks->add(&new_mask);
						Table* tt = (Table*)malloc(sizeof(Table));
						tt->initialise(t->w,t->h);
						tt->pivot(t,i);
						#if DEBUG==1
							tt->print();
						#endif
						table_refs->add(tt);
					}
				}
			}
			if (table_refs->length==0) {
				#if DEBUG==1
					printf("end of run\n");
				#endif
				break;
			}
			t->free_data();
			free(t);
			t = table_refs->memory[table_refs->length-1];
			t->calculate_pivots(false);
			table_refs->length -= 1;
			t->apply_to_head(temp_head,temp_head);
			
			#if DEBUG==1
				printf("Raising table:\n");
				printhead(temp_head,t->w);
				t->print();
				t->print_pivot_info();
				t->print_pivotable_info();
			#endif
			//printf("r ");
			//t->table_pivot_column_mask->print();
		}
	}
	table_refs->destroy();
	plus_masks->destroy();
	neutral_masks->destroy();
	free(table_refs);
	free(plus_masks);
	free(neutral_masks);
	double ret = temp_head[t->w-1];
	t->free_data();
	free(t);
	free(temp_head);
	free(temp_head2);
	return ret;
}






double walk_back(Table* t, Mask* coalition, double* original_head, bool maximising) {
	#if DEBUG==1
		printf("WALKBACK: coalition: ");
		coalition->print();
		printhead(original_head, t->w);
		t->table_pivot_column_mask->print();
		t->print();
		int walkbacks = 0;
	#endif
	int max_int = maximising==true ? 1 : -1;
	int w = t->w;

	WalkBackLinked* link;
	Mask_Memory* masks;
	SortedList* pivot_list;
	Table_Memory* table_refs;
	double* head;
	
	masks = (Mask_Memory*)malloc(sizeof(Mask_Memory));
	masks->setup(MEMORY_INITIAL_SIZE);
	pivot_list = (SortedList*)calloc(sizeof(SortedList),1);
	pivot_list->setup();
	table_refs = (Table_Memory*)malloc(sizeof(Table_Memory));
	table_refs->setup(MEMORY_INITIAL_SIZE);
	head = (double*)malloc(sizeof(double)*(w));
	memcpy (head, original_head, sizeof(double)*w );

	t->simplex(head, maximising);
	t->calculate_pivots(false);
	t->apply_to_head(original_head,head);
	
	#if DEBUG==1
		printf("WALKBACK: simplexed result: \n");
		printhead(head, t->w);
		t->print();
	#endif

	masks->add(t->table_pivot_column_mask);
	Mask* new_mask = (Mask*)malloc(sizeof(Mask));
	while (t->check_subset_improvable(coalition, head, !maximising)==true) {
		#if DEBUG==1
			printf("ITERATING %i\n",walkbacks);
			walkbacks++;
		#endif
		for (int i=0; i < t->pivotable_number; i++) {
			new_mask->set(t->table_pivot_column_mask);
			new_mask->flip_bit(t->pivotable_columns[i]);
			if (t->pivotable_rows[i] != -1)
				new_mask->flip_bit(t->table_pivot_columns[t->pivotable_rows[i]]);
			if (masks->search(new_mask)==false) {
				link = (WalkBackLinked*)malloc(sizeof(WalkBackLinked));
				link->t = t;
				link->pivot_index = i;
				link->v = max_int*(head[w-1] - t->pivotable_ratios[i]*head[t->pivotable_columns[i]]);
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
		t = (Table*)malloc(sizeof(Table));
		t->initialise(w,link->t->h);
		table_refs->add(t);
		t->pivot(link->t,link->pivot_index);
		t->calculate_pivots(false);
		t->apply_to_head(original_head,head);
		free(link);
		
		#if DEBUG==1
			printf("TABLE:\n");
			printhead(head,t->w);
			t->table_pivot_column_mask->print();
			t->print();
		#endif
	}
	#if DEBUG==1
		printf("finished walkback: %i walkbacks\n",walkbacks);
		printhead(head, w);
		t->print();
	#endif
	
	double ret = head[w-1];
	table_refs->free_all();
	table_refs->destroy();
	free(table_refs);
	pivot_list->destroy();
	free(pivot_list);
	masks->clear();
	masks->destroy();
	free(masks);
	
	free(head);
	free(new_mask);
	#if DEBUG==1
		printf("RETURNING: %f\n", ret);
	#endif
	return ret;
}




