
#pragma once

#include "primitives/primitives.h"

void primitives_define_types(objects_api* NDO) {

	if (!NDO) {
		return;
	}

	NDO->define(&DictObjectType);
	NDO->define(&IntObjectType);
	NDO->define(&LinkObjectType);
	NDO->define(&ListObjectType);
	NDO->define(&NullObjectType);
	NDO->define(&StringObjectType);
}