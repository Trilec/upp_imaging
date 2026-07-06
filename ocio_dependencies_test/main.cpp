#include <Core/Core.h>

#include <stdio.h>
#include <string.h>
#include <imath/ImathVec.h>
#include <mz.h>
#include <mz_zip.h>
#include <mz_zip_rw.h>
#include <pystring.h>
#include <time.h>
#include <yaml-cpp/yaml.h>
#include <zlib.h>

#include <expat/expat.h>

using namespace Upp;

struct XmlState {
	Vector<String> events;
};

static void XMLCALL XmlStart(void* user_data, const XML_Char* name, const XML_Char** attrs)
{
	auto* state = static_cast<XmlState*>(user_data);
	String s = Format("start:%s", name);
	for(int i = 0; attrs && attrs[i]; i += 2)
		s << Format(" %s=%s", attrs[i], attrs[i + 1]);
	state->events.Add(s);
}

static void XMLCALL XmlEnd(void* user_data, const XML_Char* name)
{
	auto* state = static_cast<XmlState*>(user_data);
	state->events.Add(Format("end:%s", name));
}

static bool TestExpat()
{
	XmlState state;
	XML_Parser parser = XML_ParserCreate(NULL);
	if(!parser)
		return false;
	XML_SetUserData(parser, &state);
	XML_SetElementHandler(parser, XmlStart, XmlEnd);
	const char* xml = "<r a='1'><c>ok</c></r>";
	const int rc = XML_Parse(parser, xml, (int)strlen(xml), XML_TRUE);
	XML_ParserFree(parser);
	return rc == XML_STATUS_OK && state.events.GetCount() == 4 && state.events[0] == "start:r a=1" && state.events[1] == "start:c" && state.events[2] == "end:c" && state.events[3] == "end:r";
}

static bool TestYamlCpp()
{
	try {
		YAML::Node node = YAML::Load("name: ocio\ncount: 7\nitems: [a, b]\n");
		return node["name"].as<std::string>() == "ocio" && node["count"].as<int>() == 7 && node["items"].IsSequence() && node["items"].size() == 2 && node["items"][1].as<std::string>() == "b";
	} catch(const YAML::Exception&) {
		return false;
	}
}

static bool TestPystring()
{
	return pystring::lower("HeLLo") == "hello" && pystring::split("a,b", ",") == std::vector<std::string>{"a", "b"};
}

static bool TestMinizip()
{
	const String path = GetExeDirFile("ocio_dependencies_aggregate.zip");
	FileDelete(path);
	void* writer = mz_zip_writer_create();
	if(!writer)
		return false;
	if(mz_zip_writer_open_file(writer, ~path, 0, 0) != MZ_OK) {
		mz_zip_writer_delete(&writer);
		return false;
	}
	mz_zip_writer_set_compress_method(writer, MZ_COMPRESS_METHOD_STORE);
	const char one[] = "one";
	const char two[] = "two";
	mz_zip_file info;
	memset(&info, 0, sizeof(info));
	info.filename = "one.txt";
	info.compression_method = MZ_COMPRESS_METHOD_STORE;
	info.uncompressed_size = (int64_t)strlen(one);
	info.modified_date = time(NULL);
	if(mz_zip_writer_add_buffer(writer, (void*)one, (int)strlen(one), &info) != MZ_OK) {
		mz_zip_writer_close(writer);
		mz_zip_writer_delete(&writer);
		FileDelete(path);
		return false;
	}
	memset(&info, 0, sizeof(info));
	info.filename = "two.txt";
	info.compression_method = MZ_COMPRESS_METHOD_STORE;
	info.uncompressed_size = (int64_t)strlen(two);
	info.modified_date = time(NULL);
	if(mz_zip_writer_add_buffer(writer, (void*)two, (int)strlen(two), &info) != MZ_OK) {
		mz_zip_writer_close(writer);
		mz_zip_writer_delete(&writer);
		FileDelete(path);
		return false;
	}
	mz_zip_writer_close(writer);
	mz_zip_writer_delete(&writer);

	void* reader = mz_zip_reader_create();
	if(!reader)
		return false;
	bool ok = mz_zip_reader_open_file(reader, ~path) == MZ_OK;
	if(ok) {
		ok = mz_zip_reader_locate_entry(reader, "one.txt", 0) == MZ_OK && mz_zip_reader_entry_open(reader) == MZ_OK;
		if(ok) {
			char buf[8] = {};
			const int rc = mz_zip_reader_entry_read(reader, buf, (int)sizeof(buf));
			ok = rc == 3 && memcmp(buf, one, 3) == 0;
			mz_zip_reader_entry_close(reader);
		}
		if(ok) {
			ok = mz_zip_reader_locate_entry(reader, "two.txt", 0) == MZ_OK && mz_zip_reader_entry_open(reader) == MZ_OK;
			if(ok) {
				char buf[8] = {};
				const int rc = mz_zip_reader_entry_read(reader, buf, (int)sizeof(buf));
				ok = rc == 3 && memcmp(buf, two, 3) == 0;
				mz_zip_reader_entry_close(reader);
			}
		}
	}
	mz_zip_reader_delete(&reader);
	FileDelete(path);
	return ok;
}

static bool TestImath()
{
	const Imath::V3f a(1.0f, 2.0f, 3.0f);
	const Imath::V3f b(4.0f, 5.0f, 6.0f);
	const Imath::V3f c = a + b;
	return c.x == 5.0f && c.y == 7.0f && c.z == 9.0f;
}

static bool TestZlib()
{
	return strcmp(zlibVersion(), "1.3.2") == 0;
}

int main()
{
	int passed = 0;
	int failed = 0;

	if(TestExpat()) {
		printf("OCIO dependency Expat: OK\n");
		passed++;
	} else {
		printf("OCIO dependency Expat: FAIL\n");
		failed++;
	}

	if(TestYamlCpp()) {
		printf("OCIO dependency yaml-cpp: OK\n");
		passed++;
	} else {
		printf("OCIO dependency yaml-cpp: FAIL\n");
		failed++;
	}

	if(TestPystring()) {
		printf("OCIO dependency pystring: OK\n");
		passed++;
	} else {
		printf("OCIO dependency pystring: FAIL\n");
		failed++;
	}

	if(TestMinizip()) {
		printf("OCIO dependency minizip-ng: OK\n");
		passed++;
	} else {
		printf("OCIO dependency minizip-ng: FAIL\n");
		failed++;
	}

	if(TestImath()) {
		printf("OCIO dependency Imath: OK\n");
		passed++;
	} else {
		printf("OCIO dependency Imath: FAIL\n");
		failed++;
	}

	const char* version = zlibVersion();
	const char src[] = "zlib round trip";
	unsigned char compressed[128];
	unsigned char restored[128];
	uLongf compressed_len = sizeof(compressed);
	uLongf restored_len = sizeof(restored);
	bool zlib_ok = version && strcmp(version, "1.3.2") == 0 && compress(compressed, &compressed_len, (const Bytef*)src, (uLong)strlen(src)) == Z_OK;
	if(zlib_ok) {
		restored_len = sizeof(restored);
		zlib_ok = uncompress(restored, &restored_len, compressed, compressed_len) == Z_OK && restored_len == strlen(src) && memcmp(restored, src, strlen(src)) == 0;
	}
	if(zlib_ok) {
		printf("OCIO dependency zlib: OK\n");
		passed++;
	} else {
		printf("OCIO dependency zlib: FAIL\n");
		failed++;
	}

	printf("SUMMARY passed=%d failed=%d\n", passed, failed);
	return failed ? 1 : 0;
}
