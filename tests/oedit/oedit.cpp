

#include "imgui.h"
#include "implot.h"
#include "imgui_utils.h"

#include "oedit.h"


oeditor::oeditor() {
	oh = objects_init();
	primitives_define_types(oh);
	test();

	root = (DictObject*)oh->create("dict");
	active = root;
	path.push(active);
}

Object* nullo_view(Object*& in, Object*& clipboard) {
	return NULL;
}

Object* linko_view(Object*& in, Object*& clipboard) {
	return NULL;
}

Object* into_view(Object*& in, Object*& clipboard) {
	return NULL;
}

Object* dicto_view(DictObject* active, Object*& clipboard) {

	if (!SubMenuBegin("Items", 2)) {
		return NULL;
	}
	
	Object* out = NULL;

	ImGui::BeginListBox("");
	for (auto childo : active->items) {
		ImGui::PushID(childo.entry_idx);
		if (ImGui::Button(childo.iter->key.cstr())) {
			out = childo.iter->val;
			ImGui::PopID();
			break;
		}
		ImGui::SameLine();
		if (ImGui::Button("Exclude")) {
			active->items.Remove(childo->key);
			ImGui::PopID();
			break;
		}
		ImGui::SameLine();
		if (ImGui::Button("Copy Link")) {
			clipboard = childo.iter->val;
			ImGui::PopID();
			break;
		}
		ImGui::PopID();
	}
	ImGui::EndListBox();


	bool paste_object = ImGui::Button("paste"); ImGui::SameLine();
	static char paste_name[100] = { "clipboard_object\0" };
	ImGui::InputText("name", paste_name, 100);
	if (paste_object) {
		
		string key_val = paste_name;
		key_val.capture();

		if (active->items.Presents(key_val) != -1) {
			if (clipboard) {
				Notify("Object replaced");
				active->items.Put(key_val, clipboard);
			}
		}
		else {
			if (clipboard) {
				Notify("Object included");
				active->items.Put(key_val, clipboard);
			}
		}

		if (!clipboard) {
			Notify("Clipboard is empty", 3);
		}
	}

	SubMenuEnd(2);
	return out;
}

Object* listo_view(Object*& in, Object*& clipboard) {
	return NULL;
}

Object* stringo_view(Object*& in, Object*& clipboard) {
	return NULL;
}

Object* floato_view(Object*& in, Object*& clipboard) {
	return NULL;
}

void oeditor::oexplorer() {

	ImGui::Text("%s at %x", active->type->name.cstr(), active);
	
	if (path.length > 1) {
		ImGui::SameLine();
		if (ImGui::Button("back")) {
			path.pop();
			active = path.last->data;
			return;
		}
	}

	bool save_object = ImGui::Button("save"); ImGui::SameLine();
	bool load_object = ImGui::Button("load"); ImGui::SameLine();

	static char path_str[100] = { "data.o\0" };

	ImGui::InputText("path", path_str, 100);
	if (save_object) {
		oh->save(active, path_str);
		Notify("Object saved");
	}

	if (load_object) {
		Object* loadedo = oh->load(path_str);
		if (loadedo) {
			clipboard = loadedo;
			Notify("Object copied to clipboard");
		}
		else {
			Notify("Can't load Object");
		}
	}

	bool create_object = ImGui::Button("create"); ImGui::SameLine();
	static char create_type[100] = { "null\0" };
	ImGui::InputText("type", create_type, 100);
	if (create_object) {
		if (oh->types.Presents(create_type) != -1) {
			clipboard = oh->create(create_type);
			Notify("Object copied to clipboard");
		}
		else {
			Notify("No such type exist");
		}
	}

	Object* new_active = NULL;

	if (active->type->name == "null") {
		new_active = nullo_view(active, clipboard);
	}
	else if (active->type->name == "link") {
		new_active = linko_view(active, clipboard);
	}
	else if (active->type->name == "int") {
		new_active = into_view(active, clipboard);
	}
	else if (active->type->name == "float") {
		new_active = floato_view(active, clipboard);
	}
	else if (active->type->name == "string") {
		new_active = stringo_view(active, clipboard);
	}
	else if (active->type->name == "list") {
		new_active = listo_view(active, clipboard);
	}
	else if (active->type->name == "dict") {
		new_active = dicto_view((DictObject*)active, clipboard);
	}
	else {
		ImGui::Text("Unknown Type");
	}

	if (new_active != NULL) {
		active = new_active;
		path.push(active);
	}
}

void oeditor::oproperties() {

}

void oeditor::draw() {
	if (EditorWindow("Explorer")) {
		oexplorer();
	}
	ImGui::End();

	return;

	if (EditorWindow("Properties")) {
		oproperties();
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

oeditor::~oeditor() {
	oh->destroy(root);
}
