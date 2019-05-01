
struct Mask_Memory {
	Mask* memory;
	unsigned long mem_size;
	unsigned long length;
	
	void add(Mask* l, bool searching);
	void add_all(Mask_Memory* m, bool searching);
	void duplicate(Mask_Memory* m, bool searching);
	bool search(Mask* datum);
	void clear();
	void destroy();
	void setup(size_t mem_size);

	void print_all();
};

void Mask_Memory::print_all() {
	printf("MEMORY_OUTPUT:\n");
	for (unsigned long l=0; l<this->length; l++) {
		printf("\t");
		this->memory[l].print();
	}
}

void Mask_Memory::add(Mask* l, bool searching) {
	if ((searching==false) || (this->search(l)==false)) {
		if (this->length == this->mem_size) {
			this->memory = (Mask*)realloc(this->memory, sizeof(Mask)*this->mem_size*2);
			if (this->memory==NULL)
				printf("ERROR: reallocating memory failure\n");
			this->mem_size *= 2;
		}
		this->memory[length].set(l);
		this->length += 1;
	}
}

void Mask_Memory::add_all(Mask_Memory* m, bool searching) {
	for (unsigned int i=0; i<m->length; i++) {
		this->add(&(m->memory[i]), searching);
	}
}

void Mask_Memory::duplicate(Mask_Memory* m, bool searching) {
	this->length=0;
	for (unsigned int i=0; i<m->length; i++) {
		this->add(&(m->memory[i]), searching);
	}
}

bool Mask_Memory::search(Mask* datum) {
	for (long l=this->length-1; l >-1; l--)
		if (this->memory[l]==(*datum))
			return true;
	return false;
}
void Mask_Memory::clear() {
	this->length=0;
}
void Mask_Memory::destroy() {
	free(this->memory);
}
void Mask_Memory::setup(size_t mem_size) {
	this->mem_size = mem_size;
	this->length = 0;
	this->memory = (Mask*)malloc(sizeof(Mask)*mem_size);
	if (this->memory==NULL)
		printf("ERROR: allocating memory failure\n");
}
