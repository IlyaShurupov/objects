#pragma once

#include "primitives/dictobject.h"

namespace obj {

	struct ClassObject : Object {

		static ObjectType TypeData;
		static void copy(ClassObject* self, const ClassObject* in);
		static void destructor(ClassObject* self);
		static void constructor(ClassObject* self);
		static tp::alni save_size(ClassObject* self);
		static void save(ClassObject* self, tp::File& file_self);
		static void load(tp::File& file_self, ClassObject* self);

		DictObject* members;

		void addMember(Object* obj, tp::string id);
		void createMember(tp::string type, tp::string id);

		template<typename Type>
		Type* getMember(tp::string id) {
			Type* out = ((Type*) obj::ndo_cast(members->items.get(id), &Type::TypeData));
			assert(out && "invalid member access");
			return out;
		}
	};


};