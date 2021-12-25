
#pragma once

#include "PyNdo.h"

#include "object.h"
#include "classobject.h"
#include "intobject.h"


static PyObject* Emb_new(PyTypeObject* type, PyObject* args, PyObject* kwargs) {

	Py_EmbObj* out = (Py_EmbObj*)PyObject_Malloc(sizeof(Py_EmbObj));

	out->ob_base.ob_type = type;
	out->ob_base.ob_refcnt = 1;

	out->ndo_ptr = 0;

	return (PyObject*)out;
}

PyObject* Emb_vectorcall(PyObject* callable, PyObject* const* args, size_t nargsf, PyObject* kwnames) {
	return  PyEmbObject_New(0);
}

void Emb_dealloc(Py_EmbObj* self) {
}

PyObject* Emb_repr(PyObject* self) {
	Object* ndo = ((Py_EmbObj*)self)->ndo_ptr;
	return PyUnicode_FromString(ndo->type->name.str);
}

static PyObject* EmbObj_get_child(PyObject* self, PyObject* args) {

	Py_EmbObj* me = ((Py_EmbObj*)self);

	const char* arg;
	if (!PyArg_ParseTuple(args, "s", &arg)) {
		return NULL;
	}

	string name = arg;

	NDO_CASTV(ClassObject, me->ndo_ptr, ndo_class);

	alni child_idx = ndo_class->members->items.Presents(name);

	if (child_idx == -1) {
		return NULL;
	}

	PyObject* out = PyEmbObject_New(ndo_class->members->items.table[child_idx]->val);
	return out;
}

Py_EmbObj* NodesPyObject_fromPyObject(PyObject* in) {
	Object* out = nullptr;

	if (string("int") == in->ob_type->tp_name) {
		IntObject* ndi = (IntObject*)NDO.create("int");
		ndi->val = PyLong_AsLong(in);
		out = ndi;
	}

	return (Py_EmbObj*)PyEmbObject_New(out);
}

static PyObject* EmbObj_call(PyObject* in, PyObject* args) {

	Py_EmbObj* self = ((Py_EmbObj*)in);
	Object* ndo_out = nullptr;
	
	PyObject* list_arg;
	const char* name_arg;

	if (!PyArg_ParseTuple(args, "sO", &name_arg, &list_arg)) {
		PyErr_SetString(PyExc_ValueError, "Expected a name and a list of ndo arguments");
		return NULL;
	}

	string name = name_arg;
	PyListObject* list = (PyListObject*)list_arg;

	alni n = PyList_Size(list_arg);
	for (alni i = 0; i < n; i++) {
		
		PyObject* pItem = PyList_GetItem(list_arg, i);
		Py_EmbObj* ndo_arg = nullptr;
		
		if (pItem->ob_type->tp_name != "EmbObj") {
			Py_EmbObj* convert = NodesPyObject_fromPyObject(pItem);
			if (!convert) {
				PyErr_SetString(PyExc_TypeError, "list items must be Nodes Objects.");
				return NULL;
			}
			ndo_arg = convert;
		}
		else {
			ndo_arg = (Py_EmbObj*)pItem;
		}

		NDO.push(ndo_arg->ndo_ptr);
	}


	if (ndo_cast(self->ndo_ptr, &ClassObjectType)) {
		NDO_CASTV(ClassObject, self->ndo_ptr, ndo_class);
		ndo_out = ndo_class->call(name);
	}
	else {
		ndo_out = self->ndo_ptr->call_type_method(name);
	}

	return PyEmbObject_New(ndo_out);
}

static PyObject* EmbObj_set(PyObject* self, PyObject* args) {
	PyObject* arg;

	if (!PyArg_ParseTuple(args, "O", &arg)) {
		PyErr_SetString(PyExc_ValueError, "Expected a value");
		return NULL;
	}

	string arg_type = arg->ob_type->tp_name;

	Object* ndo = ((Py_EmbObj*)self)->ndo_ptr;

	if (arg_type == "int") {
		long val = PyLong_AsLong(arg);
		NDO.set(ndo, (alni)val);
	}
	else if (arg_type == "float") {
		float val = (float)PyFloat_AsDouble(arg);
		NDO.set(ndo, val);
	}
	else if (arg_type == "str") {
		const char* val = PyUnicode_AsUTF8(arg);
		string str = val;
		NDO.set(ndo, val);
	}
	else if (arg_type == "bool") {
		long val = PyLong_AsLong(arg);
		NDO.set(ndo, (alni)val);
	}

	return PyLong_FromLong(0);
}

static PyObject* EmbObj_get(PyObject* self, PyObject* Py_UNUSED(args)) {

	Py_EmbObj* pyndo = (Py_EmbObj*)self;

	if (pyndo->ndo_ptr->type == &IntObjectType) {
		return PyLong_FromLong((long)NDO_CAST(IntObject, pyndo->ndo_ptr)->val);
	}
	else {
		return NULL;
	}
}

static PyObject* EmbObj_destroy(PyObject* self, PyObject* Py_UNUSED(args)) {
	Py_EmbObj* pyndo = (Py_EmbObj*)self;
	NDO.destroy(pyndo->ndo_ptr);
	return PyBool_FromLong(0);
}

static PyObject* EmbObj_save(PyObject* self, PyObject* Py_UNUSED(args)) {
	Py_EmbObj* pyndo = (Py_EmbObj*)self;
	NDO.save(pyndo->ndo_ptr);
	return PyBool_FromLong(0);
}

static PyObject* EmbObj_load(PyObject* self, PyObject* Py_UNUSED(args)) {
	Py_EmbObj* pyndo = (Py_EmbObj*)self;
	return PyEmbObject_New(NDO.load());
}

