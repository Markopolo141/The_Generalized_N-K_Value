

struct Table {
	double* data; // the table itself, including the head,
	int w,h;
	
	int* table_pivot_columns; //the columns which are pivot columns, defines the table, ordered by the row the 1.0 occurs on.
	Mask* table_pivot_column_mask; //the binary mask of the pivot columns
	int table_pivot_column_number; // the number of columns that are established pivot columns of the table. 
	
	unsigned char* pivotable_columns; //the columns of potential pivot points, not ordered and not nessisarily unique
	unsigned char* pivotable_rows; // the respective rows of potential pivot points, not ordered and not nessisarily unique
	double* pivotable_ratios; // the per-unit improvement of pivoting from the respective pivot point
	Mask* pivotable_columns_mask; // the binary mask of pivotable columns
	int pivotable_number; // the number potential pivot points.
	bool need_to_recalculate_pivotable; // a flag that is raised to indicate whether the calculate_pivots needs tobe called
	
	void initialise(int w, int h);
	void initialise_and_wipe(int w, int h);
	void initialise_and_load(Table* t);
	void reset_pivot_information();
	void free_data();
	void load(Table* t);
	
	inline double get(int c, int r);
	inline void set(int c, int r, double v);
	void delete_row(int r);
	void delete_column(int c, double* head);
	
	void apply_to_head(double* head, double* new_head); // for a given head, apply the pivot rows to the head to make it consistent with the table
	void calculate_pivots(); // scan through the table and select all the viable pivot points
	void pivot(Table* t, int pivotable_index); // given table t (which can be -this-) with its pivotable index, mutate this table to be the pivoted result
	int shallow_check_subset_improvable(Mask* anti_subset_mask, double* head, bool maximising); // scan through this table and detect whether the coalition identified by the anti-subset-mask can improve on a one pass
	bool check_subset_improvable(Mask* anti_subset_mask, double* head, bool maximising); // do a comprehensive check whether the coalition identified by the anti-subset-mask can improve.
	double simplex(double* head, bool maximising); // conduct a full simplex to the table.
	bool simplex_improve(double* head, int max_int, Mask_Memory* masks, double* best_improvement, int* best_improvement_index); // do degenerate pivots until the first nondegenerate simplex step (if possible)

	void print(); // print table itself
	void print_pivot_info(); // debug info on table pivot columns
	void print_pivotable_info(); // debug info on pivotable column info attached to table
};

void Table::print() {
	for (int j=0;j<this->h;j++) {
		printf("[");
		for (int i=0;i<this->w-1;i++) {
			printf("%f,\t",this->get(i,j));
		}
		printf("%f",this->get(this->w-1,j));
		printf("]\n");
	}
}

void Table::print_pivot_info() {
	printf("TABLE w=%i h=%i pivot info: table_pivot_column_number: %i\n\t",this->w,this->h,this->table_pivot_column_number);
	printf("table_pivot_column_mask: ");
	this->table_pivot_column_mask->print();
	printf("\tpivot row columns: ");
	for (int i=0; i<this->h; i++)
		printf("%i ",this->table_pivot_columns[i]);
	printf("\n");
}

void Table::print_pivotable_info() {
	printf("TABLE w=%i h=%i pivotable info: pivotable_number: %i\n\t",this->w,this->h,this->pivotable_number);
	printf("table_pivotable_column_mask: ");
	this->pivotable_columns_mask->print();
	printf("\tpivots: ");
	for (int i=0; i<this->pivotable_number; i++)
		printf("(%i,%i,%f), ",this->pivotable_rows[i],this->pivotable_columns[i],this->pivotable_ratios[i]);
	printf("\n");
}


void Table::reset_pivot_information() {
	for (int i=0;i<h;i++) this->table_pivot_columns[i]=-1;
	this->table_pivot_column_mask->set_zero();
	this->pivotable_columns_mask->set_zero();
	this->pivotable_number = 0;
	this->table_pivot_column_number = 0;
	this->need_to_recalculate_pivotable = true;
}

