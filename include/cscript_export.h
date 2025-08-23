#ifndef CSCRIPT_EXPORT_H
#define CSCRIPT_EXPORT_H

#if defined(_WIN32) || defined(__CYGWIN__)
    #ifdef CSCRIPT_EXPORTS
        #ifdef __GNUC__
            #define CSCRIPT_API __attribute__((dllexport))
        #else
            #define CSCRIPT_API __declspec(dllexport)
        #endif
    #else
        #ifdef __GNUC__
            #define CSCRIPT_API __attribute__((dllimport))
        #else
            #define CSCRIPT_API __declspec(dllimport)
        #endif
    #endif
    #define CSCRIPT_PRIVATE
#else
    #if __GNUC__ >= 4
        #define CSCRIPT_API __attribute__((visibility("default")))
        #define CSCRIPT_PRIVATE __attribute__((visibility("hidden")))
    #else
        #define CSCRIPT_API
        #define CSCRIPT_PRIVATE
    #endif
#endif

#endif // CSCRIPT_EXPORT_H
