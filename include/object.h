
#pragma once

#include "Types.h"

#include "objectcall.h"

#ifdef _DEBUG
#define NDO_CAST(cast_type, ptr) ((cast_type*)ndo_cast(ptr, &cast_type##Type))
#else
#define NDO_CAST(cast_type, ptr) (cast_type*)ptr)
#endif

#define NDO_CASTV(cast_type, ptr, var_name) cast_type* var_name = NDO_CAST(cast_type, ptr); var_name

/* Steps to create custom Object:
define name of object
define base type
define struct members
implement construct, destruct and copy methods */


extern struct object_types NDO;

struct Object {
	const struct ObjectType* type;

	Object* call_type_method(string name);
};

typedef void (*object_from_int)(Object* self, alni in);
typedef void (*object_from_float)(Object* self, alnf in);
typedef void (*object_from_string)(Object* self, string in);
typedef string (*object_to_string)(Object* self);
typedef alni (*object_to_int)(Object* self);
typedef alnf (*object_to_float)(Object* self);

struct ObjectTypeConversions {
	object_from_int from_int;
	object_from_float from_float;
	object_from_string from_string;
	object_to_string to_string;
	object_to_int to_int;
	object_to_float to_float;
};

typedef void (*object_constructor)(Object* self);
typedef void (*object_destructor)(Object* self);
typedef void (*object_copy)(Object* self, const Object* target);
typedef alni (*object_save_size)(Object* self);
typedef void (*object_save)(Object*, File&);
typedef void (*object_load)(File&, Object*);

struct ObjectType {
	const ObjectType* base;
	object_constructor constructor = NULL;
	object_destructor destructor = NULL;
	object_copy copy = NULL;
	alni size = NULL;
	string name;
	const ObjectTypeConversions* convesions = NULL;
	object_save_size save_size = NULL;
	object_save save = NULL;
	object_load load = NULL;
	ObjectStaticMethod* type_methods = NULL;
	hmap<ndo_static_method*, string> type_methods_dict;
};

struct object_types {

	ObjectCallStack callstak;
	hmap<const ObjectType*, string> types;

	void define(ObjectType* type);
	Object* create(string name);
	Object* copy(Object* self, const Object* in);
	
	void set(Object* self, alni val);
	void set(Object* self, alnf val);
	void set(Object* self, string val);

	alni save(File&, Object*);
	Object* load(File&, alni file_adress);

	void save(Object*);
	Object* load();

	void push(Object*);
	Object* call(Object* self, ndo_static_method meth);

	void destroy(Object* in);

};

Object* ndo_cast(const Object* in, const ObjectType* to_type);