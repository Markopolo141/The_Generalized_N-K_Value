

/*void simplex_regularlise(Table* t) {
	printf("SIMPLEX_REGULARLISE\n");
	t->print();
	t->calculate_pivots();
	int minhwminusone = t->h;
	if (t->w-1<minhwminusone)
		minhwminusone = t->w-1;
	int ii = 0;
	bool found_pivot;
	while((t->table_pivot_column_number < minhwminusone)&&(ii<50)) {
		found_pivot=false;
		for (int i=0; i < t->pivotable_number; i++) {
			if (((~(t->table_pivot_column_mask)) & (1<<(t->pivotable_columns[i])) ) && (t->table_pivot_columns[t->pivotable_rows[i]]==-1)) {
				t->pivot(i);
				found_pivot=true;
				break;
			}
		}
		if (found_pivot==false) {
			printf("ERROR: Simplex_regularlise cannot find novel pivot\n");
			break;
		}
		ii++;
	}
	printbin(t->table_pivot_column_mask);
	t->print();
}*/




/*// pivot the table and head by the indexed pivotable column (not optimised function)
void Table::pivot(int pivotable_index, double* head) {
	this->pivot(pivotable_index);
	int column = this->pivotable_columns[pivotable_index];
	double* pivot_row = &(this->data)[this->pivotable_rows[pivotable_index]*this->w];
	double head_column = head[column];
	for (int i=0;i<this->w;i++)
		head[i] -= head_column*pivot_row[i];
}*/



struct MemoryLinked {
	MemoryLinked *next;
	unsigned long data;
};

struct MemoryList {
	MemoryLinked *root;
	
	void add(MemoryLinked *l);
	void* pop();
	void destroy();
	bool search(unsigned long datum);

	MemoryList();
	~MemoryList();
};

MemoryList::MemoryList() {
	this->root = NULL;
}
MemoryList::~MemoryList() {
	this->destroy();
}

void MemoryList::destroy() {
	MemoryLinked* current;
	MemoryLinked* next;
	current = this->root;
	while(current != NULL) {
		next = current->next;
		free(current);
		current = next;
	}
	this->root = NULL;
}
void MemoryList::add(MemoryLinked *l) {
	MemoryLinked* current;
	current = this->root;
	if (current==NULL) {
		l->next = NULL;
		this->root = l;
		return;
	}
	this->root = l;
	l->next = current;
	return;
}
void* MemoryList::pop() {
	if (this->root == NULL) {
		return NULL;
	}
	MemoryLinked* l;
	l = this->root;
	this->root = l->next;
	return l;
}
bool MemoryList::search(unsigned long datum) {
	MemoryLinked* current;
	current = this->root;
	while (current!= NULL)
		if (current->data == datum)
			return true;
	return false;
}



void set_head(PyObject *input_head, int table_width, unsigned long deleted_columns) {
	for (int i=0; i<table_width; i++)
		master_head[i]=0;
	int length = (int)PyList_Size(input_head);
	for (int i=0; i<table_width; i++)
		master_head[i]=0;
	int master_index=0;
	int array_index=0;
	while ((master_index<table_width) && (array_index<length)) {
		if (1&deleted_columns) {
			array_index += 1;
		} else {
			master_head[master_index] = PyFloat_AsDouble(PyList_GetItem(input_head, array_index));
			master_index += 1;
			array_index += 1;
		}
		deleted_columns >>= 1;
	}
}



// given a head, apply all pivot column information to it
void Table::apply_to_head(double* head) {
	int h = this->h;
	int w = this->w;
	for (int j=0;j<h;j++) {
		int pivot_column = this->table_pivot_columns[j];
		if (pivot_column != -1) {
			double head_pivot_column = head[pivot_column];
			for (int i=0;i<w;i++) {
				head[i] -= head_pivot_column*(this->get(i,j));
			}
		}
	}
}


