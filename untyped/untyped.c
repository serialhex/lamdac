/*
 * First of all... some quick thanks:
 *
 * A lot of this is coming from the article "A Tutorial Introduction to the
 * Lambda Calculus" by Raul Rojas.  It's a really good paper describing the Lambda
 * calculus, and gives a good overview on everything.
 *
 * Also thanks to https://github.com/mpu/lambda/ for some of the code ideas.
 */


/* The Untyped Lambda Calculus
 * ...because i'm a fucking masochist
 *
 * This is going to be slightly different from typical lambda-calculus in one
 * major way: syntax.  As I'm typing on a keyboard in American, and I use
 * 'merican ass-key to encode all my letters, I don't have the fancy letter
 * lambda and also since I'm not using TeX I can't just type \lambda everywhere.
 *
 * Lambda expressions are defined recursively, like this:
 *
 *    <expression>  := <name>|<function>|<application>
 *    <function>    := \<name>.<expression>
 *    <application> := <expression> <expression>
 *    <name>        := 'a', 'b', 'c', ... 'X', 'Y', 'Z', '0', ... '9', '@', '#'
 *                     This gives us 64 different characters to use in our functions
 *
 * Note that \x.yx is treated as \x.(yx) and not (\x.y)x (abstraction extends as far right as possible)
 *
 * But this is left-recursive, so we need to fix that and make something that
 * isn't.  Also, it suffers from the fact that it ends as soon as it can.  This
 * non-greedy behavior means that "x \y.x" only consumes the first variable, not
 * the lambda term that follows.
 *
 *    <expression>  := <application>|<parens>|<function>|<name>
 *    <function>    := \<name>.<expression>
 *    <application> := ( <parens>|<function>|<name> ) <expression>
 *    <parens>      := '(' <expression>+ ')' <expression>*
 *    <name>        := 'a', 'b', 'c', ... 'X', 'Y', 'Z', '0', ... '9', '@', '#'
 *
 * name, function and parens hasn't changed, but we changed the order for
 * expression, and made application look for either a name or function, then
 * some expression, that way it doesn't left-recurse all the time.
 *
 * So as we figure out how to represent all this, we need to keep all this shindig
 * in our minds...
 */

// some includes, so we can use exactly-specified numeric types, and not any of
// those dirty variable-width bit types.  Also stdio so we can print out the result
// of any computation we do.
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

// Various Includes
#include "debug.h"

//#define Stringification_Implementationification
#include "strings.h"

// I guess the first thing is a set of enums for the kinds of things we are dealing
// with and a struct describing our data.
typedef enum {
  UL_name,
  UL_function,
  UL_application,
  UL_paren,
} UL_kind;

struct UL_expression; // forward declaration... because... C!

// the struct takes a "kind" and then the actual information is placed inside
// the right spots (hopefully)
typedef struct UL_expression {
// BAH!!!
// https://www.reddit.com/r/c_language/comments/5qhs3s/mutually_recursive_types/dczssv8?utm_source=share&utm_medium=web2x&context=3
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

    struct UL_expression* expr;

    char name;
  } data;
} UL_expression;


UL_expression* alloc_ul_expr() {
  UL_expression* expr = (UL_expression*)malloc(sizeof(UL_expression));
  if (!expr) {
    EXPLOSION("Ran out of memory, or something else equally bad...");
  }
  return expr;
}

