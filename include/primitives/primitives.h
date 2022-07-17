
#pragma once

#include "primitives/dictobject.h"
#include "primitives/intobject.h"
#include "primitives/linkobject.h"
#include "primitives/listobject.h"
#include "primitives/nullobject.h"
#include "primitives/stringobject.h"
#include "primitives/boolobject.h"
#include "primitives/floatobject.h"
#include "primitives/enumobject.h"
#include "primitives/classobject.h"

namespace obj {
	void primitives_define_types();
};