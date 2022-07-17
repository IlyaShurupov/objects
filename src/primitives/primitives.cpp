
#pragma once

#include "primitives/primitives.h"


void obj::primitives_define_types() {
	
	using namespace obj;
	using namespace tp;

	static bool initialized = false;
	if (initialized) {
		return;
	}

	assert(NDO && "Objects library is not initialized");

	NDO->define(&DictObject::TypeData);
	NDO->define(&IntObject::TypeData);
	NDO->define(&LinkObject::TypeData);
	NDO->define(&ListObject::TypeData);
	NDO->define(&NullObject::TypeData);
	NDO->define(&StringObject::TypeData);
	NDO->define(&BoolObject::TypeData);
	NDO->define(&FloatObject::TypeData);
	NDO->define(&EnumObject::TypeData);
	NDO->define(&ClassObject::TypeData);

	NDO->type_groups.addType(&DictObject::TypeData, {"Primitives"});
	NDO->type_groups.addType(&IntObject::TypeData, {"Primitives"});
	NDO->type_groups.addType(&LinkObject::TypeData, {"Primitives"});
	NDO->type_groups.addType(&NullObject::TypeData, {"Primitives"});
	NDO->type_groups.addType(&StringObject::TypeData, {"Primitives"});
	NDO->type_groups.addType(&BoolObject::TypeData, {"Primitives"});
	NDO->type_groups.addType(&FloatObject::TypeData, {"Primitives"});
	NDO->type_groups.addType(&EnumObject::TypeData, {"Primitives"});
	NDO->type_groups.addType(&ClassObject::TypeData, {"Primitives"});

	initialized = true;
}