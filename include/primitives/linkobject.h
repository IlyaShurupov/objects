
#pragma once

#include "object/object.h"

struct LinkObject : Object {
	Object* link;

	static void constructor(Object* self);
	static void copy(Object* self, const Object* in);
	static LinkObject* create(Object* in);
};
 

extern ObjectType LinkObjectType;