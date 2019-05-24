

struct Table {
	double* data; // the table itself, including the head,
	int w,h;
	
	int* table_pivot_columns; //the columns which are pivot columns, defines the table, ordered by the row the 1.0 occurs on.
	Mask* table_pivot_column_mask; //the binary mask of the pivot columns
	int table_pivot_column_number; // the number of columns that are established pivot columns of the table. 
	
	unsigned int* pivotable_columns; //the columns of potential pivot points, not ordered and not nessisarily unique
	unsigned int* pivotable_rows; // the respective rows of potential pivot points, not ordered and not nessisarily unique
	double* pivotable_ratios; // the per-unit improvement of pivoting from the respective pivot point
	Mask* pivotable_columns_mask; // the binary mask of pivotable columns
	int pivotable_number; // the number potential pivot points.
	bool need_to_recalculate_pivotable; // a flag that is raised to indicate whether the calculate_pivots needs tobe called
	
	void initialise(int w, int h);
	void reset_pivot_information();
	void free_data();
	void load(Table* t);
	
	inline double get(int c, int r);
	inline void set(int c, int r, double v);
	void delete_row(int r);
	void delete_column(int c, double* head);
	
	void apply_to_head(double* head, double* new_head); // for a given head, apply the pivot rows to the head to make it consistent with the table
	void calculate_pivots(bool negatives); // scan through the table and select all the viable pivot points
	void pivot(Table* t, int pivotable_index); // given table t (which can be -this-) with its pivotable index, mutate this table to be the pivoted result
	void pivot(Table* t, int column, int row); // given table t (which can be -this-) mutate this table to be pivoted by the row,column pivot point.
	double simplex(double* head, bool maximising, Mask* column_exclusions); // conduct a full simplex to the table, while avoiding pivoting on column_exclusion columns (can be NULL)
	double simplex(double* head, bool maximising); // conduct a full simplex to the table.

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
	int wminusonetimesh = (w-1)*h;
	this->data = (double*)calloc(sizeof(double),w*h);
	this->w = w;
	this->h = h;
	
	this->table_pivot_columns = (int*)malloc(sizeof(int)*h);
	this->pivotable_columns = (unsigned int*)calloc(sizeof(unsigned int),wminusonetimesh);
	this->pivotable_rows = (unsigned int*)calloc(sizeof(unsigned int),wminusonetimesh);
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
	memcpy ( this->pivotable_columns, t->pivotable_columns, sizeof(unsigned int)*wminusonetimesh );
	memcpy ( this->pivotable_rows, t->pivotable_rows, sizeof(unsigned int)*wminusonetimesh );
	memcpy ( this->pivotable_ratios, t->pivotable_ratios, sizeof(double)*wminusonetimesh );
	this->need_to_recalculate_pivotable = t->need_to_recalculate_pivotable;
}

inline double Table::get(int c, int r) {
	return (this->data)[c+r*this->w];
}
inline void Table::set(int c, int r, double v) {
	(this->data)[c+r*this->w] = v;
	need_to_recalculate_pivotable = true;
}

