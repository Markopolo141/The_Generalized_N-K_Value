

struct Row_iter {
	double val;
	double right;
	int index;
};

struct Row_Memory {
	Row_iter* memory;
	int mem_size;
	int length;
	
	void add(double val,double right, int index);
	void clear();
	void destroy();
	void setup(size_t mem_size);
};

void Row_Memory::add(double val,double right, int index) {
	if (this->length == this->mem_size) {
		this->memory = (Row_iter*)realloc(this->memory, sizeof(Row_iter)*this->mem_size*2);
		if (this->memory==NULL)
			printf("ERROR: reallocating memory failure\n");
		this->mem_size *= 2;
	}
	this->memory[length].val = val;
	this->memory[length].right = right;
	this->memory[length].index = index;
	this->length += 1;
}
void Row_Memory::clear() {
	this->length=0;
}
void Row_Memory::destroy() {
	free(this->memory);
}
void Row_Memory::setup(size_t mem_size) {
	this->mem_size = mem_size;
	this->memory = (Row_iter*)malloc(sizeof(Row_iter)*mem_size);
	if (this->memory==NULL)
		printf("ERROR: allocating memory failure\n");
	this->clear();
}



