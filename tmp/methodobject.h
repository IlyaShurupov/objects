
#pragma once

#include "object.h"

struct StaticCCodeTable {
	
	hmap_s<ndo_static_method, string> table;

	void add(string name, ndo_static_method adress) {
		alni idx;
		if (table.Presents(name, idx)) {
			assert(0); // redefinition
		}

		table.Put(name, adress);
	}

	ndo_static_method get(string name) {
		alni idx;
		if (!table.Presents(name, idx)) {
			assert(0); // access violation
		}

		return table.from_slot_idx(idx);
	}

	string address_name_from_adress(ndo_static_method address) {
		for (auto iter : table) {
			if (iter->val.method == address.method) {
				return iter->key;
			}
		}

		assert(0); // access violation
	}

};

extern StaticCCodeTable static_ccode_table;

union MethoObjectCode {
	string pycode;
	ndo_static_method ccode;

	~MethoObjectCode() {
	}
};

struct MethodObject : Object {

	alni code_flags;
	struct ClassObject* self;
	MethoObjectCode code;

	void assign(string pycode) {
		if (code_flags == 1) {
			code.pycode = pycode;
		}
		else {
			new (&code.pycode) string(pycode);
			code_flags = 1;
		}
	}

	void assign_ccode(string address_name) {
		if (code_flags == 1) {
			code.pycode.~string();
			code_flags = 0;
		}
		code.ccode = static_ccode_table.get(address_name);
	}

	static void constructor(Object* in);
	static void copy(Object* self, const Object* in);

	Object* operator()();

	static void from_string(Object* self, string in);
};

extern ObjectType MethodObjectType;