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
	if (this->length == this->mem_size) {
		this->mem_size *= 2;
		this->memory = (Table**)realloc(this->memory, sizeof(Table*)*this->mem_size);
		if (this->memory==NULL)
			printf("ERROR: reallocating memory failure\n");
	}
	this->memory[length] = l;
	this->length += 1;
}
void Table_Memory::free_all() {
	for (unsigned long l=0; l <this->length; l++) {
		this->memory[l]->free_data();
		free(this->memory[l]);
	}
	this->length = 0;
}
void Table_Memory::destroy() {
	free(this->memory);
}
void Table_Memory::setup(size_t mem_size) {
	this->mem_size = mem_size;
	this->length = 0;
	this->memory = (Table**)malloc(sizeof(Table*)*mem_size);
	if (this->memory==NULL)
		printf("ERROR: allocating memory failure\n");
}
