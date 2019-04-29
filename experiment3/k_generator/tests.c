#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "limits.h"

#define TINY 0.000001
#define MEMORY_INITIAL_SIZE 131072

#include "utils.cpp"
#include "solver.cpp"


void test1() {
	printf("TEST1:\n");
	Table* t = (Table*)malloc(sizeof(Table));
	t->initialise(5, 5);
	t->print();
	t->free_data();
	free(t);
}

void test2() {
	printf("TEST2:\n");
	Table* t = (Table*)malloc(sizeof(Table));
	t->initialise(5, 5);
	for (int i=0;i<4;i++)
		for (int j=0;j<5;j++)
			t->set(i,j,j*1.0-i-0.5*(i%2));
	for (int j=0;j<5;j++)
		t->set(4,j,1.0);
	t->print();
	t->calculate_pivots(false);
	t->print_pivotable_info();
	t->free_data();
	free(t);
}

void test3() {
	printf("TEST3\n");
	Table* t = (Table*)malloc(sizeof(Table));
	t->initialise_and_wipe(5, 5);
	for (int i=0;i<4;i++)
		for (int j=0;j<5;j++)
			t->set(i,j,j*1.0-i-0.5*(i%2));
	for (int j=0;j<5;j++)
		t->set(4,j,1.0);
	t->calculate_pivots(false);
	printf("PIVOTING\n");
	t->pivot(t,0);
	t->print();
	t->print_pivot_info();
	t->print_pivotable_info();
	t->free_data();
	free(t);
}

void test4() {
	printf("TEST4\n");
	double* head = (double*)calloc(sizeof(double),4);
	Table* t = (Table*)malloc(sizeof(Table));
	t->initialise_and_wipe(4, 2);
	t->set(0,0,1.0/40);
	t->set(1,0,1.0/60);
	t->set(0,1,1.0/50);
	t->set(1,1,1.0/50);
	t->set(2,0,1.0);
	t->set(2,1,1.0);
	t->set(3,0,1.0);
	t->set(3,1,1.0);
	head[0]=-200;
	head[1]=-400;
	printhead(head,4);
	t->print();
	printf("SIMPLEXING\n");
	t->simplex(head,true);
	printhead(head,4);
	t->print();
	printf("SIMPLEXING\n");
	t->simplex(head,false);
	printhead(head,4);
	t->print();

	t->free_data();
	free(t);
	free(head);
}

void test5() {
	printf("TEST5\n");
	double* head = (double*)calloc(sizeof(double),8);
	Table* t = (Table*)malloc(sizeof(Table));
	t->initialise_and_wipe(8, 3);
	head[0] = -10;	head[1] = 57;	head[2] = 9;	head[3] = 24;
	t->set(0,0,0.5);	t->set(1,0,-5.5);	t->set(2,0,-2.5);	t->set(3,0,9.0);	t->set(4,0,1.0);	t->set(5,0,0.0);	t->set(6,0,0.0);	t->set(7,0,0.0);
	t->set(0,1,0.5);	t->set(1,1,-1.5);	t->set(2,1,-0.5);	t->set(3,1,1.0);	t->set(4,1,0.0);	t->set(5,1,1.0);	t->set(6,1,0.0);	t->set(7,1,0.0);
	t->set(0,2,1.0);	t->set(1,2,0.0);	t->set(2,2,0.0);	t->set(3,2,0.0);	t->set(4,2,0.0);	t->set(5,2,0.0);	t->set(6,2,1.0);	t->set(7,2,1.0);

	printhead(head,8);
	t->print();
	printf("SIMPLEXING\n");
	t->simplex(head,true);
	printhead(head,8);
	t->print();
	printf("SIMPLEXING\n");
	t->simplex(head,false);
	printhead(head,8);
	t->print();

	t->free_data();
	free(t);
	free(head);
}

