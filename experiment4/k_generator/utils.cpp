

inline double SNAPTOZERO(double a) {return a < TINY ? (-a < TINY ? 0 : a) : a;}

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

int max_3(int a, int b, int c){
	if (a>b) {
		if (a>c) {
			return a;
		} else {
			return c;
		}
	} else {
		if (b>c) {
			return b;
		} else {
			return c;
		}
	}
}
inline int count_the_bits(unsigned long A) {
	unsigned long B;
	B = A;
	int i=0;
	while (B!=0) {
		i += B&1;
		B >>= 1;
	}
	return i;
}

#define DOUBLE 1
#define TRIPPLE 0

class Mask {
	public:
	unsigned long A;
	#if DOUBLE==1
		unsigned long B;
		#if TRIPPLE==1
			unsigned long C;
			static const unsigned long length = sizeof(unsigned long)*8*3;
		#else
			static const unsigned long length = sizeof(unsigned long)*8*2;
		#endif
	#else
		static const unsigned long length = sizeof(unsigned long)*8;
	#endif

	void print() {
		for (unsigned long i=0; i<this->length; i++) {
			printf("%i",this->get_bit(i));
		}
		printf("\n");
	}

	void print_small() {
		long a = this->A;
		while (a!=0) {
			printf("%i",(int)(a&1));
			a >>= 1;
		}
	}

	inline bool operator==(const Mask& m) {
		#if DOUBLE==1
			#if TRIPPLE==1
				if ((m.A == this->A) && (m.B == this->B) && (m.C == this->C))
					return true;
			#else
				if ((m.A == this->A) && (m.B == this->B))
					return true;
			#endif
		#else
			if (m.A == this->A)
				return true;
		#endif
		return false;
	}
	inline Mask operator&(const Mask& m) {
		Mask m2;
		m2.A = (this->A)&(m.A);
		#if DOUBLE==1
			m2.B = (this->B)&(m.B);
			#if TRIPPLE==1
				m2.C = (this->C)&(m.C);
			#endif
		#endif
		return m2;
	}
	inline Mask operator~() {
		Mask m2;
		m2.A = ~(this->A);
		#if DOUBLE==1
			m2.B = ~(this->B);
			#if TRIPPLE==1
				m2.C = ~(this->C);
			#endif
		#endif
		return m2;
	}
	inline Mask operator^(const Mask& m) {
		Mask m2;
		m2.A = (this->A)^(m.A);
		#if DOUBLE==1
			m2.B = (this->B)^(m.B);
			#if TRIPPLE==1
				m2.C = (this->C)^(m.C);
			#endif
		#endif
		return m2;
	}
	inline Mask operator|(const Mask& m) {
		Mask m2;
		m2.A = (this->A)|(m.A);
		#if DOUBLE==1
			m2.B = (this->B)|(m.B);
			#if TRIPPLE==1
				m2.C = (this->C)|(m.C);
			#endif
		#endif
		return m2;
	}
	inline void set_zero() {
		this->A = 0;
		#if DOUBLE==1
			this->B = 0;
			#if TRIPPLE==1
				this->C = 0;
			#endif
		#endif
	}
	inline void operator=(Mask* m) {
		this->A = m->A;
		#if DOUBLE==1
			this->B = m->B;
			#if TRIPPLE==1
				this->C = m->C;
			#endif
		#endif
	}
	inline void set(Mask* m) {
		this->A = m->A;
		#if DOUBLE==1
			this->B = m->B;
			#if TRIPPLE==1
				this->C = m->C;
			#endif
		#endif
	}
	inline void set(Mask m) {
		this->A = m.A;
		#if DOUBLE==1
			this->B = m.B;
			#if TRIPPLE==1
				this->C = m.C;
			#endif
		#endif
	}
	inline void flip_bit(unsigned int bit) {
		#if DOUBLE==1
		#if TRIPPLE==1
		if (bit>=sizeof(unsigned long)*8*2) {
			this->C ^= ((unsigned long)1)<<(bit-sizeof(unsigned long)*8*2);
		} else 
		#endif
		if (bit>=sizeof(unsigned long)*8) {
			this->B ^= ((unsigned long)1)<<(bit-sizeof(unsigned long)*8);
		} else {
		#endif
			this->A ^= ((unsigned long)1)<<(bit);
		#if DOUBLE==1
		}
		#endif
	}
	int count_bits() {
		int a = count_the_bits(this->A);
		#if DOUBLE==1
		#if TRIPPLE==1
		a += count_the_bits(this->C);
		#endif
		a += count_the_bits(this->B);
		#endif
		return a;
	}
	inline unsigned int get_bit(unsigned int bit) {
		#if DOUBLE==1
		#if TRIPPLE==1
		if (bit>=sizeof(unsigned long)*8*2) {
			return (C & ((unsigned long)1)<<(bit-sizeof(unsigned long)*8*2))!=0;
		} else 
		#endif
		if (bit>=sizeof(unsigned long)*8) {
			return (B & ((unsigned long)1)<<(bit-sizeof(unsigned long)*8))!=0;
		} else {
		#endif
			return (A & ((unsigned long)1)<<(bit))!=0;
		#if DOUBLE==1
		}
		#endif
	}
	
