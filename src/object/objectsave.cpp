

#include "object/object.h"

#define NDO_MEMH_FROM_NDO(ndo_ptr) (((ObjectMemHead*)ndo_ptr) - 1)
#define NDO_FROM_MEMH(ndo_ptr) ((Object*)(ndo_ptr + 1))

struct ObjectMemHead {
	ObjectMemHead* up;
	ObjectMemHead* down;
	alni flags;
};

ObjectMemHead* bottom = nullptr;

Object* ObjectMemAllocate(const ObjectType* type) {
	ObjectMemHead* memh = (ObjectMemHead*)malloc(type->size + sizeof(ObjectMemHead));
	if (!memh) {
		return NULL;
	}

	memh->down = NULL;
	memh->flags = NULL;

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
		memh->down = memh->up;
	}
	else {
		bottom = memh->up;
	}

	free(memh);
}

struct ObjectFileHead {

	ObjectFileHead(const ObjectType* in) {
		alni i = 0;
		for (; in->name.str[i] != '\0'; i++) {
			type_name[i] = in->name.str[i];
		}
		type_name[i] = '\0';
		load_head_adress = 0;
	}

	ObjectFileHead() {
		type_name[15] = 0;
		load_head_adress = 0;
	}

	char type_name[16];
	Object* load_head_adress;
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
	ObjectFileHead ofh(in->type);
	ndf.write<ObjectFileHead>(&ofh);

	// allocate for object file header
	ndf.avl_adress += sizeof(ObjectFileHead);

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

	const ObjectType* object_type = NDO->types.Get(ofh.type_name);
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
	File ndf(path.str, FileOpenFlags::SAVE);

	for (ObjectMemHead* iter = bottom; iter; iter = iter->up) {
		iter->flags = -1;
	}

	save(ndf, in);
}

Object* objects_api::load(string path) {
	File ndf(path.str, FileOpenFlags::LOAD);

	loaded_file = (int1*)malloc(ndf.size());
	ndf.read_bytes(loaded_file, ndf.size());

	Object* out = load(ndf, 0);

	free(loaded_file);
	return out;
}
