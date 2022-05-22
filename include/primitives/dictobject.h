#pragma once

#include "object/object.h"

namespace obj {

	struct DictObject : Object {
		tp::HashMap<Object*, tp::string> items;

		static void copy(Object* self, const Object* in);
		static void destructor(Object* self);
		static void constructor(Object* self);
	};

	extern ObjectType DictObjectType;

};