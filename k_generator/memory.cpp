
struct Memory {
	unsigned long* memory;
	unsigned long mem_size;
	unsigned long length;
	
	void add(unsigned long l);
	bool search(unsigned long datum);
	void clear();
	void destroy();
	void setup(size_t mem_size);

	void print_all();
};

void Memory::print_all() {
	printf("MEMORY_OUTPUT:\n");
	for (unsigned long l=0; l<this->length; l++) {
		printf("\t");
		printbin(this->memory[l]);
		printf("\n");
	}
}

void Memory::add(unsigned long l) {
	if (this->length == this->mem_size) {
		this->memory = (unsigned long*)realloc(this->memory, sizeof(unsigned long)*this->mem_size*2);
		if (this->memory==NULL)
			printf("ERROR: reallocating memory failure\n");
		this->mem_size *= 2;
	}
	this->memory[length] = l;
	this->length += 1;
}
bool Memory::search(unsigned long datum) {
	for (long l=this->length-1; l >-1; l--)
		if (this->memory[l]==datum)
			return true;
	return false;
}
void Memory::clear() {
	this->length=0;
}
void Memory::destroy() {
	free(this->memory);
}
void Memory::setup(size_t mem_size) {
	this->mem_size = mem_size;
	this->memory = (unsigned long*)malloc(sizeof(unsigned long)*mem_size);
	if (this->memory==NULL)
		printf("ERROR: allocating memory failure\n");
}
