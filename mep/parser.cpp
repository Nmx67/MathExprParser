#include "parser.hpp"
#include "parser.hpp"
#include "parser.hpp"
#include "parser.hpp"
#include "parser.hpp"
#include "parser.hpp"
#include "parser.hpp"
#include "parser.hpp"
#include "parser.hpp"
#include "parser.hpp"
#include <mep.hpp>

#include <mep/parser.hpp>
/*

*/


namespace mep {

Token* Parser::peek_token() 
{
   return lexer.peek_token();
}

void Parser::consume_token(Token* tok)
{
   lexer.consume_token();
   if (m_previous_token) delete m_previous_token;
   m_previous_token = tok;
}

void Parser::expect_token(TokenType tok_type)
{
   Token* next = lexer.peek_token();
   if (next->tag != tok_type) {
      throw ParserException("Expected token not found");
   }
   lexer.consume_token();
}


AST* Parser::mk_leaf(const std::string& var) {
   AST* node = new TerminalNode(var);
   return node;
}
AST* Parser::mk_unary(FunctionId& func, AST* child) {
   AST* node = new UnaryNode(func, child);
   return node;
}
AST* Parser::mk_binary(Operator& op, AST* left, AST* right) {
   AST* node = new BinaryNode(op, left, right);
   return node;
}


Operator Parser::reduce_top_operator()
{
   Operator top = m_op_stack.top(); m_op_stack.pop();
   if (top.is_binary()) {
      AST* right = m_var_stack.top(); m_var_stack.pop();
      AST* left = m_var_stack.top(); m_var_stack.pop();
      m_var_stack.push(mk_binary(top, left, right));
   } else {
      AST* child = m_var_stack.top(); m_var_stack.pop();
      m_var_stack.push(mk_unary(top.m_func, child));
   }

   return m_op_stack.top();
}

void Parser::insert_operator_ontop(OperatorToken& tok)
{
   Operator top = m_op_stack.top();
   while( top.rank() >= tok.op.rank() ) { // precedence check
      top = reduce_top_operator();
   }
   m_op_stack.push(tok.op);

}


void Parser::parse_E()
{
   parse_T();
   
   while (true) {
      Token* tok = peek_token();
      if (!tok->is_binary()) break;

      OperatorToken* ot = dynamic_cast<OperatorToken*>(tok);
      insert_operator_ontop(*ot);
      consume_token(ot);
      parse_T();
   }

   Operator top = m_op_stack.top();
   while (top.m_operation != Operator::Nil) {
      top = reduce_top_operator();
   }
}

void Parser::parse_T()
{
   Token* tok = peek_token();
   if (tok->tag == TokenType::T_TERM) {
      TermToken* tt = dynamic_cast<TermToken*>(tok);
      consume_token(tok);
      m_var_stack.push(mk_leaf(tt->value));
   } else if (tok->tag == TokenType::T_LP) {
      consume_token(tok);
      m_op_stack.push(sentinel);
      parse_E();
      expect_token(TokenType::T_RP);
      m_op_stack.pop(); // pop the sentinel
   } else if (tok->tag == TokenType::T_UNARY_OP) {
      OperatorToken* ut = dynamic_cast<OperatorToken*>(tok);
      if(!ut->op.is_sign()) { // expect function call ala func(expr)
         m_op_stack.push(sentinel);
         m_op_stack.push(ut->op);
         consume_token(tok);
         expect_token(TokenType::T_LP);
         parse_E();
         expect_token(TokenType::T_RP);
         m_op_stack.pop(); // pop the sentinel
      } else { // sign operators : handle special cases as --X, +-X, +-+X, -+X, etc.
         bool previous_is_sign = false;
         if(m_previous_token && m_previous_token->tag == TokenType::T_UNARY_OP) {
            OperatorToken* ot = dynamic_cast<OperatorToken*>(m_previous_token);
            previous_is_sign = ot->op.is_sign();
         }
         if (previous_is_sign && ut->op.is_sign()) {
            consume_token(ut);
            // Test first if we have successive unary operators, e.g --1
            Operator top = m_op_stack.top(); 
            if (top.m_func == ut->op.m_func && top.m_func == FunctionId::Negate) {
               m_op_stack.pop();
            } else {
               // insert_operator_ontop(tok.op);
               m_op_stack.push(ut->op);
            }
            parse_T();
         } else {
            consume_token(ut);
            insert_operator_ontop(*ut);
            parse_T();
      }
      }
   } else {
      throw ParserException("error");
   }
}


AST* Parser::parse(const std::string& input)
{
   // prepare
   lexer.init(input);
   // lexer.debug(true);

   m_op_stack = std::stack<Operator>(); // TODO : properly clean
   m_op_stack.push(sentinel);
   if (m_previous_token) delete m_previous_token;
   m_previous_token = nullptr;

   m_var_stack = std::stack<AST*>(); // TODO : properly clean

   // start
   parse_E();
   expect_token(TokenType::T_EOF);
   return m_var_stack.top();
}



} // ns