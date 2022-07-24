
#pragma once

#include "primitives/LinkObject.h"

using namespace obj;
using namespace tp;

void LinkObject::constructor(Object* self) {
	NDO_CAST(LinkObject, self)->link = 0;
}

void LinkObject::copy(Object* self, const Object* in) {
	NDO_CAST(LinkObject, self)->link = NDO_CAST(LinkObject, in)->link;
}

LinkObject* LinkObject::create(Object* in) {
	NDO_CASTV(LinkObject, NDO->create("link"), out)->link = in;
	return out;
}

static alni save_size(LinkObject* self) {
	return sizeof(alni);
}

static void save(LinkObject* self, File& file_self) {
	if (self->link != NULL) {
		alni link_object_save_adress = NDO->save(file_self, self->link);
		file_self.write<alni>(&link_object_save_adress);
	}
	else {
		alni null = -1;
		file_self.write<alni>(&null);
	}
}

static void load(File& file_self, LinkObject* self) {
	
	alni saved_object_adress;
	file_self.read<alni>(&saved_object_adress);

	if (saved_object_adress == -1) {
		self->link = NULL;
	}
	else {
		self->link = NDO->load(file_self, saved_object_adress);
	}
}

tp::Array<Object*> childs_retrival(LinkObject* self) {
	tp::Array<Object*> out;
	if (self->link) out.pushBack(self->link);
	return out;
}

alni allocated_size(LinkObject* self) {
	return sizeof(Object*);
}

alni allocated_size_recursive(LinkObject* self) {
	alni out = sizeof(Object*);
	if (self->link) {
		out += NDO->objsize_ram_recursive_util(self->link, self->link->type);
	}
	return out;
}

struct obj::ObjectType LinkObject::TypeData = {
	.base = NULL,
	.constructor = LinkObject::constructor,
	.destructor = NULL,
	.copy = LinkObject::copy,
	.size = sizeof(LinkObject),
	.name = "link",
	.convesions = NULL,
	.save_size = (object_save_size)save_size,
	.save = (object_save)save,
	.load = (object_load)load,
	.childs_retrival = (object_debug_all_childs_retrival) childs_retrival,
	.allocated_size = (object_allocated_size) allocated_size,
	.allocated_size_recursive = (object_allocated_size_recursive) allocated_size_recursive
};