#include <CtrlLib/CtrlLib.h>
#include <Ui/Ui.h>

#include <stdio.h>
#include <string.h>

#include <expat/expat.h>
#include <imath/ImathVec.h>
#include <minizip_ng/mz.h>
#include <minizip_ng/mz_zip.h>
#include <minizip_ng/mz_zip_rw.h>
#include <pystring/pystring.h>
#include <yaml_cpp/yaml.h>
#include <zlib.h>

using namespace Upp;

GUI_APP_MAIN
{
	int passed = 0;
	int failed = 0;

	XML_Parser parser = XML_ParserCreate(NULL);
	if(parser) {
		XML_ParserFree(parser);
		passed++;
	} else {
		failed++;
	}

	try {
		YAML::Node node = YAML::Load("name: ocio\n");
		if(node["name"].as<std::string>() == "ocio")
			passed++;
		else
			failed++;
	} catch(const YAML::Exception&) {
		failed++;
	}

	if(pystring::lower("UI") == "ui")
		passed++;
	else
		failed++;

	void* writer = mz_zip_writer_create();
	if(writer) {
		const String path = GetExeDirFile("ocio_dependencies_gui.zip");
		FileDelete(path);
		if(mz_zip_writer_open_file(writer, ~path, 0, 0) == MZ_OK) {
			mz_zip_writer_close(writer);
			passed++;
		} else {
			failed++;
		}
		mz_zip_writer_delete(&writer);
		FileDelete(path);
	} else {
		failed++;
	}

	const Imath::V3f v(1.0f, 2.0f, 3.0f);
	if(v.length() > 0.0f)
		passed++;
	else
		failed++;

	if(zlibVersion() && strcmp(zlibVersion(), "1.3.2") == 0)
		passed++;
	else
		failed++;

	SetExitCode(failed ? 1 : 0);
}
