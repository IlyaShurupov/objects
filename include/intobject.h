
#pragma once

#include "object.h"

struct IntObject : Object {
	alni val;

	static void constructor(Object* self);
	static void copy(Object* self, const Object* in);
	static IntObject* create(alni in);

	static void from_int(Object* self, alni in);
	static void from_float(Object* self, alnf in);
	static void from_string(Object* self, string in);
	static string to_string(Object* self);
	static alni to_int(Object* self);
	static alnf to_float(Object* self);
};
 

extern ObjectType IntObjectType;