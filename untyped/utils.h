
// Just some convience functions for error checking & stuff...
static void EXPLOSION(char* text) {
  fputs(text, stderr);
  exit(1);
}

static void EXPECT_OR_EXPLODE(char expected, char token, char* fail_text) {
  if (!(expected == token)) {
    EXPLOSION(fail_text);
  }
}

#define WATCH 1

#include <Windows.h>
static void P(char c) {
#if WATCH
  printf("%c", c);
  Sleep(50);
#endif
}

static void Bk() {
#if WATCH
  printf("\b \b");
#endif
}

