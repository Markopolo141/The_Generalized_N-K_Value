
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

#include "mask.cpp"


