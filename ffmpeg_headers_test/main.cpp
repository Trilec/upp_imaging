#include <Core/Core.h>

extern "C" {
#include <libavutil/avutil.h>
#include <libavutil/version.h>
#include <libavcodec/avcodec.h>
#include <libavcodec/version.h>
#include <libavformat/avformat.h>
#include <libavformat/version.h>
#include <libswscale/swscale.h>
#include <libswscale/version.h>
}

using namespace Upp;

struct State {
	int passed = 0;
	int failed = 0;
};

struct ConfigAudit {
	bool root_found = false;
	bool manifests_ok = true;
	int source_files = 0;
	int scanned_files = 0;
	Index<String> referenced;
	Index<String> generated;
	Index<String> local;
	Vector<String> missing;
};

static void Check(State& state, bool condition, const char* label)
{
	Cout() << (condition ? "PASS " : "FAIL ") << label << '\n';
	(condition ? state.passed : state.failed)++;
}

static void AddUnique(Index<String>& index, const String& value)
{
	if(index.Find(value) < 0)
		index.Add(value);
}

static bool IsIdentifierStart(int c)
{
	return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '_';
}

static bool IsIdentifierChar(int c)
{
	return IsIdentifierStart(c) || (c >= '0' && c <= '9');
}

static bool HasConfigPrefix(const String& id)
{
	return id.GetCount() >= 7 && id.Mid(0, 7) == "CONFIG_" ||
	       id.GetCount() >= 5 && id.Mid(0, 5) == "HAVE_" ||
	       id.GetCount() >= 5 && id.Mid(0, 5) == "ARCH_" ||
	       id.GetCount() >= 8 && id.Mid(0, 8) == "AV_HAVE_";
}

static String StripComments(const String& source)
{
	String out;
	out.Reserve(source.GetCount());
	bool block = false;
	bool quote = false;
	bool apostrophe = false;
	bool escape = false;

	for(int i = 0; i < source.GetCount(); i++) {
		int c = source[i];
		int n = i + 1 < source.GetCount() ? source[i + 1] : 0;

		if(block) {
			if(c == '*' && n == '/') {
				out.Cat(' ');
				out.Cat(' ');
				i++;
				block = false;
			}
			else
				out.Cat(c == '\n' ? '\n' : ' ');
			continue;
		}

		if(quote || apostrophe) {
			out.Cat(c);
			if(escape)
				escape = false;
			else if(c == '\\')
				escape = true;
			else if((quote && c == '"') || (apostrophe && c == '\'')) {
				quote = false;
				apostrophe = false;
			}
			continue;
		}

		if(c == '/' && n == '*') {
			out.Cat(' ');
			out.Cat(' ');
			i++;
			block = true;
			continue;
		}
		if(c == '/' && n == '/') {
			out.Cat(' ');
			out.Cat(' ');
			i += 2;
			while(i < source.GetCount() && source[i] != '\n') {
				out.Cat(' ');
				i++;
			}
			if(i < source.GetCount())
				out.Cat('\n');
			continue;
		}
		if(c == '"')
			quote = true;
		else if(c == '\'')
			apostrophe = true;
		out.Cat(c);
	}
	return out;
}

static void CollectMacroTokens(const String& code, Index<String>& out)
{
	for(int i = 0; i < code.GetCount();) {
		int c = code[i];
		if(c == '"' || c == '\'') {
			int quote = c;
			i++;
			bool escape = false;
			while(i < code.GetCount()) {
				c = code[i++];
				if(escape)
					escape = false;
				else if(c == '\\')
					escape = true;
				else if(c == quote)
					break;
			}
			continue;
		}
		if(!IsIdentifierStart(c)) {
			i++;
			continue;
		}
		int begin = i++;
		while(i < code.GetCount() && IsIdentifierChar(code[i]))
			i++;
		String id = code.Mid(begin, i - begin);
		if(HasConfigPrefix(id))
			AddUnique(out, id);
	}
}

