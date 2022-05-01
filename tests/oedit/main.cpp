

#include "oedit.h"

int main() {
	objects_init();
	primitives_define_types();

	{
		oeditor_test app;
		app.Run();
	}

	objects_finalize();
}