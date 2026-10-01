#include <Core/Core.h>
#ifdef PLATFORM_WIN32
#include <windows.h>
#endif
using namespace Upp;

// Fixed project validation entry point; no arbitrary command or argv forwarding.
CONSOLE_APP_MAIN
{
#ifdef PLATFORM_WIN32
	if(!FileExists("tools/validate.ps1") || !FileExists("tests/acceptance.txt")) {
		Cerr() << "Run from the upp_imaging repository root.\n"; SetExitCode(2); return;
	}
	wchar_t command[] = L"powershell.exe -NoProfile -NonInteractive -ExecutionPolicy Bypass -File tools/validate.ps1 -Configuration debug";
	STARTUPINFOW start = {};
	start.cb = sizeof(start); start.dwFlags = STARTF_USESTDHANDLES;
	start.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
	start.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	start.hStdError = GetStdHandle(STD_ERROR_HANDLE);
	PROCESS_INFORMATION process = {};
	HANDLE job = CreateJobObjectW(nullptr, nullptr);
	JOBOBJECT_EXTENDED_LIMIT_INFORMATION policy = {};
	policy.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
	if(!job || !SetInformationJobObject(job, JobObjectExtendedLimitInformation, &policy, sizeof(policy))) {
		if(job) CloseHandle(job);
		Cerr() << "Unable to contain validation process.\n"; SetExitCode(2); return;
	}
	if(!CreateProcessW(nullptr, command, nullptr, nullptr, TRUE,
	                   CREATE_NO_WINDOW | CREATE_SUSPENDED, nullptr, nullptr, &start, &process)) {
		CloseHandle(job); Cerr() << "Unable to start PowerShell validation.\n"; SetExitCode(2); return;
	}
	if(!AssignProcessToJobObject(job, process.hProcess)) {
		TerminateProcess(process.hProcess, 2);
		CloseHandle(process.hThread); CloseHandle(process.hProcess); CloseHandle(job);
		Cerr() << "Unable to contain validation child.\n"; SetExitCode(2); return;
	}
	ResumeThread(process.hThread);
	DWORD code = 2;
	if(WaitForSingleObject(process.hProcess, 550000) == WAIT_OBJECT_0)
		GetExitCodeProcess(process.hProcess, &code);
	else Cerr() << "Debug acceptance exceeded its block deadline.\n";
	CloseHandle(process.hThread); CloseHandle(process.hProcess); CloseHandle(job);
	SetExitCode((int)code);
#else
	Cerr() << "This driver validates Windows Debug only.\n"; SetExitCode(2);
#endif
}