void test6() {
	printf("TEST6\n");
	double* head = (double*)calloc(sizeof(double),8);
	Table* t = (Table*)malloc(sizeof(Table));
	t->initialise_and_wipe(8, 3);
	head[0] = -10;	head[1] = 57;	head[2] = 9;	head[3] = 24;
	t->set(0,0,0.5);	t->set(1,0,-5.5);	t->set(2,0,-2.5);	t->set(3,0,9.0);	t->set(4,0,1.0);	t->set(5,0,0.0);	t->set(6,0,0.0);	t->set(7,0,0.0);
	t->set(0,1,0.5);	t->set(1,1,-1.5);	t->set(2,1,-0.5);	t->set(3,1,1.0);	t->set(4,1,0.0);	t->set(5,1,1.0);	t->set(6,1,0.0);	t->set(7,1,0.0);
	t->set(0,2,1.0);	t->set(1,2,0.0);	t->set(2,2,0.0);	t->set(3,2,0.0);	t->set(4,2,0.0);	t->set(5,2,0.0);	t->set(6,2,1.0);	t->set(7,2,1.0);

	Mask mask;
	mask.set_zero();
	mask.print();

	printhead(head,8);
	t->print();
	printf("subset_improvable: %i\n",t->check_subset_improvable(&mask, head, true)==true);
	printhead(head,8);
	t->print();
	
	printf("SIMPLEXING\n");
	t->simplex(head,true);
	printhead(head,8);
	t->print();
	printf("subset_improvable: %i\n",t->check_subset_improvable(&mask, head, true)==true);

	t->free_data();
	free(t);
	free(head);
}
void test7() {
	printf("TEST7\n");
	double* head = (double*)calloc(sizeof(double),9);
	Table* t = (Table*)malloc(sizeof(Table));
	t->initialise_and_wipe(9, 3);
	head[1] = -10;	head[2] = 57;	head[3] = 9;	head[4] = 24;
	t->set(0,0,1.0);		t->set(1,0,0.5);	t->set(2,0,-5.5);	t->set(3,0,-2.5);	t->set(4,0,9.0);	t->set(5,0,1.0);	t->set(6,0,0.0);	t->set(7,0,0.0);	t->set(8,0,0.0);
	t->set(0,1,0.0);		t->set(1,1,0.5);	t->set(2,1,-1.5);	t->set(3,1,-0.5);	t->set(4,1,1.0);	t->set(5,1,0.0);	t->set(6,1,1.0);	t->set(7,1,0.0);	t->set(8,1,0.0);
	t->set(0,2,0.0);		t->set(1,2,1.0);	t->set(2,2,0.0);	t->set(3,2,0.0);	t->set(4,2,0.0);	t->set(5,2,0.0);	t->set(6,2,0.0);	t->set(7,2,1.0);	t->set(8,2,1.0);
	t->table_pivot_column_number = 3;
	t->table_pivot_column_mask->A = 1+64+128;
	t->table_pivot_columns[0] = 0;
	t->table_pivot_columns[1] = 6;
	t->table_pivot_columns[2] = 7;

	Mask mask;
	mask.set_zero();
	mask.A = 1;//(1<<7)-1;
	mask.print();

	t->print_pivot_info();
	printhead(head,9);
	t->print();
	printf("subset_improvable: %i\n",t->check_subset_improvable(&mask, head, true)==true);
	printhead(head,8);
	t->print();
	
	printf("SIMPLEXING\n");
	t->simplex(head,true);
	printhead(head,9);
	t->print();
	printf("subset_improvable: %i\n",t->check_subset_improvable(&mask, head, true)==true);

	t->free_data();
	free(t);
	free(head);
}
void test8() {
	printf("TEST8\n");
	double* head = (double*)calloc(sizeof(double),9);
	Table* t = (Table*)malloc(sizeof(Table));
	t->initialise_and_wipe(9, 5);
	head[2] = -1;

	t->set(0,0,1.0);	t->set(1,0,0.0);	t->set(2,0,-0.1);	t->set(3,0,1.0);	t->set(4,0,0.0);	t->set(5,0,0.0);	t->set(6,0,0.0);	t->set(7,0,0.0);	t->set(8,0,1.0);
	t->set(0,1,0.0);	t->set(1,1,1.0);	t->set(2,1,-0.1);	t->set(3,1,0.0);	t->set(4,1,1.0);	t->set(5,1,0.0);	t->set(6,1,0.0);	t->set(7,1,0.0);	t->set(8,1,1.0);
	t->set(0,2,-0.1);	t->set(1,2,-0.1);	t->set(2,2,1.0);	t->set(3,2,0.0);	t->set(4,2,0.0);	t->set(5,2,1.0);	t->set(6,2,0.0);	t->set(7,2,0.0);	t->set(8,2,1.0);
	t->set(0,3,0.5);	t->set(1,3,0.2);	t->set(2,3,1.0);	t->set(3,3,0.0);	t->set(4,3,0.0);	t->set(5,3,0.0);	t->set(6,3,1.0);	t->set(7,3,0.0);	t->set(8,3,1.5);
	t->set(0,4,0.2);	t->set(1,4,0.5);	t->set(2,4,1.0);	t->set(3,4,0.0);	t->set(4,4,0.0);	t->set(5,4,0.0);	t->set(6,4,0.0);	t->set(7,4,1.0);	t->set(8,4,1.4);
	t->table_pivot_column_number = 5;
	t->table_pivot_column_mask->A = 8+16+32+64+128;
	t->table_pivot_columns[0] = 3;
	t->table_pivot_columns[1] = 4;
	t->table_pivot_columns[2] = 5;
	t->table_pivot_columns[3] = 6;
	t->table_pivot_columns[4] = 7;

	Mask mask;
	mask.set_zero();
	mask.A = 3;
	mask.print();

	printhead(head,t->w);
	t->print();
	t->calculate_pivots(false);
	t->print_pivot_info();
	t->print_pivotable_info();
	printf("pivoting for non-zero z\n");
	t->pivot(t,2);
	t->apply_to_head(head,head);
	printf("\nBILEVEL SOLVING, should be 0.952381\n");
	
	for (int i=0; i<10; i++)
	printf("bilevel_solve: %f\n", bilevel_solve(t, &mask, head, true));

	printf("\n\nBILEVEL SOLVE COMPLETE\n");
	printhead(head,t->w);
	t->print();
	t->print_pivot_info();

	t->free_data();
	free(t);
	free(head);
}
void test9() {
	printf("TEST9\n");
	double* head = (double*)calloc(sizeof(double),9);
	Table* t = (Table*)malloc(sizeof(Table));
	t->initialise_and_wipe(9, 5);
	head[2] = -1;

	t->set(0,0,1.0);	t->set(1,0,0.0);	t->set(2,0,-0.1);	t->set(3,0,1.0);	t->set(4,0,0.0);	t->set(5,0,0.0);	t->set(6,0,0.0);	t->set(7,0,0.0);	t->set(8,0,1.0);
	t->set(0,1,0.0);	t->set(1,1,1.0);	t->set(2,1,-0.1);	t->set(3,1,0.0);	t->set(4,1,1.0);	t->set(5,1,0.0);	t->set(6,1,0.0);	t->set(7,1,0.0);	t->set(8,1,1.0);
	t->set(0,2,-0.1);	t->set(1,2,-0.1);	t->set(2,2,1.0);	t->set(3,2,0.0);	t->set(4,2,0.0);	t->set(5,2,1.0);	t->set(6,2,0.0);	t->set(7,2,0.0);	t->set(8,2,1.0);
	t->set(0,3,0.5);	t->set(1,3,0.2);	t->set(2,3,1.0);	t->set(3,3,0.0);	t->set(4,3,0.0);	t->set(5,3,0.0);	t->set(6,3,1.0);	t->set(7,3,0.0);	t->set(8,3,1.5);
	t->set(0,4,0.2);	t->set(1,4,0.5);	t->set(2,4,1.0);	t->set(3,4,0.0);	t->set(4,4,0.0);	t->set(5,4,0.0);	t->set(6,4,0.0);	t->set(7,4,1.0);	t->set(8,4,1.4);
	t->table_pivot_column_number = 5;
	t->table_pivot_column_mask->A = 8+16+32+64+128;
	t->table_pivot_columns[0] = 3;
	t->table_pivot_columns[1] = 4;
	t->table_pivot_columns[2] = 5;
	t->table_pivot_columns[3] = 6;
	t->table_pivot_columns[4] = 7;

	Mask mask;
	mask.set_zero();
	mask.A = 3;
	mask.print();

	printhead(head,t->w);
	t->print();
	t->calculate_pivots(false);
	t->print_pivot_info();
	t->print_pivotable_info();
	printf("pivoting for non-zero z\n");
	t->pivot(t,2);
	t->apply_to_head(head,head);
	printf("\nWALKBACK SOLVING, should be 0.952381\n");
	
	for (int i=0; i<5; i++)
	printf("bilevel_solve: %f\n", walk_back(t, &mask, head, true));

	printf("\n\nWALKBACK SOLVE COMPLETE\n");
	printhead(head,t->w);
	t->print();
	t->print_pivot_info();

	t->free_data();
	free(t);
	free(head);
}

