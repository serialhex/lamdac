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

#include "parse.h"
#include "utils.h"

bool ul_equal(UL_expression* l, UL_expression* r) {
  // super base case
  if (l == NULL && r == NULL) {
    return true;
  }
  
  if (l->kind != r->kind) {
    return false;
  }

  switch (l->kind) {
    case UL_name: {
      return true;
    } break;

    case UL_function: {
      if (l->data.lambda.name == r->data.lambda.name) {
        return ul_equal(l->data.lambda.body, r->data.lambda.body);
      } else {
        return false;
      }
    } break;

    case UL_application: {
      return ul_equal(l->data.apply.left, r->data.apply.left) &&
             ul_equal(l->data.apply.right, r->data.apply.right);
    } break;
  }

  return false; // complete fallthrough... shouldn't need this.
}

/*******************************************************************************
 * Here we do some weird things to enable some testing...
 * We are defining a function (in this case the identity function) and then 2
 * UL_expressions, one simply holds a variable (which we need for the next one),
 * and the next which is the actual function.
 */


// Test functions, just the prototypes though, cause C
bool test_variables();
bool test_function();
bool test_apply();
bool test_paren();
bool test_multi();

int main(int argc, char* argv[]) {
  printf("Starting...\n");

  printf("Testing Variables %s\n", test_variables() ? "Succeeded!" : "Failed!");
  printf("Testing Functions %s\n", test_function() ? "Succeeded!" : "Failed!");
}

bool test_variables() {
  UL_expression example_var = { .kind = UL_name, .data = { .name = 'x'}};
  char* var = "x";
  UL_expression* expr = read_ul(&var);

  return ul_equal(expr, &example_var);
}

bool test_function() {
  UL_expression example_var = { .kind = UL_name, .data = { .name = 'x'}};
  UL_expression example_fun = { .kind = UL_function, .data = { .lambda = { .name = 'x', .body = &example_var}}};
  char* fun = "\\x.x";
  UL_expression* expr = read_ul(&fun);

  return ul_equal(expr, &example_fun);
}

bool test_apply();
bool test_paren();
bool test_multi();

