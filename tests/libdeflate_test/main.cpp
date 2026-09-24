#include <Core/Core.h>
#include <libdeflate/libdeflate.h>

using namespace Upp;

CONSOLE_APP_MAIN
{
	int passed = 0;
	int failed = 0;

	String source = "upp_imaging libdeflate user package test";
	Buffer<byte> compressed(256);
	Buffer<byte> restored(source.GetCount());

	struct libdeflate_compressor *comp = libdeflate_alloc_compressor(6);
	struct libdeflate_decompressor *decomp = libdeflate_alloc_decompressor();
	if(comp && decomp) {
		Cout() << "libdeflate user package: OK\n";
		passed++;
	}
	else {
		Cout() << "libdeflate user package: FAIL\n";
		failed++;
	}

	if(comp && decomp) {
		size_t out_len = libdeflate_zlib_compress(comp, source.Begin(), source.GetCount(), compressed, 256);
		size_t actual = 0;
		enum libdeflate_result r = out_len ? libdeflate_zlib_decompress(decomp, compressed, out_len, restored, source.GetCount(), &actual) : LIBDEFLATE_BAD_DATA;
		if(out_len && r == LIBDEFLATE_SUCCESS && (int)actual == source.GetCount() && memcmp(restored, source.Begin(), source.GetCount()) == 0) {
			Cout() << "compress/decompress round trip: OK\n";
			passed++;
		}
		else {
			Cout() << "compress/decompress round trip: FAIL\n";
			failed++;
		}
	}
	else {
		Cout() << "compress/decompress round trip: FAIL\n";
		failed++;
	}

	libdeflate_free_compressor(comp);
	libdeflate_free_decompressor(decomp);

	Cout() << "SUMMARY passed=" << passed << " failed=" << failed << '\n';
	SetExitCode(failed ? 1 : 0);
}
