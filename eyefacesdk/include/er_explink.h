/* **************************************************************** */
/* **************************************************************** */
/* *                                                              * */
/* *    Copyright (c) 2015-2017 by Eyedea Recognition, s.r.o.     * */
/* *                  ALL RIGHTS RESERVED.                        * */
/* *                                                              * */
/* *    Author:  Eyedea Recognition, s.r.o.                       * */
/* *                                                              * */
/* *    Contact:                                                  * */
/* *             web: http://www.eyedea.cz                        * */
/* *             email: info@eyedea.cz                            * */
/* *                                                              * */
/* **************************************************************** */
/* **************************************************************** */

#ifndef EYEDEA_ER_EXPLINK_H
#define EYEDEA_ER_EXPLINK_H

//////////////////////////////////////////////////////////////////////
//                                                                  //
// EXPLICIT LINKING                                                 //
// Explicit linking is a type of linkage, which links the library   //
// to the program. Explicit linking links at runtime, as oppose     //
// to traditional linkage, which links at build time.               //
// This is required by the DRM used (gemalto's Sentinel LDK).       //
// See the Developer's Guide and examples to learn more.            //
// We provide multiplatform defines to unify linkage in Windows,    //
// Linux and macOS.                                                 //
//////////////////////////////////////////////////////////////////////

#if _WIN32 || _WIN64 /* WINDOWS Platform */
/* Standard includes */
#   include <windows.h>
#   include <direct.h>
#   include <io.h>

/* function prefix*/
#ifdef __cplusplus
# define ER_FUNCTION_PREFIX extern "C" __declspec(dllexport)
#else
# define ER_FUNCTION_PREFIX __declspec(dllexport)
#endif

/* explicit linking types and  macros*/
typedef HMODULE shlib_hnd; /* type definition of shared lib. handle */
                           /* Shared lib. open and function load routines. */
#   define ER_OPEN_SHLIB(shlib_hnd, shlib_filename) (shlib_hnd = LoadLibrary(shlib_filename))
#   define ER_LOAD_SHFCN(shfcn_ptr, shfcn_type, shlib_hnd, shfcn_name) (shfcn_ptr = (shfcn_type) GetProcAddress(shlib_hnd, shfcn_name))
#   define ER_FREE_LIB(shlib_hnd) FreeLibrary(shlib_hnd)
#   define ER_LIB_PREFIX 
#   define ER_LIB_EXT ".dll"
#   if _WIN64
#       define ER_LIB_TARGET "x64"
#   else
#       define ER_LIB_TARGET "Win32"
#   endif
                           /* last error string of linking procedure */
static char __lastErrorMsg__[256]; 
static inline const char* __getLastErrorMsg() {
    wchar_t lastErrorMsgW[256];
    FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), lastErrorMsgW, 256, NULL); 
    size_t i;  wcstombs_s(&i, __lastErrorMsg__, (size_t) 256, lastErrorMsgW, (size_t) 256);
    return __lastErrorMsg__;
}
#   define ER_SHLIB_LASTERROR __getLastErrorMsg()

#elif __linux__ || __unix /* LINUX/UNIX Platforms */
/* Standard includes */
#   include <dirent.h>
#   include <dlfcn.h>
#   include <sys/stat.h>

/* function prefix */
#ifdef __cplusplus
# define ER_FUNCTION_PREFIX extern "C" __attribute__ ((visibility ("default"))) 
#else
# define ER_FUNCTION_PREFIX __attribute__ ((visibility ("default"))) 
#endif

/* explicit linking types and macros */
typedef void* shlib_hnd; /* type definition of shared lib. handle */
                         /* Shared lib. open and function load routines. */
#   define ER_OPEN_SHLIB(shlib_hnd, shlib_filename) (shlib_hnd = dlopen(shlib_filename,RTLD_LAZY))
#   define ER_LOAD_SHFCN(shfcn_ptr, shfcn_type, shlib_hnd, shfcn_name) (shfcn_ptr = (shfcn_type) dlsym(shlib_hnd, shfcn_name))
#   define ER_FREE_LIB(shlib_hnd) dlclose(shlib_hnd)
#   define ER_LIB_PREFIX "lib"
#   define ER_LIB_EXT ".so"
#   if defined(__x86_64__) || defined(_M_X64) || defined(__ppc64__)
#       define ER_LIB_TARGET "x86_64"
#   else
#       define ER_LIB_TARGET x86_32
#   endif
#   define ER_SHLIB_LASTERROR dlerror()
#else
# error Unkown platform
#endif /* Multiplatform defines */ 

#endif
