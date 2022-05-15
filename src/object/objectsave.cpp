

#include "object/object.h"

#include <malloc.h>

ObjectMemHead* bottom = nullptr;

struct ObjectsFileHeader {
	char name[10] = {0};
	char version[10] = {0};

	ObjectsFileHeader(bool default_val = true) {
		if (default_val) {
			memcp(&name, "objects", slen("objects") + 1);
			memcp(&version, "0", slen("0") + 1);
		}
	}
};


Object* ObjectMemAllocate(const ObjectType* type) {
	ObjectMemHead* memh = (ObjectMemHead*)malloc(type->size + sizeof(ObjectMemHead));
	if (!memh) {
		return NULL;
	}

	memh->down = NULL;
	memh->flags = NULL;

	#ifdef OBJECT_REF_COUNT
	memh->refc = (alni) 1;
	#endif

	if (bottom) {
		bottom->down = memh;
	}
	memh->up = bottom;
	bottom = memh;

	NDO_FROM_MEMH(memh)->type = type;
	return NDO_FROM_MEMH(memh);
}

void ObjectMemDeallocate(Object* in) {
	ObjectMemHead* memh = ((ObjectMemHead*)in) - 1;

	if (memh->up) {
		memh->up->down = memh->down;
	}

	if (memh->down) {
		memh->down->up = memh->up;
	}
	else {
		bottom = memh->up;
	}

	free(memh);
}

struct ObjectFileHead {
	Object* load_head_adress = 0;
};

int1* loaded_file = nullptr;

alni object_full_file_size(Object* self, const ObjectType* type) {
	alni out = 0;

	if (type->save_size) {
		out += type->save_size(self);
	}

	if (type->base) {
		return out + object_full_file_size(self, type->base);
	}
	return out;
}

void object_recursive_save(File& ndf, Object* self, const ObjectType* type) {
	if (type->base) {
		object_recursive_save(ndf, self, type->base);
	}

	// automatically offsets for parent type to write
	if (type->save) {
		type->save(self, ndf);
	}
}

alni objects_api::save(File& ndf, Object* in) {

	// if already saved return file_adress
	if (NDO_MEMH_FROM_NDO(in)->flags != -1) {
		return NDO_MEMH_FROM_NDO(in)->flags;
	}

	// save write adress for parent save function call 
	alni tmp_adress = ndf.adress;

	// save requested object to first available adress
	alni save_adress = ndf.avl_adress;

	// save file_adress in memhead
	NDO_MEMH_FROM_NDO(in)->flags = save_adress;

	// update write adress
	ndf.adress = save_adress;

	// save file object header
	ObjectFileHead ofh;
	ndf.write(&ofh);
	ndf.write(in->type->name);

	// allocate for object file header
	ndf.avl_adress += sizeof(ObjectFileHead) + in->type->name.size() + 1;

	// calc max size needed for saving all hierarchy of types
	alni file_alloc_size = object_full_file_size(in, in->type);

	// offes first available adress
	ndf.avl_adress += file_alloc_size;

	object_recursive_save(ndf, in, in->type);

	// restore adress for parent save function
	ndf.adress = tmp_adress;

	// return addres of saved object in file space
	return save_adress;
}

void object_recursive_load(File& ndf, Object* out, const ObjectType* type) {
	if (type->base) {
		object_recursive_load(ndf, out, type->base);
	}

	// automatically offsets for parent type to read
	if (type->load) {
		type->load(ndf, out);
	}
}

Object* objects_api::load(File& ndf, alni file_adress) {

	// check if already saved
	if (((ObjectFileHead*)(loaded_file + file_adress))->load_head_adress) {
		return ((ObjectFileHead*)(loaded_file + file_adress))->load_head_adress;
	}

	// save read adress
	alni parent_file_adress = ndf.adress;

	// set read adress
	ndf.adress = file_adress;

	ObjectFileHead ofh;
	ndf.read<ObjectFileHead>(&ofh);
	string type_name;
	ndf.read(type_name);

	const ObjectType* object_type = NDO->types.Get(type_name);
	Object* out = ObjectMemAllocate(object_type);

	if (!out) {
		return NULL;
	}

	// save heap adress in "loaded_file"
	((ObjectFileHead*)(loaded_file + file_adress))->load_head_adress = out;

	// loads recursivelly
	object_recursive_load(ndf, out, object_type);

	// restore read adress for parent call to continue
	ndf.adress = parent_file_adress;

	// return heap memory adress
	return out;
}

void objects_api::save(Object* in, string path) {
	File ndf(path, osfile_openflags::SAVE);

	// clear all object flags
	for (ObjectMemHead* iter = bottom; iter; iter = iter->up) {
		iter->flags = -1;
	}

	// presave
	for (alni i = 0; i < SAVE_LOAD_MAX_CALLBACK_SLOTS; i++) {
		if (sl_callbacks[i] && sl_callbacks[i]->pre_save) {
			sl_callbacks[i]->pre_save(sl_callbacks[i]->self, ndf);
		}
	}

	// save vesion info
	ObjectsFileHeader header;
	ndf.write(&header);

	ndf.avl_adress = ndf.adress;

	save(ndf, in);

	// postsave
	for (alni i = 0; i < SAVE_LOAD_MAX_CALLBACK_SLOTS; i++) {
		if (sl_callbacks[i] && sl_callbacks[i]->post_save) {
			sl_callbacks[i]->post_save(sl_callbacks[i]->self, ndf);
		}
	}
}

Object* objects_api::load(string path) {
	File ndf(path, osfile_openflags::LOAD);

	if (!ndf.opened) {
		return NULL;
	}
	
	// check for compability
	ObjectsFileHeader current_header;
	ObjectsFileHeader loaded_header(false);
	ndf.read(&loaded_header);
	if (!memequal(&current_header, &loaded_header, sizeof(ObjectsFileHeader))) {
		return NULL;
	}

	ndf.adress = 0;
	loaded_file = (int1*)malloc(ndf.size());
	ndf.read_bytes(loaded_file, ndf.size());
	ndf.adress = sizeof(ObjectsFileHeader);

	// preload
	for (alni i = 0; i < SAVE_LOAD_MAX_CALLBACK_SLOTS; i++) {
		if (sl_callbacks[i] && sl_callbacks[i]->pre_load) {
			sl_callbacks[i]->pre_load(sl_callbacks[i]->self, ndf);
		}
	}

	Object* out = load(ndf, ndf.adress);

	// post
	for (alni i = 0; i < SAVE_LOAD_MAX_CALLBACK_SLOTS; i++) {
		if (sl_callbacks[i] && sl_callbacks[i]->post_save) {
			sl_callbacks[i]->post_load(sl_callbacks[i]->self, ndf);
		}
	}

	free(loaded_file);
	return out;
}

void objects_api::add_sl_callbacks(save_load_callbacks* in) {
	sl_callbacks[sl_callbacks_load_idx] = in;
	sl_callbacks_load_idx++;
}