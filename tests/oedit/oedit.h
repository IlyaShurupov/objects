
//#include "object/object.h"
//#include "primitives/primitives.h"

//#include "gl.h"
//#include "window.h"

#include "ImGuiObjectEditor.h"

class oeditor_test : public ImGui::CompleteApp {

	objects_api* oh;

	DictObject* root;

	Object* active;
	stack<ImGui::object_path> path;
	Object* clipboard = NULL;


	ImGui::ImGuiObjectEditor objects_gui;

	public:

	oeditor_test() {
		oh = objects_init();
		primitives_define_types(oh);
		root = (DictObject*) oh->create("dict");
		active = root;
		path.push({active , "dict 'root'"});

		objects_gui.oedior_init(root, oh, &window);
	}

	
	void MainDrawTick() {
		objects_gui.Draw();
	}
	
	~oeditor_test() {
		oh->destroy(root);
		objects_finalize();
	}
};