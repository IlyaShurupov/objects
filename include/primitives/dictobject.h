#pragma once

#include "object/object.h"

struct DictObject : Object {
	hmap<Object*, string> items;

	static void copy(Object* self, const Object* in);
	static void destructor(Object* self);
	static void constructor(Object* self);
};

extern ObjectType DictObjectType;
