#pragma once 

#include <mep/AST.hpp>

namespace mep {

class EvaluatorException : public std::exception
{
public:
   EvaluatorException(const std::string& message)
      : std::exception(message.c_str())
   {
   }
};

#if 1
class Evaluator 
{
   // traverse the AST tree and evaluate 
   number_t evaluate_subtree(Node* ast)
   {
      if(ast == nullptr) 
         throw EvaluatorException("Empty tree!");

      if(ast->m_type == Node::N_VALUE) {
          std::string v = dynamic_cast<TerminalNode*>(ast)->m_value;
          return std::stod(v);
      } else if(ast->m_type == Node::N_OPERATOR) {
         OperatorNode* node = dynamic_cast<OperatorNode*>(ast);
         if(node->m_operator.is_unary()) {
            return -evaluate_subtree(dynamic_cast<UnaryNode*>(ast)->m_child);
         } else { // assume binary
            number_t v1 = evaluate_subtree(dynamic_cast<BinaryNode*>(ast)->m_left);
            number_t v2 = evaluate_subtree(dynamic_cast<BinaryNode*>(ast)->m_right);
            switch (node->m_operator.m_operation) {
            case Operator::Add :  return v1 + v2;
            case Operator::Sub :  return v1 - v2;
            case Operator::Mul :  return v1 * v2;
            case Operator::Div :  return v1 / v2;
            }
         }
      } else {
         
      }
      throw EvaluatorException("Incorrect syntax tree!");
   }
public:
   number_t evaluate(Node* ast)
   {
      if(ast == nullptr)
         throw EvaluatorException("Empty abstract syntax tree");
      // recursive travering
      return evaluate_subtree(ast);
   }
};
#endif

} // ns