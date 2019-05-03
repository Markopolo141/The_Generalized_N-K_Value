//AUTO GENERATED CODE FILE, by gen_mask.py

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

class Mask {
	public:
	unsigned long A0;
	unsigned long A1;
	static const unsigned long length = sizeof(unsigned long)*8*2;

	void print() {
		for (unsigned long i=0; i<this->length; i++) { printf("%i",this->get_bit(i)); } printf("\n");
	}
	inline bool operator==(const Mask& m) {
		if((m.A0 == this->A0) && (m.A1 == this->A1))
			return true;
		return false;
	}
	inline Mask operator&(const Mask& m) {
		Mask m2;
		m2.A0 = (this->A0)&(m.A0);
		m2.A1 = (this->A1)&(m.A1);
		return m2;
	}
	inline Mask operator~() {
		Mask m2;
		m2.A0 = ~(this->A0);
		m2.A1 = ~(this->A1);
		return m2;
	}
	inline Mask operator^(const Mask& m) {
		Mask m2;
		m2.A0 = (this->A0)^(m.A0);
		m2.A1 = (this->A1)^(m.A1);
		return m2;
	}
	inline Mask operator|(const Mask& m) {
		Mask m2;
		m2.A0 = (this->A0)|(m.A0);
		m2.A1 = (this->A1)|(m.A1);
		return m2;
	}
	inline void set_zero() {
		this->A0 = 0;
		this->A1 = 0;
	}
	inline void operator=(Mask* m) {
		this->A0 = m->A0;
		this->A1 = m->A1;
	}
	inline void set(Mask* m) {
		this->A0 = m->A0;
		this->A1 = m->A1;
	}
	inline void set(Mask m) {
		this->A0 = m.A0;
		this->A1 = m.A1;
	}
	inline void flip_bit(unsigned int bit) {
		if ((bit>=sizeof(unsigned long)*8*2) || (bit<0)) {
			 printf("ERROR: mask bit overflow\n");
		}
		if (bit>=sizeof(unsigned long)*8*1) {
			this->A1 ^= ((unsigned long)1)<<(bit-sizeof(unsigned long)*8*1); return;
		}
		if (bit>=sizeof(unsigned long)*8*0) {
			this->A0 ^= ((unsigned long)1)<<(bit-sizeof(unsigned long)*8*0); return;
		}
	}
	int count_bits() {
		int a=0;
		a += count_the_bits(this->A0);
		a += count_the_bits(this->A1);
		return a;
	}
	inline unsigned int get_bit(unsigned int bit) {
		if ((bit>=sizeof(unsigned long)*8*2) || (bit<0)) {
			 printf("ERROR: mask bit overflow\n");
		}
		if (bit>=sizeof(unsigned long)*8*1) {
			return (A1 & ((unsigned long)1)<<(bit-sizeof(unsigned long)*8*1))!=0;
		}
		if (bit>=sizeof(unsigned long)*8*0) {
			return (A0 & ((unsigned long)1)<<(bit-sizeof(unsigned long)*8*0))!=0;
		}
	}
	inline void set_bit(unsigned int bit, unsigned int value) {
		if ((bit>=sizeof(unsigned long)*8*2) || (bit<0)) {
			 printf("ERROR: mask bit overflow\n");
		}
		if (value==1) {
			if (bit>=sizeof(unsigned long)*8*1) {A1 |= ((unsigned long)1)<<(bit-sizeof(unsigned long)*8*1);return;}
			if (bit>=sizeof(unsigned long)*8*0) {A0 |= ((unsigned long)1)<<(bit-sizeof(unsigned long)*8*0);return;}
		} else {
			if (bit>=sizeof(unsigned long)*8*1) {A1 &= ~(((unsigned long)1)<<(bit-sizeof(unsigned long)*8*1));return;}
			if (bit>=sizeof(unsigned long)*8*0) {A0 &= ~(((unsigned long)1)<<(bit-sizeof(unsigned long)*8*0));return;}
		}
	}
	inline void remove_bit(unsigned int bit) {
		if ((bit>=sizeof(unsigned long)*8*2) || (bit<0)) {
			 printf("ERROR: mask bit overflow\n");
		}
		unsigned long t1;
		unsigned long t2;
		if (bit>=sizeof(unsigned long)*8*1) {
			bit -= sizeof(unsigned long)*8*1;
			t1 = (A1>>bit)<<bit;
			t2 = (A1>>(bit+1))<<(bit);
			A1 = (A1^t1)|t2;
			
			return;
		}
		if (bit>=sizeof(unsigned long)*8*0) {
			bit -= sizeof(unsigned long)*8*0;
			t1 = (A0>>bit)<<bit;
			t2 = (A0>>(bit+1))<<(bit);
			A0 = (A0^t1)|t2;
			A0 |= ((A1&1)<<(sizeof(unsigned long)*8-1)); A1 >>= 1;
			return;
		}
	}
	inline bool non_zero() {
		if (((this->A0)|(this->A1))==0)
			return false;
		return true;
	}
	inline void set_ones(unsigned int ones) {
		this->set_zero();
		if ((ones>sizeof(unsigned long)*8*2)||(ones<0)) {
			printf("ERROR: mask bit overflow\n"); return;
		}
		if (ones==sizeof(unsigned long)*8*2) {
			A1 = ~((unsigned long)0);
			A0 = ~((unsigned long)0);
			return;
		}
		if (ones>=sizeof(unsigned long)*8*1) {
			A1 = ((((unsigned long)1)<<(ones-sizeof(unsigned long)*8*1))-1);
			A0 = ~((unsigned long)0);
			return;
		}
		if (ones>=sizeof(unsigned long)*8*0) {
			A0 = ((((unsigned long)1)<<(ones-sizeof(unsigned long)*8*0))-1);
			
			return;
		}
	}
};
