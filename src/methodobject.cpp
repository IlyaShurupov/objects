
#pragma once

#include "methodobject.h"

#include "PyInterp.h"

PyInterp GlobalMethod_PyInterp;

StaticCCodeTable static_ccode_table;

void MethodObject::constructor(Object* in) {
	NDO_CASTV(MethodObject, in, self);

	self->self = NULL;
	self->code_flags = NULL;
}

void MethodObject::copy(Object* o_self, const Object* o_in) {

	NDO_CASTV(MethodObject, o_self, self);
	NDO_CASTV(MethodObject, o_in, in);

	if (in->code_flags == 1) {
		self->assign(in->code.pycode);
	}
	else {
		self->assign(in->code.ccode);
	}
	
	self->self = in->self;
}

Object* MethodObject::operator()() {
	if (!code_flags) {
		return NDO.call((Object*)self, code.ccode);
	}
	else if (code_flags == 1) {
		NDO.callstak.call();
		return GlobalMethod_PyInterp.exec(code.pycode, (Object*)self);
		NDO.callstak.ret();
	}
}

void MethodObject::from_string(Object* in, string code) {
	NDO_CASTV(MethodObject, in, self);
	self->assign(code);
}

static alni save_size(MethodObject* self) {

	// self adress
	alni save_size = sizeof(alni);

	// flags
	save_size += sizeof(self->code_flags);

	// python code
	if (self->code_flags == 1) {
		alni pycode_len = self->code.pycode.len();
		save_size += pycode_len + sizeof(alni);
	}
	// ccode address name
	else {
		string ccode_address_name = static_ccode_table.address_name_from_adress(self->code.ccode);
		save_size += ccode_address_name.len() + sizeof(alni);
	}

	return save_size;
}

static void save(MethodObject* self, File& file_self) {

	// self save
	alni method_self_adress = NDO.save(file_self, (Object*)self->self);
	file_self.write<alni>(&method_self_adress);

	// flags save
	file_self.write<alni>(&self->code_flags);
	
	// pycode save
	if (self->code_flags == 1) {
		alni pycode_len = self->code.pycode.len();

		file_self.write<alni>(&pycode_len);
		file_self.write_bytes(self->code.pycode.str, pycode_len);
	}
	// ccode address name save
	else {
		string ccode_address_name = static_ccode_table.address_name_from_adress(self->code.ccode);
		alni ccode_address_name_len = ccode_address_name.len();

		file_self.write<alni>(&ccode_address_name_len);
		file_self.write_bytes(ccode_address_name.str, ccode_address_name_len);
	}
}

static void load(File& file_self, MethodObject* self) {
	
	// self address
	alni method_self_adress;
	file_self.read<alni>(&method_self_adress);
	
	// load self
	self->self = (ClassObject*)NDO.load(file_self, method_self_adress);

	// read flags
	file_self.read<alni>(&self->code_flags);

	// read string
	alni len;
	file_self.read<alni>(&len);

	// read key value
	string saved_str;
	saved_str.alloc(len);
	file_self.read_bytes(saved_str.str, len);

	// if pycode
	if (self->code_flags == 1) {
		new (&self->code.pycode) string(saved_str);
	}
	// read ccode adress name
	else {
		self->code.ccode = static_ccode_table.get(saved_str);
	}
}

struct ObjectTypeConversions MethodTypeConversions = {
	NULL,
	NULL,
	MethodObject::from_string,
	NULL,
	NULL,
	NULL,
};

struct ObjectType MethodObjectType = {
	.base = NULL,
	.constructor = MethodObject::constructor,
	.destructor = NULL,
	.copy = MethodObject::copy,
	.size = sizeof(MethodObject),
	.name = "method",
	.convesions = &MethodTypeConversions,
	.save_size = (object_save_size)save_size,
	.save = (object_save)save,
	.load = (object_load)load,
};
