#include <Core/Core.h>
#include <zlib/zlib.h>

using namespace Upp;

CONSOLE_APP_MAIN
{
	int passed = 0;
	int failed = 0;

	const char *version = zlibVersion();
	Cout() << "zlib version: " << (version ? version : "(null)") << '\n';
	if(version && *version)
		passed++;
	else
		failed++;

	const char source[] = "upp_imaging zlib smoke test";
	uLong source_len = (uLong)strlen(source);
	uLongf compressed_len = compressBound(source_len);
	Buffer<Byte> compressed((int)compressed_len);

	int rc = compress(compressed, &compressed_len, reinterpret_cast<const Bytef *>(source), source_len);
	if(rc == Z_OK) {
		uLongf restored_len = source_len;
		Buffer<Byte> restored((int)restored_len + 1);
		rc = uncompress(restored, &restored_len, compressed, compressed_len);
		if(rc == Z_OK && restored_len == source_len && memcmp(restored, source, (size_t)source_len) == 0) {
			restored[(int)restored_len] = 0;
			Cout() << "compress/uncompress round trip: OK\n";
			passed++;
		}
		else {
			Cout() << "compress/uncompress round trip: FAIL\n";
			failed++;
		}
	}
	else {
		Cout() << "compress/uncompress round trip: FAIL\n";
		failed++;
	}

	Cout() << "SUMMARY passed=" << passed << " failed=" << failed << '\n';
	SetExitCode(failed ? 1 : 0);
}
