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

#include "parse.h"
#include "utils.h"

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
      printf("(");
      // RECURSE!!!
      if (expr->data.apply.left) { print_expr(expr->data.apply.left); }
      // TWICE!!!
      if (expr->data.apply.right) { print_expr(expr->data.apply.right); }
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

// Recursion!
UL_expression* read_expr(char* src[]);

UL_expression* read_name(char* src[]) {
  P('_');
  char* orig = *src;
  UL_expression* expr;

  skip_whitespace(src);
printf("%c", *src[0]);
  if (varchar_p(*src[0])) {
    expr = ul_name(*src[0]);
    *src = *src + 1;
    return expr;
  }
Sleep(50);
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
  *src = *src + 1;
  return expr;
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
      if (r_expr = read_expr(src)) {
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

  if (expr = read_app(src)) {
     // good stuff happened!
     return expr;
   }

  *src = orig;
  Bk();
  return NULL;
}

UL_expression* read_ul(char* src[]) {
  UL_expression* expr = read_expr(src);
  P('\n');
  return expr;
}

