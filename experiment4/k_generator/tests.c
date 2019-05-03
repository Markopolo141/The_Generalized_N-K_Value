#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "limits.h"

#include "mask.cpp"

void test1() {
	printf("TEST1:\n");
	Mask m;
	printf("setting zero\n");
	m.set_zero();
	m.print();
	printf("setting the first 40 to ones\n");
	m.set_ones(40);
	printf("itteratively setting ones\n");
	for (int i=0; i<m.length; i++) {
		m.set_bit(i,1);
		m.print();
	}
	printf("flipping every third bit\n");
	for (int i=0; i<m.length; i+=3) {
		m.flip_bit(i);
		m.print();
	}
	printf("flipping every second bit\n");
	for (int i=0; i<m.length; i+=2) {
		m.flip_bit(i);
		m.print();
	}
	printf("resetting\n");
	m.print();
	printf("removing bits\n");
	for (int i=0; i<m.length; i++) {
		m.remove_bit(i);
		m.print();
	}
}

int main() {
	test1();
	printf("DONE\n");
	return 0;
}
