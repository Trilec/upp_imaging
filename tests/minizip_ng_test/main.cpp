#include <Core/Core.h>

#include <stdio.h>
#include <string.h>
#include <time.h>

#include <minizip_ng/mz.h>
#include <minizip_ng/mz_strm.h>
#include <minizip_ng/mz_os.h>
#include <minizip_ng/mz_zip.h>
#include <minizip_ng/mz_zip_rw.h>

#ifndef UPP_IMAGING_LOCAL_MINIZIP_NG_INCLUDE
#error minizip-ng did not resolve through the local upp_imaging include tree
#endif

using namespace Upp;

struct EntrySpec {
	const char* name;
	const void* data;
	int size;
	bool symlink = false;
};

static bool FileIsNonEmpty(const String& path)
{
	FileIn in(path);
	return in.IsOpen() && in.GetSize() > 0;
}

static bool WriteArchive(const String& path, const EntrySpec* entries, int count, bool deflated = false)
{
	void* writer = mz_zip_writer_create();
	if(!writer) {
		FileDelete(path);
		return false;
	}

	int rc = mz_zip_writer_open_file(writer, ~path, 0, 0);
	if(rc != MZ_OK) {
		mz_zip_writer_delete(&writer);
		FileDelete(path);
		return false;
	}

	mz_zip_writer_set_compress_method(writer, deflated ? MZ_COMPRESS_METHOD_DEFLATE : MZ_COMPRESS_METHOD_STORE);
	for(int i = 0; i < count; ++i) {
		mz_zip_file info;
		memset(&info, 0, sizeof(info));
		info.filename = entries[i].name;
		info.compression_method = deflated ? MZ_COMPRESS_METHOD_DEFLATE : MZ_COMPRESS_METHOD_STORE;
		info.uncompressed_size = entries[i].size;
		info.modified_date = time(NULL);
		if(entries[i].symlink) {
			info.version_madeby = (MZ_HOST_SYSTEM_UNIX << 8) | 20;
			info.external_fa = 0120777u << 16;
		}
		rc = mz_zip_writer_add_buffer(writer, (void*)entries[i].data, entries[i].size, &info);
		if(rc != MZ_OK)
			break;
	}

	const int close_rc = mz_zip_writer_close(writer);
	mz_zip_writer_delete(&writer);
	if(rc != MZ_OK || close_rc != MZ_OK) {
		FileDelete(path);
		return false;
	}
	return true;
}

static bool CheckEntryMetadata(void* reader, const EntrySpec& spec)
{
	if(mz_zip_reader_locate_entry(reader, spec.name, 0) != MZ_OK)
		return false;
	if(mz_zip_reader_entry_open(reader) != MZ_OK)
		return false;
	mz_zip_file* info = NULL;
	const int info_rc = mz_zip_reader_entry_get_info(reader, &info);
	const bool ok = info_rc == MZ_OK && info && info->filename && strcmp(info->filename, spec.name) == 0
		&& info->uncompressed_size == spec.size && info->compression_method == MZ_COMPRESS_METHOD_STORE;
	mz_zip_reader_entry_close(reader);
	return ok;
}

static bool ReadExactEntry(void* reader, const EntrySpec& spec)
{
	if(mz_zip_reader_locate_entry(reader, spec.name, 0) != MZ_OK)
		return false;
	if(mz_zip_reader_entry_open(reader) != MZ_OK)
		return false;
	mz_zip_file* info = NULL;
	if(mz_zip_reader_entry_get_info(reader, &info) != MZ_OK || !info || !info->filename
	   || strcmp(info->filename, spec.name) != 0 || info->uncompressed_size != spec.size
	   || info->compression_method != MZ_COMPRESS_METHOD_STORE) {
		mz_zip_reader_entry_close(reader);
		return false;
	}

	Vector<byte> got;
	got.SetCount(spec.size);
	int offset = 0;
	while(offset < spec.size) {
		const int rc = mz_zip_reader_entry_read(reader, got.Begin() + offset, spec.size - offset);
		if(rc < 0) {
			mz_zip_reader_entry_close(reader);
			return false;
		}
		if(rc == 0)
			break;
		offset += rc;
	}
	const int close_rc = mz_zip_reader_entry_close(reader);
	return close_rc == MZ_OK && offset == spec.size && memcmp(got.Begin(), spec.data, spec.size) == 0;
}