void free_ul_expr(UL_expression* expr) {
  if (expr == NULL) return;

  switch (expr->kind) {
    case UL_name: {
      free(expr);
    } break;

    case UL_function: {
      free_ul_expr(expr->data.lambda.body);
      free(expr);
    } break;

    case UL_application: {
      free_ul_expr(expr->data.apply.left);
      free_ul_expr(expr->data.apply.right);
      free(expr);
    } break;

    case UL_paren: {
      free_ul_expr(expr->data.expr);
      free(expr);
    } break;
  }
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

UL_expression* ul_paren(UL_expression* paren) {
  UL_expression* expr = alloc_ul_expr();
  expr->kind = UL_paren;
  expr->data.expr = paren;
  return expr;
}

char* sprint_expr(UL_expression* expr) {
  if (!expr) { EXPLOSION("EXPRESSION IS NULL!!!"); }

  size_t len = 0;
  char* str;
  switch (expr->kind) {
    case UL_name: {
      str = (char*)malloc(sizeof(char));
      str[0] = expr->data.name;
    } break;

    case UL_function: {

    }
  }

  return str;
}

// Lets get some printing done, so we can see the fruits of our labors!
// We want to make sure that we are getting what we expect, while also enabling us
// to copy-and-paste the results of our explorations back into the program.
// Input -> Output -> Input
void print_expr(UL_expression* expr) {
  if (!expr) { EXPLOSION("EXPRESSION IS NULL!!!"); }

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

    case UL_paren: {
      printf("(");
      print_expr(expr->data.expr);
      printf(")");
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
UL_expression* read_expr(char* src[]);
UL_expression* read_app(char* src[]);
UL_expression* read_fun(char* src[]);
UL_expression* read_paren(char* src[]);

UL_expression* read_name(char* src[]) {
  P('_');
  char* orig = *src;
  UL_expression* expr;

  skip_whitespace(src);
  if (varchar_p(*src[0])) {
    expr = ul_name(*src[0]);
    *src = *src + 1;
    return expr;
  }
  
  // Didn't find what we are looking for
  *src = orig;
  Bk();
  return NULL;
}

UL_expression* read_paren(char* src[]) {
  P('(');
  char* orig = *src;
  skip_whitespace(src);

  if (!(*src[0] == '(')) {
    *src = orig;
    Bk();
    return NULL;
  }

  *src = *src + 1; // nom the paren and get the expression
  UL_expression* expr = read_expr(src);

  EXPECT_OR_EXPLODE(')', *src[0], "Mismatched Parens!!! EXPLOSION!!!!");
  P(')');

  *src = *src + 1; // nom the close

  return ul_paren(expr);
}

UL_expression* read_fun(char* src[]) {
  P('\\');
  char* orig = *src;
  skip_whitespace(src);

  if (!(*src[0] == '\\')) {
    // early exit
    *src = orig;
    Bk();
    return NULL;
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

  UL_expression* body = NULL;

  if (!(body = read_expr(src))) {
    EXPLOSION("No body in lambda...");
  }

  UL_expression* expr = ul_function(var, body);
  return expr;
}

UL_expression* read_app(char* src[]) {
  P('@');
  char* orig = *src;
  skip_whitespace(src);

  UL_expression* l_expr;

  // left-branch
  if ((l_expr = read_paren(src))
   || (l_expr = read_fun(src))
   || (l_expr = read_name(src))) {

      // right-branch
      UL_expression* r_expr;
      orig = *src;
      r_expr = read_expr(src);
      if (r_expr) {
        return ul_application(l_expr, r_expr);
      } else {
        *src = orig;
        // just return previous result...
        return l_expr;
      }
  }

  *src = orig;
  Bk();
  return NULL;
}

UL_expression* read_expr(char* src[]) {
  P('!');
  char* orig = *src;
  UL_expression* expr;

  expr = read_app(src);
  if (expr) {
     // good stuff happened!
     return expr;
   }

  *src = orig;
  Bk();
  return NULL;
}


/*******************************************************************************
 * Here we do some weird things to enable some testing...
 * We are defining a function (in this case the identity function) and then 2
 * UL_expressions, one simply holds a variable (which we need for the next one),
 * and the next which is the actual function.
 */
char* t_var = "x";
char* t_fun = "\\x.x";
char* t_app = "xy";
char* t_par = "xyd(zang)bc";
char* example_in = "\\x.xy(\\y.x(zde)yab)";
UL_expression example_var = { .kind = UL_name, .data = { .name = 'x'}};
UL_expression example_out = { .kind = UL_function, .data = { .lambda = { .name = 'x', .body = &example_var}}};
UL_expression* t_expr = NULL;

// Test functions, just the prototypes though, cause C
void test();
bool test_variables();
bool test_function();
bool test_apply();
bool test_paren();
bool test_multi();

int main(int argc, char* argv[]) {
  // Run tests
  test();
}

void test_read(char* test_var, char* res_var) {
  printf("Testing: '%s'. Should result in: '%s'. Actual: '", test_var, res_var);
  UL_expression* t_expr = read_expr(&test_var);
  print_expr(t_expr);
  free_ul_expr(t_expr);
  printf("'\n");
}

void test() {
  test_read("x", "x");
  test_read("\\x.x", "\\x.x");
  test_read("(\\x.x)y", "(\\x.x)y");
  test_read("\\x.xy(\\y.x(zde)yab)", "\\x.xy(\\y.x(zde)yab)");
}

bool test_variables() {
  return false;
}

bool test_function() {
  return false;
}

bool test_apply() {
  return false;
}

bool test_paren() {
  return false;
}

bool test_multi() {
  return false;
}
