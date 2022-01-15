
#pragma once

#include "Types.h"

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

extern struct objects_api* NDO;

struct Object {
	const struct ObjectType* type;
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

struct object_caller {
	virtual Object* get(alni idx) = 0;
	virtual void ret(Object*) = 0;
};

typedef void (type_method)(Object* self, object_caller*);

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
	type_method** methods = NULL;
};

struct objects_api {

	hmap<const ObjectType*, string> types;

	void define(ObjectType* type);

	Object* create(string name);
	Object* copy(Object* self, const Object* in);
	
	void set(Object* self, alni val);
	void set(Object* self, alnf val);
	void set(Object* self, string val);

	alni save(File&, Object*);
	Object* load(File&, alni file_adress);

	void save(Object*, string path);
	Object* load(string path);

	void destroy(Object* in);

};

Object* ndo_cast(const Object* in, const ObjectType* to_type);

objects_api* objects_init();