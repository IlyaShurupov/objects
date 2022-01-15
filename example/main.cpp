

#include "object/object.h"
#include "primitives/primitives.h"

int main() {

	objects_api* oh = objects_init();
	primitives_define_types(oh);

	Object* myint = oh->create("int");
	
	oh->set(myint, alni(12));

	oh->save(myint, "C:\\myint.o");
	
	Object* saved_int = oh->load("C:\\myint.o");
	
	NDO_CAST(IntObject, saved_int)->val;
}