
#include "PyInterp.h"

#include "PyNdo.h"


PyObject* pModule;

PyObject* ndo_create_object(PyObject* self, PyObject* args) {

	const char* arg;
	if (!PyArg_ParseTuple(args, "s", &arg)) {
		return NULL;
	}

	string name = arg;

	Object* out = NDO.create(name);
	
	if (!out) {
		return NULL;
	}

	return PyEmbObject_New(out);
}

static PyMethodDef ModuleMethods[] = { 
	{"ndo_create", ndo_create_object, METH_VARARGS, "creates nd object of type"},
	{NULL, NULL} 
};

static struct PyModuleDef cModPyDem = { PyModuleDef_HEAD_INIT, "npy", "", -1, ModuleMethods };

PyMODINIT_FUNC PyInit_cModPyDem() {
	return PyModule_Create(&cModPyDem);
}

PyInterp::PyInterp() {

	const char* stdOutErr =
"import sys\n\
import npy\n\
class CatchOut:\n\
	def __init__(self):\n\
			self.value = ''\n\
	def write(self, txt):\n\
			self.value += txt\n\
	def clear(self):\n\
			self.value = ''\n\
catchOut = CatchOut()\n\
sys.stdout = catchOut\n\
sys.stderr = catchOut\n\
";

	Py_SetPythonHome(L"G:/src/python310");

	PyImport_AppendInittab("npy", PyInit_cModPyDem);

	Py_Initialize();

	pModule = PyImport_AddModule("__main__");

	PyRun_SimpleString(stdOutErr);

	PyObject* self_arg = PyEmbObject_New(0);
	PyObject* module_dict = PyModule_GetDict(pModule);

	PyDict_SetItemString(module_dict, "ndo_self", self_arg);
}

Object* PyInterp::exec(string cmd, Object* self) {

	PyObject* module_dict = PyModule_GetDict(pModule);

	Py_EmbObj* py_self = (Py_EmbObj*)PyDict_GetItemString(module_dict, "ndo_self");
	py_self->ndo_ptr = self;

	PyObject* arg_list = PyList_New(0);
	PyDict_SetItemString(module_dict, "ndo_args", arg_list);
	
	for (alni i = 0; NDO.callstak.get_arg(i); i++) {
		PyList_Append(arg_list, PyEmbObject_New(NDO.callstak.get_arg(i)));
	}

	PyRun_SimpleString(cmd.str);

	PyObject* catcher = PyObject_GetAttrString(pModule, "catchOut");
	PyObject* output = PyObject_GetAttrString(catcher, "value");

	string out = PyUnicode_AsUTF8(output);

	NdLog_write(out.str); 

	PyObject* function = PyObject_GetAttrString(catcher, "clear");
	PyObject_CallObject(function, NULL);

	return NULL;
}

PyInterp::~PyInterp() {
	Py_Finalize();
}