// set this table to be pivoted from another table by the indexed pivotable column, transforming a head
void Table::pivot_external(Table* t, int pivotable_index, double* head) {
	int row = t->pivotable_rows[pivotable_index];
	int column = t->pivotable_columns[pivotable_index];
	double* this_pivot_row = &(this->data)[row*this->w];
	double* pivot_row = &(t->data)[row*this->w];
	double center = pivot_row[column];
	for (int i=0;i<this->w;i++) {
		this_pivot_row[i] = pivot_row[i] / center;
		head[i] -= head[column]*pivot_row[i];
	}
	for (int j=0;j<this->h;j++)
		if (j!=row) {
			double* this_scan_row = &(this->data)[j*this->w];
			double* scan_row = &(this->data)[j*this->w];
			for (int i=0;i<this->w;i++)
				this_scan_row[i] = scan_row[i] - scan_row[column]*pivot_row[i];
		}
	this->table_pivot_column_mask = t->table_pivot_column_mask ^ (1<<column);
	int old_pivot_column = t->table_pivot_columns[row];
	if (old_pivot_column != -1) {
		this->table_pivot_column_mask ^= 1<<old_pivot_column;
	} else {
		this->table_pivot_column_number = t->table_pivot_column_number + 1;
	}
	memcpy ( this->table_pivot_columns, t->table_pivot_columns, sizeof(int)*(this->h) );
	this->table_pivot_columns[row] = column;
	this->calculate_pivots();
}



inline Table* TableStructure::pivot(unsigned long mask, int pivot_index, double* head) {
	return this->pivot(&(this->tables[mask]),pivot_index,head);
}
inline Table* TableStructure::pivot(unsigned long mask, int pivot_index) {
	return this->pivot(&(this->tables[mask]),pivot_index);
}



Table* TableStructure::pivot(Table* t, int pivot_index, double* head) {
	unsigned long new_mask = t->table_pivot_column_mask ^ ((1<<(t->pivotable_column[pivot_index]))|(1<<(t->table_pivot_columns[t->pivotable_rows[pivot_index]])));
	Table* new_t = &(this->tables[new_mask]);
	if ((*new_t).w==0) {
		new_t->initialise_and_load(t);
		new_t->pivot_external(t,pivot_index,head);
	} else {
		new_t->apply_to_head(head);
	}
	return new_t;
}






// pivot the table by the indexed pivotable column
void Table::pivot(int pivotable_index) {
	int row = this->pivotable_rows[pivotable_index];
	int column = this->pivotable_columns[pivotable_index];
	double* pivot_row = &(this->data[row*this->w]);
	double center = pivot_row[column];
	for (int i=0;i<this->w;i++)
		pivot_row[i] /= center;
	for (int j=0;j<this->h;j++)
		if (j!=row) {
			double* scan_row = &(this->data[j*this->w]);
			double scan_row_column = scan_row[column];
			for (int i=0;i<this->w;i++)
				scan_row[i] -= scan_row_column*pivot_row[i];
		}
	this->table_pivot_column_mask ^= (1<<column);
	int old_pivot_column = this->table_pivot_columns[row];
	if (old_pivot_column != -1) {
		this->table_pivot_column_mask ^= 1<<old_pivot_column;
	} else {
		this->table_pivot_column_number++;
	}
	this->table_pivot_columns[row] = column;
	this->calculate_pivots();
}

// pivot the table and head by the indexed pivotable column
void Table::pivot(int pivotable_index, double* head, double* new_head) {
	int row = this->pivotable_rows[pivotable_index];
	int column = this->pivotable_columns[pivotable_index];
	double* pivot_row = &(this->data[row*this->w]);
	double center = pivot_row[column];
	double head_column = head[column];
	for (int i=0;i<this->w;i++) {
		pivot_row[i] /= center;
		new_head[i] = head[i] - head_column*pivot_row[i];
	}
	for (int j=0;j<this->h;j++)
		if (j!=row) {
			double* scan_row = &(this->data[j*this->w]);
			double scan_row_column = scan_row[column];
			for (int i=0;i<this->w;i++)
				scan_row[i] -= scan_row_column*pivot_row[i];
		}
	this->table_pivot_column_mask ^= (1<<column);
	int old_pivot_column = this->table_pivot_columns[row];
	if (old_pivot_column != -1) {
		this->table_pivot_column_mask ^= 1<<old_pivot_column;
	} else {
		this->table_pivot_column_number++;
	}
	this->table_pivot_columns[row] = column;
	this->calculate_pivots();
}