double Table::simplex(double* head, bool maximising, Mask* column_exclusions) {
	//if (this->table_pivot_column_number != this->h)
	//	printf("WARNING: simplex method on potentially badly formed table.\n");

	int max_int = maximising==true ? 1 : -1;
	this->calculate_pivots(false);
	this->apply_to_head(head,head);
	Mask_Memory* masks;
	masks = (Mask_Memory*)calloc(sizeof(Mask_Memory),1);
	masks->setup(MEMORY_INITIAL_SIZE);
	double best_improvement;
	int best_improvement_index;
	Mask new_mask;

	masks->add(this->table_pivot_column_mask, false);
	while (true) {
		best_improvement=0;
		best_improvement_index = -1;
		int smallest_out_column = -1;
		for (int pivot_index=0; pivot_index < this->pivotable_number; pivot_index++) {
			int pivotable_column = this->pivotable_columns[pivot_index];
			if (
				(column_exclusions == NULL) || 
				((column_exclusions->get_bit(pivotable_column)==0) && 
					(column_exclusions->get_bit(this->table_pivot_columns[this->pivotable_rows[pivot_index]])==0))
			) {
				double head_value = head[pivotable_column];
				if ((head_value*max_int < 0)) {
					double ratio = -max_int*head_value*this->pivotable_ratios[pivot_index];
					if (ratio>best_improvement) {
						best_improvement = ratio;
						best_improvement_index = pivot_index;
					} else if ((ratio==0) && (best_improvement==0)) { //bland's rule
						//if (smallest_out_column == -1) {
						//	smallest_out_column = this->table_pivot_columns[pivotable_rows[pivot_index]];
							best_improvement_index = pivot_index;
						//} else {
						//	if (this->table_pivot_columns[pivotable_rows[pivot_index]] < smallest_out_column) {
						//		smallest_out_column = this->table_pivot_columns[pivotable_rows[pivot_index]];
						//		best_improvement_index = pivot_index;
						//	}
						//}
					}
				}
			}
		}
		if (best_improvement_index==-1) // destinct optima attained
			break;
		new_mask.set(this->table_pivot_column_mask);
		new_mask.flip_bit(this->pivotable_columns[best_improvement_index]);
		if (this->table_pivot_columns[best_improvement_index] != -1)
			new_mask.flip_bit(this->table_pivot_columns[this->pivotable_rows[best_improvement_index]]);
		if (masks->search(&new_mask)==true) // if degenerate cycling is occuring in context of bland's rule
			break;
		this->pivot(this, best_improvement_index);
		this->calculate_pivots(false);
		this->apply_to_head(head,head);
		masks->add(this->table_pivot_column_mask, false);
	}
	masks->destroy();
	free(masks);
	return head[this->w-1];
}



double Table::simplex(double* head, bool maximising) {
	//if (this->table_pivot_column_number != this->h)
	//	printf("WARNING: simplex method on potentially badly formed table.\n");

	int max_int = maximising==true ? 1 : -1;
	this->calculate_pivots(false);
	this->apply_to_head(head,head);
	Mask_Memory* masks;
	masks = (Mask_Memory*)calloc(sizeof(Mask_Memory),1);
	masks->setup(MEMORY_INITIAL_SIZE);
	double best_improvement;
	int best_improvement_index;
	Mask new_mask;

	masks->add(this->table_pivot_column_mask, false);
	while (true) {
		best_improvement=0;
		best_improvement_index = -1;
		int smallest_out_column = -1;
		for (int pivot_index=0; pivot_index < this->pivotable_number; pivot_index++) {
			double head_value = head[this->pivotable_columns[pivot_index]];
			if ((head_value*max_int < 0)) {
				double ratio = -max_int*head_value*this->pivotable_ratios[pivot_index];
				if (ratio>best_improvement) {
					best_improvement = ratio;
					best_improvement_index = pivot_index;
				} else if ((ratio==0) && (best_improvement==0)) { //bland's rule
					//if (smallest_out_column == -1) {
					//	smallest_out_column = this->table_pivot_columns[pivotable_rows[pivot_index]];
						best_improvement_index = pivot_index;
					//} else {
					//	if (this->table_pivot_columns[pivotable_rows[pivot_index]] < smallest_out_column) {
					//		smallest_out_column = this->table_pivot_columns[pivotable_rows[pivot_index]];
					//		best_improvement_index = pivot_index;
					//	}
					//}
				}
			}
		}
		if (best_improvement_index==-1) // destinct optima attained
			break;
		new_mask.set(this->table_pivot_column_mask);
		new_mask.flip_bit(this->pivotable_columns[best_improvement_index]);
		if (this->table_pivot_columns[best_improvement_index] != -1)
			new_mask.flip_bit(this->table_pivot_columns[this->pivotable_rows[best_improvement_index]]);
		if (masks->search(&new_mask)==true) // if degenerate cycling is occuring in context of bland's rule
			break;
		this->pivot(this, best_improvement_index);
		this->calculate_pivots(false);
		this->apply_to_head(head,head);
		masks->add(this->table_pivot_column_mask, false);
	}
	masks->destroy();
	free(masks);
	return head[this->w-1];
}




