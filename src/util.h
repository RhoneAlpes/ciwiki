#ifndef _HAVE_UTIL_HEADER
#define _HAVE_UTIL_HEADER

char *
util_mprintf(const char *format, ...);

int 
util_ascii_to_hex(int c);

void 
util_dehttpize(char *z);

char *
util_extract_token(char *input, char **leftOver);

char *
util_htmlize(const char *in, int n);

char * 
util_httpize(const char* url);

int 
validDate(char *input);
#endif
