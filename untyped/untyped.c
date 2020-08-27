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
 * Note that \x.yx is \x(yx) and not ((\x.y)x)
 *
 * Maybe I'm making the syntax too complicated?  If I am, I'll scrap it and fix
 * it later... or something.
 */
