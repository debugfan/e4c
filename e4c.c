#include <stdio.h>
#include <setjmp.h>
#include <signal.h>
#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>
#endif

static jmp_buf jmp_env;

#ifdef _WIN32
LPTOP_LEVEL_EXCEPTION_FILTER old_handle;
#else
typedef void (*sighandler_t)(int);
sighandler_t old_handle;
#endif

#ifdef _WIN32
LONG WINAPI exception_handle(
	_In_ struct _EXCEPTION_POINTERS *ExceptionInfo
	)
{
	printf("exception_handle called.\n");
	longjmp(jmp_env, ExceptionInfo->ExceptionRecord->ExceptionCode);
}
#else
void exception_handle(int signum)
{
	printf("exception_handle called.\n");
    longjmp(jmp_env, signum);
}    
#endif

int divide(int a, int b)
{
	return a / b;
}

int main(int argc, char* argv[])
{
	int code;  
	code = setjmp(jmp_env);
	if (0 == code)
	{
        // Install exception handler
#ifdef _WIN32
        old_handle = SetUnhandledExceptionFilter(exception_handle);
#else
        old_handle = signal(SIGFPE, exception_handle);
#endif
		// Try to raise exception
		printf("Try doing division by zero.\n");
		divide(5, 0);
	}
	else
	{
		// Catch exception
		printf("Catch an exception: 0x%x.\n", code);
	}

// Restore exception_handle
#ifdef _WIN32
    SetUnhandledExceptionFilter(old_handle);
#else
    signal(SIGFPE, old_handle);
#endif
    
	return 0;
}
