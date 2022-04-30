

#include "oedit.h"

//#include "imgui.h"
#include "imgui_internal.h"

oeditor::oeditor() : ImGui::CompleteApp(vec2f(1400, 800)) {
	oh = objects_init();
	primitives_define_types(oh);
	test();

	root = (DictObject*) oh->create("dict");
	active = root;
	path.push({active , "dict 'root'"});
}

object_path nullo_view(Object* in, Object*& clipboard, objects_api* oh) {
	ImGui::Text("Null Object.");
	return object_path();
}

object_path linko_view(LinkObject* in, Object*& clipboard, objects_api* oh) {

	if (in->link) {

		ImGui::Text("%s at %x", in->link->type->name.cstr(), in->link);

		if (ImGui::Selectable("View")) {
			return {in->link, "adress"};
		}
	} else {
		ImGui::Text("Link Is Null");
	}

	if (ImGui::Selectable("Set from clipboard")) {
		in->link = clipboard;
	}

	if (ImGui::Selectable("Copy value")) {
		clipboard = in->link;
	}
	if (ImGui::Selectable("Set Null")) {
		in->link = NULL;
	}

	return object_path();
}

object_path into_view(IntObject* in, Object*& clipboard, objects_api* oh) {
	ImGui::Text("Int Value: "); ImGui::SameLine();
	int val = (int) in->val;
	ImGui::InputInt(" ", &val);
	in->val = (alni) val;
	return object_path();
}

object_path dicto_view(DictObject* active, Object*& clipboard, objects_api* oh) {

	bool popup = false;
	object_path out;

	if (active->items.nentries) {
		ImGui::Text("Dictinary Items: ");
	} else {
		ImGui::Text("Dictinary Is Empty. ");
	}

	ImGui::BeginChild("child_2", {0, 0}, true, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground);
	for (auto childo : active->items) {
		ImGui::PushID((int) childo.entry_idx);

		if (ImGui::Selectable(childo.iter->key.cstr())) {
			out = {childo.iter->val,  childo.iter->val->type->name + " '" + childo.iter->key + "'"};
			ImGui::PopID();
			break;
		}

		if (ImGui::BeginPopupContextItem(childo.iter->key.cstr(), ImGuiPopupFlags_MouseButtonRight)) {
			popup = true;

			ImGui::Text("%s at %x", childo->val->type->name.cstr(), childo->val);

			static Object* name_parent = NULL;
			static char name[100] = {"asdas"};

			if (name_parent != childo) {
				memcp(name, childo->key.cstr(), childo->key.size() + 1);
				name_parent = childo;
			}

			if (ImGui::InputTextEx(" ", "new name", name, 100, {140 , 30}, ImGuiInputTextFlags_EnterReturnsTrue)) {
				alni idx = active->items.Presents(name);
				if (idx != -1) {
					//gui.Notify("Object with such name Already Exists");
				} else {
					active->items.Remove(childo->key);
					active->items.Put(string(name).capture(), name_parent);
					name_parent = NULL;
					ImGui::EndPopup();
					ImGui::PopID();
					break;
				}
			}

			if (ImGui::Selectable("Exclude")) {
				active->items.Remove(childo->key);
				ImGui::EndPopup();
				ImGui::PopID();
				break;
			}

			if (ImGui::Selectable("Destroy")) {
				oh->destroy(childo->val);
				active->items.Remove(childo->key);
				ImGui::EndPopup();
				ImGui::PopID();
				break;
			}

			if (ImGui::Selectable("Copy Link")) {
				clipboard = childo.iter->val;
				ImGui::EndPopup();
				ImGui::PopID();
				break;
			}

			name_parent = childo;
			ImGui::EndPopup();
		}

		ImGui::PopID();
	}
	ImGui::EndChild();

	if (!popup && ImGui::BeginPopupContextItem("child_2", ImGuiPopupFlags_MouseButtonRight)) {

		if (ImGui::Selectable("Paste")) {

			alni idx = 1;
			string name_base = string("clipboard ") + clipboard->type->name + string(" ");
			string name_out = name_base;

			while (active->items.Presents(name_out) != -1) {
				name_out = name_base + string(idx);
				idx++;
			}

			active->items.Put(name_out, clipboard);
		}

		if (ImGui::BeginMenu("Create")) {

			ImGui::Text("Types: ");

			ImGui::BeginChild("child_4", {100, 140}, true, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground);

			for (auto childo : oh->types) {
				ImGui::PushID((int) childo.entry_idx);

				if (ImGui::Selectable(childo.iter->key.cstr())) {
					Object* newo = oh->create(childo->key);

					alni idx = 1;
					string name_base = string("new ") + newo->type->name + string(" ");
					string name_out = name_base;

					while (active->items.Presents(name_out) != -1) {
						name_out = name_base + string(idx);
						idx++;
					}

					active->items.Put(name_out, newo);

					ImGui::PopID();
					break;
				}

				ImGui::PopID();
			}

			ImGui::EndChild();

			ImGui::EndMenu();
		}

		ImGui::EndPopup();
	}

	return out;
}

