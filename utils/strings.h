/*
  Because C doesn'e have any proper String-handling functions,
  I gotta build em... including types... -_-
*/

#ifndef Stringification_Headerish
#define Stringification_Headerish

// The type
typedef struct String {
  size_t len;
  char* str;
} String;

// birth & death
void free_string(String* s);
String* init_string(char* str);

// life
bool str_eq(String* s1, String* s2);

#include <string.h>
String* init_string(char* str) {
  String* s = (String*)malloc(sizeof(String));
  s->len = 0;
  s->str = NULL;

  if (str) {
    s->len = strlen(str) + 1;
    s->str = (char*)malloc(sizeof(char) * s->len);
    strcpy(s->str, str);
  }

  return s;
}

void free_string(String* s) {
  if (s) {
    free(s->str);
    s->len = 0;
    s->str = NULL;
  }
}

bool str_eq(String* s1, String* s2) {
  if (s1->length != s2->length) {
    return false;
  }

  if (strcmp(s1->str, s2->str) == 0)) {
    return true;
  }

  return false;
}

#endif

