#include <stdbool.h>
#include <float.h>

#include "mask_memory.cpp"
#include "row_memory.cpp"
#include "table.cpp"

Table* t = NULL;
Table* anticoalition_table = NULL;
Table* coalition_table = NULL;

double* master_head = NULL;
double* coalition_head = NULL;
double* anticoalition_head = NULL;
int players;

void free_memory() {
	if (anticoalition_table != NULL) {
		anticoalition_table->free_data();
		free(anticoalition_table);
		anticoalition_table = NULL;
	}
	if (coalition_table != NULL) {
		coalition_table->free_data();
		free(coalition_table);
		coalition_table = NULL;
	}
	if (master_head != NULL) {
		free(master_head);
		master_head = NULL;
	}
	if (coalition_head != NULL) {
		free(coalition_head);
		coalition_head = NULL;
	}
	if (anticoalition_head != NULL) {
		free(anticoalition_head);
		anticoalition_head = NULL;
	}
	if (t!=NULL) {
		t->free_data();
		free(t);
		t = NULL;
	}
}


void setup_memory(Table* t) {
	anticoalition_table = (Table*)malloc(sizeof(Table));
	anticoalition_table->initialise(t->w,t->h);
	anticoalition_table->load(t);
	coalition_table = (Table*)malloc(sizeof(Table));
	coalition_table->initialise(t->w,t->h);
	coalition_table->load(t);
	master_head = (double*)calloc(sizeof(double),t->w+1);
	coalition_head = (double*)calloc(sizeof(double),t->w+1);
	anticoalition_head = (double*)calloc(sizeof(double),t->w+1);
}


bool artificial_variables_simplex(Table* t, int artificial_variables) {
	double* head = (double*)calloc(sizeof(double),t->w);
	for (int i=t->w-1-artificial_variables; i<t->w-1; i++)
		head[i] = -1.0;
	if (t->simplex(head,false,NULL)>TINY) {
		printf("ERROR: INFEASIBIILTY\n");
		return false;
	}
	free(head);
	for (int i=0; i<artificial_variables; i++)
		t->delete_column(t->w-2,NULL);
	return true;
}

void equation_pruning(Table* t, int* slackness_columns) {
	double* head = (double*)calloc(sizeof(double),t->w);
	int original_h = t->h;
	int jj=0;
	for (int j=0; j<original_h; j++) {
		if (slackness_columns[j]!=-1) {
			for (int i=0;i<t->w;i++) {
				if (i==slackness_columns[j])
					head[i]=-1;
				else
					head[i]=0;
			}
			if (t->simplex(head,false,NULL)>TINY) {
				t->delete_row(jj);
				t->delete_column(slackness_columns[j],NULL);
				for (int jjj=0;jjj<original_h;jjj++)
					if (slackness_columns[jjj]>slackness_columns[j])
						slackness_columns[jjj] -= 1;
			} else
				jj++;
		} else
			jj++;
	}
	free(head);
}




