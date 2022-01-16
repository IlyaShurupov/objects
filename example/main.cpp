

#include "object/object.h"
#include "primitives/primitives.h"

int main() {

	objects_api* oh = objects_init();
	primitives_define_types(oh);

	Object* myint = oh->create("int");
	
	oh->set(myint, alni(12));

	oh->save(myint, "A:\\data\\tmp\\myint.o");
	
	Object* saved_int = oh->load("A:\\data\\tmp\\myint.o");
	
	NDO_CAST(IntObject, saved_int)->val;
}