

struct Table {
	double* data; // the table itself, including the head,
	int w,h;
	
	int* table_pivot_columns; //the columns which are pivot columns, defines the table, ordered by the row the 1.0 occurs on.
	unsigned long table_pivot_column_mask; //the binary mask of the pivot columns
	int table_pivot_column_number; // the number of columns that are established pivot columns of the table. 
	
	unsigned char* pivotable_columns; //the columns of potential pivot points, not ordered and not nessisarily unique
	unsigned char* pivotable_rows; // the respective rows of potential pivot points, not ordered and not nessisarily unique
	double* pivotable_ratios; // the per-unit improvement of pivoting from the respective pivot point
	unsigned long pivotable_columns_mask; // the binary mask of pivotable columns
	int pivotable_number; // the number potential pivot points.
	
	void initialise(int w, int h);
	void initialise_and_wipe(int w, int h);
	void initialise_and_load(Table* t);
	void reset_pivot_information();
	void free_data();
	void load(Table* t);
	Table();
	Table(int w, int h);
	Table(Table* t);
	~Table();
	
	inline double get(int c, int r);
	inline void set(int c, int r, double v);
	void delete_row(int r);
	void delete_column(int c);
	
	void apply_to_head(double* head, double* new_head);
	void calculate_pivots();
	void pivot(Table* t, int pivotable_index);
	bool check_subset_improvable(unsigned long subset_mask, unsigned long not_subset_mask, double* head, bool maximising);

	void print();
	void print_pivot_info();
	void print_pivotable_info();
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
	printf("TABLE pivot info: table_pivot_column_number: %i\n\t",this->table_pivot_column_number);
	printf("table_pivot_column_mask: ");
	printbin(this->table_pivot_column_mask);
	printf("\n\tpivot row columns: ");
	for (int i=0; i<this->h; i++)
		printf("%i ",this->table_pivot_columns[i]);
	printf("\n");
}

void Table::print_pivotable_info() {
	printf("TABLE pivotable info: pivotable_number: %i\n\t",this->pivotable_number);
	printf("table_pivotable_column_mask: ");
	printbin(this->pivotable_columns_mask);
	printf("\n\tpivots: ");
	for (int i=0; i<this->pivotable_number; i++)
		printf("(%i,%i,%f), ",this->pivotable_rows[i],this->pivotable_columns[i],this->pivotable_ratios[i]);
	printf("\n");
}


void Table::reset_pivot_information() {
	for (int i=0;i<h;i++) this->table_pivot_columns[i]=-1;
	this->table_pivot_column_mask = 0;
	this->pivotable_columns_mask = 0;
	this->pivotable_number = 0;
	this->table_pivot_column_number = 0;
}

void Table::initialise(int w, int h) {
	int wminusonetimesh = (w-1)*h;
	this->data = (double*)malloc(sizeof(double)*w*h);
	this->w = w;
	this->h = h;
	
	this->table_pivot_columns = (int*)malloc(sizeof(int)*h);
	this->pivotable_columns = (unsigned char*)malloc(sizeof(unsigned char)*wminusonetimesh);
	this->pivotable_rows = (unsigned char*)malloc(sizeof(unsigned char)*wminusonetimesh);
	this->pivotable_ratios = (double*)malloc(sizeof(double)*wminusonetimesh);

	this->reset_pivot_information();
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

	this->reset_pivot_information();
}
void Table::free_data() {
	free(this->data);
	free(this->table_pivot_columns);
	free(this->pivotable_columns);
	free(this->pivotable_rows);
	free(this->pivotable_ratios);
}
void Table::load(Table* t) {
	int wminusonetimesh = ((this->w)-1)*(this->h);
	this->table_pivot_column_mask = t->table_pivot_column_mask;
	this->pivotable_columns_mask = t->pivotable_columns_mask;
	this->table_pivot_column_number = t->table_pivot_column_number;
	this->pivotable_number = t->pivotable_number;
	memcpy ( this->data, t->data, sizeof(double)*(this->h)*(this->w) );
	memcpy ( this->table_pivot_columns, t->table_pivot_columns, sizeof(int)*(this->h) );
	memcpy ( this->pivotable_columns, t->pivotable_columns, sizeof(unsigned char)*wminusonetimesh );
	memcpy ( this->pivotable_rows, t->pivotable_rows, sizeof(unsigned char)*wminusonetimesh );
	memcpy ( this->pivotable_ratios, t->pivotable_ratios, sizeof(double)*wminusonetimesh );
}


void Table::initialise_and_load(Table* t) {
	this->initialise(t->w,t->h);
	this->load(t);
}

Table::Table() {}
Table::~Table() {
	this->free_data();
}
Table::Table(int w, int h) {
	this->initialise_and_wipe(w,h);
}
Table::Table(Table* t) {
	this->initialise_and_load(t);
}
inline double Table::get(int c, int r) {
	return (this->data)[c+r*this->w];
}
inline void Table::set(int c, int r, double v) {
	(this->data)[c+r*this->w] = v;
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
}

