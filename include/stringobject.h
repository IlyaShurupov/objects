
#pragma once

#include "object.h"

struct StringObject : Object {
	string val;

	static void constructor(Object* self);
	static void copy(Object* self, const Object* in);
	static StringObject* create(string in);

	static void from_int(StringObject* self, alni in);
	static void from_float(StringObject* self, alnf in);
	static void from_string(StringObject* self, string in);
	static string to_string(StringObject* self);
	static alni to_int(StringObject* self);
	static alnf to_float(StringObject* self);
};
 

extern ObjectType StringObjectType;