void test10() {
	printf("TEST10\n");
	double* head = (double*)calloc(sizeof(double),9);
	Table* t = (Table*)malloc(sizeof(Table));
	t->initialise_and_wipe(9, 5);
	head[2] = -1;

	t->set(0,0,1.0);	t->set(1,0,0.0);	t->set(2,0,-0.1);	t->set(3,0,1.0);	t->set(4,0,0.0);	t->set(5,0,0.0);	t->set(6,0,0.0);	t->set(7,0,0.0);	t->set(8,0,1.0);
	t->set(0,1,0.0);	t->set(1,1,1.0);	t->set(2,1,-0.1);	t->set(3,1,0.0);	t->set(4,1,1.0);	t->set(5,1,0.0);	t->set(6,1,0.0);	t->set(7,1,0.0);	t->set(8,1,1.0);
	t->set(0,2,-0.1);	t->set(1,2,-0.1);	t->set(2,2,1.0);	t->set(3,2,0.0);	t->set(4,2,0.0);	t->set(5,2,1.0);	t->set(6,2,0.0);	t->set(7,2,0.0);	t->set(8,2,1.0);
	t->set(0,3,0.5);	t->set(1,3,0.2);	t->set(2,3,1.0);	t->set(3,3,0.0);	t->set(4,3,0.0);	t->set(5,3,0.0);	t->set(6,3,1.0);	t->set(7,3,0.0);	t->set(8,3,1.5);
	t->set(0,4,0.2);	t->set(1,4,0.5);	t->set(2,4,1.0);	t->set(3,4,0.0);	t->set(4,4,0.0);	t->set(5,4,0.0);	t->set(6,4,0.0);	t->set(7,4,1.0);	t->set(8,4,1.4);
	t->table_pivot_column_number = 5;
	t->table_pivot_column_mask->A = 8+16+32+64+128;
	t->table_pivot_columns[0] = 3;
	t->table_pivot_columns[1] = 4;
	t->table_pivot_columns[2] = 5;
	t->table_pivot_columns[3] = 6;
	t->table_pivot_columns[4] = 7;

	Mask mask;
	mask.set_zero();
	mask.A = 1;
	mask.print();

	printhead(head,t->w);
	t->print();
	t->calculate_pivots(false);
	t->print_pivot_info();
	t->print_pivotable_info();
	printf("pivoting for non-zero z\n");
	t->pivot(t,2);
	t->apply_to_head(head,head);
	printf("\nBILEVEL SOLVING, should be 0.952381\n");
	
	for (int i=0; i<10; i++)
	printf("bilevel_solve: %f\n", bilevel_solve(t, &mask, head, true));

	printf("\n\nBILEVEL SOLVE COMPLETE\n");
	printhead(head,t->w);
	t->print();
	t->print_pivot_info();

	t->free_data();
	free(t);
	free(head);
}
void test11() {
	printf("TEST11\n");
	double* head = (double*)calloc(sizeof(double),9);
	Table* t = (Table*)malloc(sizeof(Table));
	t->initialise_and_wipe(9, 5);
	head[2] = -1;

	t->set(0,0,1.0);	t->set(1,0,0.0);	t->set(2,0,-0.1);	t->set(3,0,1.0);	t->set(4,0,0.0);	t->set(5,0,0.0);	t->set(6,0,0.0);	t->set(7,0,0.0);	t->set(8,0,1.0);
	t->set(0,1,0.0);	t->set(1,1,1.0);	t->set(2,1,-0.1);	t->set(3,1,0.0);	t->set(4,1,1.0);	t->set(5,1,0.0);	t->set(6,1,0.0);	t->set(7,1,0.0);	t->set(8,1,1.0);
	t->set(0,2,-0.1);	t->set(1,2,-0.1);	t->set(2,2,1.0);	t->set(3,2,0.0);	t->set(4,2,0.0);	t->set(5,2,1.0);	t->set(6,2,0.0);	t->set(7,2,0.0);	t->set(8,2,1.0);
	t->set(0,3,0.5);	t->set(1,3,0.2);	t->set(2,3,1.0);	t->set(3,3,0.0);	t->set(4,3,0.0);	t->set(5,3,0.0);	t->set(6,3,1.0);	t->set(7,3,0.0);	t->set(8,3,1.5);
	t->set(0,4,0.2);	t->set(1,4,0.5);	t->set(2,4,1.0);	t->set(3,4,0.0);	t->set(4,4,0.0);	t->set(5,4,0.0);	t->set(6,4,0.0);	t->set(7,4,1.0);	t->set(8,4,1.4);
	t->table_pivot_column_number = 5;
	t->table_pivot_column_mask->A = 8+16+32+64+128;
	t->table_pivot_columns[0] = 3;
	t->table_pivot_columns[1] = 4;
	t->table_pivot_columns[2] = 5;
	t->table_pivot_columns[3] = 6;
	t->table_pivot_columns[4] = 7;

	Mask mask;
	mask.set_zero();
	mask.A = 1;
	mask.print();

	printhead(head,t->w);
	t->print();
	t->calculate_pivots(false);
	t->print_pivot_info();
	t->print_pivotable_info();
	printf("pivoting for non-zero z\n");
	t->pivot(t,2);
	t->apply_to_head(head,head);
	printf("\nWALKBACK SOLVING, should be 0.952381\n");
	
	for (int i=0; i<5; i++)
	printf("bilevel_solve: %f\n", walk_back(t, &mask, head, true));

	printf("\n\nWALKBACK SOLVE COMPLETE\n");
	printhead(head,t->w);
	t->print();
	t->print_pivot_info();

	t->free_data();
	free(t);
	free(head);
}

