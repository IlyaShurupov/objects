
#include "object/object.h"
#include "primitives/primitives.h"

class oeditor {
	
	objects_api* oh;

	DictObject* root;
	
	Object* active;
	stack<Object*> path;
	Object* clipboard = NULL;

public:
	oeditor();

	void oexplorer();
	void oproperties();

	void draw();
	void test();

	~oeditor();
};