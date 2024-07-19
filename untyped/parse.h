
#pragma once

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

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
    struct UL_expression* expr;
  } data;
} UL_expression;

// Prototypes!
UL_expression* read_ul(char* src[]);
UL_expression* alloc_ul_expr();
void free_ul_expr(UL_expression* expr);
UL_expression* ul_name(char var);
UL_expression* ul_function(char var, UL_expression* body);
UL_expression* ul_application(UL_expression* left, UL_expression* right);
char* sprint_expr(UL_expression* expr);
void print_expr(UL_expression* expr);