void test12() {
	printf("TEST12\n");
	double* head = (double*)calloc(sizeof(double),9);
	Table* t = (Table*)malloc(sizeof(Table));
	t->initialise_and_wipe(9, 5);
	head[2] = -1;

	t->set(0,0,1.0);	t->set(1,0,0.0);	t->set(2,0,-0.1);	t->set(3,0,1.0);	t->set(4,0,0.0);	t->set(5,0,0.0);	t->set(6,0,0.0);	t->set(7,0,0.0);	t->set(8,0,1.0);
	t->set(0,1,0.0);	t->set(1,1,1.0);	t->set(2,1,-0.1);	t->set(3,1,0.0);	t->set(4,1,1.0);	t->set(5,1,0.0);	t->set(6,1,0.0);	t->set(7,1,0.0);	t->set(8,1,1.0);
	t->set(0,2,-0.1);	t->set(1,2,-0.1);	t->set(2,2,1.0);	t->set(3,2,0.0);	t->set(4,2,0.0);	t->set(5,2,1.0);	t->set(6,2,0.0);	t->set(7,2,0.0);	t->set(8,2,1.0);
	t->set(0,3,0.5);	t->set(1,3,0.2);	t->set(2,3,1.0);	t->set(3,3,0.0);	t->set(4,3,0.0);	t->set(5,3,0.0);	t->set(6,3,1.0);	t->set(7,3,0.0);	t->set(8,3,1.5);
	t->set(0,4,0.2);	t->set(1,4,0.5);	t->set(2,4,1.0);	t->set(3,4,0.0);	t->set(4,4,0.0);	t->set(5,4,0.0);	t->set(6,4,0.0);	t->set(7,4,1.0);	t->set(8,4,1.4);
	t->table_pivot_column_number = 5;
	t->table_pivot_column_mask->A = 8+16+32+64+128;
	t->table_pivot_columns[0] = 3;
	t->table_pivot_columns[1] = 4;
	t->table_pivot_columns[2] = 5;
	t->table_pivot_columns[3] = 6;
	t->table_pivot_columns[4] = 7;

	Mask mask;
	mask.set_zero();
	mask.A = 1;
	mask.print();

	printhead(head,t->w);
	t->print();
	t->calculate_pivots(false);
	t->print_pivot_info();
	t->print_pivotable_info();
	printf("pivoting for non-zero z\n");
	t->pivot(t,2);
	t->apply_to_head(head,head);
	printf("\nBILEVEL SOLVING, should be 0.952381\n");
	
	for (int i=0; i<10; i++)
	printf("bilevel_solve: %f\n", bilevel_solve(t, &mask, head, false));

	printf("\n\nBILEVEL SOLVE COMPLETE\n");
	printhead(head,t->w);
	t->print();
	t->print_pivot_info();

	t->free_data();
	free(t);
	free(head);
}
void test13() {
	printf("TEST13\n");
	double* head = (double*)calloc(sizeof(double),9);
	Table* t = (Table*)malloc(sizeof(Table));
	t->initialise_and_wipe(9, 5);
	head[2] = -1;

	t->set(0,0,1.0);	t->set(1,0,0.0);	t->set(2,0,-0.1);	t->set(3,0,1.0);	t->set(4,0,0.0);	t->set(5,0,0.0);	t->set(6,0,0.0);	t->set(7,0,0.0);	t->set(8,0,1.0);
	t->set(0,1,0.0);	t->set(1,1,1.0);	t->set(2,1,-0.1);	t->set(3,1,0.0);	t->set(4,1,1.0);	t->set(5,1,0.0);	t->set(6,1,0.0);	t->set(7,1,0.0);	t->set(8,1,1.0);
	t->set(0,2,-0.1);	t->set(1,2,-0.1);	t->set(2,2,1.0);	t->set(3,2,0.0);	t->set(4,2,0.0);	t->set(5,2,1.0);	t->set(6,2,0.0);	t->set(7,2,0.0);	t->set(8,2,1.0);
	t->set(0,3,0.5);	t->set(1,3,0.2);	t->set(2,3,1.0);	t->set(3,3,0.0);	t->set(4,3,0.0);	t->set(5,3,0.0);	t->set(6,3,1.0);	t->set(7,3,0.0);	t->set(8,3,1.5);
	t->set(0,4,0.2);	t->set(1,4,0.5);	t->set(2,4,1.0);	t->set(3,4,0.0);	t->set(4,4,0.0);	t->set(5,4,0.0);	t->set(6,4,0.0);	t->set(7,4,1.0);	t->set(8,4,1.4);
	t->table_pivot_column_number = 5;
	t->table_pivot_column_mask->A = 8+16+32+64+128;
	t->table_pivot_columns[0] = 3;
	t->table_pivot_columns[1] = 4;
	t->table_pivot_columns[2] = 5;
	t->table_pivot_columns[3] = 6;
	t->table_pivot_columns[4] = 7;

	Mask mask;
	mask.set_zero();
	mask.A = 1;
	mask.print();

	printhead(head,t->w);
	t->print();
	t->calculate_pivots(false);
	t->print_pivot_info();
	t->print_pivotable_info();
	printf("pivoting for non-zero z\n");
	t->pivot(t,2);
	t->apply_to_head(head,head);
	printf("\nWALKBACK SOLVING, should be 0.952381\n");
	
	for (int i=0; i<5; i++)
	printf("bilevel_solve: %f\n", walk_back(t, &mask, head, false));

	printf("\n\nWALKBACK SOLVE COMPLETE\n");
	printhead(head,t->w);
	t->print();
	t->print_pivot_info();

	t->free_data();
	free(t);
	free(head);
}



