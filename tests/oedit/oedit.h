
#include "object/object.h"
#include "primitives/primitives.h"

#include "gl.h"
#include "window.h"

#include "DebugGui.h"

struct object_path {
	object_path(Object* obj, string id) : obj(obj), id(id) {
	}
	object_path() {
		obj = NULL;
	}

	operator bool() { return obj; }
	Object* obj;
	string id;
};

class oeditor {
	ogl::opengl gl;
	ogl::window window;
	DebugGui gui = DebugGui(window.geth());

	objects_api* oh;

	DictObject* root;
	
	Object* active;
	stack<object_path> path;
	Object* clipboard = NULL;

public:
	oeditor();

	void oexplorer();
	void oproperties(const ObjectType*);

	void run();
	void test();

	~oeditor();
};