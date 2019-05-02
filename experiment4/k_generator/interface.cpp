#include <Python.h>

#define TINY 0.0000001
#define MEMORY_INITIAL_SIZE 131072

#define DEBUG 0
#define PRUNING 1

#include <utils.cpp>
#include <solver.cpp>
#include <sub_interface.cpp>


PyObject* python_error(const char *ss) {
	PyErr_SetString(PyExc_TypeError, ss);
	return NULL;
}

static PyObject* setup_solver(PyObject* self, PyObject* args) {
	PyObject *ge_array, *le_array, *eq_array, *input_head;

	if (!PyArg_ParseTuple(args, "OOOO", &le_array, &eq_array, &ge_array, &input_head)) {
		return python_error("Argument must be two 2D python arrays, and a python array");
	}

	#if DEBUG==1
		printf("SETTING UP SOLVER\n");
		printf("FREEING ANY PREVIOUS MEMORY\n");
	#endif
	free_memory();
	
	int artificial_variables = 0;
	int* slackness_columns;
	// analyze tables and parse them into a single table with slack,excess and artificial variables.
	t = analyse_pyobject_tables(
		le_array,
		eq_array,
		ge_array,
		&artificial_variables,
		&slackness_columns,
		&players);
	if (t==NULL)
		return NULL;

	#if DEBUG==1
		printf("Analysed table:\n\tw=%i\th=%i\tAV=%i\n",t->w,t->h,artificial_variables);
		printf("\nTable Loaded:\n");
		t->print();
		printf("Slackness Columns:\n");
		for (int j=0; j<t->h; j++) printf("row %i, column %i\n",j,slackness_columns[j]);
	#endif
	
	// do simplex on artificial variables to determine initial feasible solution
	#if DEBUG==1
		printf("\nConducting Artificial variable simplex to get initial feasible solution:\n");
	#endif
	if (artificial_variables_simplex(t,artificial_variables) == false)
		return NULL;
	#if DEBUG==1
		printf("Artificial variable simplex resulting in table:\n");
		t->print();
		t->print_pivot_info();
	#endif

	// prune redundant columns/rows from the table representing redundant constraints
	#if PRUNING==1
		#if DEBUG==1
			printf("\nConducting Equation Pruning of redundant columns/rows from the table:\n");
			equation_pruning(t, slackness_columns);
			printf("Equation Pruning resulting in table:\n");
			t->print();
			printf("\n");
		#else
			equation_pruning(t, slackness_columns);
		#endif
	#endif
	free(slackness_columns);
	
	#if DEBUG==1
		printf("Loading Solver Working Memory\n");
	#endif
	setup_memory(t);

	#if DEBUG==1
		printf("SETTING UP HEAD\n");
	#endif
	if (set_head(input_head, t->w)==-1)
		return python_error("bad head.");
	#if DEBUG==1
		printf("Head set to:\n");
		printhead(master_head,t->w);
	#endif

	#if DEBUG==1
		printf("Setup Solver routine complete\n\n");
	#endif

	return PyFloat_FromDouble(1);
}


static PyObject* solve(PyObject* self, PyObject* args) {
	Mask player_mask, auxiliary_mask, coalition, anticoalition;
	player_mask.set_ones(players);
	auxiliary_mask.set_ones(t->w);
	auxiliary_mask.set(auxiliary_mask^player_mask);
	coalition.set_zero();
	anticoalition.set_zero();
	#if DEBUG==1
		printf("SOLVING\n");
	#endif
	PyObject *obj;

	#if DEBUG==1
		printf("parsing coalition\n");
	#endif
	Py_ssize_t TupleSize = PyTuple_Size(args);
	if(TupleSize != 1)
		return python_error("You must supply one argument.");
	obj = PyTuple_GetItem(args,0);
	if (PyNumber_Check(obj) != 1)
		return python_error("Non-numeric argument.");
	coalition.A = PyLong_AsUnsignedLong(PyNumber_Long(obj));
	if (PyErr_Occurred()!= NULL)
		return python_error("Non-numeric argument...");
	if (((~player_mask)&coalition).non_zero())
		return python_error("coalition too big!");
	anticoalition.set(player_mask^coalition);
	
	#if DEBUG==1
		printf("coalition: \t");
		coalition.print();
		printf("anticoalition: \t");
		anticoalition.print();
		printf("player_mask: \t");
		player_mask.print();
		printf("auxiliary_mask: \t");
		auxiliary_mask.print();
	#endif

	#if DEBUG==1
		printf("applying coalition\n");
	#endif

	double epsilon = 0.001;

	double r = 0;
	for (int i=0; i< t->w; i++) {
		if (coalition.get_bit(i)==1) {
			temporary_head[i] = -1*master_head[i];
		} else {
			temporary_head[i] = -epsilon*master_head[i];
		}
	}
	#if DEBUG==1
		printf("about to compute on coalition\n");
	#endif
	prev_min_table->simplex(temporary_head, true);
	for (int i=0; i< t->w; i++) {
		if (coalition.get_bit(i)==1) {
			temporary_head[i] = -epsilon*master_head[i];
		} else {
			temporary_head[i] = -1*master_head[i];
		}
	}
	#if DEBUG==1
		printf("about to compute on anticoalition\n");
	#endif
	prev_max_table->simplex(temporary_head, true);

	double coalition_value = 0;
	double anticoalition_value = 0;

	for (int i=0; i< t->w; i++) {
		if (coalition.get_bit(i)==1) {
			temporary_head[i] = -1*master_head[i];
		} else {
			temporary_head[i] = 0;
		}
	}
	prev_min_table->apply_to_head(temporary_head,temporary_head);
	coalition_value += 0.5*temporary_head[t->w-1];
	prev_max_table->apply_to_head(temporary_head,temporary_head);
	coalition_value += 0.5*temporary_head[t->w-1];

	for (int i=0; i< t->w; i++) {
		if (coalition.get_bit(i)==1) {
			temporary_head[i] = 0;
		} else {
			temporary_head[i] = -1*master_head[i];
		}
	}
	prev_min_table->apply_to_head(temporary_head,temporary_head);
	anticoalition_value += 0.5*temporary_head[t->w-1];
	prev_max_table->apply_to_head(temporary_head,temporary_head);
	anticoalition_value += 0.5*temporary_head[t->w-1];

	r = coalition_value-anticoalition_value;
	
	return PyFloat_FromDouble(r);
}

static PyObject* spruik(PyObject* self, PyObject* args) {
	prev_max_table->load(t);
	prev_min_table->load(t);
	return PyFloat_FromDouble(1);
}
static PyObject* destroy(PyObject* self, PyObject* args) {
	free_memory();
	return PyFloat_FromDouble(1);
}



static char setup_solver_docs[] = "setup_solver(): does all the setup for the solver apparatus, conducting Phase I feasibility solving and initialising all memory.\n";
static char solve_docs[] = "solve(): for a coalition calculate the minimax value.\n";
static char spruik_docs[] = "asdfas.\n";
static char destroy_docs[] = "frees all memory.\n";

static PyMethodDef bilevel_solver_funcs[] = {
	{"setup_solver", (PyCFunction)setup_solver, 
		METH_VARARGS, setup_solver_docs},
	{"solve", (PyCFunction)solve, 
		METH_VARARGS, solve_docs},
	{"spruik", (PyCFunction)spruik, 
		METH_NOARGS, spruik_docs},
	{"destroy", (PyCFunction)destroy, 
		METH_NOARGS, destroy_docs},
		{NULL}
};
extern "C" {
	void initbilevel_solver(void) {
		Py_InitModule("bilevel_solver", bilevel_solver_funcs);
	}
}
