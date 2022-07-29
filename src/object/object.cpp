

#include "object/object.h"

namespace obj {

	Object* ObjectMemAllocate(const ObjectType* type);
	void ObjectMemDeallocate(Object* in);

	objects_api* NDO = NULL;

	void hierarchy_copy(Object* self, const Object* in, const ObjectType* type);
	void hierarchy_construct(Object* self, const ObjectType* type);

	objects_api::objects_api() {
		tp::memsetv(sl_callbacks, SAVE_LOAD_MAX_CALLBACK_SLOTS * sizeof(save_load_callbacks*), 0);
	}

	void objects_api::define(ObjectType* type) {
		assert(NDO && "using uninitialize objects api");
		assert(!types.presents(type->name) && "Type Redefinition");
		types.put(type->name, type);
	}

	Object* objects_api::create(const tp::string& name) {
		const ObjectType* type = types.get(name);

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

	bool objects_api::compare(Object* first, Object* second) {
		if (first && second && first->type == second->type) {
			if (first->type->comparison) {
				return first->type->comparison(first, second);
			} else { // raw data comparison
				return tp::memequal(first, second, first->type->size);
			}
		}

		return false;
	}

	Object* objects_api::instatiate(Object* in) {
		obj::Object* obj = NDO->create(in->type->name);
		NDO->copy(obj, in);
		return obj;
	}

	void objects_api::set(Object* self, tp::alni val) {
		if (self->type->convesions && self->type->convesions->from_int) {
			self->type->convesions->from_int(self, val);
			return;
		}
	}

	void objects_api::set(Object* self, tp::alnf val) {
		if (self->type->convesions && self->type->convesions->from_float) {
			self->type->convesions->from_float(self, val);
			return;
		}
	}

	void objects_api::set(Object* self, tp::string val) {
		if (self->type->convesions && self->type->convesions->from_string) {
			self->type->convesions->from_string(self, val);
			return;
		}
	}

	tp::alni objects_api::toInt(Object* self) {
		assert(self->type->convesions && self->type->convesions->to_int);
		return self->type->convesions->to_int(self);
	}

	tp::alnf objects_api::toFloat(Object* self) {
		assert(self->type->convesions && self->type->convesions->to_float);
		return self->type->convesions->to_float(self);
	}

	bool objects_api::toBool(Object* self) {
		assert(self->type->convesions && self->type->convesions->to_int);
		return (bool) self->type->convesions->to_int(self);
	}

	tp::string objects_api::toString(Object* self) {
		assert(self->type->convesions && self->type->convesions->to_string);
		return self->type->convesions->to_string(self);
	}

	void objects_api::destroy(Object* in) {

		#ifdef OBJECT_REF_COUNT
		ObjectMemHead* mh = NDO_MEMH_FROM_NDO(in);
		if (mh->refc > 1) {
			mh->refc--;
			return;
		}
		#endif

		if (!in) {
			return;
		}

		for (const ObjectType* iter = in->type; iter && iter->destructor; iter = iter->base) {
			iter->destructor(in);
		}

		ObjectMemDeallocate(in);
	}

	#ifdef OBJECT_REF_COUNT
	void objects_api::refinc(Object* in) {
		ObjectMemHead* mh = NDO_MEMH_FROM_NDO(in);
		mh->refc++;
	}
	#endif


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
				return (Object*) in;
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

};