object_path listo_view(ListObject* active, Object*& clipboard, objects_api* oh) {
	bool popup = false;
	object_path out;

	if (active->items.Len()) {
		ImGui::Text("List Items: ");
	} else {
		ImGui::Text("List Is Empty. ");
	}

	ImGui::BeginChild("child_2", {0, 0}, true, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground);
	for (auto childo : active->items) {
		ImGui::PushID((int) childo.Idx());

		if (ImGui::Selectable(childo->type->name.cstr())) {
			out = object_path(childo.Data(), childo->type->name + " at " + string(childo.Idx()));
			ImGui::PopID();
			break;
		}

		if (ImGui::BeginPopupContextItem(childo->type->name.cstr(), ImGuiPopupFlags_MouseButtonRight)) {
			popup = true;

			ImGui::Text("%s at %x", childo->type->name.cstr(), childo.Data());

			if (ImGui::Selectable("Exclude")) {
				active->items.DelNode(childo.node());
				ImGui::EndPopup();
				ImGui::PopID();
				break;
			}

			if (ImGui::Selectable("Copy Link")) {
				clipboard = childo.Data();
				ImGui::EndPopup();
				ImGui::PopID();
				break;
			}

			if (childo.node()->prev && ImGui::Selectable("Move Up")) {
				SWAP(childo.node()->prev->data, childo.Data(), Object*);
				ImGui::EndPopup();
				ImGui::PopID();
				break;
			}

			if (childo.node()->next && ImGui::Selectable("Move Down")) {
				SWAP(childo.node()->next->data, childo.Data(), Object*);
				ImGui::EndPopup();
				ImGui::PopID();
				break;
			}

			if (ImGui::Selectable("Destroy")) {
				oh->destroy(childo.Data());
				active->items.DelNode(childo.node());
				ImGui::EndPopup();
				ImGui::PopID();
				break;
			}

			ImGui::EndPopup();
		}

		ImGui::PopID();
	}
	ImGui::EndChild();

	if (!popup && ImGui::BeginPopupContextItem("child_2", ImGuiPopupFlags_MouseButtonRight)) {

		if (ImGui::Selectable("Paste")) {
			active->items.PushBack(clipboard);
		}

		if (ImGui::BeginMenu("Create")) {

			ImGui::Text("Types: ");

			ImGui::BeginChild("child_4", {100, 140}, true, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground);

			for (auto childo : oh->types) {
				ImGui::PushID((int) childo.entry_idx);

				if (ImGui::Selectable(childo.iter->key.cstr())) {
					Object* newo = oh->create(childo->key);
					active->items.PushBack(newo);
					ImGui::PopID();
					break;
				}

				ImGui::PopID();
			}

			ImGui::EndChild();

			ImGui::EndMenu();
		}

		ImGui::EndPopup();
	}

	return out;
}

object_path stringo_view(StringObject* in, Object*& clipboard, objects_api* oh) {

	ImGui::Text("String Data: ");

	static char val[2048] = {" "};

	if (in->val != val) {
		memcp(val, in->val.cstr(), in->val.size() + 1);
	}

	ImGui::InputTextMultiline(" ", val, 2048, {ImGui::GetWindowContentRegionWidth(), ImGui::GetWindowContentRegionWidth() * 1.1f});

	if (in->val != val) {
		in->val = string(val).capture();
	}

	return object_path();
}

object_path floato_view(Object* in, Object*& clipboard, objects_api* oh) {
	return object_path();
}

