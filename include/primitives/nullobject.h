
#pragma once

#include "object/object.h"

extern struct NullObject* NdoNull_globalInstance;

struct NullObject : Object {

	static void destructor(Object* self);

	static Object* null_return() {
		if (!NdoNull_globalInstance) {
			NdoNull_globalInstance = (NullObject*)NDO->create("null");
		}
		return (Object*)NdoNull_globalInstance;
	}
};

extern ObjectType NullObjectType;

#define NDO_NULL NullObject::null_return()