#ifndef INSIST_DOT_H
#define INSIST_DOT_H

/*
   Copyright (c) 2016, James Hunt

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.
 */

#include <stdio.h>
#include <stdlib.h>

/* ASSERTION_OUTPUT_STREAM defines the FILE* that the `insist()` macro will
   print error messages to, before exiting.  If ASSERTION_PRINT_ALWAYS is also
   defined, this stream will be used to print successes as well. */
#ifndef ASSERTION_OUTPUT_STREAM
#define ASSERTION_OUTPUT_STREAM stderr
#endif

/* ASSERTION_FAIL_EXIT_CODE defines the exit code value that the `insist()`
   macro will exit with, in the event of an assertion failure. */
#ifndef ASSERTION_FAIL_EXIT_CODE
#define ASSERTION_FAIL_EXIT_CODE 7
#endif

/* Two (2) #define's control the behavior of the `insist()` macro:

   ASSERTION_DEBUGGING causes the function (__func__), file (__FILE__) and
   line number (__LINE__) of the assertion call to be printed with the
   failure message, which can be useful for tracking down flaky assertions.

   ASSERTION_PRINT_ALWAYS causes `insist()` to *always* print a diagnostic
   (to ASSERTION_OUTPUT_STREAM), regardless of success / failure.  The
   program will not abort on success, of course, but the messages may prove
   useful for output verification during testing.

   Note that ASSERTION_PRINT_ALWAYS is only checked if ASSERTION_DEBUGGING
   is defined.  That is, you cannot get the "print always" behavior without
   the "debugging" behavior.
 */
#if   !defined(ASSERTION_DEBUGGING)
#  define insist(test,msg) ({ \
	if (!(test)) { \
		fprintf((ASSERTION_OUTPUT_STREAM), "ASSERTION FAILED: " msg " (`" #test "` was false)\n"); \
		exit((ASSERTION_FAIL_EXIT_CODE)); \
	} \
})
#elif !defined(ASSERTION_PRINT_ALWAYS)
#  define insist(test,msg) ({ \
	if (!(test)) { \
		fprintf((ASSERTION_OUTPUT_STREAM), "ASSERTION FAILED: " msg " (`" #test "` was false, in %s(), at %s:%i)\n", __func__, __FILE__, __LINE__); \
		exit((ASSERTION_FAIL_EXIT_CODE)); \
	} \
})
#else
#  define insist(test,msg) ({ \
	if (!(test)) { \
		fprintf((ASSERTION_OUTPUT_STREAM), "ASSERTION FAILED: " msg " (`" #test "` was false, in %s(), at %s:%i)\n", __func__, __FILE__, __LINE__); \
		exit((ASSERTION_FAIL_EXIT_CODE)); \
	} else { \
		fprintf((ASSERTION_OUTPUT_STREAM), "assertion succeeded: " msg " (`" #test "` was true, in %s(), at %s:%i)\n", __func__, __FILE__, __LINE__); \
	} \
})
#endif

#endif
