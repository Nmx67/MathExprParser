#pragma once

#include <string>
#include <iostream>
#include <stack>

#include <mep/mep_export.h>
#include <mep/math.hpp>
#include <mep/lexer.hpp>
#include <mep/AST.hpp>

namespace mep {
// Abstart Syntax Tree
using AST = Node;

/* Precedence
   ^ (exponentiation) has precedence over unary - and the binary operators /, *, -, and +.
   * and / have precedence over unary - and binary - and +.
   Unary - and + have precedence over binary - and +.
   ^ is right associative while all other binary operators are left associative

For example :
   a ^ b * c ^ d + e ^ f / g ^ (h + i) 
parses to :
   +( *( ^(a,b), ^(c,d) ), /( ^(e,f), ^(g,+(h,i)) ) )
and 
   a - b - c
parses to 
   -(-(a,b),c)
*/


//###########################################
//### Parser
// https://www.engr.mun.ca/~theo/Misc/exp_parsing.htm
/* Shunting yard algorithm
 Basic idea is to keep operators on a stack until both their operands have been parsed
 - maintains up to date two stacks : one for the operators and the other for the operands
 - The key to the algorithm is to keep the operators on the operator stack
   ordered by precedence (lowest at bottom and highest at top), at least in the absence of parentheses
 - Before pushing an operator onto the operator stack, all higher precedence operators are cleared
   from the stack. Clearing an operator consists of removing the operator from the operator stack
   and its operand(s) from the operand stack, making a new tree, and pushing that tree onto the operand stack.
   At the end of an expression the remaining operators are put into trees with their operands and that is that.


Example : x*y+z

   the input is tokenized to the sequence
   token : x, *, y, +, z, <end>
   type  : T, O, T, O, T, <end>
   T == term or factor
   O == operator

   ; is the stack element seperator
   nil is a sentinel indicating an operator of lowest precedence

Treat input token one by one :

|tok  |  input  | stack       |  OP       | Action                        |
|---  |---      | ---         | ---       | :---                          |
|  x  | x*y+z   |             | nil       | push x on stack               |
|  *  | *y+z    |  x          | nil       | compare operator precedence   |
|     | *y+z    |  x          | *; nil    | * is higher, insert it        |
|  y  | y+z     |  x          | *; nil    | push y on stack               |
|  +  | +z      | y;x         | *; nil    | compare operator precedence   |
|     | +z      | y;x         | *; nil    | + is lower, make AST node     |
|     | +z      | *(x,y)      | nil       | compare precedence again      |
|     | +z      | *(x,y)      | +; nil    | + is higher, insert it        |
|  z  | z       | *(x,y)      | +; nil    | push z on stack               |
|<end>|         | z;*(x,y)    | +; nil    | try reduce, make AST node     |
|     |         | +(*(x,y),z) | nil       |                               |


Grammar (non-left-recursive grammar) :
    E --> T {B T}
    T --> v | "(" E ")" | U T
    B --> "+" | "-" | "*" | "/" | "^"
    U --> "-"

   {} indicate zero or n repitions
   | indicates an alternative
   U unary operator
   B binary operator
   V terminal value

Valid expressions are thus of the form
E -> T | T B T | T B T B T | T B T B T B T |... ad infinitum
Parsing routines
Tokens :
  U unary operator +- sign
  B binary operator : + - * / ^
  V literal value (or variable)
  LP left parenthesis
  RP right parentheses
  EOF  

Token routines
  peek_token() : returns the nex token in the input
  consume_token() : consumes the current token
  expect_token(t) : checks that the next token is t and consumes it



 parse() {
   parse_E();
   expect_token(Token::EOF);
 }

 parse_E() {
   parse_T();
   tok = next_token();
   while( tok is B) {
      consume_token();
      parse_T();
   }
 }

 parse_T() {
   token = next_token();
   switch(token) {
      case V  : consume_token(); return;
      case LP : consume_token();  parse_E(); expect_token(Token::RP); return;
      case U  : consume_token(); parse_T(); return;
   }
   error("Unexpected);
 }

*/

class MEP_EXPORTS ParserException : public std::exception {
public:
   ParserException(const std::string& message)
      : std::exception(message.c_str())
   {}
};

class MEP_EXPORTS Parser {
   Lexer lexer;
   bool m_f_debug{ false };

   std::stack<Operator> m_op_stack; // operator (sentinel guarded) stack
   std::stack<AST*> m_var_stack;     // operands stack (formed as an AST tree)
   Operator sentinel{};
   Token* m_previous_token{nullptr};
public:
   Parser()
   {

   }
  
   Token* peek_token();
   void consume_token(Token* tok);
   void expect_token(TokenType tok);

   void insert_operator_ontop(OperatorToken& tok);
   Operator reduce_top_operator();

   AST* mk_leaf(const std::string& var);
   AST* mk_unary(FunctionId& func, AST* child);
   AST* mk_binary(Operator& op, AST* left, AST* right);

   void parse_T();
   void parse_E();

   AST* parse(const std::string& input);
};


} // ns


