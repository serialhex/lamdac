

// Just some convience functions for error checking & stuff...
void EXPLOSION(char* text) {
  fputs(text, stderr);
  exit(1);
}

void EXPECT_OR_EXPLODE(char expected, char token, char* fail_text) {
  if (!(expected == token)) {
    EXPLOSION(fail_text);
  }
}

#define WATCH 0

#ifdef WIN32
#include <Windows.h>
#endif

void P(char c) {
#if WATCH
  printf("%c", c);
  Sleep(500);
#endif
}

void Bk() {
#if WATCH
  printf("\b \b");
#endif
}

