#pragma once

#include "primitives/dictobject.h"

void DictObject::constructor(Object* self) {
	NDO_CASTV(DictObject, self, dict);

	new (&dict->items) HashMap<Object*, string>();
}

void DictObject::copy(Object* in, const Object* target) {
	NDO_CASTV(DictObject, in, self);
	NDO_CASTV(DictObject, target, src);

	self->items.~HashMap();

	new (&self->items) HashMap<Object*, string>();

	for (auto item : src->items) {
		Object* instance = NDO->create(item->val->type->name);
		instance->type->copy(instance, item->val);
		self->items.Put(item->key, instance);
	}
}

void DictObject::destructor(Object* self) {
	NDO_CASTV(DictObject, self, dict);
	dict->items.clear();
}

static alni save_size(DictObject* self) {
	// calculate size needed
	alni save_size = 0;

	// number on entries
	save_size += sizeof(alni);

	for (auto item : self->items) {
		// string length
    save_size += (item->key.size() + 1) * sizeof(*item->key.cstr());
		// object file adress
		save_size += sizeof(alni);
	}

	return save_size;
}

static void save(DictObject* self, File& file_self) {

	// write size
	alni len = self->items.nentries;
	file_self.write<alni>(&len);

	// save hashmap pairs
	for (auto item : self->items) {
		// item val
		alni ndo_object_adress = NDO->save(file_self, item->val);
		file_self.write<alni>(&ndo_object_adress);

		// item key
		file_self.write(item->key);
	}
}

static void load(File& file_self, DictObject* self) {

	new (&self->items) HashMap<Object*, string>();

	alni len;
	file_self.read<alni>(&len);

	for (alni i = 0; i < len; i++) {

		// read val
		alni ndo_object_adress;
		file_self.read<alni>(&ndo_object_adress);
		Object* val = NDO->load(file_self, ndo_object_adress);

		// read key value
		string key;
		file_self.read(key);

		// add to dictinary
		self->items.Put(key, val);
	}
}

struct ObjectType DictObjectType = {
	.base = NULL,
	.constructor = DictObject::constructor,
	.destructor = DictObject::destructor,
	.copy = DictObject::copy,
	.size = sizeof(DictObject),
	.name = "dict",
	.save_size = (object_save_size)save_size,
	.save = (object_save)save,
  .load = (object_load)load,
};
