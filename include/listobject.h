
#pragma once

#include "object.h"

struct ListObject : Object {
	List<Object*> items;

	static void constructor(Object* self);
	static void copy(Object* self, const Object* in);
	static void destructor(Object* self);


	static Object* length(ListObject* self, Object* arg);
	static Object* insert(ListObject* self, Object* arg);
	static Object* remove(ListObject* self, Object* arg);
	static Object* get(ListObject* self, Object* arg);
};
 

extern ObjectType ListObjectType;