void test14() {
	printf("TEST14\n");
	double* head = (double*)calloc(sizeof(double),9);
	Table* t = (Table*)malloc(sizeof(Table));
	t->initialise_and_wipe(9, 5);
	head[2] = -1;

	t->set(0,0,1.0);	t->set(1,0,0.0);	t->set(2,0,-0.1);	t->set(3,0,1.0);	t->set(4,0,0.0);	t->set(5,0,0.0);	t->set(6,0,0.0);	t->set(7,0,0.0);	t->set(8,0,1.0);
	t->set(0,1,0.0);	t->set(1,1,1.0);	t->set(2,1,-0.1);	t->set(3,1,0.0);	t->set(4,1,1.0);	t->set(5,1,0.0);	t->set(6,1,0.0);	t->set(7,1,0.0);	t->set(8,1,1.0);
	t->set(0,2,-0.1);	t->set(1,2,-0.1);	t->set(2,2,1.0);	t->set(3,2,0.0);	t->set(4,2,0.0);	t->set(5,2,1.0);	t->set(6,2,0.0);	t->set(7,2,0.0);	t->set(8,2,1.0);
	t->set(0,3,0.5);	t->set(1,3,0.2);	t->set(2,3,1.0);	t->set(3,3,0.0);	t->set(4,3,0.0);	t->set(5,3,0.0);	t->set(6,3,1.0);	t->set(7,3,0.0);	t->set(8,3,1.5);
	t->set(0,4,0.2);	t->set(1,4,0.5);	t->set(2,4,1.0);	t->set(3,4,0.0);	t->set(4,4,0.0);	t->set(5,4,0.0);	t->set(6,4,0.0);	t->set(7,4,1.0);	t->set(8,4,1.4);
	t->table_pivot_column_number = 5;
	t->table_pivot_column_mask->A = 8+16+32+64+128;
	t->table_pivot_columns[0] = 3;
	t->table_pivot_columns[1] = 4;
	t->table_pivot_columns[2] = 5;
	t->table_pivot_columns[3] = 6;
	t->table_pivot_columns[4] = 7;

	Mask mask;
	mask.set_zero();
	mask.A = 3;
	mask.print();

	printhead(head,t->w);
	t->print();
	t->calculate_pivots(false);
	t->print_pivot_info();
	t->print_pivotable_info();
	printf("pivoting for non-zero z\n");
	t->pivot(t,2);
	t->apply_to_head(head,head);
	printf("\nBILEVEL SOLVING, should be 0.654206\n");
	
	for (int i=0; i<10; i++)
	printf("bilevel_solve: %f\n", bilevel_solve(t, &mask, head, false));

	printf("\n\nBILEVEL SOLVE COMPLETE\n");
	printhead(head,t->w);
	t->print();
	t->print_pivot_info();

	t->free_data();
	free(t);
	free(head);
}
void test15() {
	printf("TEST15\n");
	double* head = (double*)calloc(sizeof(double),9);
	Table* t = (Table*)malloc(sizeof(Table));
	t->initialise_and_wipe(9, 5);
	head[2] = -1;

	t->set(0,0,1.0);	t->set(1,0,0.0);	t->set(2,0,-0.1);	t->set(3,0,1.0);	t->set(4,0,0.0);	t->set(5,0,0.0);	t->set(6,0,0.0);	t->set(7,0,0.0);	t->set(8,0,1.0);
	t->set(0,1,0.0);	t->set(1,1,1.0);	t->set(2,1,-0.1);	t->set(3,1,0.0);	t->set(4,1,1.0);	t->set(5,1,0.0);	t->set(6,1,0.0);	t->set(7,1,0.0);	t->set(8,1,1.0);
	t->set(0,2,-0.1);	t->set(1,2,-0.1);	t->set(2,2,1.0);	t->set(3,2,0.0);	t->set(4,2,0.0);	t->set(5,2,1.0);	t->set(6,2,0.0);	t->set(7,2,0.0);	t->set(8,2,1.0);
	t->set(0,3,0.5);	t->set(1,3,0.2);	t->set(2,3,1.0);	t->set(3,3,0.0);	t->set(4,3,0.0);	t->set(5,3,0.0);	t->set(6,3,1.0);	t->set(7,3,0.0);	t->set(8,3,1.5);
	t->set(0,4,0.2);	t->set(1,4,0.5);	t->set(2,4,1.0);	t->set(3,4,0.0);	t->set(4,4,0.0);	t->set(5,4,0.0);	t->set(6,4,0.0);	t->set(7,4,1.0);	t->set(8,4,1.4);
	t->table_pivot_column_number = 5;
	t->table_pivot_column_mask->A = 8+16+32+64+128;
	t->table_pivot_columns[0] = 3;
	t->table_pivot_columns[1] = 4;
	t->table_pivot_columns[2] = 5;
	t->table_pivot_columns[3] = 6;
	t->table_pivot_columns[4] = 7;

	Mask mask;
	mask.set_zero();
	mask.A = 3;
	mask.print();

	printhead(head,t->w);
	t->print();
	t->calculate_pivots(false);
	t->print_pivot_info();
	t->print_pivotable_info();
	printf("pivoting for non-zero z\n");
	t->pivot(t,2);
	t->apply_to_head(head,head);
	printf("\nWALKBACK SOLVING, should be 0.654206\n");
	
	for (int i=0; i<5; i++)
	printf("bilevel_solve: %f\n", walk_back(t, &mask, head, false));

	printf("\n\nWALKBACK SOLVE COMPLETE\n");
	printhead(head,t->w);
	t->print();
	t->print_pivot_info();

	t->free_data();
	free(t);
	free(head);
}