// calculate all the potential pivot columns and calculate the ratios by which the pivoting will improve the objective
void Table::calculate_pivots() {
	int wminusone = this->w-1;
	int h = this->h;
	this->pivotable_columns_mask = 0;
	this->pivotable_number = 0;
	for (unsigned char i=0;i<wminusone;i++) { // for each column
		if ((((unsigned long)1)<<i)&(this->table_pivot_column_mask)) // scip if it is already table pivot column
			continue;
		unsigned long best_mask=0;
		double best_ratio = DBL_MAX; // find if the column can be pivoted and detect the pivot row/s for the column
		for (int j=0;j<h;j++) {
			double v = this->get(i,j);
			if (v>0) {
				double ratio = (this->get(wminusone,j))/v;
				if (ratio<best_ratio) {
					best_ratio = ratio;
					best_mask = ((unsigned long)1)<<j;
				} else if (ratio==best_ratio) {
					best_mask |= ((unsigned long)1)<<j;
				}
			}
		}
		unsigned char z=0;
		while (best_mask != 0) {  // if there is a best row add the info to the datastructure
			if (1&best_mask) {
				this->pivotable_columns[this->pivotable_number] = i;
				this->pivotable_rows[this->pivotable_number] = z;
				this->pivotable_ratios[this->pivotable_number]=best_ratio;
				this->pivotable_columns_mask |= (((unsigned long)1)<<i);
				this->pivotable_number += 1;
			}
			best_mask = best_mask >> 1;
			z++;
		}
	}
}

// set this table to be as pivoted from another table by the indexed pivotable point
void Table::pivot(Table* t, int pivotable_index) {
	int row = t->pivotable_rows[pivotable_index];
	int column = t->pivotable_columns[pivotable_index];
	double* this_pivot_row = &(this->data)[row*this->w];
	double* pivot_row = &(t->data)[row*this->w];
	double center = pivot_row[column];
	for (int i=0;i<this->w;i++)
		this_pivot_row[i] = pivot_row[i] / center;
	for (int j=0;j<this->h;j++)
		if (j!=row) {
			double* this_scan_row = &(this->data)[j*this->w];
			double* scan_row = &(t->data)[j*this->w];
			double scan_row_column = scan_row[column];
			for (int i=0;i<this->w;i++)
				this_scan_row[i] = scan_row[i] - scan_row_column*this_pivot_row[i];
		}
	this->table_pivot_column_mask = t->table_pivot_column_mask ^ (((unsigned long)1)<<column);
	int old_pivot_column = t->table_pivot_columns[row];
	if (old_pivot_column != -1) {
		this->table_pivot_column_mask ^= ((unsigned long)1)<<old_pivot_column;
	} else {
		this->table_pivot_column_number = t->table_pivot_column_number + 1;
	}
	memcpy ( this->table_pivot_columns, t->table_pivot_columns, sizeof(int)*(this->h) );
	this->table_pivot_columns[row] = column;
	this->calculate_pivots();
}


// deletes row r, (not optimised function)
void Table::delete_row(int r) {
	if (this->table_pivot_columns[r] != -1) {
		this->table_pivot_column_number -= 1;
		this->table_pivot_column_mask ^= (((unsigned long)1)<<this->table_pivot_columns[r]);
	}
	for (int ii=r; ii<this->h-1;ii++) {
		this->table_pivot_columns[ii] = this->table_pivot_columns[ii+1];
	}
	for (int j=r;j<(this->h-1);j++)
		for (int i=0;i<(this->w);i++)
			this->set(i,j,this->get(i,j+1));
	this->h -= 1;
}

// deletes column c, (not optimised function), do not use on columns that are pivot columns, and recalculate pivotable information.
void Table::delete_column(int c) {
	for (int i=0; i<this->h; i++) {
		if (this->table_pivot_columns[i]==c) {
			this->table_pivot_column_number -= 1;
			this->table_pivot_columns[i] = -1;
		}
		if (this->table_pivot_columns[i]>c)
			this->table_pivot_columns[i] -= 1;
	}
	this->table_pivot_column_mask = this->table_pivot_column_mask^((((unsigned long)1)<<c)&this->table_pivot_column_mask);
	unsigned long z = ((this->table_pivot_column_mask>>c)<<c);
	this->table_pivot_column_mask = (this->table_pivot_column_mask ^ z) | (z>>1);
	for (int i=0; i<(this->w-1)*this->h; i++)
		this->data[i] = this->data[i+((i+this->w-c-1)/(this->w-1))];
	this->w -= 1;
}

// for an applied head, can the set of players given by the subset mask succeed in maximising the utility by themselves
bool Table::check_subset_improvable(unsigned long subset_mask, unsigned long not_subset_mask, double* head, bool maximising) {
	unsigned long column_mask = subset_mask & this->pivotable_columns_mask;
	for (int i=0;i<this->w-1;i++) {
		if (maximising==true) {
			if (head[i]>=0)
				continue;
		} else {
			if (head[i]<=0)
				continue;
		}
		unsigned long imask = ((unsigned long)1)<<i;
		if (imask & column_mask) {
			int j;
			bool viable = false;
			for (j=0;j<this->h;j++) {
				double v = get(i,j);
				if (v>0) {
					if ((((unsigned long)1)<<(this->table_pivot_columns[j])) & not_subset_mask) {
						break;
					}
					viable=true;
				}
			}
			if ((j==this->h) && viable) {
				return true;
			}
		}
	}
	return false;
}