void Table::initialise(int w, int h) {
	/*int wminusonetimesh = (w-1)*h;
	this->data = (double*)malloc(sizeof(double)*w*h);
	this->w = w;
	this->h = h;
	
	this->table_pivot_columns = (int*)malloc(sizeof(int)*h);
	this->pivotable_columns = (unsigned char*)malloc(sizeof(unsigned char)*wminusonetimesh);
	this->pivotable_rows = (unsigned char*)malloc(sizeof(unsigned char)*wminusonetimesh);
	this->pivotable_ratios = (double*)malloc(sizeof(double)*wminusonetimesh);
	this->table_pivot_column_mask = (Mask*)malloc(sizeof(Mask));
	this->pivotable_columns_mask = (Mask*)malloc(sizeof(Mask));

	this->reset_pivot_information();*/
	this->initialise_and_wipe(w, h);
}
void Table::initialise_and_wipe(int w, int h) {
	int wminusonetimesh = (w-1)*h;
	this->data = (double*)calloc(sizeof(double),w*h);
	this->w = w;
	this->h = h;
	
	this->table_pivot_columns = (int*)malloc(sizeof(int)*h);
	this->pivotable_columns = (unsigned char*)calloc(sizeof(unsigned char),wminusonetimesh);
	this->pivotable_rows = (unsigned char*)calloc(sizeof(unsigned char),wminusonetimesh);
	this->pivotable_ratios = (double*)calloc(sizeof(double),wminusonetimesh);
	this->table_pivot_column_mask = (Mask*)calloc(sizeof(Mask),1);
	this->pivotable_columns_mask = (Mask*)calloc(sizeof(Mask),1);

	this->reset_pivot_information();
}
void Table::free_data() {
	free(this->data);
	free(this->table_pivot_columns);
	free(this->pivotable_columns);
	free(this->pivotable_rows);
	free(this->pivotable_ratios);
	free(this->table_pivot_column_mask);
	free(this->pivotable_columns_mask);
}
void Table::load(Table* t) {
	int wminusonetimesh = ((this->w)-1)*(this->h);
	this->table_pivot_column_mask->set(t->table_pivot_column_mask);
	this->pivotable_columns_mask->set(t->pivotable_columns_mask);
	this->table_pivot_column_number = t->table_pivot_column_number;
	this->pivotable_number = t->pivotable_number;
	memcpy ( this->data, t->data, sizeof(double)*(this->h)*(this->w) );
	memcpy ( this->table_pivot_columns, t->table_pivot_columns, sizeof(int)*(this->h) );
	memcpy ( this->pivotable_columns, t->pivotable_columns, sizeof(unsigned char)*wminusonetimesh );
	memcpy ( this->pivotable_rows, t->pivotable_rows, sizeof(unsigned char)*wminusonetimesh );
	memcpy ( this->pivotable_ratios, t->pivotable_ratios, sizeof(double)*wminusonetimesh );
	this->need_to_recalculate_pivotable = t->need_to_recalculate_pivotable;
}
void Table::initialise_and_load(Table* t) {
	this->initialise(t->w,t->h);
	this->load(t);
}

inline double Table::get(int c, int r) {
	return (this->data)[c+r*this->w];
}
inline void Table::set(int c, int r, double v) {
	(this->data)[c+r*this->w] = v;
	need_to_recalculate_pivotable = true;
}

