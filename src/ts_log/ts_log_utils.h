#ifndef __TS_LOG_UTILS__
#define __TS_LOG_UTILS__

#ifndef __HERE__

#ifdef DEBUG

#ifdef __GNUC__
#include <string.h>
#define __QHERE__ QString::fromLatin1(((strrchr(__FILE__, '/') ?: __FILE__ - 1) + 1)) + ":" __HERE1__(__LINE__)
#define __HERE__  std::string(((strrchr(__FILE__, '/') ?: __FILE__ - 1) + 1)) + ":" __HERE1__(__LINE__)
#define __HERE1__(x)   STR(x)"\t" + __PRETTY_FUNCTION__
#define STR(a) #a
#else
#include <string>
#include <xstring>
#define __QHERE__ QString::fromLatin1(__FILE__) + ":"__HERE1__(__LINE__)
#define __HERE__  std::string(__FILE__) + ":"__HERE1__(__LINE__)
#define __HERE1__(x)   STR(x)"\t" +  __FUNCSIG__
#define STR(a) #a
#endif

#else
#define __QHERE__ QString()
#define __HERE__ std::string()
#endif // DEBUG

#endif // __HERE__

//////////////////////////////////////////////////////////////////////////////////
// Platform independent compiler warning                                        //
// see: http://stackoverflow.com/questions/471935/user-warnings-on-msvc-and-gcc //
//////////////////////////////////////////////////////////////////////////////////
// compiler_warning.h
#define STRINGISE_IMPL(x) #x
#define STRINGISE(x) STRINGISE_IMPL(x)
// Use: #pragma message WARN("My message")
#if _MSC_VER
#   define FILE_LINE_LINK __FILE__ "(" STRINGISE(__LINE__) ") : "
#   define WARN(exp) (FILE_LINE_LINK "WARNING: " exp)
#elif __clang__
//#   define DEFER(M,...) M(__VA_ARGS__)
//#	define WARN(exp) _Pragma(STRINGISE_IMPL(GCC warning(exp " at line " DEFER(STRINGISE_IMPL,__LINE__))))
#   define WARN(exp) ("WARNING: " exp)
#else//__GNUC__ - may need other defines for different compilers
#   define WARN(exp) ("WARNING: " exp)
#endif


#endif
