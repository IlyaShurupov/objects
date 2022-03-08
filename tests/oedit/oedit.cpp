

#include "imgui.h"
#include "implot.h"
#include "imgui_utils.h"

#include "oedit.h"


oeditor::oeditor() {
	oh = objects_init();
	primitives_define_types(oh);
	test();
}

void oeditor::draw() {
	if (EditorWindow("Info")) {
		ImGui::Text("Under development");
	}
	ImGui::End();
}

void oeditor::test() {

	Object* myint = oh->create("int");

	oh->set(myint, alni(12));

	oh->save(myint, "A:\\data\\tmp\\myint.o");

	Object* saved_int = oh->load("A:\\data\\tmp\\myint.o");

	NDO_CAST(IntObject, saved_int)->val;
}