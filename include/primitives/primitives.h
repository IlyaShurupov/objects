
#pragma once

#include "primitives/dictobject.h"
#include "primitives/intobject.h"
#include "primitives/linkobject.h"
#include "primitives/listobject.h"
#include "primitives/nullobject.h"
#include "primitives/stringobject.h"

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