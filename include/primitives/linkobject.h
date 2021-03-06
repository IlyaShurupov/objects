
#pragma once

#include "object/object.h"

namespace obj {

	struct LinkObject : Object {
		Object* link;
		static ObjectType TypeData;
		static void constructor(Object* self);
		static void copy(Object* self, const Object* in);
		static LinkObject* create(Object* in);
	};

};