
#pragma once

#include "listobject.h"

#include "intobject.h"
#include "nullobject.h"

void ListObject::constructor(Object* in) {
	NDO_CASTV(ListObject, in, self);

	new (&self->items) List<Object*>(false);
}

void ListObject::copy(Object* in, const Object* target) {
	NDO_CASTV(ListObject, in, self);
	
	for (auto item : self->items) {
		NDO.destroy(item.Data());
	}

	self->items = NDO_CAST(ListObject, target)->items;
}

void ListObject::destructor(Object* in) {
	NDO_CASTV(ListObject, in, self);

	for (auto item : self->items) {
		NDO.destroy(item.Data());
	}
	
	self->items.Clear();
}

Object* ListObject::length(ListObject* self, Object* arg) {
	return IntObject::create(self->items.Len());
}

Object* ListObject::insert(ListObject* self, Object* arg) {
	Object* idx_obj = arg;
	Object* insert_obj = NDO.callstak.get_arg(1);

	alni idx = NDO_CAST(IntObject, idx_obj)->val;
	self->items.Insert(insert_obj, idx);

	return self;
}

Object* ListObject::remove(ListObject* self, Object* arg) {
	Object* rem_obj = (&arg)[0];
	self->items.DelNode(self->items.Find(rem_obj));
	return NDO_NULL;
}

Object* ListObject::get(ListObject * self, Object * arg) {
	alni idx = NDO_CAST(IntObject, arg)->val;
	return self->items[idx];
}

static alni save_size(ListObject* self) {
	alni len = self->items.Len();
	return (len + 1) * sizeof(alni);
}

static void save(ListObject* self, File& file_self) {

	alni len = self->items.Len();
	file_self.write<alni>(&len);

	for (auto item : self->items) {
		alni ndo_object_adress = NDO.save(file_self, item.Data());
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
		self->items.PushBack(NDO.load(file_self, ndo_object_adress));
	}
}

ObjectStaticMethod ListTypeMethods[] = {
	{(ndo_method)ListObject::get, "get"},
	{(ndo_method)ListObject::remove, "remove"},
	{(ndo_method)ListObject::insert, "insert"},
	{(ndo_method)ListObject::length, "length"},
	{ NULL , NULL},
};

struct ObjectType ListObjectType = {
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
	.type_methods = ListTypeMethods,
};