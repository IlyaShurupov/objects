

#include "oedit.h"

int main() {

	tp::alloc_init();

	{
		obj::objects_init();
		obj::primitives_define_types();

		oeditor_test app;
		app.Run();

		obj::objects_finalize();
	}

	tp::alloc_uninit();
}