void test16() {
	printf("TEST16\n");
	double* head = (double*)calloc(sizeof(double),8);
	Table* t = (Table*)malloc(sizeof(Table));
	t->initialise_and_wipe(8, 3);
	head[0] = 10;	head[1] = -57;	head[2] = -9;	head[3] = -24;
	t->set(0,0,0.5);	t->set(1,0,-5.5);	t->set(2,0,-2.5);	t->set(3,0,9.0);	t->set(4,0,1.0);	t->set(5,0,0.0);	t->set(6,0,0.0);	t->set(7,0,0.0);
	t->set(0,1,0.5);	t->set(1,1,-1.5);	t->set(2,1,-0.5);	t->set(3,1,1.0);	t->set(4,1,0.0);	t->set(5,1,1.0);	t->set(6,1,0.0);	t->set(7,1,0.0);
	t->set(0,2,1.0);	t->set(1,2,0.0);	t->set(2,2,0.0);	t->set(3,2,0.0);	t->set(4,2,0.0);	t->set(5,2,0.0);	t->set(6,2,1.0);	t->set(7,2,1.0);

	Mask mask;
	mask.set_zero();
	mask.print();

	printhead(head,8);
	t->print();
	printf("subset_improvable: %i\n",t->check_subset_improvable(&mask, head, false)==true);
	printhead(head,8);
	t->print();
	
	printf("SIMPLEXING\n");
	t->simplex(head,false);
	printhead(head,8);
	t->print();
	printf("subset_improvable: %i\n",t->check_subset_improvable(&mask, head, false)==true);

	t->free_data();
	free(t);
	free(head);
}

