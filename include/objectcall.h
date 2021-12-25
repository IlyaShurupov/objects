
#pragma once

#include "Types.h"

struct Object;

typedef Object* (*ndo_method)(Object* self, Object* args);

struct ndo_static_method {
	ndo_method method;
};

struct ObjectStaticMethod {
	ndo_static_method function_adress;
	string name;
};

union StackSlot {
	alni arg_count;
	Object* arg;
	Object* ret;
};

struct ObjectCallStack {
	StackSlot cs[1024];

	alni frameb = 0;
	alni frame_argc = 0;

	void push(Object* arg) {
		cs[frameb + frame_argc].arg = arg;
		frame_argc++;
	}

	Object* get_arg(alni idx) {
		return cs[frameb - cs[frameb - 1].arg_count - 1 + idx].arg;
	}

	void call() {
		push(NULL);
		cs[frameb + frame_argc].arg_count = frame_argc;
		frameb += frame_argc + 1;
		frame_argc = 0;
	}

	void ret() {
		frameb -= cs[frameb - 1].arg_count + 1;
		frame_argc = 0;
	}

};