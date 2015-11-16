#include <errno.h>

/*
  From: http://stackoverflow.com/questions/32896211/strtod-ambiguity-format-error-and-zero-conversion
*/

int isDouble(const char* s){  
    char* rest = (char*) s;

    strtod(s, &rest);

    if ((rest == s) || (errno == ERANGE)) return false;

    // If all WS after the last char used in the conversion, then the string is considered a 'clean float'
    while (isSpace(*rest)) ++rest;

    return (*rest == 0);
}
