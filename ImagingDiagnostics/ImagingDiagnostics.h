#ifndef _ImagingDiagnostics_ImagingDiagnostics_h_
#define _ImagingDiagnostics_ImagingDiagnostics_h_

#include <ImagingCore/ImagingCore.h>

namespace Upp {
namespace Imaging {

struct NumericTolerance {
	double absolute = 1e-6;
	double relative = 1e-6;

	bool IsValid() const;
};

struct NumericComparison {
	double actual = 0.0;
	double expected = 0.0;
	double absolute_error = 0.0;
	double relative_error = 0.0;
	bool actual_finite = false;
	bool expected_finite = false;
	bool match = false;
};

struct NumericComparisonSummary {
	int64 compared = 0;
	int64 mismatched = 0;
	int64 first_mismatch = -1;
	double max_absolute_error = 0.0;
	double max_relative_error = 0.0;

	bool AllMatch() const { return mismatched == 0; }
};

Result CompareNumeric(double actual, double expected,
                      NumericComparison& output,
                      NumericTolerance tolerance = NumericTolerance(),
                      Diagnostics* diagnostics = nullptr);

Result CompareNumericArrays(const double* actual, const double* expected,
                            int64 count, NumericComparisonSummary& output,
                            NumericTolerance tolerance = NumericTolerance(),
                            Diagnostics* diagnostics = nullptr);

struct ReportItem : Moveable<ReportItem> {
	String section;
	String key;
	String value;
};

struct DiagnosticReport {
	String title;
	Vector<ReportItem> items;

	void Clear() { title.Clear(); items.Clear(); }
	bool IsEmpty() const { return title.IsEmpty() && items.IsEmpty(); }
	String ToText() const;
};

String SampleTypeName(SampleType type);
String ChannelLayoutName(ChannelLayout layout);
String ResultCodeName(ResultCode code);
String DiagnosticSeverityName(DiagnosticSeverity severity);

void BuildImageSpecReport(const ImageSpec& spec, DiagnosticReport& output);
void BuildMetadataReport(const Metadata& metadata, DiagnosticReport& output);
void BuildDiagnosticsReport(const Diagnostics& diagnostics, DiagnosticReport& output);
void BuildResultReport(const Result& result, DiagnosticReport& output);

Result BuildOperationReport(const String& operation, const Result& result,
                            const Diagnostics& operation_diagnostics,
                            double elapsed_ms, DiagnosticReport& output,
                            Diagnostics* diagnostics = nullptr);

} // namespace Imaging
} // namespace Upp

#endif
