
#pragma once

#include "primitives/primitives.h"

void primitives_define_types() {

	static bool initialized = false;
	if (initialized) {
		return;
	}

	assert(NDO && "Objects library is not initialized");

	NDO->define(&DictObjectType);
	NDO->define(&IntObjectType);
	NDO->define(&LinkObjectType);
	NDO->define(&ListObjectType);
	NDO->define(&NullObjectType);
	NDO->define(&StringObjectType);

	initialized = true;
}