bool Table::simplex_improve(double* head, int max_int, Mask_Memory* masks, double* best_improvement, int* best_improvement_index) {
	if (this->need_to_recalculate_pivotable==true)
		printf("WARNING: sub-simplexing on outdated table pivot info\n");
	masks->clear();
	masks->add(this->table_pivot_column_mask);
	while (true) {
		*best_improvement=0;
		*best_improvement_index = -1;
		//int smallest_out_column = -1;
		for (int pivot_index=0; pivot_index < this->pivotable_number; pivot_index++) {
			double head_value = head[this->pivotable_columns[pivot_index]];
			if (head_value*max_int < 0) {
				double ratio = -max_int*head_value*this->pivotable_ratios[pivot_index];
				if (ratio>*best_improvement) {
					*best_improvement = ratio;
					*best_improvement_index = pivot_index;
				} else if ((ratio==0) && (*best_improvement_index==-1)) { //bland's rule
					//if (smallest_out_column == -1) {
					//	smallest_out_column = this->pivotable_columns[pivot_index];
						*best_improvement_index = pivot_index;
					//} else {
					//	if (this->pivotable_columns[pivot_index] < smallest_out_column) {
					//		smallest_out_column = this->pivotable_columns[pivot_index];
					//		*best_improvement_index = pivot_index;
					//	}
					//}
				}
			}
		}
		if (*best_improvement_index==-1) // destinct optima attained
			return false;
		if (*best_improvement>0) // if positive improvement possible
			return true;
		this->pivot(this, *best_improvement_index);
		if (masks->search(this->table_pivot_column_mask)==true) // if degenerate cycling is occuring in context of bland's rule
			return false;
		this->calculate_pivots();
		this->apply_to_head(head,head);
		masks->add(this->table_pivot_column_mask);
	}
}

double Table::simplex(double* head, bool maximising) {
	//if (this->table_pivot_column_number != this->h)
	//	printf("WARNING: simplex method on potentially badly formed table.\n");
	int max_int = maximising==true ? 1 : -1;
	this->calculate_pivots();
	this->apply_to_head(head,head);
	Mask_Memory* masks;
	masks = (Mask_Memory*)calloc(sizeof(Mask_Memory),1);
	masks->setup(MEMORY_INITIAL_SIZE);
	double best_improvement;
	int best_improvement_index;

	while (this->simplex_improve(head, max_int, masks, &best_improvement, &best_improvement_index)==true) {
		this->pivot(this, best_improvement_index);
		this->calculate_pivots();
		this->apply_to_head(head,head);
	}

	masks->destroy();
	free(masks);
	return head[this->w-1];
}


// given a head, apply all pivot column information to it
void Table::apply_to_head(double* head, double* new_head) {
	int h = this->h;
	int w = this->w;
	for (int j=0;j<h;j++) {
		int pivot_column = this->table_pivot_columns[j];
		if (pivot_column != -1) {
			double head_pivot_column = head[pivot_column];
			if (j==0) {
				for (int i=0;i<w;i++)
					new_head[i] = head[i] - head_pivot_column*(this->get(i,j));
			} else {
				for (int i=0;i<w;i++)
					new_head[i] = new_head[i] - head_pivot_column*(this->get(i,j));
			}
		}
	}
	for (int i=0;i<w;i++)
		new_head[i] = SNAPTOZERO(new_head[i]);
}

// calculate all the potential pivot columns and calculate the ratios by which the pivoting will improve the objective
void Table::calculate_pivots() {
	int wminusone = this->w-1;
	int h = this->h;
	this->pivotable_columns_mask->set_zero();
	this->pivotable_number = 0;
	Row_Memory* row_memory;
	row_memory = (Row_Memory*)calloc(sizeof(Row_Memory),1);
	row_memory->setup(MEMORY_INITIAL_SIZE);
	for (unsigned char i=0;i<wminusone;i++) { // for each column
		if (this->table_pivot_column_mask->get_bit(i)==1) // scip if it is already table pivot column
			continue;
		row_memory->clear();
		double best_ratio = DBL_MAX; // find if the column can be pivoted and detect the pivot row/s for the column
		for (int j=0;j<h;j++) {
			double v = this->get(i,j);
			if (v>0) {
				double right_value = this->get(wminusone,j);
				row_memory->add(v,right_value,j);
				double ratio = right_value/v;
				if (ratio<best_ratio)
					best_ratio = ratio;
			}
		}
		for (unsigned char z=0; z<row_memory->length; z++) {// if there is a best row add the info to the datastructure
			Row_iter* r = &(row_memory->memory[z]);
			if ((r->right) - best_ratio*(r->val) < TINY) {
				this->pivotable_columns[this->pivotable_number] = i;
				this->pivotable_rows[this->pivotable_number] = r->index;
				this->pivotable_ratios[this->pivotable_number]=best_ratio;
				this->pivotable_columns_mask->set_bit(i,1);
				this->pivotable_number += 1;
			}
		}
	}
	row_memory->destroy();
	free(row_memory);
	this->need_to_recalculate_pivotable = false;
}

