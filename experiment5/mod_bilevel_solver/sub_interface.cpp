

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


int get_py_object_width(PyObject *array) {
	int h=(int)PyList_Size(array);
	int w=-2;
	for (int i = 0; i < h; i++) { //parse through the python data and get the metrics of the numerical rows.
		int temp = (int)PyList_Size(PyList_GetItem(array, i));
		if (w==-2) {
			w = temp; //check square and set width.
		} else if (w!=temp) {
			return -1;
		}
	}
	return w;
}


Table* analyse_pyobject_tables(
		PyObject *le_array,
		PyObject *eq_array,
		PyObject *ge_array,
		int* artificial_variables,
		int** slackness_columns,
		int* players) {
	
	int le_h = (int)PyList_Size(le_array); // get heights
	int eq_h = (int)PyList_Size(eq_array);
	int ge_h = (int)PyList_Size(ge_array);
	int h = le_h+eq_h+ge_h;
	*artificial_variables = ge_h+eq_h;

	int le_w = get_py_object_width(le_array); // get width and check all tables have same width
	int eq_w = get_py_object_width(eq_array);
	int ge_w = get_py_object_width(ge_array);
	int w = max_3(le_w,eq_w,ge_w);
	if (  ((le_w!=w)&&(le_w!=-2))
		||((eq_w!=w)&&(eq_w!=-2))
		||((ge_w!=w)&&(ge_w!=-2)) ) {
		PyErr_Format(PyExc_TypeError, "Argument to %s must be 2D arrays that have same consistent width", __FUNCTION__);
		return NULL;
	}
	
	#if DEBUG==1
		printf("parsing all tables:\n");
		printf("le_w=%i, eq_w=%i, ge_w=%i, w=%i\n",le_w,eq_w,ge_w,w);
		printf("le_h=%i, eq_h=%i, ge_h=%i, h=%i\n",le_h,eq_h,ge_h,h);
	#endif

	
	int ww = w+le_h+eq_h+ge_h*2; //assign width of the table and the number of players
	*players = w-1;

	*slackness_columns = (int*)calloc(sizeof(int),h);  // construct holder for slackness columns
	for (int i=0;i<h;i++) (*slackness_columns)[i]=-1;

	Table* t; // construct the table to appropriate size
	t = (Table*)malloc(sizeof(Table));
	t->initialise(ww, h);
	
	PyObject *row;
	int r = 0;
	double v;
	for (int j=0; j<le_h; j++) {
		row = PyList_GetItem(le_array, j);
		for (int i=0; i<w-1; i++) {
			v = PyFloat_AsDouble(PyList_GetItem(row,i));
			t->set(i,r,v);
		}
		v = PyFloat_AsDouble(PyList_GetItem(row,w-1));
		if (v<0) {
			PyErr_Format(PyExc_TypeError, "Argument to %s must be 2D arrays that have right hand column >= 0", __FUNCTION__);
			return NULL;
		}
		t->set(ww-1,r,v);
		t->set(w+r-1,r,1.0);
		t->table_pivot_columns[r]=w+r-1;
		t->table_pivot_column_mask->set_bit(w+r-1,1);
		(*slackness_columns)[r]=w+r-1;
		r++;
	}
	for (int j=0; j<eq_h; j++) {
		row = PyList_GetItem(eq_array, j);
		for (int i=0; i<w-1; i++) {
			v = PyFloat_AsDouble(PyList_GetItem(row,i));
			t->set(i,r,v);
		}
		v = PyFloat_AsDouble(PyList_GetItem(row,w-1));
		if (v<0) {
			PyErr_Format(PyExc_TypeError, "Argument to %s must be 2D arrays that have right hand column >= 0", __FUNCTION__);
			return NULL;
		}
		t->set(ww-1,r,v);
		t->set(w+r+ge_h-1,r,1.0);
		t->table_pivot_columns[r]=w+r+ge_h-1;
		t->table_pivot_column_mask->set_bit(w+r+ge_h-1,1);
		r++;
	}	
	for (int j=0; j<ge_h; j++) {
		row = PyList_GetItem(ge_array, j);
		for (int i=0; i<w-1; i++) {
			v = PyFloat_AsDouble(PyList_GetItem(row,i));
			t->set(i,r,v);
		}
		v = PyFloat_AsDouble(PyList_GetItem(row,w-1));
		if (v<0) {
			PyErr_Format(PyExc_TypeError, "Argument to %s must be 2D arrays that have right hand column >= 0", __FUNCTION__);
			return NULL;
		}
		t->set(ww-1,r,v);
		t->set(w+r-eq_h-1,r,-1.0);
		t->set(w+r+eq_h-1,r,1.0);
		t->table_pivot_columns[r]=w+r+eq_h-1;
		t->table_pivot_column_mask->set_bit(w+r+eq_h-1,1);
		(*slackness_columns)[r]=w+r-eq_h-1;
		r++;
	}
	t->table_pivot_column_number = h;
	return t;
}


Table* analyse_pyobject_table(PyObject *array) {
	int h = (int)PyList_Size(array); // get heights
	int w = get_py_object_width(array); // get width and check all tables have same width
	if (w<1) {
		PyErr_Format(PyExc_TypeError, "Argument to %s must be 2D arrays that have same consistent width", __FUNCTION__);
		return NULL;
	}
	Table* t; // construct the table to appropriate size
	t = (Table*)malloc(sizeof(Table));
	t->initialise(w, h);
	
	PyObject *row;
	double v;
	for (int j=0; j<h; j++) {
		row = PyList_GetItem(array, j);
		for (int i=0; i<w; i++) {
			v = PyFloat_AsDouble(PyList_GetItem(row,i));
			t->set(i,j,v);
		}
	}
	return t;
}


