
struct Value_Memory {
	double* memory;
	int mem_size;
	int length;
	
	void add(double l);
	void clear();
	void destroy();
	void setup(size_t mem_size);
	double max();
};


void Value_Memory::add(double l) {
	if (this->length == this->mem_size) {
		this->memory = (double*)realloc(this->memory, sizeof(double)*this->mem_size*2);
		if (this->memory==NULL)
			printf("ERROR: reallocating memory failure\n");
		this->mem_size *= 2;
	}
	this->memory[length] = l;
	this->length += 1;
}
void Value_Memory::clear() {
	this->length = 0;
}
void Value_Memory::destroy() {
	free(this->memory);
}
void Value_Memory::setup(size_t mem_size) {
	this->mem_size = mem_size;
	this->memory = (double*)malloc(sizeof(double)*mem_size);
	if (this->memory==NULL)
		printf("ERROR: allocating memory failure\n");
}
double Value_Memory::max() {
	double max_val = DBL_MIN;
	for (int i=0; i<this->length; i++)
		if (this->memory[i]>max_val)
			max_val = this->memory[i];
	return max_val;
}
