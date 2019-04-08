struct Reference_Memory {
	void** memory;
	unsigned long mem_size;
	unsigned long length;
	
	void add(void* l);
	void free_all();
	void destroy();
	void setup(size_t mem_size);
};


void Reference_Memory::add(void* l) {
	if (this->length == this->mem_size) {
		this->memory = (void**)realloc(this->memory, sizeof(void*)*this->mem_size*2);
		if (this->memory==NULL)
			printf("ERROR: reallocating memory failure\n");
		this->mem_size *= 2;
	}
	this->memory[length] = l;
	this->length += 1;
}
void Reference_Memory::free_all() {
	for (unsigned long l=0; l <this->length; l++)
		free(this->memory[l]);
	this->length = 0;
}
void Reference_Memory::destroy() {
	free(this->memory);
}
void Reference_Memory::setup(size_t mem_size) {
	this->mem_size = mem_size;
	this->memory = (void**)malloc(sizeof(void*)*mem_size);
	this->length = 0;
	if (this->memory==NULL)
		printf("ERROR: allocating memory failure\n");
}
