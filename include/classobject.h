#pragma once

#include "object.h"

#include "nullobject.h"
#include "dictobject.h"
#include "methodobject.h"

struct ClassObject : Object {

	DictObject*  members;

	static void copy(Object* self, const Object* in);
	static void destructor(Object* in);
	static void constructor(Object* in);

	// called on construction of class
	void add_methods(const ObjectStaticMethod* methods) {

		for (; *((alni*)methods); methods++) {
			NDO_CASTV(MethodObject, NDO.create("method"), class_method);
			class_method->self = this;
			class_method->assign_ccode(string(this->type->name) + methods->name);

			members->items.Put(methods->name, class_method);
		}
	}

	// called once
	static void register_ccode(ObjectType* class_type, const ObjectStaticMethod* methods) {
		for (; *((alni*)methods); methods++) {
			static_ccode_table.add(string(class_type->name) + methods->name, methods->function_adress);
		}
	}

	Object* call(string name) {

		NDO_CASTV(MethodObject, members->items.Get(name), method);
		
		if (method->type != &MethodObjectType) {
			return NULL;
		}

		return (*method)();
	}

	Object* member(string name) {
		return members->items.Get(name);
	}
};

extern ObjectType ClassObjectType;
