#include <ImagingDiagnostics/ImagingDiagnostics.h>

#include <cmath>
#include <limits>

using namespace Upp;
using namespace Upp::Imaging;

struct State {
	int passed = 0;
	int failed = 0;
};

static void Check(State& state, bool condition, const char* name)
{
	Cout() << (condition ? "PASS " : "FAIL ") << name << '\n';
	(condition ? state.passed : state.failed)++;
}

static bool HasCode(const Diagnostics& diagnostics, const char* code)
{
	for(const DiagnosticEntry& entry : diagnostics.Entries())
		if(entry.code == code)
			return true;
	return false;
}

static bool HasItem(const DiagnosticReport& report, const char* section,
                    const char* key, const char* value)
{
	for(const ReportItem& item : report.items)
		if(item.section == section && item.key == key && item.value == value)
			return true;
	return false;
}

CONSOLE_APP_MAIN
{
	State state;
	Diagnostics diagnostics;

	Check(state, SampleTypeName(SampleType::Float16) == "Float16" &&
	      SampleTypeName(SampleType::Invalid) == "Invalid",
	      "sample type names");
	Check(state, ChannelLayoutName(ChannelLayout::RGBA) == "RGBA" &&
	      ChannelLayoutName(ChannelLayout::MultiChannel) == "MultiChannel",
	      "channel layout names");
	Check(state, ResultCodeName(ResultCode::IOError) == "IOError" &&
	      DiagnosticSeverityName(DiagnosticSeverity::Warning) == "Warning",
	      "result and severity names");

	NumericComparison comparison;
	Check(state, CompareNumeric(1.0000005, 1.0, comparison, NumericTolerance(), &diagnostics).IsOk() &&
	      comparison.match && diagnostics.IsEmpty(), "absolute tolerance comparison");
	NumericTolerance relative;
	relative.absolute = 0.0;
	relative.relative = 0.01;
	Check(state, CompareNumeric(101.0, 100.0, comparison, relative, &diagnostics).IsOk() &&
	      comparison.match, "relative tolerance comparison");
	Check(state, CompareNumeric(102.0, 100.0, comparison, relative, &diagnostics).IsOk() &&
	      !comparison.match && comparison.absolute_error == 2.0,
	      "numeric mismatch evidence");
	Check(state, CompareNumeric(std::numeric_limits<double>::quiet_NaN(),
	                            std::numeric_limits<double>::quiet_NaN(), comparison,
	                            NumericTolerance(), &diagnostics).IsOk() && comparison.match,
	      "matching NaN classification");
	Check(state, CompareNumeric(std::numeric_limits<double>::infinity(),
	                            std::numeric_limits<double>::infinity(), comparison,
	                            NumericTolerance(), &diagnostics).IsOk() && comparison.match,
	      "matching infinity classification");
	Check(state, CompareNumeric(std::numeric_limits<double>::infinity(),
	                            -std::numeric_limits<double>::infinity(), comparison,
	                            NumericTolerance(), &diagnostics).IsOk() && !comparison.match,
	      "opposite infinity mismatch");

	NumericComparison sentinel;
	sentinel.actual = 77.0;
	NumericTolerance invalid_tolerance;
	invalid_tolerance.absolute = -1.0;
	Result bad_tolerance = CompareNumeric(1.0, 1.0, sentinel, invalid_tolerance, &diagnostics);
	Check(state, bad_tolerance.code == ResultCode::InvalidArgument &&
	      HasCode(diagnostics, "IMGDIAG_TOLERANCE"), "invalid tolerance stable code");
	Check(state, sentinel.actual == 77.0, "invalid tolerance preserves comparison output");

	const double actual[] = {1.0, 2.0, 4.0, 8.0};
	const double expected[] = {1.0, 2.1, 4.0, 7.0};
	NumericComparisonSummary summary;
	NumericTolerance exact;
	exact.absolute = 0.0;
	exact.relative = 0.0;
	Check(state, CompareNumericArrays(actual, expected, 4, summary, exact, &diagnostics).IsOk(),
	      "array comparison succeeds");
	Check(state, summary.compared == 4 && summary.mismatched == 2 &&
	      summary.first_mismatch == 1 && !summary.AllMatch(),
	      "array mismatch summary");
	Check(state, summary.max_absolute_error == 1.0 && summary.max_relative_error > 0.12,
	      "array maximum error evidence");
	NumericComparisonSummary preserved_summary;
	preserved_summary.compared = 91;
	Result bad_array = CompareNumericArrays(nullptr, expected, 4, preserved_summary,
	                                       NumericTolerance(), &diagnostics);
	Check(state, bad_array.code == ResultCode::InvalidArgument &&
	      HasCode(diagnostics, "IMGDIAG_COMPARE"), "invalid array stable code");
	Check(state, preserved_summary.compared == 91, "invalid array preserves summary output");

	ImageSpec spec;
	spec.data_window = {-2, 3, 1, 4};
	spec.depth = 2;
	spec.channels = 4;
	spec.channel_layout = ChannelLayout::RGBA;
	spec.sample_type = SampleType::Float32;
	spec.alpha_channel = 3;
	spec.channel_names.Add("R");
	spec.channel_names.Add("G");
	spec.channel_names.Add("B");
	spec.channel_names.Add("A");
	DiagnosticReport report;
	BuildImageSpecReport(spec, report);
	Check(state, report.title == "Image specification" && HasItem(report, "ImageSpec", "valid", "true"),
	      "image spec report validity");
	Check(state, HasItem(report, "ImageSpec", "data_window", "-2,3 - 1,4") &&
	      HasItem(report, "ImageSpec", "depth", "2"), "image spec geometry report");
	Check(state, HasItem(report, "ImageSpec", "layout", "RGBA") &&
	      HasItem(report, "ImageSpec", "sample_type", "Float32"), "image spec type report");
	Check(state, HasItem(report, "ImageSpec", "pixel_count", "16") &&
	      HasItem(report, "ImageSpec", "sample_count", "64") &&
	      HasItem(report, "ImageSpec", "byte_count", "256"), "image spec count report");
	String spec_text = report.ToText();
	Check(state, spec_text.Find("[ImageSpec]") >= 0 && spec_text.Find("sample_type: Float32") >= 0,
	      "image spec deterministic text");

	Metadata metadata;
	metadata.Set("zeta", Value(7));
	ValueArray values;
	values.Add(1);
	values.Add(2.5);
	metadata.Set("array", Value(values));
	metadata.Set("alpha", Value("first"));
	BuildMetadataReport(metadata, report);
	Check(state, HasItem(report, "Metadata", "count", "3") &&
	      HasItem(report, "Metadata", "array", "[1, 2.5]"), "metadata scalar and array report");
	String metadata_text = report.ToText();
	int alpha_pos = metadata_text.Find("alpha: first");
	int array_pos = metadata_text.Find("array: [1, 2.5]");
	int zeta_pos = metadata_text.Find("zeta: 7");
	Check(state, alpha_pos >= 0 && array_pos > alpha_pos && zeta_pos > array_pos,
	      "metadata report key ordering");

	Diagnostics operation_diagnostics;
	operation_diagnostics.Info("started", "fixture");
	operation_diagnostics.Add(DiagnosticSeverity::Warning, "careful", "TEST_WARN", "fixture");
	operation_diagnostics.Error("failed", "TEST_ERR", "fixture");
	BuildDiagnosticsReport(operation_diagnostics, report);
	Check(state, HasItem(report, "Diagnostics", "count", "3"), "diagnostics report count");
	Check(state, HasItem(report, "Diagnostic 0", "severity", "Information") &&
	      HasItem(report, "Diagnostic 1", "code", "TEST_WARN") &&
	      HasItem(report, "Diagnostic 2", "severity", "Error"),
	      "diagnostics report preserves order");

	Result failure = Result::Failure(ResultCode::IOError, "could not read", "fixture.exr");
	BuildResultReport(failure, report);
	Check(state, HasItem(report, "Result", "code", "IOError") &&
	      HasItem(report, "Result", "message", "could not read") &&
	      HasItem(report, "Result", "context", "fixture.exr"), "result report");

	Diagnostics report_diagnostics;
	Result operation_result = BuildOperationReport("load", failure, operation_diagnostics,
	                                              12.5, report, &report_diagnostics);
	Check(state, operation_result.IsOk() && report_diagnostics.IsEmpty(), "operation report succeeds");
	Check(state, report.title == "load" && HasItem(report, "Operation", "elapsed_ms", "12.500000"),
	      "operation timing report");
	Check(state, HasItem(report, "Result", "code", "IOError") &&
	      HasItem(report, "Diagnostics", "count", "3"), "operation combines result and diagnostics");

	DiagnosticReport preserved_report;
	preserved_report.title = "sentinel";
	Result bad_timing = BuildOperationReport("load", failure, operation_diagnostics,
	                                        -1.0, preserved_report, &report_diagnostics);
	Check(state, bad_timing.code == ResultCode::InvalidArgument &&
	      HasCode(report_diagnostics, "IMGDIAG_TIMING"), "invalid timing stable code");
	Check(state, preserved_report.title == "sentinel", "invalid timing preserves report output");
	Result bad_operation = BuildOperationReport(String(), failure, operation_diagnostics,
	                                           0.0, preserved_report, &report_diagnostics);
	Check(state, bad_operation.code == ResultCode::InvalidArgument &&
	      HasCode(report_diagnostics, "IMGDIAG_OPERATION"), "empty operation stable code");
	Check(state, BuildOperationReport("load", Result::Success(), report_diagnostics,
	                                 0.0, report, &report_diagnostics).IsOk() &&
	      report_diagnostics.IsEmpty(), "operation report tolerates diagnostics aliasing");

	Cout() << "SUMMARY passed=" << state.passed
	       << " failed=" << state.failed << '\n';
	SetExitCode(state.failed ? 1 : 0);
}