void test17() {
	printf("TEST17\n");
	double* head = (double*)calloc(sizeof(double),8);
	Table* t = (Table*)malloc(sizeof(Table));
	t->initialise_and_wipe(8, 4);
	head[0] = -1.5;	head[1] = 0.1;	head[2] = -1.9;	head[3] = 2.1;
	t->set(0,0,1.0);	t->set(1,0,0.0);	t->set(2,0,0.0);	t->set(3,0,0.0);	t->set(4,0,1.0);	t->set(5,0,0.0);	t->set(6,0,-1.0);	t->set(7,0,0.0);
	t->set(0,1,-0.5);	t->set(1,1,1.0);	t->set(2,1,0.0);	t->set(3,1,0.0);	t->set(4,1,0.0);	t->set(5,1,0.5);	t->set(6,1,-0.5);	t->set(7,1,0.0);
	t->set(0,2,0.0);	t->set(1,2,0.0);	t->set(2,2,1.0);	t->set(3,2,0.0);	t->set(4,2,0.0);	t->set(5,2,0.0);	t->set(6,2,1.0);	t->set(7,2,1.0);
	t->set(0,3,0.5);	t->set(1,3,0.0);	t->set(2,3,0.0);	t->set(3,3,1.0);	t->set(4,3,0.0);	t->set(5,3,0.5);	t->set(6,3,0.5);	t->set(7,3,1.0);
	t->reverse_engineer_pivots();

	Mask mask;
	mask.set_zero();
	mask.set_bit(2,1);
	mask.set_bit(3,1);
	mask.print();

	printhead(head,8);
	t->print();
	
	printf("should be 1.7\n");
	printf("bilevel solving: %f\n",bilevel_solve(t, &mask, head, true));
	printf("walkback solving: %f\n",walk_back(t, &mask, head, true));

	printf("flipping\n");
	
	printf("should be -1.1\n");
	printf("bilevel solving: %f\n",bilevel_solve(t, &mask, head, false));
	printf("walkback solving: %f\n",walk_back(t, &mask, head, false));

	t->free_data();
	free(t);
	free(head);
}

