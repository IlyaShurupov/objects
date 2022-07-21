#pragma once

#include "object/object.h"

namespace obj {

	struct DictObject : Object {
		tp::HashMap<Object*, tp::string> items;

		static ObjectType TypeData;
		static void copy(Object* self, const Object* in);
		static void destructor(Object* self);
		static void constructor(Object* self);
	};
};