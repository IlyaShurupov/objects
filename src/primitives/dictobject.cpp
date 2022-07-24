#pragma once

#include "primitives/dictobject.h"

using namespace obj;
using namespace tp;

void DictObject::constructor(Object* self) {
	NDO_CASTV(DictObject, self, dict);

	new (&dict->items) HashMap<Object*, string>();
}

void DictObject::copy(Object* in, const Object* target) {
	NDO_CASTV(DictObject, in, self);
	NDO_CASTV(DictObject, target, src);

	destructor(self);
	constructor(self);

	for (auto item : src->items) {
		Object* instance = NDO->create(item->val->type->name);
		instance->type->copy(instance, item->val);
		self->items.put(item->key, instance);
	}
}

void DictObject::destructor(Object* self) {
	NDO_CASTV(DictObject, self, dict);
	for (auto item : dict->items) {
		NDO->destroy(item->val);
	}
	dict->items.~HashMap();
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
	alni len = self->items.size();
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

	new (&self->items) tp::HashMap<Object*, tp::string>();

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
		self->items.put(key, val);
	}
}

tp::Array<Object*> childs_retrival(DictObject* self) {
	tp::Array<Object*> out;
	out.reserve(self->items.size());
	for (auto item : self->items) {
		out[item.entry_idx] = item->val;
	}
	return out;
}

alni allocated_size(DictObject* self) {
	alni out = self->items.sizeAllocatedMem();
	for (auto item : self->items) {
		out += item->key.sizeAllocatedMem();
	}
	return out;
}

alni allocated_size_recursive(DictObject* self) {
	alni out = allocated_size(self);
	for (auto item : self->items) {
		out += NDO->objsize_ram_recursive_util(item->val, item->val->type);
	}
	return out;
}

struct obj::ObjectType DictObject::TypeData = {
	.base = NULL,
	.constructor = DictObject::constructor,
	.destructor = DictObject::destructor,
	.copy = DictObject::copy,
	.size = sizeof(DictObject),
	.name = "dict",
	.save_size = (object_save_size)save_size,
	.save = (object_save)save,
  .load = (object_load)load,
	.childs_retrival = (object_debug_all_childs_retrival) childs_retrival,
	.allocated_size = (object_allocated_size) allocated_size,
	.allocated_size_recursive = (object_allocated_size_recursive) allocated_size_recursive
};
