struct Table_Pivot {
	Table* t;
	double* head;
};

struct Table_Pivot_Memory {
	Table_Pivot** memory;
	unsigned long mem_size;
	unsigned long length;
	
	void add(Table_Pivot* l);
	void free_all();
	void destroy();
	void setup(size_t mem_size);
};


void Table_Pivot_Memory::add(Table_Pivot* l) {
	if (this->length == this->mem_size) {
		this->mem_size *= 2;
		this->memory = (Table_Pivot**)realloc(this->memory, sizeof(Table_Pivot*)*this->mem_size);
		if (this->memory==NULL)
			printf("ERROR: reallocating memory failure\n");
	}
	this->memory[length] = l;
	this->length += 1;
}
void Table_Pivot_Memory::free_all() {
	for (unsigned long l=0; l <this->length; l++)
		free(this->memory[l]);
	this->length = 0;
}
void Table_Pivot_Memory::destroy() {
	free(this->memory);
}
void Table_Pivot_Memory::setup(size_t mem_size) {
	this->mem_size = mem_size;
	this->length = 0;
	this->memory = (Table_Pivot**)malloc(sizeof(Table_Pivot*)*mem_size);
	if (this->memory==NULL)
		printf("ERROR: allocating memory failure\n");
}