	inline void set_bit(unsigned int bit, unsigned int value) {
		if (value==1) {
			#if DOUBLE==1
			#if TRIPPLE==1
			if (bit>=sizeof(unsigned long)*8*2) {
				C |= ((unsigned long)1)<<(bit-sizeof(unsigned long)*8*2);
			} else 
			#endif
			if (bit>=sizeof(unsigned long)*8) {
				B |= ((unsigned long)1)<<(bit-sizeof(unsigned long)*8);
			} else {
			#endif
				A |= ((unsigned long)1)<<(bit);
			#if DOUBLE==1
			}
			#endif
		} else {
			#if DOUBLE==1
			#if TRIPPLE==1
			if (bit>=sizeof(unsigned long)*8*2) {
				C &= (~((unsigned long)1)<<(bit-sizeof(unsigned long)*8*2));
			} else 
			#endif
			if (bit>=sizeof(unsigned long)*8) {
				B &= (~((unsigned long)1)<<(bit-sizeof(unsigned long)*8));
			} else {
			#endif
				A &= (~((unsigned long)1)<<(bit));
			#if DOUBLE==1
			}
			#endif
		}
	}
	inline void remove_bit(unsigned int bit) {
		unsigned long t1;
		unsigned long t2;
		#if DOUBLE==1
		#if TRIPPLE==1
		if (bit>=sizeof(unsigned long)*8*2) {
			bit -= sizeof(unsigned long)*8*2;
			t1 = (C>>bit)<<bit;
			t2 = (C>>(bit+1))<<(bit);
			C = (C^t1)|t2;
		} else 
		#endif
		if (bit>=sizeof(unsigned long)*8) {
			bit -= sizeof(unsigned long)*8;
			t1 = (B>>bit)<<bit;
			t2 = (B>>(bit+1))<<(bit);
			B = (B^t1)|t2;
			#if TRIPPLE==1
			B |= ((C&1)<<(sizeof(unsigned long)*8-1));
			C >>= 1;
			#endif
		} else {
		#endif
			t1 = (A>>bit)<<bit;
			t2 = (A>>(bit+1))<<(bit);
			A = (A^t1)|t2;
			#if DOUBLE==1
			A |= ((B&1)<<(sizeof(unsigned long)*8-1));
			B >>= 1;
			#if TRIPPLE==1
			B |= ((C&1)<<(sizeof(unsigned long)*8-1));
			C >>= 1;
			#endif
		}
		#endif
	}
	inline bool non_zero() {
		if (((this->A)
		#if DOUBLE==1
			|(this->B)
		#if TRIPPLE==1
			|(this->C)
		#endif
		#endif
		) ==0)
			return false;
		return true;
	}
	inline void set_ones(unsigned int ones) {
		#if DOUBLE==1
		#if TRIPPLE==1
		if (ones>sizeof(unsigned long)*8*3) {
			printf("ERROR: mask bit overflow\n");
		} else if (ones == sizeof(unsigned long)*8*3) {
			C = ~((unsigned long)0);
			B = ~((unsigned long)0);
			A = ~((unsigned long)0);
		} else if (ones>=sizeof(unsigned long)*8*2) {
			C = ((((unsigned long)1)<<(ones-sizeof(unsigned long)*8*2))-1);
			B = ~((unsigned long)0);
			A = ~((unsigned long)0);
		} else 
		#endif
		if (ones>=sizeof(unsigned long)*8) {
			#if TRIPPLE==1
			C = ((unsigned long)0);
			#endif
			B = ((((unsigned long)1)<<(ones-sizeof(unsigned long)*8))-1);
			A = ~((unsigned long)0);
		} else {
			#if TRIPPLE==1
			C = ((unsigned long)0);
			#endif
			B = ((unsigned long)0);
		#endif
			A = ((((unsigned long)1)<<(ones))-1);
		#if DOUBLE==1
		}
		#endif
	}
};

