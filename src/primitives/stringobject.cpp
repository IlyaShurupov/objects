
#pragma once

#include "primitives/stringobject.h"

void StringObject::constructor(Object* self) {
	new (&NDO_CAST(StringObject, self)->val) string();
}

void StringObject::copy(Object* self, const Object* in) {
	NDO_CAST(StringObject, self)->val = NDO_CAST(StringObject, in)->val;
}

StringObject* StringObject::create(string in) {
	NDO_CASTV(StringObject, NDO->create("str"), out)->val = in;
	return out;
}

void StringObject::from_int(StringObject* self, alni in) {
	self->val = in;
}

void StringObject::from_float(StringObject* self, alnf in) {
	self->val = in;
}

void StringObject::from_string(StringObject* self, string in) {
	self->val = in;
}

string StringObject::to_string(StringObject* self) {
	return self->val;
}

alni StringObject::to_int(StringObject* self) {
	return self->val;
}

alnf StringObject::to_float(StringObject* self) {
	return self->val;
}

static alni save_size(StringObject* self) {
	return self->val.size() + 1;
}

static void save(StringObject* self, File& file_self) {
	file_self.write(self->val);
}

static void load(File& file_self, StringObject* self) {
  new (&self->val) string();
	file_self.read(self->val);
}

struct ObjectTypeConversions StringObjectTypeConversions = {
	.from_int = (object_from_int)StringObject::from_int,
	.from_float = (object_from_float)StringObject::from_float,
	.from_string = (object_from_string)StringObject::from_string,
	.to_string = (object_to_string)StringObject::to_string,
	.to_int = (object_to_int)StringObject::to_int,
	.to_float = (object_to_float)StringObject::to_float,
};

struct ObjectType StringObjectType = {
	.base = NULL,
	.constructor = StringObject::constructor,
	.destructor = NULL,
	.copy = StringObject::copy,
	.size = sizeof(StringObject),
	.name = "str",
	.convesions = &StringObjectTypeConversions,
	.save_size = (object_save_size)save_size,
	.save = (object_save)save,
	.load = (object_load)load,
};