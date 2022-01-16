
#pragma once

#include "object/object.h"

enum ListMethods {
	LISTOBJECT_PUSH_BACK,
	LISTOBJECT_GET_LENGTH,
};

struct ListObject : Object {
	List<Object*> items;

	static void constructor(Object* self);
	static void copy(Object* self, const Object* in);
	static void destructor(Object* self);

};
 

extern ObjectType ListObjectType;