// set this table to be as pivoted from another table by the indexed pivotable point
void Table::pivot(Table* t, int pivotable_index) {
	if (t->need_to_recalculate_pivotable==true)
		printf("WARNING: pivoting from outdated table pivot info\n");
	int row = t->pivotable_rows[pivotable_index];
	int column = t->pivotable_columns[pivotable_index];
	double* pivot_row = &(t->data)[row*this->w];
	double center = pivot_row[column];
	if (t==this) {
		for (int i=0;i<this->w;i++)
			pivot_row[i] /= center;
		for (int j=0;j<this->h;j++)
			if (j!=row) {
				double* scan_row = &(t->data)[j*this->w];
				double scan_row_column = scan_row[column];
				if (scan_row_column!=0)
					for (int i=0;i<this->w;i++)
						scan_row[i] = SNAPTOZERO(scan_row[i] - scan_row_column*pivot_row[i]);
			}

	} else {
		double* this_pivot_row = &(this->data)[row*this->w];
		for (int i=0;i<this->w;i++)
			this_pivot_row[i] = pivot_row[i] / center;
		for (int j=0;j<this->h;j++)
			if (j!=row) {
				double* this_scan_row = &(this->data)[j*this->w];
				double* scan_row = &(t->data)[j*this->w];
				double scan_row_column = scan_row[column];
				if (scan_row_column!=0) {
					for (int i=0;i<this->w;i++)
						this_scan_row[i] = SNAPTOZERO(scan_row[i] - scan_row_column*this_pivot_row[i]);
				} else {
					for (int i=0;i<this->w;i++)
						this_scan_row[i] = scan_row[i];
				}
			}
		this->table_pivot_column_mask->set(t->table_pivot_column_mask);
		this->table_pivot_column_number = t->table_pivot_column_number;
	}
	this->table_pivot_column_mask->flip_bit(column);
	int old_pivot_column = t->table_pivot_columns[row];
	if (old_pivot_column != -1) {
		this->table_pivot_column_mask->flip_bit(old_pivot_column);
	} else {
		this->table_pivot_column_number = t->table_pivot_column_number + 1;
	}
	if (t!=this)
		memcpy ( this->table_pivot_columns, t->table_pivot_columns, sizeof(int)*(this->h) );
	this->table_pivot_columns[row] = column;
	this->need_to_recalculate_pivotable = true;
}


// deletes row r, (not optimised function), need to recalculate pivotable info after
void Table::delete_row(int r) {
	if (this->table_pivot_columns[r] != -1) {
		this->table_pivot_column_number -= 1;
		this->table_pivot_column_mask->flip_bit(this->table_pivot_columns[r]);
	}
	for (int ii=r; ii<this->h-1;ii++) {
		this->table_pivot_columns[ii] = this->table_pivot_columns[ii+1];
	}
	for (int j=r;j<(this->h-1);j++)
		for (int i=0;i<(this->w);i++)
			this->set(i,j,this->get(i,j+1));
	this->h -= 1;
	need_to_recalculate_pivotable = true;
}

// deletes column c, (not optimised function), do not use on columns that are pivot columns, and recalculate pivotable information after.
void Table::delete_column(int c, double* head) {
	for (int i=0; i<this->h; i++) {
		if (this->table_pivot_columns[i]==c) {
			this->table_pivot_column_number -= 1;
			this->table_pivot_columns[i] = -1;
		}
		if (this->table_pivot_columns[i]>c)
			this->table_pivot_columns[i] -= 1;
	}
	this->table_pivot_column_mask->remove_bit(c);
	for (int i=0; i<(this->w-1)*this->h; i++)
		this->data[i] = this->data[i+((i+this->w-c-1)/(this->w-1))];
	if (head != NULL)
		for (int i=0; i<this->w; i++)
			if (i>c)
				head[i-1]=head[i];
	this->w -= 1;
	need_to_recalculate_pivotable = true;
}

