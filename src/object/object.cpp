

#include "object/object.h"

Object* ObjectMemAllocate(const ObjectType* type);
void ObjectMemDeallocate(Object* in);

objects_api* NDO = NULL;

void hierarchy_copy(Object* self, const Object* in, const ObjectType* type);
void hierarchy_construct(Object* self, const ObjectType* type);

void objects_api::define(ObjectType* type) {
	assert(!types.Presents(type->name) && "Type Redefinition");
	types.Put(type->name, type);
}

Object* objects_api::create(const string& name) {
	const ObjectType* type = types.Get(name);

	Object* obj_instance = ObjectMemAllocate(type);

	if (!obj_instance) {
		return NULL;
	}

	hierarchy_construct(obj_instance, obj_instance->type);

	return obj_instance;
}

Object* objects_api::copy(Object* self, const Object* in) {
	if (self->type != in->type) {
		return NULL;
	}

	hierarchy_copy(self, in, self->type);

	return self;
}

void objects_api::set(Object* self, alni val) {
	if (self->type->convesions && self->type->convesions->from_int) {
		self->type->convesions->from_int(self, val);
		return;
	}
}

void objects_api::set(Object* self, alnf val) {
	if (self->type->convesions && self->type->convesions->from_float) {
		self->type->convesions->from_float(self, val);
		return;
	}
}

void objects_api::set(Object* self, string val) {
	if (self->type->convesions && self->type->convesions->from_string) {
		self->type->convesions->from_string(self, val);
		return;
	}
}


void objects_api::destroy(Object* in) {
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

objects_api* objects_init() {
	if (!NDO) {
		NDO = new objects_api();
		return NDO;
	}
	return NULL;
}

void objects_finalize() {
	if (NDO) {
		delete NDO;
	}
}