static void CollectDefinitions(const String& code, Index<String>& out)
{
	StringStream stream(code);
	while(!stream.IsEof()) {
		String line = stream.GetLine();
		int p = 0;
		while(p < line.GetCount() && (line[p] == ' ' || line[p] == '\t'))
			p++;
		if(p >= line.GetCount() || line[p] != '#')
			continue;
		p++;
		while(p < line.GetCount() && (line[p] == ' ' || line[p] == '\t'))
			p++;
		if(p + 6 > line.GetCount() || line.Mid(p, 6) != "define")
			continue;
		p += 6;
		if(p < line.GetCount() && IsIdentifierChar(line[p]))
			continue;
		while(p < line.GetCount() && (line[p] == ' ' || line[p] == '\t'))
			p++;
		if(p >= line.GetCount() || !IsIdentifierStart(line[p]))
			continue;
		int begin = p++;
		while(p < line.GetCount() && IsIdentifierChar(line[p]))
			p++;
		String id = line.Mid(begin, p - begin);
		if(HasConfigPrefix(id))
			AddUnique(out, id);
	}
}

static String FindRepositoryRoot()
{
	String dir = GetCurrentDirectory();
	for(int i = 0; i < 8 && !dir.IsEmpty(); i++) {
		if(FileExists(AppendFileName(dir, "ffmpeg_headers/generated/config.h")) &&
		   FileExists(AppendFileName(dir, "ffmpeg_avutil_src/import.ext")))
			return dir;
		String parent = GetFileFolder(dir);
		if(parent.IsEmpty() || parent == dir)
			break;
		dir = parent;
	}
	return String();
}

static bool StartsWithLibraryPath(const String& include)
{
	static const char *prefix[] = {
		"libavutil/", "libavcodec/", "libavformat/", "libswscale/"
	};
	for(const char *p : prefix) {
		int n = (int)strlen(p);
		if(include.GetCount() >= n && include.Mid(0, n) == p)
			return true;
	}
	return false;
}

static String ResolveInclude(const String& current_file, const String& include,
                             const String& upstream_root, const String& generated_root)
{
	if(include == "config.h" || include == "config_components.h")
		return String();

	String path = NormalizePath(AppendFileName(GetFileFolder(current_file), include));
	if(FileExists(path))
		return path;

	if(StartsWithLibraryPath(include)) {
		path = NormalizePath(AppendFileName(upstream_root, include));
		if(FileExists(path))
			return path;
		path = NormalizePath(AppendFileName(generated_root, include));
		if(FileExists(path))
			return path;
	}
	return String();
}

static void ScanSourceFile(const String& file, const String& upstream_root,
                           const String& generated_root, ConfigAudit& audit,
                           Index<String>& visited)
{
	String path = NormalizePath(file);
	if(visited.Find(path) >= 0)
		return;
	visited.Add(path);
	if(!FileExists(path))
		return;

	String code = StripComments(LoadFile(path));
	audit.scanned_files++;
	CollectMacroTokens(code, audit.referenced);
	CollectDefinitions(code, audit.local);

	StringStream stream(code);
	while(!stream.IsEof()) {
		String line = stream.GetLine();
		int p = 0;
		while(p < line.GetCount() && (line[p] == ' ' || line[p] == '\t'))
			p++;
		if(p >= line.GetCount() || line[p] != '#')
			continue;
		p++;
		while(p < line.GetCount() && (line[p] == ' ' || line[p] == '\t'))
			p++;
		if(p + 7 > line.GetCount() || line.Mid(p, 7) != "include")
			continue;
		p += 7;
		while(p < line.GetCount() && (line[p] == ' ' || line[p] == '\t'))
			p++;
		if(p >= line.GetCount() || (line[p] != '"' && line[p] != '<'))
			continue;
		int close = line[p] == '"' ? '"' : '>';
		int begin = ++p;
		while(p < line.GetCount() && line[p] != close)
			p++;
		if(p >= line.GetCount())
			continue;
		String include = line.Mid(begin, p - begin);
		String child = ResolveInclude(path, include, upstream_root, generated_root);
		if(!child.IsEmpty())
			ScanSourceFile(child, upstream_root, generated_root, audit, visited);
	}
}

static bool LoadGeneratedDefinitions(const String& root, ConfigAudit& audit)
{
	static const char *files[] = {
		"ffmpeg_headers/generated/config.h",
		"ffmpeg_headers/generated/config_components.h",
		"ffmpeg_headers/generated/libavutil/avconfig.h",
	};
	for(const char *rel : files) {
		String path = AppendFileName(root, rel);
		if(!FileExists(path))
			return false;
		CollectDefinitions(StripComments(LoadFile(path)), audit.generated);
	}
	// Supplied by every FFmpeg implementation package through BUILDER_OPTION.
	AddUnique(audit.generated, "HAVE_AV_CONFIG_H");
	return true;
}