// for an applied head, can the set of players given by the anti-subset mask succeed in maximising the utility by themselves
// this function returns 1 if positively 'yes', -1 if positively 'no', and 0 if undecidable on a one-pass check.
// with 0, a more comprehensive check (which can handle degeneracy) should be performed.
int Table::shallow_check_subset_improvable(Mask* anti_subset_mask, double* head, bool maximising) {
	#if DEBUG==1
		printf("SUBSET IMPROVABLE\n");
		printf("anti_subset_mask: ");
		anti_subset_mask->print();
		printf("table pivotable_columns_mask: ");
		this->pivotable_columns_mask->print();
		printf("passed head:\n");
		printhead(head,this->w);
	#endif
	bool unviable = true;
	//printf("shallow: initialise\n");
	for (int i=0;i<this->w-1;i++) {
		if (maximising==true) {
			if (head[i]>=0)
				continue;
		} else {
			if (head[i]<=0)
				continue;
		}
		if (anti_subset_mask->get_bit(i)==0) {
		//if ((subset_mask->get_bit(i)==1)&&(this->pivotable_columns_mask->get_bit(i)==1)) {
			//printf("shallow: checking %i\n",i);
			int j;
			bool viable = false;
			for (j=0;j<this->h;j++) {
				//printf("shallow: checking row %i\n",i);
				double v = this->get(i,j);
				if (v>0) {
					//printf("shallow: checked >0\n");
					unviable=false;
					if ((this->get(this->w-1,j)==0.0)||(anti_subset_mask->get_bit(this->table_pivot_columns[j])==1)) {
						//printf("shallow: check break\n");
						break;
					}
					viable=true;
				}
			}
			if ((j==this->h) && viable)
				return 1;
		}
	}
	if (unviable==true)
		return -1;
	return 0;
}


bool Table::check_subset_improvable(Mask* anti_subset_mask, double* head, bool maximising) {
	if (this->table_pivot_column_number != this->h)
		printf("WARNING: check subset improvable method on table without full pivot set.\n");
	//printf("shallow check\n");
	int shallow_check = this->shallow_check_subset_improvable(anti_subset_mask, head, maximising);
	if (shallow_check==1) //check if trivially improvable/unimprovable
		return true;
	if (shallow_check==-1)
		return false;
	// otherwise perform more comprehensive check.
	//printf("comprehensive check\n");

	Table* t = (Table*)malloc(sizeof(Table)); // create dataspace for compacted table
	t->initialise_and_load(this);
	double* new_head = (double*)malloc(sizeof(double)*t->w);
	memcpy(new_head, head, sizeof(double)*t->w);
	
	for (int j=0; j<t->h; j++) // wipe specific right hand side entries
		if (anti_subset_mask->get_bit(table_pivot_columns[j])==1)
			t->set(t->w-1,j,0.0);
	for (int i=t->w-2;i>-1;i--) // cull anticoalition columns (TODO: inefficient)
		if (anti_subset_mask->get_bit(i)==1)
			t->delete_column(i,new_head);
	t->calculate_pivots();
	
	Mask_Memory* masks;
	masks = (Mask_Memory*)calloc(sizeof(Mask_Memory),1);
	masks->setup(MEMORY_INITIAL_SIZE);
	int max_int = maximising==true ? 1 : -1;
	double best_improvement;
	int best_improvement_index;
	
	//printhead(new_head,t->w);
	//t->print();
	//t->print_pivot_info();
	//t->print_pivotable_info();
	bool ret = t->simplex_improve(new_head, max_int, masks, &best_improvement, &best_improvement_index); // see if it is possible to simplex improve one step.

	masks->destroy(); // free data
	free(masks);
	t->free_data();
	free(t);
	free(new_head);

	return ret;
}