// set this table to be pivoted from another table by the indexed pivotable column, transforming head into a new head
void Table::pivot(Table* t, int pivotable_index, double* head, double* new_head) {
	int row = t->pivotable_rows[pivotable_index];
	int column = t->pivotable_columns[pivotable_index];
	double* this_pivot_row = &(this->data)[row*this->w];
	double* pivot_row = &(t->data)[row*this->w];
	double center = pivot_row[column];
	for (int i=0;i<this->w;i++) {
		this_pivot_row[i] = pivot_row[i] / center;
		new_head[i] = head[i] - head[column]*pivot_row[i];
	}
	for (int j=0;j<this->h;j++)
		if (j!=row) {
			double* this_scan_row = &(this->data)[j*this->w];
			double* scan_row = &(this->data)[j*this->w];
			for (int i=0;i<this->w;i++)
				this_scan_row[i] = scan_row[i] - scan_row[column]*pivot_row[i];
		}
	this->table_pivot_column_mask = t->table_pivot_column_mask ^ (1<<column);
	int old_pivot_column = t->table_pivot_columns[row];
	if (old_pivot_column != -1) {
		this->table_pivot_column_mask ^= 1<<old_pivot_column;
	} else {
		this->table_pivot_column_number = t->table_pivot_column_number + 1;
	}
	memcpy ( this->table_pivot_columns, t->table_pivot_columns, sizeof(int)*(this->h) );
	this->table_pivot_columns[row] = column;
	this->calculate_pivots();
}


Table* TableStructure::pivot(Table* t, int pivot_index, double* head, double* new_head) {
	unsigned long new_mask = t->table_pivot_column_mask ^ ((1<<(t->pivotable_columns[pivot_index]))|(1<<(t->table_pivot_columns[t->pivotable_rows[pivot_index]])));
	Table* new_t = &(this->tables[new_mask]);
	if ((*new_t).w==0) {
		new_t->initialise_and_load(t);
		new_t->pivot(t,pivot_index,head,new_head);
	} else {
		new_t->apply_to_head(head, new_head);
	}
	return new_t;
}


void apply_coalition(unsigned long coalition) {
	for (int i=0; i< players; i++) {
		if (coalition&1) {
			temporary_head[i] = -master_head[i];
		} else {
			temporary_head[i] = master_head[i];
		}
		coalition >>= 1;
	}
}

int analyse_coalition(unsigned long *coalition, unsigned long *anticoalition, unsigned long *mask, int* inverting) {
	*mask = ((1<<players)-1);
	if ((*coalition) - ((*mask)&(*coalition)) != 0)
		return -1;
	long coalition2 = *coalition;
	int member_no = 0;
	while (coalition2 > 0) {
		member_no += coalition2&1;
		coalition2 >>= 1;
	}
	printf("member number %i %i\n",member_no,players>>1);
	if (member_no <= (players>>1)) {
		*inverting = 1;
		*anticoalition = *coalition;
		*coalition = (*mask)^(*coalition);
	} else {
		*inverting = -1;
		*anticoalition = (*mask)^(*coalition);
	}
	return 0;
}

int analyse_coalition(unsigned long *coalition, unsigned long *anticoalition, unsigned long *mask) {
	*mask = ((((unsigned long)1)<<players)-1);
	if ((*coalition) - ((*mask)&(*coalition)) != 0)
		return -1;
	*anticoalition = (*mask)^(*coalition);
	return 0;
}



struct Table_Memory {
	Table** memory;
	unsigned long mem_size;
	unsigned long length;
	
	void add(Table* l);
	void free_all();
	void destroy();
	void setup(size_t mem_size);
};


void Table_Memory::add(Table* l) {
	printf("allocated table at %li, %li, %li\n",(unsigned long)l, this->length, this->mem_size);
	if (this->length == this->mem_size) {
		this->mem_size *= 2;
		this->memory = (Table**)realloc(this->memory, sizeof(Table*)*this->mem_size);
	}
	this->memory[length] = l;
	this->length += 1;
}
void Table_Memory::free_all() {
	for (unsigned long l=0; l <this->length; l++) {
		this->memory[l]->free_data();
		printf("deallocating table at %li\n",(unsigned long)(this->memory[l]));
		free(this->memory[l]);
	}
}
void Table_Memory::destroy() {
	free(this->memory);
}
void Table_Memory::setup(size_t mem_size) {
	this->mem_size = mem_size;
	this->length = 0;
	this->memory = (Table**)malloc(sizeof(Table*)*mem_size);
}


