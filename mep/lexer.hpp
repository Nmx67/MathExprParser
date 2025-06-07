#pragma once

#include <string>
#include <iostream>
#include <stack>

#include <mep/mep_export.h>
#include <mep/math.hpp>
#include <mep/AST.hpp>

namespace mep {

/*
tokenize :
input :
    x * y + z
output :   
   token : x, *, y, +, z, <end>
   type  : T, O, T, O, T, <end>
   value : x,  , y,  , z  
where  : 
   T == term or factor
   O == operator
distinguishing Unary and binary operators depends on context

input : -6 + z * sin(1 + 2 * 3.14)
output :
   token : -, 6, +, 7, *, sin, LP, 1, +, 2, *, 3.14, RP, <end>
   type  : U, T, B, T, B, U,   LP, T, B, T, B, T, RP  <end>
   value : m  6     z     sin      1     2     3.14
*/

enum TokenType {
   T_UNARY_OP, T_BINARY_OP,
   T_TERM,
   T_LP, T_RP,
   T_EOF,
   T_UNDEFINED
};

class Token {
public:
   TokenType tag;
   Token() : tag(TokenType::T_UNDEFINED) {}

   Token(Token* other) : tag(other->tag) {}
   virtual ~Token() {}
   void clear() { tag = TokenType::T_UNDEFINED; }
   bool is_term() const { return tag == T_TERM; }
   bool is_binary() const { return tag == T_BINARY_OP; }
   bool is_unary() const { return tag == T_UNARY_OP; }
   bool is_operator() const { return is_unary() || is_binary(); }
   char to_char() const;
   friend std::ostream& operator<<(std::ostream& os, const Token& dt);
};

class TermToken : public Token {
public:
   enum TermType {
      Number,   // literal number/value
      Variable  // variable that needs value substitution
   };
public:
   std::string value;
   TermType term_type;
   TermToken(Token *base, TermType tt)
      : Token(std::move(base))
      , term_type(tt)
   {}

};

inline
Token* make_term_token(Token* base, const TermToken::TermType& tt,  const std::string& value)
{
   TermToken* tok = new TermToken(base, tt);
   tok->value = value;
   return tok;
}

class OperatorToken : public Token {
public:
   Operator op;  
   OperatorToken() : op() { }
   OperatorToken(Token* base, Operator op)
      : Token(std::move(base))
      , op(op)
   {}
};


inline
Token* make_operator_token(Token* base, const Operator::Tag& operation)
{
   Operator op;
   op.m_operation = operation;
   OperatorToken* tok = new OperatorToken(base, op);
   return tok;
}
inline
Token* make_operator_token(Token* base, const FunctionId& id)
{   
   Operator op;
   op.m_operation = Operator::Apply;
   op.m_func = id;
   OperatorToken* tok = new OperatorToken(base, op);
   return tok;
}


std::ostream& operator<<(std::ostream& os, const mep::Token& tok);


//////////////////////////////////////////////////////////////////////////////
// Lexer
class Lexer {
   bool m_f_debug{ false };
   std::string input_;
   Token* m_curr_token; // look ahead token : will return this untill it is consumed
   bool m_curr_token_consumed;

public:
   Lexer()
      : input_("")
      , m_curr_token_consumed(true)
   {
      
   }
   void init(const std::string& input)
   {
      input_ = input;
      m_curr_token_consumed = true;
      m_curr_token = nullptr;
   }

   void debug(bool on_or_off)
   {
      m_f_debug = on_or_off;
   }
   inline void consume(size_t n) { input_ = input_.substr(n); }

   std::string consume_number() {
      size_t n = 1;
      while (::isdigit(input_[n])) { n++; }
      std::string number_str(input_.data(), n);
      consume(n);
      return number_str;
   }
   std::string consume_identifier()
   {
      size_t n = 1;
      while (::isalnum(input_[n])) { n++; }
      if (n > 64) {
         throw std::exception("Identifier too long");
      }
      std::string id(input_.data(), n);
      consume(n);
      return id;
   }

   void consume_token()
   {
      m_curr_token_consumed = true;
      // m_curr_token = nullptr;
      // m_curr_token = get_next_token();
   }
   Token* peek_token()
   {
      if(!m_curr_token_consumed) {
         return m_curr_token;
      }
      Token* tok = new Token();
     
      do {
         if (input_.empty()) {
            tok->tag = TokenType::T_EOF; break;
         }
         char c = input_[0];
         switch (c) {
         case ' ' : case '\t' : case '\n' : case '\r' : {
            consume(1); 
            continue;
         }
         case '+':
         case '-':        
            consume(1);
            if (m_curr_token && (m_curr_token->tag == TokenType::T_TERM || m_curr_token->tag == TokenType::T_RP)) {
               tok->tag = TokenType::T_BINARY_OP;
               tok = make_operator_token(tok, (c == '+') ? Operator::Add : Operator::Sub);
            } else {
               tok->tag = TokenType::T_UNARY_OP;
               tok = make_operator_token(tok, (c == '+') ? FunctionId::Identity : FunctionId::Negate);               
            }
            break;
         case '^':
            consume(1);
            tok->tag = TokenType::T_BINARY_OP;
            tok = make_operator_token(tok, Operator::Pow);
            break;
         case '%': 
            consume(1);
            tok->tag = TokenType::T_BINARY_OP;
            tok = make_operator_token(tok, Operator::Mod);
            break;
         case '&': case '|':
            consume(1);
            tok->tag = TokenType::T_BINARY_OP;
            tok = make_operator_token(tok, (c == '&') ? Operator::And : Operator::Or);
            break;
         case '*': 
         case '/': // fall through
            consume(1);
            tok->tag = TokenType::T_BINARY_OP;
            tok = make_operator_token(tok, (c == '*') ? Operator::Mul : Operator::Div);                        
            break;
         case '(': tok->tag = TokenType::T_LP; consume(1); break;
         case ')': tok->tag = TokenType::T_RP; consume(1); break;

         default: {
            if(::isdigit(c)) {
               tok->tag = TokenType::T_TERM;
               tok = make_term_token(tok, TermToken::Number, consume_number());             
            } else {
               std::string str = consume_identifier();
               try {
                  FunctionId fid = mep_lookup_function(str);
                  tok->tag = TokenType::T_UNARY_OP;
                  tok = make_operator_token(tok, fid);
               } catch (MepFuntionNotSupported& e) {
                  tok->tag = TokenType::T_TERM;
                  tok = make_term_token(tok, TermToken::Variable, str);
               }
            }
         } break;
         }
      } while (tok->tag == TokenType::T_UNDEFINED);
      m_curr_token = tok;
      m_curr_token_consumed = false;
      if(m_f_debug) std::cout << tok;
      return tok;
   }
};

} // ns


