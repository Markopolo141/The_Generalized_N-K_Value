

int set_head(PyObject *input_head, int table_width) {
	for (int i=0; i<table_width; i++)
		master_head[i]=0;
	int length = (int)PyList_Size(input_head);
	if (length>table_width) {
		return -1;
	}
	for (int i=0; i<length; i++)
		master_head[i] = PyFloat_AsDouble(PyList_GetItem(input_head, i));
	return 0;
}


int analyse_pyobject_table(
		PyObject *input_array,
		int* w,
		int* h,
		unsigned long* invert_mask,
		unsigned long* slackness_mask,
		unsigned long* excess_mask,
		unsigned long* artificial_mask,
		int* artificial_variables,
		int* slackness_variables,
		int* excess_variables) {

	*w = -1;
	*h = (int)PyList_Size(input_array);
	*invert_mask = 0;
	*slackness_mask = 0;
	*excess_mask = 0;
	*artificial_mask = 0;
	*artificial_variables = 0;
	*slackness_variables = 0;
	*excess_variables = 0;
	int temp;
	PyObject *row;

	for (int i = 0; i < *h; i++) { //parse through the python data and get the metrics of the numerical rows.
		row = PyList_GetItem(input_array, i);
		temp = (int)PyList_Size(row);
		if (*w==-1) {
			*w = temp; //check square and set width.
		} else {
			if (*w!=temp) {
				PyErr_Format(PyExc_TypeError, "Argument to %s must be a square array", __FUNCTION__);
				return -1;
			}
		}
		int comparrison = PyInt_AsLong(PyList_GetItem(row, *w-1));
		if (PyFloat_AsDouble(PyList_GetItem(row, *w-2))>=0) {
			if (comparrison == 0) { // an equality constraint
				*artificial_variables += 1;
				*artificial_mask |= ((unsigned long)1)<<i;
			} else if (comparrison == 1) { // a greater-than constraint
				*artificial_variables += 1;
				*artificial_mask |= ((unsigned long)1)<<i;
				*excess_variables += 1;
				*excess_mask |= ((unsigned long)1)<<i;
			} else if (comparrison == -1) { // a less-than constraint
				*slackness_variables += 1;
				*slackness_mask |= ((unsigned long)1)<<i;
			} else {
				PyErr_Format(PyExc_TypeError, "Argument to %s must be a square array with right-side values of 0 or 1 or -1", __FUNCTION__);
				return -1;
			}
		} else {
			*invert_mask |= ((unsigned long)1)<<i;
			if (comparrison == 0) { // an equality constraint
				*artificial_variables += 1;
				*artificial_mask |= ((unsigned long)1)<<i;
			} else if (comparrison == 1) { // effectively a less-than constraint
				*slackness_variables += 1;
				*slackness_mask |= ((unsigned long)1)<<i;
			} else if (comparrison == -1) {  // effectively a greater-than constraint
				*artificial_variables += 1;
				*artificial_mask |= ((unsigned long)1)<<i;
				*excess_variables += 1;
				*excess_mask |= ((unsigned long)1)<<i;
			} else {
				PyErr_Format(PyExc_TypeError, "Argument to %s must be a square array with right-side values of 0 or 1 or -1", __FUNCTION__);
				return -1;
			}
		}
	}
	return 0;
}



Table* construct_table_from_analysis(
		PyObject *input_array,
		int w,
		int h,
		unsigned long invert_mask,
		unsigned long slackness_mask,
		unsigned long excess_mask,
		unsigned long artificial_mask,
		int artificial_variables,
		int slackness_variables,
		int excess_variables,
		int** slackness_columns) {

	Table* t; // construct the table
	t = (Table*)malloc(sizeof(Table));
	t->initialise_and_wipe(w-1+artificial_variables+slackness_variables+excess_variables, h);

	*slackness_columns = (int*)calloc(sizeof(int),h);  // construct holder for slackness columns
	for (int i=0;i<h;i++) (*slackness_columns)[i]=-1;

	// starting column numbers for slacks,excesses and artificials
	int s = w-2;
	int e = w-2+slackness_variables;
	int a = w-2+slackness_variables+excess_variables;
	
	for (int j=0; j<h; j++) { //for every row
		double v;
		for (int i=0; i<w-2; i++) {  // load in all the numeric entities
			v = PyFloat_AsDouble(PyList_GetItem(PyList_GetItem(input_array,j),i));
			if (invert_mask&(((unsigned long)1)<<j)) {
				t->set(i, j, -v);
			} else {
				t->set(i, j, v);
			}
		}
		v = PyFloat_AsDouble(PyList_GetItem(PyList_GetItem(input_array,j),w-2));
		if (invert_mask&(((unsigned long)1)<<j)) { // rightmost column
			t->set(w-2+artificial_variables+slackness_variables+excess_variables, j, -v);
		} else {
			t->set(w-2+artificial_variables+slackness_variables+excess_variables, j, v);
		}
		if (slackness_mask&(((unsigned long)1)<<j)) { // slackness variables
			t->set(s,j,1);
			t->table_pivot_columns[j] = s;
			t->table_pivot_column_mask |= ((unsigned long)1)<<s;
			t->table_pivot_column_number += 1;
			(*slackness_columns)[j]=s;
			s++;
		}
		if (excess_mask&(((unsigned long)1)<<j)) { // excess variables
			t->set(e,j,-1);
			(*slackness_columns)[j]=e;
			e++;
		}
		if (artificial_mask&(((unsigned long)1)<<j)) { // artificial varibles for phase I
			t->set(a,j,1);
			t->table_pivot_columns[j] = a;
			t->table_pivot_column_mask |= ((unsigned long)1)<<a;
			t->table_pivot_column_number += 1;
			a++;
		}
	}
	return t;
}



