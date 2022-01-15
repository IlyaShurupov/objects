#pragma once

#include "classobject.h"

void ClassObject::copy(Object* self, const Object* in) {
	NDO.copy(NDO_CAST(ClassObject, self)->members, NDO_CAST(ClassObject, in)->members);
}

void ClassObject::destructor(Object* in) {
	NDO_CASTV(ClassObject, in, self);

	NDO.destroy(self->members);
}

void ClassObject::constructor(Object* in) {
	NDO_CASTV(ClassObject, in, self);
	self->members = NDO_CAST(DictObject, NDO.create("dict"));
}

static alni save_size(ClassObject* self) {
	// dict file adress
	return sizeof(alni);
}

static void save(ClassObject* self, File& file_self) {
	// save dict adress
	alni dict_adress = NDO.save(file_self, self->members);
	file_self.write<alni>(&dict_adress);
}

static void load(File& file_self, ClassObject* self) {
	alni dict_adress;
	file_self.read<alni>(&dict_adress);

	Object* dictinary = NDO.load(file_self, dict_adress);

	self->members = (DictObject*)dictinary;
}

struct ObjectType ClassObjectType = {
	.base = NULL,
	.constructor = ClassObject::constructor,
	.destructor = ClassObject::destructor,
	.copy = ClassObject::copy,
	.size = sizeof(ClassObject),
	.name = "class",
	.save_size = (object_save_size)save_size,
	.save = (object_save)save,
	.load = (object_load)load,
};
