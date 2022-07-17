
#pragma once

#include "object/object.h"

namespace obj {

	struct IntObject : Object {
		tp::alni val;

		static ObjectType TypeData;
		static void constructor(Object* self);
		static void copy(Object* self, const Object* in);
		static IntObject* create(tp::alni in);

		static void from_int(Object* self, tp::alni in);
		static void from_float(Object* self, tp::alnf in);
		static void from_string(Object* self, tp::string in);
		static tp::string to_string(Object* self);
		static tp::alni to_int(Object* self);
		static tp::alnf to_float(Object* self);
	};
};