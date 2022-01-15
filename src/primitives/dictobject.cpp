#pragma once

#include "primitives/dictobject.h"


void DictObject::copy(Object* self, const Object* in) {
	NDO_CAST(DictObject, self)->items = NDO_CAST(DictObject, in)->items;
}

void DictObject::destructor(Object* self) {
	NDO_CASTV(DictObject, self, dict);

	for (auto item : dict->items) {
		NDO->destroy(item->val);
	}

	dict->items.clear();
}

static alni save_size(DictObject* self) {
	// calculate size needed
	alni save_size = 0;

	// number on entries
	save_size += sizeof(alni);

	for (auto item : self->items) {
		// string length
		save_size += sizeof(alni);
		// string itself
		save_size += item->key.len() * sizeof(*item->key.str);
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

		// item key len
		alni key_len = item->key.len();
		file_self.write<alni>(&key_len);

		// item key val
		file_self.write_bytes(item->key.str, item->key.len());
	}
}

static void load(File& file_self, DictObject* self) {

	new (&self->items) hmap<Object*, string>();

	alni len;
	file_self.read<alni>(&len);

	for (alni i = 0; i < len; i++) {

		// read val
		alni ndo_object_adress;
		file_self.read<alni>(&ndo_object_adress);
		Object* val = NDO->load(file_self, ndo_object_adress);

		// read key length
		alni key_len = 0;
		file_self.read<alni>(&key_len);

		// read key value
		string key;
		key.alloc(key_len);
		file_self.read_bytes(key.str, key_len);

		// add to dictinary
		self->items.Put(key, val);
	}
}

void DictObject::constructor(Object* self) {
	NDO_CASTV(DictObject, self, dict);

	new (&dict->items) hmap<Object*, string>();

	dict->items.del_values = false;
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