static void LoadManifestSources(const String& root, const char *package,
                                Vector<String>& sources, ConfigAudit& audit)
{
	String manifest = AppendFileName(AppendFileName(root, package), "import.ext");
	if(!FileExists(manifest)) {
		Cout() << "CONFIG AUDIT missing manifest " << manifest << '\n';
		audit.manifests_ok = false;
		return;
	}

	StringStream stream(LoadFile(manifest));
	const String marker = "../ffmpeg_headers/upstream/";
	while(!stream.IsEof()) {
		String line = stream.GetLine();
		int p = line.Find(marker);
		if(p < 0)
			continue;
		String rel = line.Mid(p + 3);
		while(!rel.IsEmpty()) {
			int c = rel[rel.GetCount() - 1];
			if(c != ';' && c != ' ' && c != '\t' && c != '\r')
				break;
			rel = rel.Left(rel.GetCount() - 1);
		}
		String path = NormalizePath(AppendFileName(root, rel));
		if(!FileExists(path)) {
			Cout() << "CONFIG AUDIT missing source " << path << '\n';
			audit.manifests_ok = false;
			continue;
		}
		sources.Add(path);
		audit.source_files++;
	}
}

static bool AuditGeneratedConfiguration(ConfigAudit& audit)
{
	String root = FindRepositoryRoot();
	audit.root_found = !root.IsEmpty();
	if(!audit.root_found) {
		Cout() << "CONFIG AUDIT repository root not found from "
		       << GetCurrentDirectory() << '\n';
		return false;
	}

	if(!LoadGeneratedDefinitions(root, audit)) {
		Cout() << "CONFIG AUDIT generated configuration headers are incomplete\n";
		return false;
	}

	Vector<String> sources;
	static const char *packages[] = {
		"ffmpeg_avutil_src", "ffmpeg_avcodec_src",
		"ffmpeg_avformat_src", "ffmpeg_swscale_src"
	};
	for(const char *package : packages)
		LoadManifestSources(root, package, sources, audit);

	String upstream_root = AppendFileName(root, "ffmpeg_headers/upstream");
	String generated_root = AppendFileName(root, "ffmpeg_headers/generated");
	Index<String> visited;
	for(const String& source : sources)
		ScanSourceFile(source, upstream_root, generated_root, audit, visited);

	for(int i = 0; i < audit.referenced.GetCount(); i++) {
		const String& id = audit.referenced[i];
		if(audit.generated.Find(id) < 0 && audit.local.Find(id) < 0)
			audit.missing.Add(id);
	}
	Sort(audit.missing);

	Cout() << Format("CONFIG AUDIT sources=%d scanned=%d referenced=%d generated=%d missing=%d\n",
	                 audit.source_files, audit.scanned_files,
	                 audit.referenced.GetCount(), audit.generated.GetCount(),
	                 audit.missing.GetCount());
	for(const String& id : audit.missing)
		Cout() << "MISSING GENERATED MACRO " << id << '\n';

	return audit.manifests_ok && audit.source_files > 0 &&
	       audit.scanned_files >= audit.source_files && audit.missing.IsEmpty();
}

CONSOLE_APP_MAIN
{
	State state;

	Check(state, AV_HAVE_BIGENDIAN == 0,
	      "FFmpeg public headers use little-endian configuration");
	Check(state, AV_HAVE_FAST_UNALIGNED == 1,
	      "FFmpeg public headers use fast unaligned access");
	Check(state, LIBAVUTIL_VERSION_MAJOR == 61,
	      "libavutil major matches FFmpeg 9.0.1");
	Check(state, LIBAVCODEC_VERSION_MAJOR == 63,
	      "libavcodec major matches FFmpeg 9.0.1");
	Check(state, LIBAVFORMAT_VERSION_MAJOR == 63,
	      "libavformat major matches FFmpeg 9.0.1");
	Check(state, LIBSWSCALE_VERSION_MAJOR == 10,
	      "libswscale major matches FFmpeg 9.0.1");

	AVFrame* frame = nullptr;
	AVCodecContext* codec = nullptr;
	AVFormatContext* format = nullptr;
	SwsContext* scale = nullptr;
	Check(state, frame == nullptr && codec == nullptr &&
	      format == nullptr && scale == nullptr,
	      "core FFmpeg public types are visible together");

	ConfigAudit audit;
	Check(state, AuditGeneratedConfiguration(audit),
	      "generated FFmpeg config covers the imported production source slice");

	Cout() << Format("SUMMARY passed=%d failed=%d\n", state.passed, state.failed);
	SetExitCode(state.failed ? 1 : 0);
}
