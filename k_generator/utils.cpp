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


