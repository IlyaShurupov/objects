
#pragma once

#include "intobject.h"

void IntObject::constructor(Object* self) {
	NDO_CAST(IntObject, self)->val = 0;
}

void IntObject::copy(Object* self, const Object* in) {
	NDO_CAST(IntObject, self)->val = NDO_CAST(IntObject, in)->val;
}

IntObject* IntObject::create(alni in) {
	NDO_CASTV(IntObject, NDO.create("int"), out)->val = in;
	return out;
}

void IntObject::from_int(Object* self, alni in) {
	NDO_CAST(IntObject, self)->val = in;
}

void IntObject::from_float(Object* self, alnf in) {
	NDO_CAST(IntObject, self)->val = (alni)in;
}

void IntObject::from_string(Object* self, string in) {
	int out;
	str_from_string(&in, out);
	NDO_CAST(IntObject, self)->val = out;
}

string IntObject::to_string(Object* self) {
	return string(NDO_CAST(IntObject, self)->val);
}

alni IntObject::to_int(Object* self) {
	return alni(NDO_CAST(IntObject, self)->val);
}

alnf IntObject::to_float(Object* self) {
	return alnf(NDO_CAST(IntObject, self)->val);
}

static alni save_size(IntObject* self) {
	return sizeof(alni);
}

static void save(IntObject* self, File& file_self) {
	file_self.write<alni>(&self->val);
}

static void load(File& file_self, IntObject* self) {
	file_self.read<alni>(&self->val);
}

struct ObjectTypeConversions IntObjectTypeConversions = {
	.from_int = IntObject::from_int,
	.from_float = IntObject::from_float,
	.from_string = IntObject::from_string,
	.to_string = IntObject::to_string,
	.to_int = IntObject::to_int,
	.to_float = IntObject::to_float,
};

struct ObjectType IntObjectType = {
	.base = NULL,
	.constructor = IntObject::constructor,
	.destructor = NULL,
	.copy = IntObject::copy,
	.size = sizeof(IntObject),
	.name = "int",
	.convesions = &IntObjectTypeConversions,
	.save_size = (object_save_size)save_size,
	.save = (object_save)save,
	.load = (object_load)load,
};