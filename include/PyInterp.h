
#pragma once

#include <Types.h>

struct PyInterp {
	PyInterp();
	struct Object* exec(string cmd, Object* self);
	~PyInterp();
};