// given a head, apply all pivot column information to it
void Table::apply_to_head(double* head, double* new_head) {
	int h = this->h;
	int w = this->w;
	bool first_set = true;
	for (int j=0;j<h;j++) {
		int pivot_column = this->table_pivot_columns[j];
		if (pivot_column != -1) {
			double head_pivot_column = head[pivot_column];
			if (first_set==true) {
				for (int i=0;i<w;i++)
					new_head[i] = head[i] - head_pivot_column*(this->get(i,j));
				first_set=false;
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
void Table::calculate_pivots(bool negatives) {
	int wminusone = this->w-1;
	int h = this->h;
	this->pivotable_columns_mask->set_zero();
	this->pivotable_number = 0;
	Row_Memory* row_memory;
	row_memory = (Row_Memory*)calloc(sizeof(Row_Memory),1);
	row_memory->setup(MEMORY_INITIAL_SIZE);
	for (int i=0;i<wminusone;i++) { // for each column
		if (this->table_pivot_column_mask->get_bit(i)==1) // scip if it is already table pivot column
			continue;
		row_memory->clear();
		double best_ratio = DBL_MAX; // find if the column can be pivoted and detect the pivot row/s for the column
		for (int j=0;j<h;j++) {
			double v = this->get(i,j);
			double right_value = this->get(wminusone,j);
			if (v>0) {
				row_memory->add(v,right_value,j);
				double ratio = right_value/v;
				if (ratio<best_ratio)
					best_ratio = ratio;
			}
			// the following block allows -1,0 pivoting... *carefull*
			/*else if ((v<0) && (right_value==0) && (negatives==true)) {
				this->pivotable_columns[this->pivotable_number] = i;
				this->pivotable_rows[this->pivotable_number] = j;
				this->pivotable_ratios[this->pivotable_number]=0.0;
				this->pivotable_columns_mask->set_bit(i,1);
				this->pivotable_number += 1;
			}*/
		}
		for (unsigned int z=0; z<row_memory->length; z++) {// if there is a best row add the info to the datastructure
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
/*void Table::calculate_pivots(bool negatives) {
	this->pivotable_columns_mask->set_zero();
	this->pivotable_number = 0;
	for (int i=0;i<this->w-1;i++) { // for each column
		if (this->table_pivot_column_mask->get_bit(i)==1) // scip if it is already table pivot column
			continue;
		double best_ratio = DBL_MAX;
		double best_index = -1;
		for (int j=0;j<this->h;j++) {
			double v = this->get(i,j);
			double right_value = this->get(this->w-1,j);
			if (v>0) {
				double ratio = right_value/v;
				if (ratio<best_ratio) {
					best_ratio = ratio;
					best_index = j;
				}
			}
		}
		if (best_index != -1) {
			this->pivotable_columns[this->pivotable_number] = i;
			this->pivotable_rows[this->pivotable_number] = best_index;
			this->pivotable_ratios[this->pivotable_number]=best_ratio;
			this->pivotable_columns_mask->set_bit(i,1);
			this->pivotable_number += 1;
		}
	}
	this->need_to_recalculate_pivotable = false;
}*/



// set this table to be as pivoted from another table by the indexed pivotable point
void Table::pivot(Table* t, int pivotable_index) {
	if (t->need_to_recalculate_pivotable==true)
		printf("WARNING: pivoting from outdated table pivot info\n");
	this->pivot(t,t->pivotable_columns[pivotable_index],t->pivotable_rows[pivotable_index]);
}

void Table::pivot(Table* t, int column, int row) {
	//printf("pivoting: column %i row %i\n",column,row);
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