/*double walk_back(Table* t, unsigned long coalition, unsigned long anticoalition, double* head, TableStructure* ts) {
	if (debug==1) {
		printf("WALKBACK: coalition: ");
		printbin(coalition);
		printf("   anticoalition: ");
		printbin(anticoalition);
		printf("\n");
		printhead(head, t->w);
		t->print();
	}
	double* original_head = head;
	
	Memory* masks = (Memory*)calloc(sizeof(Memory),1);
	Reference_Memory* refs = (Reference_Memory*)calloc(sizeof(Reference_Memory),1);
	SortedList* pivot_list = (SortedList*)calloc(sizeof(SortedList),1);

	WalkBackLinked* link;
	
	masks->setup(MEMORY_INITIAL_SIZE);
	refs->setup(MEMORY_INITIAL_SIZE);
	masks->add(t->table_pivot_column_mask);
	
	while (t->check_subset_improvable(anticoalition, coalition, head, false)) {
		if (debug==1)
			printf("ITERATING\n");
		for (int i=0; i < t->pivotable_number; i++) {
			unsigned long new_mask = t->table_pivot_column_mask ^ ((1<<(t->pivotable_columns[i]))|(1<<(t->table_pivot_columns[t->pivotable_rows[i]])));
			if (masks->search(new_mask)==false) {
				link = (WalkBackLinked*)malloc(sizeof(WalkBackLinked));
				link->t = t;
				link->head = head;
				link->pivot_index = i;
				link->v = -(head[t->w-1] - t->pivotable_ratios[i]*head[t->pivotable_columns[i]]);
				pivot_list->add(link);
				masks->add(new_mask);
			}
		}
		if (debug==1) {
			masks->print_all();
			pivot_list->print_all();
		}
		
		link = (WalkBackLinked*)(pivot_list->pop());
		head = (double*)malloc(sizeof(double)*(link->t->w));
		refs->add(head);
		t = ts->pivot(link->t,link->pivot_index);
		t->apply_to_head(link->head,head);
		free(link);
	}
	if (debug==1) {
		printf("finished walkback\n");
		printhead(head, t->w);
		t->print();
	}
	
	memcpy(original_head,head,sizeof(double)*(t->w));
	refs->free_all();
	refs->destroy();
	masks->destroy();
	pivot_list->destroy();
	free(refs);
	free(masks);
	free(pivot_list);
	return original_head[t->w-1];
}*/



struct TableStructure {
	Table* tables;
	int size;
	Reference_Memory* mem;

	void init(int size);
	void destroy();

	inline Table* getTable(unsigned long mask);
	Table* setTable(Table* t);
	Table* pivot(Table* t, int pivot_index, double* head, double* new_head);
	Table* pivot(Table* t, int pivot_index);
};

void TableStructure::init(int size) {
	#if DEBUG==1
		printf("TableStructure initialise with size %li bytes\n",(((unsigned long)1)<<size)*sizeof(Table));
	#endif
	this->size = size;
	this->tables = (Table*)calloc(sizeof(Table),((unsigned long)1)<<size);
	if (this->tables==NULL)
		printf("ERROR: allocating memory failure\n");
	this->mem = (Reference_Memory*)malloc(sizeof(Reference_Memory));
	this->mem->setup(MEMORY_INITIAL_SIZE);
}
void TableStructure::destroy() {
	for (unsigned long l=0; l < this->mem->length; l++)
		((Table*)(this->mem->memory[l]))->free_data();
	free(this->tables);
}

inline Table* TableStructure::getTable(unsigned long mask) {
	return &(this->tables[mask]);
}
Table* TableStructure::setTable(Table* t) {
	t->calculate_pivots();
	Table* new_t = &(this->tables[t->table_pivot_column_mask]);
	new_t->initialise_and_load(t);
	return new_t;
}

Table* TableStructure::pivot(Table* t, int pivot_index) {
	unsigned long new_mask = t->table_pivot_column_mask ^ ((((unsigned long)1)<<(t->pivotable_columns[pivot_index]))|(((unsigned long)1)<<(t->table_pivot_columns[t->pivotable_rows[pivot_index]])));
	Table* new_t = &(this->tables[new_mask]);
	if ((*new_t).w==0) {
		new_t->initialise_and_load(t);
		this->mem->add(new_t);
		new_t->pivot(t,pivot_index);
	}
	return new_t;
}




