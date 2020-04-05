#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <stdint.h>
#include <string.h>
#include <execinfo.h>
#include<stdarg.h>
#include "libwrench.h"

int libwrench_level = 0;

int magic_printf(const char *format, ...) {
  char buffer[2048];
  va_list va;
  va_start(va, format);
  int ret = vsnprintf(buffer, 2047, format, va);
  va_end(va);
  printf("\e[37m[%04d] ", libwrench_level);
  for (int i = 0; i < libwrench_level; i++) printf("__");
  printf("|\e[39m %s",buffer);
  return ret;
}

char *get_object_name(char*s, void *ptr) {
	static char buffer[1024];
	char temp_buffer[1024];
	strcpy(temp_buffer, s);
	int found = 0;
	for (int i = strlen(temp_buffer)-1; i >= 0; i--) if (temp_buffer[i] == '(') {
		temp_buffer[i] = 0;
		found = 1;
	}
	if (!found) return "(unknown)::";
	buffer[0] = 0;
	found = 0;
	int found2 = 0;
	for (int i = strlen(temp_buffer)-1; i >= 0; i--) {
		if (temp_buffer[i] == ':') if (temp_buffer[i-1] == ':') {
			found = 1 + i;
			break;
		}
	}
	if (found) {
		for (int i = (found - 3); i >= 0; i--) {
			if (temp_buffer[i] == ':') if (temp_buffer[i-1] == ':') {
				found2 = 1 + i;
				break;
			}
		}
		if (found <= 2) return "(unknown)::";
		found -= 2;
                for (int i = found2; i < found; i++) {
			buffer[i-found2] = temp_buffer[i];
			buffer[i-found2+1] = 0;
		}
	} else {
		return "(unknown)::";
	}
	sprintf(temp_buffer, "(class %p %s)::", ptr, buffer);
	strcpy(buffer, temp_buffer);
	return buffer;
}

int get_commas(char *s) {
        char *working_string = s;
	int found = 0;
	for (int i = strlen(s)-1; i >= 0; i--) {
		if (s[i] == '(') {
			found = 1 + i;
			break;
		}
	}
	if (!found) {
		printf("Wrong function args format!!!\n");
		exit(1);
	}
	working_string = s + found - 1;
        int result = 0;
	if (strcmp(working_string, "()") == 0) return 0;
	if (strcmp(working_string, "() const") == 0) return 0;
	if (strcmp(working_string, "(void)") == 0) return 0;
	if (strcmp(working_string, "(void) const") == 0) return 0;
	for (int i = 0; i < strlen(working_string); i++) {
		if (working_string[i] == ',') result++;
	}
	return result + 1;
}

void *get_function(const char *s, const char *lib) {
	static void *handle = NULL;
	if (handle == NULL) {
		handle = dlopen(lib, RTLD_LAZY | RTLD_LOCAL);
	}
	void *import = (void*) dlsym(handle, s);
	printf("    [ Resolved '%s' -> %s @ %p ]\n", s, lib, import);
	return import;
}


