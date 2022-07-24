
#pragma once

#include "common.h"
#include "containers.h"
#include "allocators.h"
#include "strings.h"
#include "file.h"

#include "object/typegroups.h"

/* Steps to create custom Object:
define name of object
define base type
define struct members
implement construct, destruct and copy methods */

#define OBJECT_REF_COUNT

#ifndef TYPES_V2
#error "types lib branch missmatch - switch to 'types2'"
#endif

#ifdef _DEBUG
#define NDO_CAST(cast_type, ptr) ((cast_type*)ndo_cast(ptr, &cast_type::TypeData))
#define NDO_CAST_ASSERT(cast_type, ptr) {assert(ndo_cast(ptr, &cast_type::TypeData))}
#else
#define NDO_CAST_ASSERT(cast_type, ptr) {assert(ndo_cast(ptr, &cast_type::TypeData))}
#define NDO_CAST(cast_type, ptr) ((cast_type*)ndo_cast(ptr, &cast_type::TypeData))
#endif

#define NDO_CASTV(cast_type, ptr, var_name) cast_type* var_name = NDO_CAST(cast_type, ptr); var_name

#define NDO_MEMH_FROM_NDO(ndo_ptr) (((ObjectMemHead*)ndo_ptr) - 1)
#define NDO_FROM_MEMH(ndo_ptr) ((Object*)(ndo_ptr + 1))

namespace obj {

	extern struct objects_api* NDO;

	struct ObjectMemHead {
		ObjectMemHead* up;
		ObjectMemHead* down;
		tp::alni flags;
		#ifdef OBJECT_REF_COUNT
		tp::alni refc;
		#endif
	};

	struct Object {
		const struct ObjectType* type;
	};

	typedef void (*object_from_int)(Object* self, tp::alni in);
	typedef void (*object_from_float)(Object* self, tp::alnf in);
	typedef void (*object_from_string)(Object* self, tp::string in);
	typedef tp::string(*object_to_string)(Object* self);
	typedef tp::alni(*object_to_int)(Object* self);
	typedef tp::alnf(*object_to_float)(Object* self);

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
	typedef tp::alni(*object_save_size)(Object* self);
	typedef void (*object_save)(Object*, tp::File&);
	typedef void (*object_load)(tp::File&, Object*);
	typedef bool (*object_compare)(Object*, Object*);
	typedef tp::Array<Object*> (*object_debug_all_childs_retrival)(Object*);
	typedef tp::alni (*object_allocated_size)(Object*); // default value = type->size - sizeof(ObjectType*)
	typedef tp::alni (*object_allocated_size_recursive)(Object*); // default value = object_allocated_size

	struct object_caller {
		virtual Object* get(tp::alni idx) = 0;
		virtual void ret(Object*) = 0;
	};

	typedef void (type_method_adress)(Object* self, object_caller*);
	typedef struct {
		tp::string name;
		type_method_adress* adress;
	} type_method;

	struct ObjectType {
		const ObjectType* base = NULL;
		object_constructor constructor = NULL;
		object_destructor destructor = NULL;
		object_copy copy = NULL;
		tp::alni size = NULL;
		tp::string name;
		const ObjectTypeConversions* convesions = NULL;
		object_save_size save_size = NULL;
		object_save save = NULL;
		object_load load = NULL;
		object_compare comparison = NULL;
		type_method* methods = NULL;
		void* vtable = NULL;
		tp::string description;
		void* nodes_custom_data = NULL;
		object_debug_all_childs_retrival childs_retrival = NULL;
		object_allocated_size allocated_size = NULL;
		object_allocated_size_recursive allocated_size_recursive = NULL;
	};


	#define SAVE_LOAD_MAX_CALLBACK_SLOTS 100
	typedef void (pre_save_callback)(void* self, tp::File&);
	typedef void (pre_load_callback)(void* self, tp::File&);
	typedef void (post_save_callback)(void* self, tp::File&);
	typedef void (post_load_callback)(void* self, tp::File&);
	struct save_load_callbacks {
		void* self;
		pre_save_callback* pre_save;
		pre_load_callback* pre_load;
		post_save_callback* post_save;
		post_load_callback* post_load;
	};


	struct objects_api {

		tp::HashMap<const ObjectType*, tp::string> types;
		obj::TypeGroups type_groups;

		void define(ObjectType* type);
		Object* create(const tp::string& name);
		Object* copy(Object* self, const Object* in);
		bool compare(Object* first, Object* second);
		Object* instatiate(Object*);
		void set(Object* self, tp::alni val);
		void set(Object* self, tp::alnf val);
		void set(Object* self, tp::string val);

		tp::alni toInt(Object* self);
		tp::alnf toFloat(Object* self);
		bool toBool(Object* self);
		tp::string toString(Object* self);

		void destroy(Object* in);

		#ifdef OBJECT_REF_COUNT
		void refinc(Object* in);
		#endif

		save_load_callbacks* sl_callbacks[SAVE_LOAD_MAX_CALLBACK_SLOTS];
		tp::alni sl_callbacks_load_idx = 0;

		void add_sl_callbacks(save_load_callbacks*);

		tp::alni objsize_file(Object* self);
		tp::alni objsize_file_recursive(Object* self);

		tp::alni objsize_ram(Object* self);
		tp::alni objsize_ram_recursive_util(Object* self, const ObjectType* type, bool different_object = true);
		tp::alni objsize_ram_recursive(Object* self);

		bool save(Object*, tp::string path);
		Object* load(tp::string path);
		tp::alni save(tp::File&, Object*);
		Object* load(tp::File&, tp::alni file_adress);
	};

	Object* ndo_cast(const Object* in, const ObjectType* to_type);

	objects_api* objects_init();
	void objects_finalize();

};