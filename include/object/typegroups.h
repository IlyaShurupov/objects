
#pragma once

#include "map.h"
#include "strings.h"

namespace obj {

	struct ObjectType;
	struct objects_api;

	class TypeGroups {

		typedef tp::HashMap<obj::TypeGroups*, tp::string> Dict;

		bool is_group;
		union {
			Dict childs;
			const obj::ObjectType* type;
		};

		TypeGroups();

		friend struct obj::objects_api;

		public:

		typedef tp::HashMap<obj::TypeGroups*, tp::string> Dict;

		TypeGroups(bool is_group);

		void addType(ObjectType* type, tp::init_list<const char*> path, tp::alni cur_arg = 0);
		void setType(ObjectType* type);
		bool isGroup();
		Dict* getChilds();
		const obj::ObjectType* getType();
		~TypeGroups();
	};

};