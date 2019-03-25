
struct ValueLinked {
	ValueLinked *next;
	double v;
};

struct SortedList {
	ValueLinked *root;
	
	void add(ValueLinked *l);
	void* pop();
	void destroy();
	void print_all();
	void setup();

	SortedList();
	~SortedList();
};

void SortedList::print_all() {
	printf("SORTEDLIST contents:\n");
	ValueLinked* l = this->root;
	if (l==NULL) {
		printf("\t--EMPTY\n");
		return;
	}
	printf("\t%f\n",l->v);
	while (l->next != NULL) {
		l = l->next;
		printf("\t%f\n",l->v);
	}
}

SortedList::SortedList() {
	this->setup();
}
SortedList::~SortedList() {
	this->destroy();
}

void SortedList::setup() {
	this->root = NULL;
}

void SortedList::destroy() {
	ValueLinked* current;
	ValueLinked* next;
	current = this->root;
	while(current != NULL) {
		next = current->next;
		free(current);
		current = next;
	}
	this->root = NULL;
}
void SortedList::add(ValueLinked *l) {
	ValueLinked* current;
	ValueLinked* previous;
	current = this->root;
	if (current==NULL) {
		l->next = NULL;
		this->root = l;
		return;
	}
	double v = l->v;
	if (current->v < v) {
		this->root = l;
		l->next = current;
		return;
	}
	previous = current;
	current = current->next;
	while (current!=NULL) {
		if (current->v < v) {
			previous->next = l;
			l->next = current;
			return;
		}
		previous = current;
		current = current->next;
	}
	previous->next = l;
	l->next = NULL;
}
void* SortedList::pop() {
	if (this->root == NULL) {
		return NULL;
	}
	ValueLinked* l;
	l = this->root;
	this->root = l->next;
	return l;
}
