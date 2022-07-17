
#pragma once

#include "object/object.h"

namespace obj {

	extern struct NullObject* NdoNull_globalInstance;

	struct NullObject : Object {

		static void destructor(Object* self);
		static ObjectType TypeData;
		static Object* null_return() {
			if (!NdoNull_globalInstance) {
				NdoNull_globalInstance = (NullObject*) NDO->create("null");
			}
			return (Object*) NdoNull_globalInstance;
		}
	};

	#define NDO_NULL obj::NullObject::null_return()

};