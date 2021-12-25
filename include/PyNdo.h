
#pragma once

#include <corecrt.h>

#define PY_SSIZE_T_CLEAN

#ifdef _DEBUG
#undef _DEBUG
#include "Python.h"
#define _DEBUG
#else
#include "Python.h"
#endif

#include "object.h"

typedef struct {
	PyObject_HEAD;
	Object* ndo_ptr;
} Py_EmbObj;


PyObject* PyEmbObject_New(struct Object* self);