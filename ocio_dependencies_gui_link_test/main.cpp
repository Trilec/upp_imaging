#include <CtrlLib/CtrlLib.h>
#include <Ui/Ui.h>

#include <stdio.h>
#include <string.h>

#include <expat/expat.h>
#include <imath/ImathVec.h>
#include <minizip_ng/mz.h>
#include <minizip_ng/mz_strm.h>
#include <minizip_ng/mz_zip.h>
#include <minizip_ng/mz_zip_rw.h>
#include <pystring/pystring.h>
#include <yaml_cpp/yaml.h>
#include <zlib.h>

#ifndef UPP_IMAGING_LOCAL_YAML_CPP_INCLUDE_TREE
#error local yaml-cpp package not selected
#endif

#ifndef UPP_IMAGING_LOCAL_PYSTRING_INCLUDE
#error local pystring package not selected
#endif

#ifndef UPP_IMAGING_LOCAL_MINIZIP_NG_INCLUDE
#error local minizip-ng package not selected
#endif

using namespace Upp;

GUI_APP_MAIN
{
	int passed = 0;
	int failed = 0;

	XML_Parser parser = XML_ParserCreate(NULL);
	if(parser) {
		XML_ParserFree(parser);
		printf("OCIO GUI dependency Expat: OK\n");
		passed++;
	} else {
		printf("OCIO GUI dependency Expat: FAIL\n");
		failed++;
	}

	try {
		YAML::Node node = YAML::Load("name: ocio\nitems: [a, b]\n");
		if(node["name"].as<std::string>() == "ocio" && node["items"].IsSequence() && node["items"].size() == 2) {
			printf("OCIO GUI dependency yaml-cpp: OK\n");
			passed++;
		} else {
			printf("OCIO GUI dependency yaml-cpp: FAIL\n");
			failed++;
		}
	} catch(const YAML::Exception&) {
		printf("OCIO GUI dependency yaml-cpp: FAIL\n");
		failed++;
	}

	if(pystring::lower("UI") == "ui") {
		printf("OCIO GUI dependency pystring: OK\n");
		passed++;
	} else {
		printf("OCIO GUI dependency pystring: FAIL\n");
		failed++;
	}

	const String zip_path = GetExeDirFile("ocio_dependencies_gui.zip");
	FileDelete(zip_path);
	void* writer = mz_zip_writer_create();
	if(writer) {
		const int open_rc = mz_zip_writer_open_file(writer, ~zip_path, 0, 0);
		if(open_rc == MZ_OK) {
			const int close_rc = mz_zip_writer_close(writer);
			if(close_rc == MZ_OK) {
				FileIn in(zip_path);
				if(in.IsOpen() && in.GetSize() > 0) {
					printf("OCIO GUI dependency minizip-ng: OK\n");
					passed++;
				} else {
					printf("OCIO GUI dependency minizip-ng: FAIL\n");
					failed++;
				}
			} else {
				printf("OCIO GUI dependency minizip-ng: FAIL\n");
				failed++;
			}
		} else {
			printf("OCIO GUI dependency minizip-ng: FAIL\n");
			failed++;
		}
		mz_zip_writer_delete(&writer);
		FileDelete(zip_path);
	} else {
		printf("OCIO GUI dependency minizip-ng: FAIL\n");
		failed++;
	}

	const Imath::V3f v(1.0f, 2.0f, 3.0f);
	if(v.length() > 0.0f) {
		printf("OCIO GUI dependency Imath: OK\n");
		passed++;
	} else {
		printf("OCIO GUI dependency Imath: FAIL\n");
		failed++;
	}

	if(zlibVersion() && strcmp(zlibVersion(), "1.3.2") == 0) {
		printf("OCIO GUI dependency zlib: OK\n");
		passed++;
	} else {
		printf("OCIO GUI dependency zlib: FAIL\n");
		failed++;
	}

	printf("SUMMARY passed=%d failed=%d\n", passed, failed);
	SetExitCode(failed ? 1 : 0);
}