int main()
{
	int passed = 0;
	int failed = 0;

	if(strcmp(MZ_VERSION, "4.2.2") == 0) {
		printf("version macro: OK\n");
		passed++;
	} else {
		printf("version macro: FAIL\n");
		failed++;
	}

	const String zip_path = GetExeDirFile("ocio_dependencies_roundtrip.zip");
	const char alpha[] = "alpha from memory";
	const byte beta[] = {0x00, 0x01, 0x02, 0xFE, 0xFF, 0x10};
	const EntrySpec entries[] = {
		{"alpha.txt", alpha, (int)strlen(alpha)},
		{"beta.bin", beta, (int)sizeof(beta)},
	};

	FileDelete(zip_path);
	if(WriteArchive(zip_path, entries, 2)) {
		printf("zip write: OK\n");
		passed++;
		printf("zip writer close: OK\n");
		passed++;
	} else {
		printf("zip write: FAIL\n");
		failed++;
	}

	if(FileIsNonEmpty(zip_path)) {
		printf("zip output non-empty: OK\n");
		passed++;
	} else {
		printf("zip output non-empty: FAIL\n");
		failed++;
	}

	void* reader = mz_zip_reader_create();
	if(!reader) {
		FileDelete(zip_path);
		printf("zip reader create: FAIL\n");
		failed++;
	} else {
		const int open_rc = mz_zip_reader_open_file(reader, ~zip_path);
		if(open_rc == MZ_OK) {
			Vector<String> names;
			if(mz_zip_reader_goto_first_entry(reader) == MZ_OK) {
				do {
					mz_zip_file* info = NULL;
					if(mz_zip_reader_entry_get_info(reader, &info) == MZ_OK && info && info->filename)
						names.Add(info->filename);
				} while(mz_zip_reader_goto_next_entry(reader) == MZ_OK);
			}
			if(names.GetCount() == 2 && names[0] == "alpha.txt" && names[1] == "beta.bin") {
				printf("zip enumerate: OK\n");
				passed++;
			} else {
				printf("zip enumerate: FAIL\n");
				failed++;
			}

			if(CheckEntryMetadata(reader, entries[0]) && CheckEntryMetadata(reader, entries[1])) {
				printf("zip metadata: OK\n");
				passed++;
			} else {
				printf("zip metadata: FAIL\n");
				failed++;
			}

			if(ReadExactEntry(reader, entries[0]) && ReadExactEntry(reader, entries[1])) {
				printf("zip exact readback: OK\n");
				passed++;
			} else {
				printf("zip exact readback: FAIL\n");
				failed++;
			}

			if(mz_zip_reader_close(reader) != MZ_OK) {
				printf("zip reader close: FAIL\n");
				failed++;
			} else {
				printf("zip reader close: OK\n");
				passed++;
			}
		} else {
			printf("zip open: FAIL (%d)\n", open_rc);
			failed++;
		}
		mz_zip_reader_delete(&reader);
	}

	const String bad_path = GetExeDirFile("ocio_dependencies_bad.zip");
	{
		FileOut out(bad_path);
		const char bad[] = "not a zip";
		out.Put(bad, (int)strlen(bad));
	}
	void* bad_reader = mz_zip_reader_create();
	if(bad_reader) {
		const int bad_rc = mz_zip_reader_open_file(bad_reader, ~bad_path);
		if(bad_rc != MZ_OK) {
			printf("malformed zip: OK (%d)\n", bad_rc);
			passed++;
		} else {
			printf("malformed zip: FAIL\n");
			failed++;
			if(mz_zip_reader_close(bad_reader) != MZ_OK)
				failed++;
		}
		mz_zip_reader_delete(&bad_reader);
	} else {
		printf("malformed zip: FAIL\n");
		failed++;
	}


	// Pure policy checks do not require Windows symlink creation privileges.
	String extract = GetExeDirFile("minizip_security_extract");
	RealizeDirectory(extract);
	String link = AppendFileName(extract, "link");
	String long_link = extract + "/" + String('a', 1100) + "/link";
	bool paths_ok = mz_path_is_symlink_target_safe(~link, "inside.txt", ~extract) == MZ_OK &&
		mz_path_is_symlink_target_safe(~link, "../escape.txt", ~extract) == MZ_EXIST_ERROR &&
		mz_path_is_symlink_target_safe(~link, "C:\\escape.txt", ~extract) == MZ_EXIST_ERROR &&
		mz_path_is_symlink_target_safe(~link, "inside:stream", ~extract) == MZ_EXIST_ERROR &&
		mz_path_is_symlink_target_safe(~long_link, "inside.txt", ~extract) == MZ_BUF_ERROR;
	printf("symlink target policy: %s\n", paths_ok ? "OK" : "FAIL");
	(paths_ok ? passed : failed)++;

	void* extraction_reader = mz_zip_reader_create();
	bool extraction_ok = extraction_reader &&
		mz_zip_reader_open_file(extraction_reader, ~zip_path) == MZ_OK &&
		mz_zip_reader_save_all(extraction_reader, ~extract) == MZ_OK &&
		LoadFile(AppendFileName(extract, "alpha.txt")) == alpha &&
		LoadFile(AppendFileName(extract, "beta.bin")) == String((const char*)beta, sizeof(beta));
	mz_zip_reader_delete(&extraction_reader);
	printf("normal ZIP extraction: %s\n", extraction_ok ? "OK" : "FAIL");
	(extraction_ok ? passed : failed)++;

	const String link_zip = GetExeDirFile("minizip_escape_test.zip");
	const char target[] = "../minizip_escape_target.txt";
	const EntrySpec attack[] = {{"escape-link", target, (int)strlen(target), true}};
	void* escape_reader = mz_zip_reader_create();
	bool escape_ok = WriteArchive(link_zip, attack, 1) && escape_reader &&
		mz_zip_reader_open_file(escape_reader, ~link_zip) == MZ_OK &&
		mz_zip_reader_save_all(escape_reader, ~extract) == MZ_EXIST_ERROR &&
		!FileExists(AppendFileName(extract, "escape-link")) &&
		!FileExists(GetExeDirFile("minizip_escape_target.txt"));
	mz_zip_reader_delete(&escape_reader);
	printf("escaping archive symlink rejected: %s\n", escape_ok ? "OK" : "FAIL");
	(escape_ok ? passed : failed)++;
	const String compressed_zip = GetExeDirFile("minizip_deflate_test.zip");
	void* compressed_reader = mz_zip_reader_create();
	bool compressed_ok = WriteArchive(compressed_zip, entries, 2, true) && compressed_reader &&
		mz_zip_reader_open_file(compressed_reader, ~compressed_zip) == MZ_OK &&
		mz_zip_reader_save_all(compressed_reader, ~extract) == MZ_OK &&
		LoadFile(AppendFileName(extract, "alpha.txt")) == alpha &&
		LoadFile(AppendFileName(extract, "beta.bin")) == String((const char*)beta, sizeof(beta));
	mz_zip_reader_delete(&compressed_reader);
	printf("deflated ZIP extraction: %s\n", compressed_ok ? "OK" : "FAIL");
	(compressed_ok ? passed : failed)++;
	FileDelete(compressed_zip);
	FileDelete(AppendFileName(extract, "alpha.txt"));
	FileDelete(AppendFileName(extract, "beta.bin"));
	FileDelete(link_zip);
	FileDelete(zip_path);
	FileDelete(bad_path);

	printf("SUMMARY passed=%d failed=%d\n", passed, failed);
	return failed ? 1 : 0;
}
