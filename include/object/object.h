
#pragma once

#include "common.h"
#include "containers.h"
#include "allocators.h"
#include "strings.h"
#include "file.h"

#ifndef TYPES_V2
#error "types lib branch dismatch - switch to 'types2'"
#endif

#ifdef _DEBUG
#define NDO_CAST(cast_type, ptr) ((cast_type*)ndo_cast(ptr, &cast_type##Type))
#else
#define NDO_CAST(cast_type, ptr) ((cast_type*)ptr)
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
typedef void (*object_save)(Object*, osfile&);
typedef void (*object_load)(osfile&, Object*);

struct object_caller {
	virtual Object* get(alni idx) = 0;
	virtual void ret(Object*) = 0;
};

typedef void (type_method_adress)(Object* self, object_caller*);
typedef struct {
	string name; 
	type_method_adress* adress;
} type_method;

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
	type_method* methods = NULL;
};


#define SAVE_LOAD_MAX_CALLBACK_SLOTS 100
typedef void (pre_save_callback)(void* self, osfile&);
typedef void (pre_load_callback)(void* self, osfile&);
typedef void (post_save_callback)(void* self, osfile&);
typedef void (post_load_callback)(void* self, osfile&);
struct save_load_callbacks {
	void* self;
	pre_save_callback* pre_save;
	pre_load_callback* pre_load;
	post_save_callback* post_save;
	post_load_callback* post_load;
};


struct objects_api {

	HashMap<const ObjectType*, string> types;

	void define(ObjectType* type);
	Object* create(string name);
	Object* copy(Object* self, const Object* in);
	void set(Object* self, alni val);
	void set(Object* self, alnf val);
	void set(Object* self, string val);
	void destroy(Object* in);

	
	save_load_callbacks* sl_callbacks[SAVE_LOAD_MAX_CALLBACK_SLOTS];
	alni sl_callbacks_load_idx = 0;

	void add_sl_callbacks(save_load_callbacks*);

	void save(Object*, string path);
	Object* load(string path);
	alni save(osfile&, Object*);
	Object* load(osfile&, alni file_adress);
};

Object* ndo_cast(const Object* in, const ObjectType* to_type);

objects_api* objects_init();