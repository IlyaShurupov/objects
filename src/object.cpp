

#include "object.h"

Object* ObjectMemAllocate(const ObjectType* type);
void ObjectMemDeallocate(Object* in);

object_types NDO;

void hierarchy_copy(Object* self, const Object* in, const ObjectType* type);
void hierarchy_construct(Object* self, const ObjectType* type);

void object_types::define(ObjectType* type) {

	ObjectStaticMethod* iter = type->type_methods;
	for (; iter && iter->function_adress.method;  iter++) {
		type->type_methods_dict.Put(iter->name, &iter->function_adress);
	}

	types.Put(type->name, type);
}

Object* object_types::create(string name) {
	const ObjectType* type = types.Get(name);

	Object* obj_instance = ObjectMemAllocate(type);

	if (!obj_instance) {
		return NULL;
	}

	hierarchy_construct(obj_instance, obj_instance->type);

	return obj_instance;
}

Object* object_types::copy(Object* self, const Object* in) {
	if (self->type != in->type) {
		return NULL;
	}

	hierarchy_copy(self, in, self->type);

	return self;
}

void object_types::set(Object* self, alni val) {
	if (self->type->convesions && self->type->convesions->from_int) {
		self->type->convesions->from_int(self, val);
		return;
	}
}

void object_types::set(Object* self, alnf val) {
	if (self->type->convesions && self->type->convesions->from_float) {
		self->type->convesions->from_float(self, val);
		return;
	}
}

void object_types::set(Object* self, string val) {
	if (self->type->convesions && self->type->convesions->from_string) {
		self->type->convesions->from_string(self, val);
		return;
	}
}

void object_types::push(Object* in) {
	callstak.push(in);
}

Object* object_types::call(Object* self, ndo_static_method meth) {
	
	callstak.call();
	Object* out = meth.method(self, callstak.get_arg(0));
	callstak.ret();
	return out;
}

Object* Object::call_type_method(string name) {
	alni idx = ((ObjectType*)type)->type_methods_dict.Presents(name);
	if (idx == -1) {
		return NULL;
	}
	ndo_static_method* type_method = type->type_methods_dict.table[idx]->val;
	return NDO.call(this, *type_method);
}

void object_types::destroy(Object* in) {
	if (!in) {
		return;
	}

	for (const ObjectType* iter = in->type; iter && iter->destructor; iter = iter->base) {
		iter->destructor(in);
	}

	ObjectMemDeallocate(in);
}

void hierarchy_copy(Object* self, const Object* in, const ObjectType* type) {
	if (type->base) {
		hierarchy_copy(self, in, type->base);
	}

	if (type->copy) {
		type->copy(self, in);
	}
}

void hierarchy_construct(Object* self, const ObjectType* type) {
	if (type->base) {
		hierarchy_construct(self, type->base);
	}

	if (type->constructor) {
		type->constructor(self);
	}
}

Object* ndo_cast(const Object* in, const ObjectType* to_type) {
	const ObjectType* typeiter = in->type;
	while (typeiter) {
		if (typeiter == to_type) {
			return (Object*)in;
		}
		typeiter = typeiter->base;
	}
	return NULL;
}