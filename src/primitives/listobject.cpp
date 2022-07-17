
#pragma once

#include "primitives/listobject.h"

#include "primitives/intobject.h"

using namespace obj;
using namespace tp;

void ListObject::constructor(Object* in) {
	NDO_CASTV(ListObject, in, self);

	new (&self->items) List<Object*>();
}

void ListObject::copy(Object* in, const Object* target) {
	NDO_CASTV(ListObject, in, self);
	NDO_CASTV(ListObject, target, src);

	self->items.free();

	for (auto item : src->items) {
		
		Object* instance = NDO->create(item->type->name);
		instance->type->copy(instance, item.data());

		self->items.pushBack(instance);
	}
}

void ListObject::destructor(Object* in) {
	NDO_CASTV(ListObject, in, self);
	for (auto item : self->items) {
		NDO->destroy(item.data());
	}
	self->items.free();
}

static alni save_size(ListObject* self) {
	alni len = self->items.length();
	return (len + 1) * sizeof(alni);
}

static void save(ListObject* self, File& file_self) {

	alni len = self->items.length();
	file_self.write<alni>(&len);

	for (auto item : self->items) {
		alni ndo_object_adress = NDO->save(file_self, item.data());
		file_self.write<alni>(&ndo_object_adress);
	}
}

static void load(File& file_self, ListObject* self) {

	new (&self->items) List<Object*>();

	alni len; 
	file_self.read<alni>(&len);
	
	for (alni i = 0; i < len; i++) {
		alni ndo_object_adress;
		file_self.read<alni>(&ndo_object_adress);
		self->items.pushBack(NDO->load(file_self, ndo_object_adress));
	}
}


void list_method_get_length(Object* self, object_caller* caller) {
	NDO_CAST(IntObject, caller->get(0))->val = NDO_CAST(ListObject, self)->items.length();
}

void list_method_push_back(Object* self, object_caller* caller) {
	NDO_CAST(ListObject, self)->items.pushBack(caller->get(0));
}

type_method ListObjectTypeMethods[] = {
	{"push_back", list_method_push_back},
	{"get_len", list_method_get_length},
};


struct obj::ObjectType obj::ListObject::TypeData = {
	.base = NULL,
	.constructor = ListObject::constructor,
	.destructor = ListObject::destructor,
	.copy = ListObject::copy,
	.size = sizeof(ListObject),
	.name = "list",
	.convesions = NULL,
	.save_size = (object_save_size)save_size,
	.save = (object_save)save,
	.load = (object_load)load,
	.methods = ListObjectTypeMethods,
};