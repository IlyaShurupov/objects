
#include "ImGuiObjectEditor.h"

class oeditor_test : public ImGui::CompleteApp {

	ImGui::ImGuiObjectEditor objects_gui;

	public:

	void MainDrawTick() {
		objects_gui.Draw();
	}
};