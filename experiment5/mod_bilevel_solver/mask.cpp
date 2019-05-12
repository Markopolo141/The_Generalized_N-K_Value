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
	unsigned long A2;
	unsigned long A3;
	unsigned long A4;
	unsigned long A5;
	unsigned long A6;
	unsigned long A7;
	unsigned long A8;
	unsigned long A9;
	static const unsigned long length = sizeof(unsigned long)*8*10;

	void print() {
		for (unsigned long i=0; i<this->length; i++) { printf("%i",this->get_bit(i)); } printf("\n");
	}
	inline bool operator==(const Mask& m) {
		if((m.A0 == this->A0) && (m.A1 == this->A1) && (m.A2 == this->A2) && (m.A3 == this->A3) && (m.A4 == this->A4) && (m.A5 == this->A5) && (m.A6 == this->A6) && (m.A7 == this->A7) && (m.A8 == this->A8) && (m.A9 == this->A9))
			return true;
		return false;
	}
	inline Mask operator&(const Mask& m) {
		Mask m2;
		m2.A0 = (this->A0)&(m.A0);
		m2.A1 = (this->A1)&(m.A1);
		m2.A2 = (this->A2)&(m.A2);
		m2.A3 = (this->A3)&(m.A3);
		m2.A4 = (this->A4)&(m.A4);
		m2.A5 = (this->A5)&(m.A5);
		m2.A6 = (this->A6)&(m.A6);
		m2.A7 = (this->A7)&(m.A7);
		m2.A8 = (this->A8)&(m.A8);
		m2.A9 = (this->A9)&(m.A9);
		return m2;
	}
	inline Mask operator~() {
		Mask m2;
		m2.A0 = ~(this->A0);
		m2.A1 = ~(this->A1);
		m2.A2 = ~(this->A2);
		m2.A3 = ~(this->A3);
		m2.A4 = ~(this->A4);
		m2.A5 = ~(this->A5);
		m2.A6 = ~(this->A6);
		m2.A7 = ~(this->A7);
		m2.A8 = ~(this->A8);
		m2.A9 = ~(this->A9);
		return m2;
	}
	inline Mask operator^(const Mask& m) {
		Mask m2;
		m2.A0 = (this->A0)^(m.A0);
		m2.A1 = (this->A1)^(m.A1);
		m2.A2 = (this->A2)^(m.A2);
		m2.A3 = (this->A3)^(m.A3);
		m2.A4 = (this->A4)^(m.A4);
		m2.A5 = (this->A5)^(m.A5);
		m2.A6 = (this->A6)^(m.A6);
		m2.A7 = (this->A7)^(m.A7);
		m2.A8 = (this->A8)^(m.A8);
		m2.A9 = (this->A9)^(m.A9);
		return m2;
	}
	inline Mask operator|(const Mask& m) {
		Mask m2;
		m2.A0 = (this->A0)|(m.A0);
		m2.A1 = (this->A1)|(m.A1);
		m2.A2 = (this->A2)|(m.A2);
		m2.A3 = (this->A3)|(m.A3);
		m2.A4 = (this->A4)|(m.A4);
		m2.A5 = (this->A5)|(m.A5);
		m2.A6 = (this->A6)|(m.A6);
		m2.A7 = (this->A7)|(m.A7);
		m2.A8 = (this->A8)|(m.A8);
		m2.A9 = (this->A9)|(m.A9);
		return m2;
	}
	inline void set_zero() {
		this->A0 = 0;
		this->A1 = 0;
		this->A2 = 0;
		this->A3 = 0;
		this->A4 = 0;
		this->A5 = 0;
		this->A6 = 0;
		this->A7 = 0;
		this->A8 = 0;
		this->A9 = 0;
	}
	inline void operator=(Mask* m) {
		this->A0 = m->A0;
		this->A1 = m->A1;
		this->A2 = m->A2;
		this->A3 = m->A3;
		this->A4 = m->A4;
		this->A5 = m->A5;
		this->A6 = m->A6;
		this->A7 = m->A7;
		this->A8 = m->A8;
		this->A9 = m->A9;
	}
	inline void set(Mask* m) {
		this->A0 = m->A0;
		this->A1 = m->A1;
		this->A2 = m->A2;
		this->A3 = m->A3;
		this->A4 = m->A4;
		this->A5 = m->A5;
		this->A6 = m->A6;
		this->A7 = m->A7;
		this->A8 = m->A8;
		this->A9 = m->A9;
	}
	inline void set(Mask m) {
		this->A0 = m.A0;
		this->A1 = m.A1;
		this->A2 = m.A2;
		this->A3 = m.A3;
		this->A4 = m.A4;
		this->A5 = m.A5;
		this->A6 = m.A6;
		this->A7 = m.A7;
		this->A8 = m.A8;
		this->A9 = m.A9;
	}
	inline void flip_bit(unsigned int bit) {
		if ((bit>=sizeof(unsigned long)*8*10) || (bit<0)) {
			 printf("ERROR: mask bit overflow\n");
		}
		if (bit>=sizeof(unsigned long)*8*9) {
			this->A9 ^= ((unsigned long)1)<<(bit-sizeof(unsigned long)*8*9); return;
		}
		if (bit>=sizeof(unsigned long)*8*8) {
			this->A8 ^= ((unsigned long)1)<<(bit-sizeof(unsigned long)*8*8); return;
		}
		if (bit>=sizeof(unsigned long)*8*7) {
			this->A7 ^= ((unsigned long)1)<<(bit-sizeof(unsigned long)*8*7); return;
		}
		if (bit>=sizeof(unsigned long)*8*6) {
			this->A6 ^= ((unsigned long)1)<<(bit-sizeof(unsigned long)*8*6); return;
		}
		if (bit>=sizeof(unsigned long)*8*5) {
			this->A5 ^= ((unsigned long)1)<<(bit-sizeof(unsigned long)*8*5); return;
		}
		if (bit>=sizeof(unsigned long)*8*4) {
			this->A4 ^= ((unsigned long)1)<<(bit-sizeof(unsigned long)*8*4); return;
		}
		if (bit>=sizeof(unsigned long)*8*3) {
			this->A3 ^= ((unsigned long)1)<<(bit-sizeof(unsigned long)*8*3); return;
		}
		if (bit>=sizeof(unsigned long)*8*2) {
			this->A2 ^= ((unsigned long)1)<<(bit-sizeof(unsigned long)*8*2); return;
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
		a += count_the_bits(this->A2);
		a += count_the_bits(this->A3);
		a += count_the_bits(this->A4);
		a += count_the_bits(this->A5);
		a += count_the_bits(this->A6);
		a += count_the_bits(this->A7);
		a += count_the_bits(this->A8);
		a += count_the_bits(this->A9);
		return a;
	}
	inline unsigned int get_bit(unsigned int bit) {
		if ((bit>=sizeof(unsigned long)*8*10) || (bit<0)) {
			 printf("ERROR: mask bit overflow\n");
		}
		if (bit>=sizeof(unsigned long)*8*9) {
			return (A9 & ((unsigned long)1)<<(bit-sizeof(unsigned long)*8*9))!=0;
		}
		if (bit>=sizeof(unsigned long)*8*8) {
			return (A8 & ((unsigned long)1)<<(bit-sizeof(unsigned long)*8*8))!=0;
		}
		if (bit>=sizeof(unsigned long)*8*7) {
			return (A7 & ((unsigned long)1)<<(bit-sizeof(unsigned long)*8*7))!=0;
		}
		if (bit>=sizeof(unsigned long)*8*6) {
			return (A6 & ((unsigned long)1)<<(bit-sizeof(unsigned long)*8*6))!=0;
		}
		if (bit>=sizeof(unsigned long)*8*5) {
			return (A5 & ((unsigned long)1)<<(bit-sizeof(unsigned long)*8*5))!=0;
		}
		if (bit>=sizeof(unsigned long)*8*4) {
			return (A4 & ((unsigned long)1)<<(bit-sizeof(unsigned long)*8*4))!=0;
		}
		if (bit>=sizeof(unsigned long)*8*3) {
			return (A3 & ((unsigned long)1)<<(bit-sizeof(unsigned long)*8*3))!=0;
		}
		if (bit>=sizeof(unsigned long)*8*2) {
			return (A2 & ((unsigned long)1)<<(bit-sizeof(unsigned long)*8*2))!=0;
		}
		if (bit>=sizeof(unsigned long)*8*1) {
			return (A1 & ((unsigned long)1)<<(bit-sizeof(unsigned long)*8*1))!=0;
		}
		if (bit>=sizeof(unsigned long)*8*0) {
			return (A0 & ((unsigned long)1)<<(bit-sizeof(unsigned long)*8*0))!=0;
		}
	}
	inline void set_bit(unsigned int bit, unsigned int value) {
		if ((bit>=sizeof(unsigned long)*8*10) || (bit<0)) {
			 printf("ERROR: mask bit overflow\n");
		}
		if (value==1) {
			if (bit>=sizeof(unsigned long)*8*9) {A9 |= ((unsigned long)1)<<(bit-sizeof(unsigned long)*8*9);return;}
			if (bit>=sizeof(unsigned long)*8*8) {A8 |= ((unsigned long)1)<<(bit-sizeof(unsigned long)*8*8);return;}
			if (bit>=sizeof(unsigned long)*8*7) {A7 |= ((unsigned long)1)<<(bit-sizeof(unsigned long)*8*7);return;}
			if (bit>=sizeof(unsigned long)*8*6) {A6 |= ((unsigned long)1)<<(bit-sizeof(unsigned long)*8*6);return;}
			if (bit>=sizeof(unsigned long)*8*5) {A5 |= ((unsigned long)1)<<(bit-sizeof(unsigned long)*8*5);return;}
			if (bit>=sizeof(unsigned long)*8*4) {A4 |= ((unsigned long)1)<<(bit-sizeof(unsigned long)*8*4);return;}
			if (bit>=sizeof(unsigned long)*8*3) {A3 |= ((unsigned long)1)<<(bit-sizeof(unsigned long)*8*3);return;}
			if (bit>=sizeof(unsigned long)*8*2) {A2 |= ((unsigned long)1)<<(bit-sizeof(unsigned long)*8*2);return;}
			if (bit>=sizeof(unsigned long)*8*1) {A1 |= ((unsigned long)1)<<(bit-sizeof(unsigned long)*8*1);return;}
			if (bit>=sizeof(unsigned long)*8*0) {A0 |= ((unsigned long)1)<<(bit-sizeof(unsigned long)*8*0);return;}
		} else {
			if (bit>=sizeof(unsigned long)*8*9) {A9 &= ~(((unsigned long)1)<<(bit-sizeof(unsigned long)*8*9));return;}
			if (bit>=sizeof(unsigned long)*8*8) {A8 &= ~(((unsigned long)1)<<(bit-sizeof(unsigned long)*8*8));return;}
			if (bit>=sizeof(unsigned long)*8*7) {A7 &= ~(((unsigned long)1)<<(bit-sizeof(unsigned long)*8*7));return;}
			if (bit>=sizeof(unsigned long)*8*6) {A6 &= ~(((unsigned long)1)<<(bit-sizeof(unsigned long)*8*6));return;}
			if (bit>=sizeof(unsigned long)*8*5) {A5 &= ~(((unsigned long)1)<<(bit-sizeof(unsigned long)*8*5));return;}
			if (bit>=sizeof(unsigned long)*8*4) {A4 &= ~(((unsigned long)1)<<(bit-sizeof(unsigned long)*8*4));return;}
			if (bit>=sizeof(unsigned long)*8*3) {A3 &= ~(((unsigned long)1)<<(bit-sizeof(unsigned long)*8*3));return;}
			if (bit>=sizeof(unsigned long)*8*2) {A2 &= ~(((unsigned long)1)<<(bit-sizeof(unsigned long)*8*2));return;}
			if (bit>=sizeof(unsigned long)*8*1) {A1 &= ~(((unsigned long)1)<<(bit-sizeof(unsigned long)*8*1));return;}
			if (bit>=sizeof(unsigned long)*8*0) {A0 &= ~(((unsigned long)1)<<(bit-sizeof(unsigned long)*8*0));return;}
		}
	}
	inline void remove_bit(unsigned int bit) {
		if ((bit>=sizeof(unsigned long)*8*10) || (bit<0)) {
			 printf("ERROR: mask bit overflow\n");
		}
		unsigned long t1;
		unsigned long t2;
		if (bit>=sizeof(unsigned long)*8*9) {
			bit -= sizeof(unsigned long)*8*9;
			t1 = (A9>>bit)<<bit;
			t2 = (A9>>(bit+1))<<(bit);
			A9 = (A9^t1)|t2;
			
			return;
		}
		if (bit>=sizeof(unsigned long)*8*8) {
			bit -= sizeof(unsigned long)*8*8;
			t1 = (A8>>bit)<<bit;
			t2 = (A8>>(bit+1))<<(bit);
			A8 = (A8^t1)|t2;
			A8 |= ((A9&1)<<(sizeof(unsigned long)*8-1)); A9 >>= 1;
			return;
		}
		if (bit>=sizeof(unsigned long)*8*7) {
			bit -= sizeof(unsigned long)*8*7;
			t1 = (A7>>bit)<<bit;
			t2 = (A7>>(bit+1))<<(bit);
			A7 = (A7^t1)|t2;
			A7 |= ((A8&1)<<(sizeof(unsigned long)*8-1)); A8 >>= 1;
			A8 |= ((A9&1)<<(sizeof(unsigned long)*8-1)); A9 >>= 1;
			return;
		}
		if (bit>=sizeof(unsigned long)*8*6) {
			bit -= sizeof(unsigned long)*8*6;
			t1 = (A6>>bit)<<bit;
			t2 = (A6>>(bit+1))<<(bit);
			A6 = (A6^t1)|t2;
			A6 |= ((A7&1)<<(sizeof(unsigned long)*8-1)); A7 >>= 1;
			A7 |= ((A8&1)<<(sizeof(unsigned long)*8-1)); A8 >>= 1;
			A8 |= ((A9&1)<<(sizeof(unsigned long)*8-1)); A9 >>= 1;
			return;
		}
		if (bit>=sizeof(unsigned long)*8*5) {
			bit -= sizeof(unsigned long)*8*5;
			t1 = (A5>>bit)<<bit;
			t2 = (A5>>(bit+1))<<(bit);
			A5 = (A5^t1)|t2;
			A5 |= ((A6&1)<<(sizeof(unsigned long)*8-1)); A6 >>= 1;
			A6 |= ((A7&1)<<(sizeof(unsigned long)*8-1)); A7 >>= 1;
			A7 |= ((A8&1)<<(sizeof(unsigned long)*8-1)); A8 >>= 1;
			A8 |= ((A9&1)<<(sizeof(unsigned long)*8-1)); A9 >>= 1;
			return;
		}
		if (bit>=sizeof(unsigned long)*8*4) {
			bit -= sizeof(unsigned long)*8*4;
			t1 = (A4>>bit)<<bit;
			t2 = (A4>>(bit+1))<<(bit);
			A4 = (A4^t1)|t2;
			A4 |= ((A5&1)<<(sizeof(unsigned long)*8-1)); A5 >>= 1;
			A5 |= ((A6&1)<<(sizeof(unsigned long)*8-1)); A6 >>= 1;
			A6 |= ((A7&1)<<(sizeof(unsigned long)*8-1)); A7 >>= 1;
			A7 |= ((A8&1)<<(sizeof(unsigned long)*8-1)); A8 >>= 1;
			A8 |= ((A9&1)<<(sizeof(unsigned long)*8-1)); A9 >>= 1;
			return;
		}
		if (bit>=sizeof(unsigned long)*8*3) {
			bit -= sizeof(unsigned long)*8*3;
			t1 = (A3>>bit)<<bit;
			t2 = (A3>>(bit+1))<<(bit);
			A3 = (A3^t1)|t2;
			A3 |= ((A4&1)<<(sizeof(unsigned long)*8-1)); A4 >>= 1;
			A4 |= ((A5&1)<<(sizeof(unsigned long)*8-1)); A5 >>= 1;
			A5 |= ((A6&1)<<(sizeof(unsigned long)*8-1)); A6 >>= 1;
			A6 |= ((A7&1)<<(sizeof(unsigned long)*8-1)); A7 >>= 1;
			A7 |= ((A8&1)<<(sizeof(unsigned long)*8-1)); A8 >>= 1;
			A8 |= ((A9&1)<<(sizeof(unsigned long)*8-1)); A9 >>= 1;
			return;
		}
		if (bit>=sizeof(unsigned long)*8*2) {
			bit -= sizeof(unsigned long)*8*2;
			t1 = (A2>>bit)<<bit;
			t2 = (A2>>(bit+1))<<(bit);
			A2 = (A2^t1)|t2;
			A2 |= ((A3&1)<<(sizeof(unsigned long)*8-1)); A3 >>= 1;
			A3 |= ((A4&1)<<(sizeof(unsigned long)*8-1)); A4 >>= 1;
			A4 |= ((A5&1)<<(sizeof(unsigned long)*8-1)); A5 >>= 1;
			A5 |= ((A6&1)<<(sizeof(unsigned long)*8-1)); A6 >>= 1;
			A6 |= ((A7&1)<<(sizeof(unsigned long)*8-1)); A7 >>= 1;
			A7 |= ((A8&1)<<(sizeof(unsigned long)*8-1)); A8 >>= 1;
			A8 |= ((A9&1)<<(sizeof(unsigned long)*8-1)); A9 >>= 1;
			return;
		}
		if (bit>=sizeof(unsigned long)*8*1) {
			bit -= sizeof(unsigned long)*8*1;
			t1 = (A1>>bit)<<bit;
			t2 = (A1>>(bit+1))<<(bit);
			A1 = (A1^t1)|t2;
			A1 |= ((A2&1)<<(sizeof(unsigned long)*8-1)); A2 >>= 1;
			A2 |= ((A3&1)<<(sizeof(unsigned long)*8-1)); A3 >>= 1;
			A3 |= ((A4&1)<<(sizeof(unsigned long)*8-1)); A4 >>= 1;
			A4 |= ((A5&1)<<(sizeof(unsigned long)*8-1)); A5 >>= 1;
			A5 |= ((A6&1)<<(sizeof(unsigned long)*8-1)); A6 >>= 1;
			A6 |= ((A7&1)<<(sizeof(unsigned long)*8-1)); A7 >>= 1;
			A7 |= ((A8&1)<<(sizeof(unsigned long)*8-1)); A8 >>= 1;
			A8 |= ((A9&1)<<(sizeof(unsigned long)*8-1)); A9 >>= 1;
			return;
		}
		if (bit>=sizeof(unsigned long)*8*0) {
			bit -= sizeof(unsigned long)*8*0;
			t1 = (A0>>bit)<<bit;
			t2 = (A0>>(bit+1))<<(bit);
			A0 = (A0^t1)|t2;
			A0 |= ((A1&1)<<(sizeof(unsigned long)*8-1)); A1 >>= 1;
			A1 |= ((A2&1)<<(sizeof(unsigned long)*8-1)); A2 >>= 1;
			A2 |= ((A3&1)<<(sizeof(unsigned long)*8-1)); A3 >>= 1;
			A3 |= ((A4&1)<<(sizeof(unsigned long)*8-1)); A4 >>= 1;
			A4 |= ((A5&1)<<(sizeof(unsigned long)*8-1)); A5 >>= 1;
			A5 |= ((A6&1)<<(sizeof(unsigned long)*8-1)); A6 >>= 1;
			A6 |= ((A7&1)<<(sizeof(unsigned long)*8-1)); A7 >>= 1;
			A7 |= ((A8&1)<<(sizeof(unsigned long)*8-1)); A8 >>= 1;
			A8 |= ((A9&1)<<(sizeof(unsigned long)*8-1)); A9 >>= 1;
			return;
		}
	}
	inline bool non_zero() {
		if (((this->A0)|(this->A1)|(this->A2)|(this->A3)|(this->A4)|(this->A5)|(this->A6)|(this->A7)|(this->A8)|(this->A9))==0)
			return false;
		return true;
	}
	inline void set_ones(unsigned int ones) {
		this->set_zero();
		if ((ones>sizeof(unsigned long)*8*10)||(ones<0)) {
			printf("ERROR: mask bit overflow\n"); return;
		}
		if (ones==sizeof(unsigned long)*8*10) {
			A9 = ~((unsigned long)0);
			A8 = ~((unsigned long)0);
			A7 = ~((unsigned long)0);
			A6 = ~((unsigned long)0);
			A5 = ~((unsigned long)0);
			A4 = ~((unsigned long)0);
			A3 = ~((unsigned long)0);
			A2 = ~((unsigned long)0);
			A1 = ~((unsigned long)0);
			A0 = ~((unsigned long)0);
			return;
		}
		if (ones>=sizeof(unsigned long)*8*9) {
			A9 = ((((unsigned long)1)<<(ones-sizeof(unsigned long)*8*9))-1);
			A8 = ~((unsigned long)0);
			A7 = ~((unsigned long)0);
			A6 = ~((unsigned long)0);
			A5 = ~((unsigned long)0);
			A4 = ~((unsigned long)0);
			A3 = ~((unsigned long)0);
			A2 = ~((unsigned long)0);
			A1 = ~((unsigned long)0);
			A0 = ~((unsigned long)0);
			return;
		}
		if (ones>=sizeof(unsigned long)*8*8) {
			A8 = ((((unsigned long)1)<<(ones-sizeof(unsigned long)*8*8))-1);
			A7 = ~((unsigned long)0);
			A6 = ~((unsigned long)0);
			A5 = ~((unsigned long)0);
			A4 = ~((unsigned long)0);
			A3 = ~((unsigned long)0);
			A2 = ~((unsigned long)0);
			A1 = ~((unsigned long)0);
			A0 = ~((unsigned long)0);
			return;
		}
		if (ones>=sizeof(unsigned long)*8*7) {
			A7 = ((((unsigned long)1)<<(ones-sizeof(unsigned long)*8*7))-1);
			A6 = ~((unsigned long)0);
			A5 = ~((unsigned long)0);
			A4 = ~((unsigned long)0);
			A3 = ~((unsigned long)0);
			A2 = ~((unsigned long)0);
			A1 = ~((unsigned long)0);
			A0 = ~((unsigned long)0);
			return;
		}
		if (ones>=sizeof(unsigned long)*8*6) {
			A6 = ((((unsigned long)1)<<(ones-sizeof(unsigned long)*8*6))-1);
			A5 = ~((unsigned long)0);
			A4 = ~((unsigned long)0);
			A3 = ~((unsigned long)0);
			A2 = ~((unsigned long)0);
			A1 = ~((unsigned long)0);
			A0 = ~((unsigned long)0);
			return;
		}
		if (ones>=sizeof(unsigned long)*8*5) {
			A5 = ((((unsigned long)1)<<(ones-sizeof(unsigned long)*8*5))-1);
			A4 = ~((unsigned long)0);
			A3 = ~((unsigned long)0);
			A2 = ~((unsigned long)0);
			A1 = ~((unsigned long)0);
			A0 = ~((unsigned long)0);
			return;
		}
		if (ones>=sizeof(unsigned long)*8*4) {
			A4 = ((((unsigned long)1)<<(ones-sizeof(unsigned long)*8*4))-1);
			A3 = ~((unsigned long)0);
			A2 = ~((unsigned long)0);
			A1 = ~((unsigned long)0);
			A0 = ~((unsigned long)0);
			return;
		}
		if (ones>=sizeof(unsigned long)*8*3) {
			A3 = ((((unsigned long)1)<<(ones-sizeof(unsigned long)*8*3))-1);
			A2 = ~((unsigned long)0);
			A1 = ~((unsigned long)0);
			A0 = ~((unsigned long)0);
			return;
		}
		if (ones>=sizeof(unsigned long)*8*2) {
			A2 = ((((unsigned long)1)<<(ones-sizeof(unsigned long)*8*2))-1);
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
	void set_long(int i, unsigned long l) {
		if ((i*(sizeof(unsigned long)*8)>=sizeof(unsigned long)*8*10) || (i*(sizeof(unsigned long)*8)<0)) {
			 printf("ERROR: mask bit overflow\n");
		}
		if (i==0) { A0=l; }
		if (i==1) { A1=l; }
		if (i==2) { A2=l; }
		if (i==3) { A3=l; }
		if (i==4) { A4=l; }
		if (i==5) { A5=l; }
		if (i==6) { A6=l; }
		if (i==7) { A7=l; }
		if (i==8) { A8=l; }
		if (i==9) { A9=l; }
	}
};
