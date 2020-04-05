#ifndef __LIBWRENCH_H__
#define __LIBWRENCH_H__

#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <stdint.h>
#include <string.h>
#include <execinfo.h>
#include <stdarg.h>

extern int libwrench_level;
extern char *__cxa_demangle(const char *, char *, size_t *, int*);
int magic_printf(const char *format, ...);

static inline void print_trace()
{
  void *array[50];
  size_t size;
  char **strings;
  size_t i;

  size = backtrace (array, 50);
  strings = backtrace_symbols (array, size);

  magic_printf("\n");
  magic_printf("\e[37m       ============ STACK TRACE ===========\e[39m\n");

  for (i = 1; i < size; i++)
     magic_printf("\e[37m        %s\e[39m\n", strings[i]);

  free (strings);
  magic_printf("\e[37m       =====================================\e[39m\n");
  magic_printf("\n");
}


#define printf magic_printf
#define HDR() void *object = NULL; HDR_INNER(0)
#define HDR_OBJECT() HDR_INNER(1)
#define HDR_INNER(xx) static void *inner_function = NULL; print_trace(); libwrench_level++; magic_printf("\n"); char demangled[255] = ""; int status = 0; size_t len = 255; __cxa_demangle(__func__, demangled, &len, &status); int comma_count_mangle = get_commas(demangled) + xx; magic_printf(">>> %s%s = \e[93m%s\e[39m\n", xx ? get_object_name(demangled, object) : "", __func__, demangled);

#define FTR_COMMON() char demangled[255] = ""; int status = 0; size_t len = 255; __cxa_demangle(__func__, demangled, &len, &status); magic_printf("<<< %s = \e[93m%s\e[39m\n",__func__, demangled);

#define FTR() { FTR_COMMON(); magic_printf("\n"); libwrench_level--; magic_printf("\n"); if (strcmp(type_string, "void") != 0) { magic_printf("ERROR: type is %s not void\n", type_string); exit(1); }  }; return;

#define FTRi(result_val) { FTR_COMMON(); if (strcmp(type_string, "int") != 0) { magic_printf("Wrong exit of function type %s is not int\n",type_string); exit(1); };  magic_printf("<<< --- returning value \e[92m%d\e[39m of type \e[92m%s\e[39m\n", result_val, type_string); }; libwrench_level--; magic_printf("\n"); return result_val; 
#define FTRptr(result_val) { FTR_COMMON(); if (result_val != NULL) if (((unsigned long)result_val) < 1000) { magic_printf("This does not look like a pointer: %p\n", result_val); exit(1); };  if (strcmp(type_string, "void*") != 0) { magic_printf("Wrong exit of function type %s is not void*\n",type_string); exit(1); }; magic_printf("<<< --- returning value \e[92m%p\e[39m of type \e[92m%s\e[39m\n", result_val, type_string); libwrench_level--; magic_printf("\n"); }; return result_val;

#define IMPORT(return_type, definition) char definition_string[500]; sprintf(definition_string, #definition); int comma_count_definition = get_commas(definition_string); if (comma_count_definition != comma_count_mangle) { magic_printf("UUUUU WE HAVE A CONFLICT OF AMOUNT OF VARS! %s (%d) vs %s (%d)\n", demangled, comma_count_mangle, definition_string, comma_count_definition); exit(1); };  char type_string[255]; sprintf(type_string, #return_type); \
                      return_type (*fn)definition; if (!inner_function) inner_function = (void*)get_function(__func__, LIBRARY); fn = (return_type (*)definition)inner_function;

#define STUB(fname) void fname() { magic_printf(">>> STUB "); HDR(); print_trace(); IMPORT(void, ()); fn(); FTR(); }
#define STUBO(fname) void fname(void *object) { magic_printf(">>> STUB "); HDR_OBJECT(); print_trace(); IMPORT(void, (void*)); fn(object); FTR(); }
#undef printf

char *get_object_name(char*s, void *ptr);
int get_commas(char *s);
void *get_function(const char *s, const char *lib);

#endif
