void printbin(unsigned long a) {
	while (a != 0){
		printf("%li",a&1);
		a = a >>1;
	}
}
void printhead(double* head, int w) {
	printf("[");
	for (int i=0; i<w-1; i++) {
		printf("%f,\t",head[i]);
	}
	printf("%f]\n",head[w-1]);
}

/*#define double 1

class Mask {
	public:

	void print() {
		unsigned long l;
		l = A;
		for (int i=0; i<sizeof(unsigned long)*8; i++) {
			printf("%i",l&1);
			l >>= 1;
		}
		l = B;
		for (int i=0; i<sizeof(unsigned long)*8; i++) {
			printf("%i",l&1);
			l >>= 1;
		}
		l = C;
		for (int i=0; i<sizeof(unsigned long)*8; i++) {
			printf("%i",l&1);
			l >>= 1;
		}
	}

	bool operator==(Mask& m) {
		if ((m->A == this->A) && (m->B == this->B) && (m->C == this->C))
			return true;
		return false;
	}
	void operator=(unsigned long l) {
		this->A = l;
		this->B = 0;
		this->C = 0;
	}
	void operator=(Mask& m) {
		this->A = m->A;
		this->B = m->B;
		this->C = m->C;
	}
	void flip_bit(int bit) {
		if (bit>=sizeof(unsigned long)*8*2) {
			C ^= ((unsigned long)1)<<(bit-sizeof(unsigned long)*8*2);
		} else if (bit>=sizeof(unsigned long)*8) {
			B ^= ((unsigned long)1)<<(bit-sizeof(unsigned long)*8);
		} else {
			A ^= ((unsigned long)1)<<(bit);
		}
	}
	void remove_bit(int bit) {
		//TODO
	}

	private:
	unsigned long A;
	unsigned long B;
	unsigned long C;
};
*/
