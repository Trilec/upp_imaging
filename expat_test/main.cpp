#include <Core/Core.h>

#include <stdio.h>
#include <string.h>

#include <expat/expat.h>

using namespace Upp;

struct XmlTrace {
	Vector<String> events;
	Vector<String> ns_events;
};

static void XMLCALL StartElement(void* user_data, const XML_Char* name, const XML_Char** attrs)
{
	auto* trace = static_cast<XmlTrace*>(user_data);
	String s = Format("start:%s", name);
	for(int i = 0; attrs && attrs[i]; i += 2)
		s << Format(" %s=%s", attrs[i], attrs[i + 1]);
	trace->events.Add(s);
}

static void XMLCALL EndElement(void* user_data, const XML_Char* name)
{
	auto* trace = static_cast<XmlTrace*>(user_data);
	trace->events.Add(Format("end:%s", name));
}

static void XMLCALL CharacterData(void* user_data, const XML_Char* s, int len)
{
	auto* trace = static_cast<XmlTrace*>(user_data);
	String text(s, len);
	if(!text.IsEmpty())
		trace->events.Add(Format("text:%s", text));
}

static void XMLCALL StartNs(void* user_data, const XML_Char* prefix, const XML_Char* uri)
{
	auto* trace = static_cast<XmlTrace*>(user_data);
	trace->ns_events.Add(Format("ns-start:%s=%s", prefix ? prefix : "", uri ? uri : ""));
}

static void XMLCALL EndNs(void* user_data, const XML_Char* prefix)
{
	auto* trace = static_cast<XmlTrace*>(user_data);
	trace->ns_events.Add(Format("ns-end:%s", prefix ? prefix : ""));
}

static bool ParseDocument(const char* xml, XmlTrace& trace)
{
	XML_Parser parser = XML_ParserCreateNS(NULL, ':');
	if(!parser)
		return false;
	XML_SetUserData(parser, &trace);
	XML_SetElementHandler(parser, StartElement, EndElement);
	XML_SetCharacterDataHandler(parser, CharacterData);
	XML_SetNamespaceDeclHandler(parser, StartNs, EndNs);
	const int ok = XML_Parse(parser, xml, (int)strlen(xml), XML_TRUE);
	XML_ParserFree(parser);
	return ok == XML_STATUS_OK;
}

int main()
{
	int passed = 0;
	int failed = 0;

	if(XML_MAJOR_VERSION == 2 && XML_MINOR_VERSION == 7 && XML_MICRO_VERSION == 2) {
		printf("version macros: OK\n");
		passed++;
	} else {
		printf("version macros: FAIL\n");
		failed++;
	}

	XmlTrace trace;
	const char* xml =
		"<?xml version='1.0'?>"
		"<r xmlns:n='urn:test' a='1'>"
		"<n:c b='2'>text<inner/>more</n:c>"
		"</r>";
	if(ParseDocument(xml, trace)) {
		Vector<String> expected;
		expected.Add("ns-start:n=urn:test");
		expected.Add("start:r a=1");
		expected.Add("start:urn:test:c b=2");
		expected.Add("text:text");
		expected.Add("start:inner");
		expected.Add("end:inner");
		expected.Add("text:more");
		expected.Add("end:urn:test:c");
		expected.Add("end:r");
		if(trace.events == expected) {
			printf("XML parse: OK\n");
			passed++;
		} else {
			printf("XML parse: FAIL\n");
			failed++;
		}
	} else {
		printf("XML parse: FAIL\n");
		failed++;
	}

	const char* bad_xml = "<r><c></r>";
	XML_Parser bad = XML_ParserCreate(NULL);
	XML_SetUserData(bad, &trace);
	const int bad_rc = XML_Parse(bad, bad_xml, (int)strlen(bad_xml), XML_TRUE);
	const enum XML_Error bad_err = XML_GetErrorCode(bad);
	const char* bad_msg = XML_ErrorString(bad_err);
	if(bad_rc == XML_STATUS_ERROR && bad_err != XML_ERROR_NONE && bad_msg && *bad_msg) {
		printf("malformed XML: OK (%s)\n", bad_msg);
		passed++;
	} else {
		printf("malformed XML: FAIL\n");
		failed++;
	}
	XML_ParserFree(bad);

	printf("SUMMARY passed=%d failed=%d\n", passed, failed);
	return failed ? 1 : 0;
}
