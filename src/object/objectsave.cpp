

#include "object/object.h"

#include <malloc.h>

namespace obj {

	ObjectMemHead* bottom = nullptr;

	struct ObjectsFileHeader {
		char name[10] = {0};
		char version[10] = {0};

		ObjectsFileHeader(bool default_val = true) {
			if (default_val) {
				tp::memcp(&name, "objects", tp::slen("objects") + 1);
				tp::memcp(&version, "0", tp::slen("0") + 1);
			}
		}
	};


	Object* ObjectMemAllocate(const ObjectType* type) {
		ObjectMemHead* memh = (ObjectMemHead*) malloc(type->size + sizeof(ObjectMemHead));
		if (!memh) {
			return NULL;
		}

		memh->down = NULL;
		memh->flags = NULL;

		#ifdef OBJECT_REF_COUNT
		memh->refc = (tp::alni) 1;
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
		ObjectMemHead* memh = ((ObjectMemHead*) in) - 1;

		if (memh->up) {
			memh->up->down = memh->down;
		}

		if (memh->down) {
			memh->down->up = memh->up;
		} else {
			bottom = memh->up;
		}

		free(memh);
	}

	struct ObjectFileHead {
		Object* load_head_adress = 0;
	};

	tp::int1* loaded_file = nullptr;


	void clear_object_flags() {
		// clear all object flags
		for (ObjectMemHead* iter = bottom; iter; iter = iter->up) {
			iter->flags = -1;
		}
	}

	tp::alni& getObjectFlags(Object* in) {
		return NDO_MEMH_FROM_NDO(in)->flags;
	}

	tp::alni objsize_ram_util(Object* self, const ObjectType* type) {
		tp::alni out = 0;

		if (type->allocated_size) {
			out += type->allocated_size(self);
		} else {
			out += type->size - sizeof(ObjectType*);
		}

		if (type->base) {
			out += objsize_ram_util(self, type->base);
		} else {
			out += sizeof(ObjectType*);
		}

		return out;
	}

	tp::alni objects_api::objsize_ram(Object* self) {
		return objsize_ram_util(self, self->type);
	}

	tp::alni objects_api::objsize_ram_recursive_util(Object* self, const ObjectType* type, bool different_object) {
		tp::alni out = 0;

		if (different_object) {
			if (getObjectFlags(self) == 1) {
				return 0;
			}

			getObjectFlags(self) = 1;
		}		
		
		if (type->allocated_size_recursive) {
			out += type->allocated_size_recursive(self);
		} else {
			if (type->allocated_size) {
				out += type->allocated_size(self);
			} else {
				out += type->size - sizeof(ObjectType*);
			}
		}

		if (type->base) {
			out += objsize_ram_recursive_util(self, type->base, false);
		} else {
			out += sizeof(ObjectType*);
		}
		
		return out;
	}

	tp::alni objects_api::objsize_ram_recursive(Object* self) {
		clear_object_flags();
		return objsize_ram_recursive_util(self, self->type);
	}

	tp::alni objsize_file_util(Object* self, const ObjectType* type) {
		tp::alni out = 0;

		if (type->save_size) {
			out += type->save_size(self);
		}

		if (type->base) {
			out += objsize_file_util(self, type->base);
		}
		return out;
	}

	tp::alni objects_api::objsize_file(Object* self) {
		return objsize_file_util(self, self->type);
	}

	tp::alni objsize_file_recursive_util(Object* self, const ObjectType* type) {
		tp::alni out = 0;

		getObjectFlags(self) = 1;

		if (type->save_size) {
			out += type->save_size(self);
		}

		if (type->childs_retrival) {
			tp::Array<Object*> childs = type->childs_retrival(self);
			for (auto child : childs) {
				if (getObjectFlags(child.data()) != 1) {
					out += objsize_file_recursive_util(child.data(), child.data()->type);
				}
			}
		}

		if (type->base) {
			out += objsize_file_recursive_util(self, type->base);
		}

		return out;
	}

	tp::alni objects_api::objsize_file_recursive(Object* self) {
		clear_object_flags();
		return objsize_file_recursive_util(self, self->type);
	}

