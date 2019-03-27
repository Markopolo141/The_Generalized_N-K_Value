#include <Python.h>

#define TINY 0.000001
#define MEMORY_INITIAL_SIZE 131072
inline double SNAPTOZERO(double a) {return a < TINY ? (-a < TINY ? 0 : a) : a;}

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
	PyObject *input_array, *input_head;

	if (!PyArg_ParseTuple(args, "OO", &input_array, &input_head)) {
		return python_error("Argument must be a square python array, and a python array, and an integer 1 to enable ts else ts is disabled");
	}

	#if DEBUG==1
		printf("SETTING UP SOLVER\n");
		printf("FREEING ANY PREVIOUS MEMORY\n");
	#endif
	free_memory();
	primary_setup_memory();
	
	int h = 0;
	int w = 0;
	int artificial_variables = 0;
	int slackness_variables = 0;
	int excess_variables = 0;
	Mask invert_mask;
	Mask slackness_mask;
	Mask excess_mask;
	Mask artificial_mask;

	// analyse submitted structure from pyobject for details about rows and columns and their masks
	if (analyse_pyobject_table(
		input_array,
		&w,
		&h,
		&invert_mask,
		&slackness_mask,
		&excess_mask,
		&artificial_mask,
		&artificial_variables,
		&slackness_variables,
		&excess_variables)==-1)
		return NULL;

	#if DEBUG==1
		printf("Analysed table:\n\tw=%i\th=%i\tSV=%i\tEV=%i\tAV=%i\n",w,h,slackness_variables, excess_variables, artificial_variables);
		printf("\tInvert: \t");
		invert_mask.print();
		printf("\tSlackness:\t");
		slackness_mask.print();
		printf("\tExcess: \t");
		excess_mask.print();
		printf("\tArtificial:\t");
		artificial_mask.print();
	#endif

	// actually construct the table from the pyobject using analysed structure, additionally determine the slackness columns
	int* slackness_columns;
	t = construct_table_from_analysis(
		input_array,
		w,
		h,
		invert_mask,
		slackness_mask,
		excess_mask,
		artificial_mask,
		artificial_variables,
		slackness_variables,
		excess_variables,
		&slackness_columns);

	#if DEBUG==1
		printf("\nTable Loaded:\n");
		t->print();
		printf("Slackness Columns:\n");
		for (int j=0; j<h; j++) printf("row %i, column %i\n",j,slackness_columns[j]);
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
	setup_memory(t,w-2);

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
	unsigned long coalition = PyLong_AsUnsignedLong(PyNumber_Long(obj));
	unsigned long anticoalition, mask;
	if (PyErr_Occurred()!= NULL)
		return python_error("Non-numeric argument...");
	
	#if DEBUG==1
		printf("original coalition: \t");
		printbin(coalition);
		printf("\n");
	#endif
	if (analyse_coalition(&coalition,&anticoalition,&mask)==-1)
		return python_error("coalition too big!");
	#if DEBUG==1
		printf("analysed anticoalition:\t");
		printbin(anticoalition);
		printf("\n");
	#endif

	#if DEBUG==1
		printf("applying coalition\n");
	#endif
	apply_coalition(coalition);
	Mask player_mask, anticoalition_player_mask, coalition_player_mask, coalition_mask, anticoalition_mask;
	player_mask.set_ones(prev_max_table->w-1);
	anticoalition_player_mask = player_mask;
	anticoalition_player_mask.A &= (~coalition);
	coalition_player_mask = player_mask;
	coalition_player_mask.A &= (~anticoalition);
	anticoalition_mask.set_zero();
	anticoalition_mask.A = anticoalition;
	coalition_mask.set_zero();
	coalition_mask.A = coalition;

	#if DEBUG==1
		printf("new temporary_head: ");
		printhead(temporary_head, prev_max_table->w);
		printf("about to simplex from mask: ");
		prev_max_table->table_pivot_column_mask->print();
	#endif
	
	prev_max_table->apply_to_head(temporary_head,temporary_head);
	#if DEBUG==1
		printf("temporary_head manipulated: ");
		printhead(temporary_head, prev_max_table->w);
		double simplex_max = simplex(prev_max_table, temporary_head, true);
		printf("simplex maximum: %f\n", simplex_max);
		printf("pivoted to mask: ");
		prev_max_table->table_pivot_column_mask->print();
		prev_max_table->print();
		printf("about to walk_back from simplex point\n");
	#else
		simplex(prev_max_table, temporary_head, true);
	#endif
	
	#if DEBUG==1
		printf("about to walkback on coalition\n");
	#endif
	double r;
	//r = temporary_head[prev_max_table->w-1];
	//printhead(temporary_head,prev_max_table->w);
	//prev_max_table->table_pivot_column_mask->print();
	//prev_max_table->print();
	//r = walk_back(prev_max_table, &coalition_mask, &anticoalition_player_mask, temporary_head);

	#if DEBUG==1
		printf("applying anticoalition\n");
	#endif
	apply_coalition(anticoalition);

	#if DEBUG==1
		printf("new temporary_head: ");
		printhead(temporary_head, prev_min_table->w);
		printf("about to simplex from mask: ");
		prev_min_table->table_pivot_column_mask.print();
	#endif
	
	prev_min_table->apply_to_head(temporary_head,temporary_head);
	#if DEBUG==1
		printf("temporary_head manipulated: ");
		printhead(temporary_head, prev_min_table->w);
		simplex_max = simplex(prev_min_table, temporary_head, true);
		printf("simplex maximum: %f\n", simplex_max);
		printf("pivoted to mask: ");
		prev_min_table->table_pivot_column_mask.print();
		prev_min_table->print();
		printf("about to walk_back from simplex point\n");
	#else
		simplex(prev_min_table, temporary_head, true);
	#endif
	
	#if DEBUG==1
		printf("about to walkback on anticoalition\n");
	#endif

	//r += 0.5*r - 0.5*temporary_head[prev_max_table->w-1];
	//r = 0.5*r - 0.5*walk_back(prev_min_table, &anticoalition_mask, &coalition_player_mask, temporary_head);
	r = walk_back(prev_min_table, &anticoalition_mask, &coalition_player_mask, temporary_head);
	
	#if DEBUG==1
		printf("finished %f\n",r);
	#endif

	return PyFloat_FromDouble(r);
}



static PyObject* spruik(PyObject* self, PyObject* args) {
	prev_max_table->load(t);
	prev_min_table->load(t);
	
	return PyFloat_FromDouble(1);
}




static char setup_solver_docs[] =
	"setup_solver(): does all the setup for the solver apparatus, conducting Phase I feasibility solving and initialising all memory.\n";
static char solve_docs[] =
	"solve(): for a coalition calculate the minimax value.\n";
static char spruik_docs[] =
	"asdfas.\n";

static PyMethodDef bilevel_solver_funcs[] = {
	{"setup_solver", (PyCFunction)setup_solver, 
		METH_VARARGS, setup_solver_docs},
	{"solve", (PyCFunction)solve, 
		METH_VARARGS, solve_docs},
	{"spruik", (PyCFunction)spruik, 
		METH_NOARGS, spruik_docs},
		{NULL}
};

extern "C" {
	void initbilevel_solver(void) {
		Py_InitModule("bilevel_solver", bilevel_solver_funcs);
	}
}