void test18() {
	printf("TEST18\n");
	Table* t = (Table*)malloc(sizeof(Table));
	t->initialise_and_wipe(10, 6);
	t->set(0,0,1.0);	t->set(1,0,0.0);	t->set(2,0,0.0);	t->set(3,0,1.0);	t->set(4,0,0.0);	t->set(5,0,0.0);	t->set(6,0,0.0);	t->set(7,0,0.0);	t->set(8,0,0.0);	t->set(9,0,1.0);
	t->set(0,1,0.0);	t->set(1,1,1.0);	t->set(2,1,0.0);	t->set(3,1,0.0);	t->set(4,1,1.0);	t->set(5,1,0.0);	t->set(6,1,0.0);	t->set(7,1,0.0);	t->set(8,1,0.0);	t->set(9,1,1.0);
	t->set(0,2,0.0);	t->set(1,2,0.0);	t->set(2,2,1.0);	t->set(3,2,0.0);	t->set(4,2,0.0);	t->set(5,2,1.0);	t->set(6,2,0.0);	t->set(7,2,0.0);	t->set(8,2,0.0);	t->set(9,2,1.0);
	t->set(0,3,0.3);	t->set(1,3,0.3);	t->set(2,3,0.4);	t->set(3,3,0.0);	t->set(4,3,0.0);	t->set(5,3,0.0);	t->set(6,3,1.0);	t->set(7,3,0.0);	t->set(8,3,0.0);	t->set(9,3,1.0);
	t->set(0,4,0.4);	t->set(1,4,0.3);	t->set(2,4,0.3);	t->set(3,4,0.0);	t->set(4,4,0.0);	t->set(5,4,1.0);	t->set(6,4,0.0);	t->set(7,4,1.0);	t->set(8,4,0.0);	t->set(9,4,1.0);
	t->set(0,5,0.3);	t->set(1,5,0.4);	t->set(2,5,0.3);	t->set(3,5,0.0);	t->set(4,5,0.0);	t->set(5,5,1.0);	t->set(6,5,0.0);	t->set(7,5,0.0);	t->set(8,5,1.0);	t->set(9,5,1.0);
	t->reverse_engineer_pivots();
	t->print();
	t->pivot(t,0,0);
	t->pivot(t,1,1);
	t->pivot(t,2,2);
	printf("\n");
	t->print();

	Mask_Memory* masks = (Mask_Memory*)malloc(sizeof(Mask_Memory));
	masks->setup(MEMORY_INITIAL_SIZE);
	t->add_degenerate_pivot_masks(masks);
	printf("masks %i\n",masks->length);
	masks->print_all();
	printf("should be 20\n");
}

void test19() {
	printf("TEST19\n");
	Mask coalition;
	coalition.set_zero();
	coalition.A = 20;
	double* head = (double*)calloc(sizeof(double),10);
	Table* t = (Table*)malloc(sizeof(Table));
	t->initialise_and_wipe(9, 3);
		
	head[0]=0.7;	head[1]=-0.8;	head[2]=-1.9;	head[3]=1.3;	head[4]=-0.4;	head[5]=1.5;
//01001100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
	t->set(0,0,1);	t->set(1,0,0);	t->set(2,0,1);	t->set(3,0,1);	t->set(4,0,1);	t->set(5,0,0);	t->set(6,0,1);	t->set(7,0,-1);	t->set(8,0,45);
	t->set(0,1,0);	t->set(1,1,0);	t->set(2,1,0);	t->set(3,1,0);	t->set(4,1,0);	t->set(5,1,1);	t->set(6,1,0);	t->set(7,1,1);	t->set(8,1,32);
	t->set(0,2,0);	t->set(1,2,1);	t->set(2,2,0);	t->set(3,2,0);	t->set(4,2,0);	t->set(5,2,0);	t->set(6,2,1);	t->set(7,2,0);	t->set(8,2,77);

	t->table_pivot_columns[0]=4;
	t->table_pivot_columns[1]=5;
	t->table_pivot_columns[2]=1;
	t->table_pivot_column_mask->set_zero();
	t->table_pivot_column_mask->A=50;
	t->table_pivot_column_number=3;

	t->calculate_pivots(false);

	printhead(head,t->w);
	t->print();
	t->print_pivot_info();
	t->print_pivotable_info();
	
	printf("should all be the same:\n");
	printf("%f\n",alt_bilevel_solve(t, &coalition, head, false));
	printf("%f\n",bilevel_solve(t, &coalition, head, false));
	printf("%f\n",walk_back(t, &coalition, head, false));
	
	free(head);
	t->free_data();
	free(t);
	
}

int main() {
	//test1();
	//test2();
	//test3();
	//test4();
	//test5();
	//test6();
	//test7();
	//test8();
	//test9();
	//test10();
	//test11();
	//test12();
	//test13();
	//test14();
	//test15();
	//test16();
	//test17();
	//test18();
	test19();
	printf("hello\n");
	return 0;
}