void oeditor::oexplorer() {

	//ImGui::Text("View Path: "); ImGui::SameLine();
	ImGui::BeginChild("child_path", {ImGui::GetWindowContentRegionWidth(), 45}, false, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_HorizontalScrollbar);
	stack<object_path*> rev_path;
	for (auto childo = path.last; childo; childo = childo->prev) {
		rev_path.push(&childo->data);
	}
	alni idx = 0;
	for (auto childo = rev_path.last; childo; childo = childo->prev) {
		ImGui::PushID((int) idx);
		bool go_back = false;
		if (childo == rev_path.last) {
			go_back = ImGui::Button(childo->data->id.cstr()); ImGui::SameLine();
		} else {
			go_back = ImGui::Button((childo->data->id).cstr()); ImGui::SameLine();
		}
		ImGui::PopID();

		if (ImGui::BeginPopupContextItem(NULL, ImGuiPopupFlags_MouseButtonRight)) {

			Object* curretn_object = childo->data->obj;
			ImGui::Text("%s at %x", curretn_object->type->name.cstr(), curretn_object);
			ImGui::Separator();

			if (ImGui::Selectable("Copy Link")) {
				clipboard = curretn_object;
			}

			if (ImGui::Selectable("Instantiate  ")) {
				clipboard = oh->create(curretn_object->type->name.cstr());
				oh->copy(clipboard, curretn_object);
				Notify("Object copied to clipboard");
			}

			ImGui::Separator();

			static char path_str[100] = {"data.o\0"};
			ImGui::InputTextEx(" ", "save path", path_str, 100, {100, 30}, 0);

			bool save_object = ImGui::Selectable("Save Object");
			bool load_object = ImGui::Selectable("Load Object");

			if (save_object) {
				oh->save(curretn_object, path_str);
				Notify("Object saved");
			}

			if (load_object) {
				Object* loadedo = oh->load(path_str);
				if (loadedo) {
					clipboard = loadedo;
					Notify("Object copied to clipboard");
				} else {
					Notify("Can't load Object");
				}
			}

			ImGui::EndPopup();
		}

		if (go_back) {
			while (&path.last->data != childo->data) {
				path.pop();
			}
			active = childo->data->obj;
			ImGui::EndChild();
			return;
		}
		idx++;
	}
	ImGui::Text(" ");
	ImGui::EndChild();

	ImGui::Separator();
	object_path new_active;
	if (active->type->name == "null") {
		new_active = nullo_view(active, clipboard, oh);
	} else if (active->type->name == "link") {
		new_active = linko_view((LinkObject*) active, clipboard, oh);
	} else if (active->type->name == "int") {
		new_active = into_view((IntObject*) active, clipboard, oh);
	} else if (active->type->name == "float") {
		new_active = floato_view(active, clipboard, oh);
	} else if (active->type->name == "str") {
		new_active = stringo_view((StringObject*) active, clipboard, oh);
	} else if (active->type->name == "list") {
		new_active = listo_view((ListObject*) active, clipboard, oh);
	} else if (active->type->name == "dict") {
		new_active = dicto_view((DictObject*) active, clipboard, oh);
	} else {
		ImGui::Text("Unknown Type");
	}

	if (new_active != NULL) {
		active = new_active.obj;
		path.push(new_active);
	}
}

void oeditor::oproperties(const ObjectType* type) {
	ImGui::SameLine();
	ImGui::Text("Type: %s", type->name.cstr());
	if (type->base) {
		ImGui::Text("Inherits From"); ImGui::SameLine();
		if (ImGui::Selectable(type->base->name.cstr())) {
			ImGui::PushID(type->base->name.cstr());
			oproperties(type->base);
			ImGui::PopID();
		}
	}
	ImGui::Text("Object Struct Size: %i bytes", type->size);
	if (type->save_size) ImGui::Text("Object Save Size: %i bytes", type->save_size(active));

	if (type->methods) {
		ImGui::BeginChild("methods", {ImGui::GetWindowContentRegionWidth(), 350}, false, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_HorizontalScrollbar);

		ImGui::Text("type Methods: ");

		for (alni idx = 0; type->methods[idx].adress; idx++) {
			type_method* method = &type->methods[idx];
			ImGui::PushID(method->name.cstr());
			ImGui::Text("%s", method->name.cstr());
			ImGui::PopID();
		}
		ImGui::EndChild();
	}
}

void oeditor::MainDrawTick() {
	if (WindowEditor("Explorer")) {
		oexplorer();
	}
	ImGui::End();

	if (WindowEditor("Object Info")) {
		oproperties(active->type);
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
