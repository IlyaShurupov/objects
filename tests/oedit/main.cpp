

#include "oedit.h"

int main() {
	obj::objects_init();
	obj::primitives_define_types();

	{
		oeditor_test app;
		app.Run();
	}

	obj::objects_finalize();
}