
#pragma once

#include "primitives/nullobject.h"

NullObject* NdoNull_globalInstance = NULL;

void NullObject::destructor(Object* self) {
	assert(0 && "Only one the instance of NullObject exists and thus it can't be destroyed");
}

struct ObjectType NullObjectType = {
	.base = NULL,
	.constructor = NULL,
	.destructor = NullObject::destructor,
	.copy = NULL,
	.size = sizeof(NullObject),
	.name = "null",
};

