#include <Core/Core.h>

#include <stdio.h>

#include <yaml-cpp/yaml.h>

using namespace Upp;

static bool CheckNode(const YAML::Node& node)
{
	if(!node.IsMap())
		return false;
	if(node["name"].as<std::string>() != "ocio")
		return false;
	if(node["count"].as<int>() != 42)
		return false;
	if(!node["enabled"].as<bool>())
		return false;
	const YAML::Node items = node["items"];
	if(!items.IsSequence() || items.size() != 3)
		return false;
	if(items[0].as<std::string>() != "red" || items[1].as<std::string>() != "green" || items[2].as<std::string>() != "blue")
		return false;
	const YAML::Node nested = node["nested"];
	if(!nested.IsMap())
		return false;
	if(nested["alpha"].as<int>() != 7)
		return false;
	if(nested["beta"].as<bool>() != false)
		return false;
	return true;
}

int main()
{
	int passed = 0;
	int failed = 0;

	const char* yaml_text =
		"name: ocio\n"
		"count: 42\n"
		"enabled: true\n"
		"items:\n"
		"  - red\n"
		"  - green\n"
		"  - blue\n"
		"nested:\n"
		"  alpha: 7\n"
		"  beta: false\n";

	try {
		YAML::Node node = YAML::Load(yaml_text);
		if(CheckNode(node)) {
			printf("yaml parse: OK\n");
			passed++;
		} else {
			printf("yaml parse: FAIL\n");
			failed++;
		}

		YAML::Emitter out;
		out << node;
		const std::string emitted = out.c_str();
		YAML::Node reparsed = YAML::Load(emitted);
		if(CheckNode(reparsed)) {
			printf("yaml emit/reparse: OK\n");
			passed++;
		} else {
			printf("yaml emit/reparse: FAIL\n");
			failed++;
		}
	} catch(const YAML::Exception& e) {
		printf("yaml parse/emit: FAIL (%s)\n", e.what());
		failed += 2;
	}

	try {
		(void)YAML::Load("name: [1, 2");
		printf("malformed yaml: FAIL\n");
		failed++;
	} catch(const YAML::ParserException& e) {
		if(e.what() && *e.what()) {
			printf("malformed yaml: OK (%s)\n", e.what());
			passed++;
		} else {
			printf("malformed yaml: FAIL\n");
			failed++;
		}
	}

	printf("SUMMARY passed=%d failed=%d\n", passed, failed);
	return failed ? 1 : 0;
}
