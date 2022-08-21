/*
  Because C doesn'e have any proper String-handling functions,
  I gotta build em... including types... -_-
*/

#ifndef Stringification_Headerish
#define Stringification_Headerish

typedef struct String {
  size_t len;
  char* str;
} String;

void free_string(String* s);
String* init_string(char* str);

#endif

#ifdef Stringification_Implementationification
// #define Stringification_Implementationification

#include <string.h>
String* init_string(char* str) {
  String* s = (String*)malloc(sizeof(String));
  s->len = 0;
  s->str = NULL;

  if (str) {
    s->len = strlen(str) + 1;
    strcpy(s->str, str);
  }

  return s;
}

void free_string(String* s) {
  if (s) {
    free(s->str);
    s->len = 0;
  }
}

#endif
