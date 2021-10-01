/* The untyped lambda calculus
 * because i'm a fucking masochist
 *
 * This is going to be slightly different from typical lambda-calculus in one
 * major way: syntax.  As I'm typing on a keyboard in American, and I use
 * 'merican ass-key to encode all my letters, I don't have the fancy letter
 * lambda and also since I'm not using TeX I can't just type \lambda everywhere.
 *
 * So yeah, the basic syntax:
 *
 * variable    := "a".."z" | variable "'"
 * lambda-term := variable
 *              | "(" lambda-term lambda-term ")"
 *              | "\" variable "(" lambda-term ")"
 *              | "\" variable+ "." lambda-term
 *
 * Note that \x.yx is \x.(yx) and not (\x.y)x (abstraction extends as far right as possible)
 *
 * Maybe I'm making the syntax too complicated?  If I am, I'll scrap it and fix
 * it later... or something.
 *
 * A lot of this is coming from the article "A Tutorial Introduction to the
 * Lambda Calculus" by Raul Rojas.  It's a really good paper describing the Lambda
 * calculus, and gives a good overview on everything.
 *
 * Also thanks to https://github.com/mpu/lambda/ for some of the code ideas.
 */

// some includes, so we can use exactly-specified numeric types, and not any of
// those dirty variable-width bit types.  Also stdio so we can print out the result
// of any computation we do.
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

/* Lambda expressions are defined recursively, like this:
 *
 *    <expression>  := <name>|<function>|<application>
 *    <function>    := \<name>.<expression>
 *    <application> := <expression> <expression>
 *    <parens>      := '(' <expression>+ ')'
 *    <name>        := 'a', 'b', 'c', ... 'X', 'Y', 'Z', '0', ... '9', '@', '#'
 *                     This gives us 64 different characters to use in our functions
 *
 * But this is left-recursive, so we need to fix that and make something that
 * isn't.  Also, it suffers from the fact that it ends as soon as it can.  This
 * non-greedy behavior means that "x \y.x" only consumes the first variable, not
 * the lambda term that follows.
 *
 *    <expression>  := <application>|<parens>|<function>|<name>
 *    <function>    := \<name>.<expression>
 *    <application> := ( <parens>|<function>|<name> ) <expression>
 *    <parens>      := '(' <expression>+ ')'
 *    <name>        := 'a', 'b', 'c', ... 'X', 'Y', 'Z', '0', ... '9', '@', '#'
 *
 * name, function and parens hasn't changed, but we changed the order for
 * expression, and made application look for either a name or function, then
 * some expression, that way it doesn't left-recurse all the time.
 *
 * So as we figure out how to represent all this, we need to keep all this shindig
 * in our minds...
 */

// I guess the first thing is a set of enums for the kinds of things we are dealing
// with and a struct describing our data.
typedef enum {
  UL_name,
  UL_function,
  UL_application,
} UL_kind;

struct UL_expression; // forward declaration... because... C!

// the struct takes a "kind" and then the actual information is placed inside
// the right spots (hopefully)
typedef struct UL_expression { // BAH!!! https://www.reddit.com/r/c_language/comments/5qhs3s/mutually_recursive_types/dczssv8?utm_source=share&utm_medium=web2x&context=3
  uint8_t kind;

  // We are using a union so we don't have *massive* wastes of space.
  union {
    struct {
      char name;
      struct UL_expression* body;
    } lambda;

    struct {
      struct UL_expression* left;
      struct UL_expression* right;
    } apply;

    char name;
  } data;
} UL_expression;

// Just some convience functions for doing some stuff...
void EXPLOSION(char* text) {
  fputs(text, stderr);
  exit(1);
}

void EXPECT_OR_EXPLODE(char expected, char token, char* fail_text) {
  if (!(expected == token)) {
    EXPLOSION(fail_text);
  }
}

#include <Windows.h>
void P(char c) {
  printf("%c", c);
  Sleep(1000);
}

UL_expression* alloc_ul_expr() {
  UL_expression* expr = (UL_expression*)malloc(sizeof(UL_expression));
  if (!expr) {
    EXPLOSION("Ran out of memory, or something else equally bad...");
  }
  return expr;
}

UL_expression* ul_name(char var) {
  UL_expression* expr = alloc_ul_expr();
  expr->kind = UL_name;
  expr->data.name = var;
  return expr;
}

