#include "ImagingDiagnostics.h"

#include <algorithm>
#include <cmath>
#include <limits>

namespace Upp {
namespace Imaging {

namespace {

static void Reset(Diagnostics* diagnostics)
{
	if(diagnostics)
		diagnostics->Clear();
}

static Result Fail(ResultCode code, Diagnostics* diagnostics,
                   const String& message, const char* diagnostic,
                   const String& context = Null)
{
	if(diagnostics)
		diagnostics->Error(message, diagnostic, context);
	return Result::Failure(code, message, context);
}

static String BoolText(bool value)
{
	return value ? "true" : "false";
}

static void AddItem(DiagnosticReport& report, const String& section,
                    const String& key, const String& value)
{
	ReportItem item;
	item.section = section;
	item.key = key;
	item.value = value;
	report.items.Add(pick(item));
}

static String JoinNames(const Vector<String>& names)
{
	String out;
	for(int i = 0; i < names.GetCount(); ++i) {
		if(i)
			out.Cat(", ");
		out.Cat(names[i]);
	}
	return out;
}

static String ValueText(const Value& value)
{
	if(value.Is<String>())
		return value.To<String>();
	if(value.Is<int>())
		return AsString(value.To<int>());
	if(value.Is<int64>())
		return AsString(value.To<int64>());
	if(value.Is<double>())
		return Format("%.17g", value.To<double>());
	if(value.Is<ValueArray>()) {
		ValueArray array = value;
		String out = "[";
		for(int i = 0; i < array.GetCount(); ++i) {
			if(i)
				out.Cat(", ");
			out.Cat(ValueText(array[i]));
		}
		out.Cat("]");
		return out;
	}
	return AsString(value);
}

static void AppendResult(const Result& result, DiagnosticReport& report)
{
	AddItem(report, "Result", "code", ResultCodeName(result.code));
	AddItem(report, "Result", "message", result.message);
	AddItem(report, "Result", "context", result.context);
}

static void AppendDiagnostics(const Diagnostics& diagnostics, DiagnosticReport& report)
{
	AddItem(report, "Diagnostics", "count", AsString(diagnostics.GetCount()));
	for(int i = 0; i < diagnostics.GetCount(); ++i) {
		const DiagnosticEntry& entry = diagnostics.Entries()[i];
		String section = Format("Diagnostic %d", i);
		AddItem(report, section, "severity", DiagnosticSeverityName(entry.severity));
		AddItem(report, section, "code", entry.code);
		AddItem(report, section, "message", entry.message);
		AddItem(report, section, "context", entry.context);
	}
}

static NumericComparison CompareUnchecked(double actual, double expected,
                                          const NumericTolerance& tolerance)
{
	NumericComparison comparison;
	comparison.actual = actual;
	comparison.expected = expected;
	comparison.actual_finite = std::isfinite(actual);
	comparison.expected_finite = std::isfinite(expected);

	if(!comparison.actual_finite || !comparison.expected_finite) {
		comparison.match = (std::isnan(actual) && std::isnan(expected)) || actual == expected;
		comparison.absolute_error = comparison.match ? 0.0 : std::numeric_limits<double>::infinity();
		comparison.relative_error = comparison.absolute_error;
		return comparison;
	}

	comparison.absolute_error = std::fabs(actual - expected);
	double scale = std::max(std::fabs(actual), std::fabs(expected));
	comparison.relative_error = scale > 0.0 ? comparison.absolute_error / scale
	                                      : comparison.absolute_error;
	comparison.match = comparison.absolute_error <= tolerance.absolute ||
	                   comparison.relative_error <= tolerance.relative;
	return comparison;
}

} // namespace

bool NumericTolerance::IsValid() const
{
	return std::isfinite(absolute) && std::isfinite(relative) &&
	       absolute >= 0.0 && relative >= 0.0;
}

Result CompareNumeric(double actual, double expected,
                      NumericComparison& output,
                      NumericTolerance tolerance, Diagnostics* diagnostics)
{
	Reset(diagnostics);
	if(!tolerance.IsValid())
		return Fail(ResultCode::InvalidArgument, diagnostics,
		            "numeric tolerance must be finite and non-negative",
		            "IMGDIAG_TOLERANCE");

	NumericComparison comparison = CompareUnchecked(actual, expected, tolerance);
	output = comparison;
	return Result::Success();
}

Result CompareNumericArrays(const double* actual, const double* expected,
                            int64 count, NumericComparisonSummary& output,
                            NumericTolerance tolerance, Diagnostics* diagnostics)
{
	Reset(diagnostics);
	if(!tolerance.IsValid())
		return Fail(ResultCode::InvalidArgument, diagnostics,
		            "numeric tolerance must be finite and non-negative",
		            "IMGDIAG_TOLERANCE");
	if(count < 0 || (count > 0 && (!actual || !expected)))
		return Fail(ResultCode::InvalidArgument, diagnostics,
		            "numeric comparison buffer is invalid",
		            "IMGDIAG_COMPARE");

	NumericComparisonSummary summary;
	summary.compared = count;
	for(int64 i = 0; i < count; ++i) {
		NumericComparison comparison = CompareUnchecked(actual[i], expected[i], tolerance);
		if(!comparison.match) {
			if(summary.first_mismatch < 0)
				summary.first_mismatch = i;
			++summary.mismatched;
		}
		summary.max_absolute_error = std::max(summary.max_absolute_error,
		                                      comparison.absolute_error);
		summary.max_relative_error = std::max(summary.max_relative_error,
		                                      comparison.relative_error);
	}
	output = summary;
	return Result::Success();
}

String DiagnosticReport::ToText() const
{
	String out;
	if(!title.IsEmpty()) {
		out.Cat(title);
		out.Cat('\n');
	}
	String section;
	for(const ReportItem& item : items) {
		if(item.section != section) {
			section = item.section;
			if(!section.IsEmpty()) {
				out.Cat('[');
				out.Cat(section);
				out.Cat("]\n");
			}
		}
		out.Cat(item.key);
		out.Cat(": ");
		out.Cat(item.value);
		out.Cat('\n');
	}
	return out;
}

String SampleTypeName(SampleType type)
{
	switch(type) {
	case SampleType::UInt8: return "UInt8";
	case SampleType::UInt16: return "UInt16";
	case SampleType::Float16: return "Float16";
	case SampleType::Float32: return "Float32";
	default: return "Invalid";
	}
}

String ChannelLayoutName(ChannelLayout layout)
{
	switch(layout) {
	case ChannelLayout::Gray: return "Gray";
	case ChannelLayout::GrayAlpha: return "GrayAlpha";
	case ChannelLayout::RGB: return "RGB";
	case ChannelLayout::RGBA: return "RGBA";
	case ChannelLayout::MultiChannel: return "MultiChannel";
	default: return "Invalid";
	}
}

String ResultCodeName(ResultCode code)
{
	switch(code) {
	case ResultCode::Ok: return "Ok";
	case ResultCode::InvalidArgument: return "InvalidArgument";
	case ResultCode::InvalidSpecification: return "InvalidSpecification";
	case ResultCode::Unsupported: return "Unsupported";
	case ResultCode::Overflow: return "Overflow";
	case ResultCode::AllocationFailure: return "AllocationFailure";
	case ResultCode::IOError: return "IOError";
	case ResultCode::InternalFailure: return "InternalFailure";
	default: return "Unknown";
	}
}

String DiagnosticSeverityName(DiagnosticSeverity severity)
{
	switch(severity) {
	case DiagnosticSeverity::Information: return "Information";
	case DiagnosticSeverity::Warning: return "Warning";
	case DiagnosticSeverity::Error: return "Error";
	default: return "Unknown";
	}
}

void BuildImageSpecReport(const ImageSpec& spec, DiagnosticReport& output)
{
	DiagnosticReport report;
	report.title = "Image specification";
	AddItem(report, "ImageSpec", "valid", BoolText(spec.IsValid()));
	AddItem(report, "ImageSpec", "data_window",
	        Format("%d,%d - %d,%d", spec.data_window.left, spec.data_window.top,
	               spec.data_window.right, spec.data_window.bottom));
	int64 width = 0, height = 0, pixels = 0, samples = 0, bytes = 0;
	AddItem(report, "ImageSpec", "width", spec.GetWidth(width) ? AsString(width) : "invalid");
	AddItem(report, "ImageSpec", "height", spec.GetHeight(height) ? AsString(height) : "invalid");
	AddItem(report, "ImageSpec", "depth", AsString(spec.depth));
	AddItem(report, "ImageSpec", "channels", AsString(spec.channels));
	AddItem(report, "ImageSpec", "layout", ChannelLayoutName(spec.channel_layout));
	AddItem(report, "ImageSpec", "sample_type", SampleTypeName(spec.sample_type));
	AddItem(report, "ImageSpec", "alpha_channel", AsString(spec.alpha_channel));
	AddItem(report, "ImageSpec", "channel_names", JoinNames(spec.channel_names));
	AddItem(report, "ImageSpec", "pixel_count", spec.GetPixelCount(pixels) ? AsString(pixels) : "invalid");
	AddItem(report, "ImageSpec", "sample_count", spec.GetSampleCount(samples) ? AsString(samples) : "invalid");
	AddItem(report, "ImageSpec", "byte_count", spec.GetByteCount(bytes) ? AsString(bytes) : "invalid");
	output = pick(report);
}

void BuildMetadataReport(const Metadata& metadata, DiagnosticReport& output)
{
	DiagnosticReport report;
	report.title = "Metadata";
	Vector<String> keys;
	const VectorMap<String, Value>& items = metadata.Items();
	for(int i = 0; i < items.GetCount(); ++i)
		keys.Add(items.GetKey(i));
	Sort(keys);
	AddItem(report, "Metadata", "count", AsString(keys.GetCount()));
	for(const String& key : keys)
		AddItem(report, "Metadata", key, ValueText(metadata.Get(key)));
	output = pick(report);
}

void BuildDiagnosticsReport(const Diagnostics& diagnostics, DiagnosticReport& output)
{
	DiagnosticReport report;
	report.title = "Diagnostics";
	AppendDiagnostics(diagnostics, report);
	output = pick(report);
}

void BuildResultReport(const Result& result, DiagnosticReport& output)
{
	DiagnosticReport report;
	report.title = "Result";
	AppendResult(result, report);
	output = pick(report);
}

Result BuildOperationReport(const String& operation, const Result& result,
                            const Diagnostics& operation_diagnostics,
                            double elapsed_ms, DiagnosticReport& output,
                            Diagnostics* diagnostics)
{
	Diagnostics source = operation_diagnostics;
	Reset(diagnostics);
	if(operation.IsEmpty())
		return Fail(ResultCode::InvalidArgument, diagnostics,
		            "operation name is empty", "IMGDIAG_OPERATION");
	if(!std::isfinite(elapsed_ms) || elapsed_ms < 0.0)
		return Fail(ResultCode::InvalidArgument, diagnostics,
		            "elapsed time must be finite and non-negative",
		            "IMGDIAG_TIMING", operation);

	DiagnosticReport report;
	report.title = operation;
	AddItem(report, "Operation", "name", operation);
	AddItem(report, "Operation", "elapsed_ms", Format("%.6f", elapsed_ms));
	AppendResult(result, report);
	AppendDiagnostics(source, report);
	output = pick(report);
	return Result::Success();
}

} // namespace Imaging
} // namespace Upp