static PyObject* EmbObj_add_child(PyObject* self, PyObject* args) {
	
	PyObject* arg_name;
	PyObject* arg_ndo;
	
	string name;
	Object* ndo_child;

	if (!PyArg_ParseTuple(args, "OO", &arg_name, &arg_ndo)) {
		PyErr_SetString(PyExc_ValueError, "function interface : str, ndo ");
	}
	if (string("str") != arg_name->ob_type->tp_name) {
		return NULL;
	}
	if (string("EmbObj") != arg_ndo->ob_type->tp_name) {
		return NULL;
	}

	
	name = PyUnicode_AsUTF8(arg_name);
	ndo_child = ((Py_EmbObj*)arg_ndo)->ndo_ptr;

	Object* ndo = ((Py_EmbObj*)self)->ndo_ptr;

	if (!ndo_cast(ndo, &ClassObjectType)) {
		return NULL;
	}

	((ClassObject*)ndo)->members->items.Put(name, ndo_child);

	return PyLong_FromLong(0);
}

static PyObject* EmbObj_copy(PyObject* self, PyObject* args) {

	PyObject* arg_ndo;
	Object* ndo_copy;

	if (!PyArg_ParseTuple(args, "O", &arg_ndo)) {
		PyErr_SetString(PyExc_ValueError, "expected a ndo");
		return NULL;
	}
	if (string("EmbObj") != arg_ndo->ob_type->tp_name) {
		PyErr_SetString(PyExc_ValueError, "expected a ndo");
		return NULL;
	}

	ndo_copy = ((Py_EmbObj*)arg_ndo)->ndo_ptr;
	Object* ndo = ((Py_EmbObj*)self)->ndo_ptr;

	NDO.copy(ndo, ndo_copy);

	return PyLong_FromLong(0);
}

static PyMethodDef Emb_methods[] = {
	{"set", EmbObj_set, METH_VARARGS | METH_KEYWORDS, "doc get_info"},
	{"get", EmbObj_get, METH_VARARGS | METH_KEYWORDS, "doc get_info"},
	{"copy", EmbObj_copy, METH_VARARGS | METH_KEYWORDS, "doc get_info"},
	{"destroy", EmbObj_destroy, METH_VARARGS | METH_KEYWORDS, "doc get_info"},
	{"save", EmbObj_save, METH_VARARGS | METH_KEYWORDS, "doc get_info"},
	{"load", EmbObj_load, METH_VARARGS | METH_KEYWORDS, "doc get_info"},
	{"call", EmbObj_call, METH_VARARGS | METH_KEYWORDS, "doc get_info"},

	{"child", EmbObj_get_child, METH_VARARGS | METH_KEYWORDS, "doc get_info"},
	{"add_child", EmbObj_add_child, METH_VARARGS | METH_KEYWORDS, "doc get_info"},
	//{"del_child", EmbObj_add_child, METH_VARARGS | METH_KEYWORDS, "doc get_info"},
	{NULL, NULL},
};

PyTypeObject Py_EmbObjType;

void init_Py_EmbObjType() {
	Py_EmbObjType =
	{

			PyVarObject_HEAD_INIT(&PyType_Type, 0)

			"EmbObj",                                   /* tp_name */
			sizeof(Py_EmbObj),                          /* tp_basicsize */
			0,                                          /* tp_itemsize */
			(destructor)Emb_dealloc,                    /* tp_dealloc */
			0,                                          /* tp_print */
			0,                                          /* tp_getattr */
			0,                                          /* tp_setattr */
			0,                                          /* tp_reserved */
			(reprfunc)Emb_repr,                         /* tp_repr */
			0,                                          /* tp_as_number */
			0,                                          /* tp_as_sequence */
			0,                                          /* tp_as_mapping */
			0,                                          /* tp_hash */
			0,                                          /* tp_call */
			0,                                          /* tp_str */
			0,                                          /* tp_getattro */
			0,                                          /* tp_setattro */
			0,                                          /* tp_as_buffer */
			Py_TPFLAGS_DEFAULT,                         /* tp_flags */
			"emb Obj doc",                              /* tp_doc */
			0,                                          /* tp_traverse */
			0,                                          /* tp_clear */
			0,                                          /* tp_richcompare */
			0,                                          /* tp_weaklistoffset */
			0,                                          /* tp_iter */
			0,                                          /* tp_iternext */
			Emb_methods,                                /* tp_methods */
			0,                                          /* tp_members */
			0,                                          /* tp_getset */
			0,                                          /* tp_base */
			0,                                          /* tp_dict */
			0,                                          /* tp_descr_get */
			0,                                          /* tp_descr_set */
			0,                                          /* tp_dictoffset */
			0,                                          /* tp_init */
			0,                                          /* tp_alloc */
			(newfunc)Emb_new,                           /* tp_new */
			0,                                          /* tp_free */
			0,                                          /* tp_is_gc */
			0,                                          /* tp_bases; */
			0,                                          /* tp_mro; */
			0,                                          /* tp_cache */
			0,                                          /* tp_subclasses */
			0,                                          /* tp_weaklist */
			0,                                          /* tp_del */
			0,                                          /* tp_version_tag */
			0,																					 /* tp_finalize */
			(vectorcallfunc)Emb_vectorcall,             /* vectorcallfunc */
	};
}

PyObject* PyEmbObject_New(Object* self) {

	init_Py_EmbObjType();

	if (PyType_Ready(&Py_EmbObjType) < 0) {
		return NULL;
	}

	Py_EmbObj* out = (Py_EmbObj*)Py_EmbObjType.tp_new(&Py_EmbObjType, NULL, NULL);
	out->ndo_ptr = self;
	return (PyObject*)out;
}