	void object_recursive_save(tp::File& ndf, Object* self, const ObjectType* type) {
		if (type->base) {
			object_recursive_save(ndf, self, type->base);
		}

		// automatically offsets for parent type to write
		if (type->save) {
			type->save(self, ndf);
		}
	}

	tp::alni objects_api::save(tp::File& ndf, Object* in) {

		// if already saved return file_adress
		if (NDO_MEMH_FROM_NDO(in)->flags != -1) {
			return NDO_MEMH_FROM_NDO(in)->flags;
		}

		// save write adress for parent save function call 
		tp::alni tmp_adress = ndf.adress;

		// save requested object to first available adress
		tp::alni save_adress = ndf.avl_adress;

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
		tp::alni file_alloc_size = objsize_file_util(in, in->type);

		// offes first available adress
		ndf.avl_adress += file_alloc_size;

		object_recursive_save(ndf, in, in->type);

		// restore adress for parent save function
		ndf.adress = tmp_adress;

		// return addres of saved object in file space
		return save_adress;
	}

	void object_recursive_load(tp::File& ndf, Object* out, const ObjectType* type) {
		if (type->base) {
			object_recursive_load(ndf, out, type->base);
		}

		// automatically offsets for parent type to read
		if (type->load) {
			type->load(ndf, out);
		}
	}

	Object* objects_api::load(tp::File& ndf, tp::alni file_adress) {

		// check if already saved
		if (((ObjectFileHead*) (loaded_file + file_adress))->load_head_adress) {
			return ((ObjectFileHead*) (loaded_file + file_adress))->load_head_adress;
		}

		// save read adress
		tp::alni parent_file_adress = ndf.adress;

		// set read adress
		ndf.adress = file_adress;

		ObjectFileHead ofh;
		ndf.read<ObjectFileHead>(&ofh);
		tp::string type_name;
		ndf.read(type_name);

		const ObjectType* object_type = NDO->types.get(type_name);
		Object* out = ObjectMemAllocate(object_type);

		if (!out) {
			return NULL;
		}

		// save heap adress in "loaded_file"
		((ObjectFileHead*) (loaded_file + file_adress))->load_head_adress = out;

		// loads recursivelly
		object_recursive_load(ndf, out, object_type);

		// restore read adress for parent call to continue
		ndf.adress = parent_file_adress;

		// return heap memory adress
		return out;
	}

	bool objects_api::save(Object* in, tp::string path) {
		tp::File ndf(path, tp::osfile_openflags::SAVE);

		if (!ndf.opened) {
			return false;
		}

		clear_object_flags();

		// presave
		for (tp::alni i = 0; i < SAVE_LOAD_MAX_CALLBACK_SLOTS; i++) {
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
		for (tp::alni i = 0; i < SAVE_LOAD_MAX_CALLBACK_SLOTS; i++) {
			if (sl_callbacks[i] && sl_callbacks[i]->post_save) {
				sl_callbacks[i]->post_save(sl_callbacks[i]->self, ndf);
			}
		}

		return true;
	}

	Object* objects_api::load(tp::string path) {
		tp::File ndf(path, tp::osfile_openflags::LOAD);

		if (!ndf.opened) {
			return NULL;
		}

		// check for compability
		ObjectsFileHeader current_header;
		ObjectsFileHeader loaded_header(false);
		ndf.read(&loaded_header);
		if (!tp::memequal(&current_header, &loaded_header, sizeof(ObjectsFileHeader))) {
			return NULL;
		}

		ndf.adress = 0;
		loaded_file = (tp::int1*) malloc(ndf.size());
		ndf.read_bytes(loaded_file, ndf.size());
		ndf.adress = sizeof(ObjectsFileHeader);

		// preload
		for (tp::alni i = 0; i < SAVE_LOAD_MAX_CALLBACK_SLOTS; i++) {
			if (sl_callbacks[i] && sl_callbacks[i]->pre_load) {
				sl_callbacks[i]->pre_load(sl_callbacks[i]->self, ndf);
			}
		}

		Object* out = load(ndf, ndf.adress);

		// post
		for (tp::alni i = 0; i < SAVE_LOAD_MAX_CALLBACK_SLOTS; i++) {
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

};