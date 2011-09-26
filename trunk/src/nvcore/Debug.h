// This code is in the public domain -- Ignacio Casta�o <castano@gmail.com>

#pragma once
#ifndef NV_CORE_DEBUG_H
#define NV_CORE_DEBUG_H

#include "nvcore.h"

#if defined(HAVE_STDARG_H)
#   include <stdarg.h> // va_list
#endif

#define NV_ABORT_DEBUG      1
#define NV_ABORT_IGNORE     2
#define NV_ABORT_EXIT       3

#define nvNoAssert(exp) \
    do { \
        (void)sizeof(exp); \
    } while(0)

#if NV_NO_ASSERT

#   define nvAssert(exp) nvNoAssert(exp)
#   define nvCheck(exp) nvNoAssert(exp)
#   define nvDebugAssert(exp) nvNoAssert(exp)
#   define nvDebugCheck(exp) nvNoAssert(exp)
#   define nvDebugBreak() nvNoAssert(0)

#else // NV_NO_ASSERT

#   if NV_CC_MSVC
// @@ Does this work in msvc-6 and earlier?
#       define nvDebugBreak()       __debugbreak()
//#       define nvDebugBreak()        __asm { int 3 }
#   elif NV_CC_GNUC && NV_CPU_PPC && NV_OS_DARWIN
// @@ Use __builtin_trap() on GCC
#       define nvDebugBreak()       __asm__ volatile ("trap");
#   elif NV_CC_GNUC && NV_CPU_X86 && NV_OS_DARWIN
#       define nvDebugBreak()       __asm__ volatile ("int3");
#   elif NV_CC_GNUC && NV_CPU_X86 
#       define nvDebugBreak()       __asm__ ( "int %0" : :"I"(3) )
#   else
#       include <signal.h>
#       define nvDebugBreak()       raise(SIGTRAP); 
// define nvDebugBreak()        *((int *)(0)) = 0
#   endif

#define nvDebugBreakOnce() \
    do { \
        static bool firstTime = true; \
        if (firstTime) { firstTime = false; nvDebugBreak(); } \
    } while(false)

#   define nvAssertMacro(exp) \
    do { \
        if (!(exp)) { \
            if (nvAbort(#exp, __FILE__, __LINE__, __FUNC__) == NV_ABORT_DEBUG) { \
                nvDebugBreak(); \
            } \
        } \
    } while(false)

#   define nvAssertMacroWithIgnoreAll(exp) \
    do { \
        static bool ignoreAll = false; \
        if (!ignoreAll && !(exp)) { \
            if (nvAbort(#exp, __FILE__, __LINE__, __FUNC__) == NV_ABORT_DEBUG) { \
                nvDebugBreak(); \
            } else { \
                ignoreAll = true; \
            } \
        } \
    } while(false)

#   define nvAssert(exp)    nvAssertMacro(exp)
#   define nvCheck(exp)     nvAssertMacro(exp)

#   if defined(_DEBUG)
#       define nvDebugAssert(exp)   nvAssertMacro(exp)
#       define nvDebugCheck(exp)    nvAssertMacro(exp)
#   else // _DEBUG
#       define nvDebugAssert(exp)   nvNoAssert(exp)
#       define nvDebugCheck(exp)    nvNoAssert(exp)
#   endif // _DEBUG

#endif // NV_NO_ASSERT

// Use nvAssume for very simple expresions only: nvAssume(0), nvAssume(value == true), etc.
#if !defined(_DEBUG)
#   if NV_CC_MSVC
#       define nvAssume(exp)    __assume(exp)
#   else
#       define nvAssume(exp)    nvCheck(exp)
#   endif
#else
#   define nvAssume(exp)    nvCheck(exp)
#endif


#define nvError(x)      nvAbort(x, __FILE__, __LINE__, __FUNC__)
#define nvWarning(x)    nvDebugPrint("*** Warning %s/%d: %s\n", __FILE__, __LINE__, (x))

#ifndef NV_DEBUG_PRINT
#define NV_DEBUG_PRINT 1 //defined(_DEBUG)
#endif

#if NV_DEBUG_PRINT
#define nvDebug(...)    nvDebugPrint(__VA_ARGS__)
#else
#if NV_CC_MSVC
#define nvDebug(...)    __noop(__VA_ARGS__)
#else
#define nvDebug(...)    ((void)0) // Non-msvc platforms do not evaluate arguments?
#endif
#endif


#if __cplusplus > 199711L
#define nvStaticCheck(x) static_assert(x)
#else
#define nvStaticCheck(x) typedef char NV_DO_STRING_JOIN2(__static_assert_,__LINE__)[(x)]
#endif

NVCORE_API int nvAbort(const char *exp, const char *file, int line, const char * func = NULL);
NVCORE_API void NV_CDECL nvDebugPrint( const char *msg, ... ) __attribute__((format (printf, 1, 2)));

namespace nv
{
    inline bool isValidPtr(const void * ptr) {
    #if NV_CPU_X86_64
        if (ptr == NULL) return true;
        if (reinterpret_cast<uint64>(ptr) < 0x10000ULL) return false;
        if (reinterpret_cast<uint64>(ptr) >= 0x000007FFFFFEFFFFULL) return false;
    #else
	    if (reinterpret_cast<uint32>(ptr) == 0xcccccccc) return false;
	    if (reinterpret_cast<uint32>(ptr) == 0xcdcdcdcd) return false;
	    if (reinterpret_cast<uint32>(ptr) == 0xdddddddd) return false;
	    if (reinterpret_cast<uint32>(ptr) == 0xffffffff) return false;
    #endif
        return true;
    }

    /// Message handler interface.
    struct MessageHandler {
        virtual void log(const char * str, va_list arg) = 0;
        virtual ~MessageHandler() {}
    };

    /// Assert handler interface.
    struct AssertHandler {
        virtual int assertion(const char *exp, const char *file, int line, const char *func = NULL) = 0;
        virtual ~AssertHandler() {}
    };


    namespace debug
    {
        NVCORE_API void dumpInfo();

        NVCORE_API void setMessageHandler( MessageHandler * messageHandler );
        NVCORE_API void resetMessageHandler();

        NVCORE_API void setAssertHandler( AssertHandler * assertHanlder );
        NVCORE_API void resetAssertHandler();

        NVCORE_API void enableSigHandler();
        NVCORE_API void disableSigHandler();
    }

} // nv namespace

#endif // NV_CORE_DEBUG_H