UL_expression* ul_function(char var, UL_expression* body) {
  UL_expression* expr = alloc_ul_expr();
  expr->kind = UL_function;
  expr->data.lambda.name = var;
  expr->data.lambda.body = body;
  return expr;
}

UL_expression* ul_application(UL_expression* left, UL_expression* right) {
  UL_expression* expr = alloc_ul_expr();
  expr->kind = UL_application;
  expr->data.apply.left = left;
  expr->data.apply.right = right;
  return expr;
}

// Lets get some printing done, so we can see the fruits of our labors!
// We want to make sure that we are getting what we expect, while also enabling us
// to copy-and-paste the results of our explorations back into the program.
// Input -> Output -> Input
void print_expr(UL_expression* expr) {
  switch (expr->kind) {
    case UL_name: {
      printf("%c", expr->data.name);
    } break;

    case UL_function: {
      printf("\\%c.", expr->data.lambda.name);
      // RECURSE!
      if (expr->data.lambda.body) { print_expr(expr->data.lambda.body); }
    } break;

    case UL_application: {
      // RECURSE!!!
      if (expr->data.apply.left) { print_expr(expr->data.apply.left); }
      // TWICE!!!
      if (expr->data.apply.right) { print_expr(expr->data.apply.right); }
    } break;
  }
}

// helper for legal variable character names!
bool varchar_p(char v) {
  return isalnum(v) || v == '@' || v == '#';
}

void skip_whitespace(char* src[]) {
  while (isspace(*src[0])) { *src = *src + 1; }
}

// Prototypes!
bool read_expr(char* src[]);
bool read_app(char* src[]);
bool read_fun(char* src[]);
bool read_paren(char* src[]);

bool read_name(char* src[]) {
  P('v');
  char* orig = *src;

  skip_whitespace(src);
  if (varchar_p(*src[0])) {
    // expr = ul_name(*src[0]);
    *src = *src + 1;
    return true;
  }

  *src = orig;
  P('\b');
  return false;
}

bool read_paren(char* src[]) {
  P('(');
  char* orig = *src;
  skip_whitespace(src);

  if (!(*src[0] == '(')) {
    *src = orig;
    P('\b');
    return false;
  }

  read_expr(src);

  EXPECT_OR_EXPLODE(')', *src[0], "Mismatched Parens!!! EXPLOSION!!!!");

  return true;
}

bool read_fun(char* src[]) {
  P('\\');
  char* orig = *src;
  skip_whitespace(src);

  if (!(*src[0] == '\\')) {
    // early exit
    *src = orig;
    P('\b');
    return false;
  }

  // It's a lambda!
  *src = *src + 1;

  if (!varchar_p((char)*src[0])) {
    // it's not a well-formed lambda...
    EXPLOSION("You need a variable after the lambda!");
  }

  // grab the variable
  char var = *src[0];
  *src = *src + 1;

  EXPECT_OR_EXPLODE('.', *src[0], "You need a period after the variable");
  *src = *src + 1;

  // UL_expression* body = NULL;

  if (!(read_app(src) || read_expr(src))) {
    EXPLOSION("No body in lambda...");
  }

  // expr = ul_function(var, body);
  return true;
}

bool read_app(char* src[]) {
  P('@');
  char* orig = *src;
  skip_whitespace(src);

  // left-branch
  if (!(read_paren(src)
     || read_fun(src)
     || read_name(src))) {
    *src = orig;
    P('\b');
    return false;
  }

  // right-branch
  if (!read_expr(src)) {
    *src = orig;
    P('\b');
    return false;
  }

  return true;
}

bool read_expr(char* src[]) {
  P('!');
  char* orig = *src;
  if (read_app(src)) {
     // good stuff happened!
     return true;
   }

  *src = orig;
  P('\b');
  return false;
}

/*******************************************************************************
 * Here we do some weird things to enable some testing...
 * We are defining a function (in this case the identity function) and then 2
 * UL_expressions, one simply holds a variable (which we need for the next one),
 * and the next which is the actual function.
 */
char* example_in = "x\\x.xy(\\y.x)";
UL_expression example_var = { .kind = UL_name, .data = { .name = 'x'}};
UL_expression example_out = { .kind = UL_function, .data = { .lambda = { .name = 'x', .body = &example_var}}};

int main(int argc, char* argv[]) {
  UL_expression* expr;

  char* src = example_in;

  if (read_expr(&src)) {
    printf("\nWe read a thing!\n");
    // print_expr(expr);
  } else {
    printf("\nWe read nothing...\n");
  }
}
