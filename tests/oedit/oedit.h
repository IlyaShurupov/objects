
//#include "object/object.h"
//#include "primitives/primitives.h"

//#include "gl.h"
//#include "window.h"

#include "ImGuiObjectEditor.h"

class oeditor_test : public ImGui::CompleteApp {

	ImGui::ImGuiObjectEditor objects_gui;

	public:

	void MainDrawTick() {
		objects_gui.Draw();
	}
};