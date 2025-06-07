#pragma once 

#include <iomanip>
#include <sstream>
#include <map>
// #include <ostream>

#include <mep/math.hpp>

/*
 ----------------------------
|  +  |       | left  | right|
 ----------------------------
                  |      |
                  |      v
                  |      --------------------------
                  |      | NUM | 2   |  nil  | nil|
                  |       -------------------------
                  v
                  -------------------------
                 | NUM | 1     | nil  | nil|
                  -------------------------
*/

namespace mep {

using number_t = double;


number_t MEP_EXPORTS call_math_function(const FunctionId& ID, const number_t& param);

enum OpType {
   Unary = 1,
   Binary = 2
};

class Operator {
public:
   enum Tag { 
      Nil = 0,    // sentinel 
      Add, Sub,   // left associative : rank 1
      Mul, Div,   // left associative : rank 2
      Mod,        // rank 2
      Pow,        // right associative : rank 3
      And, Or,
      Apply       // function application       
   };
   Tag m_operation{ Nil };
   FunctionId m_func;
   bool is_unary() const { return m_operation == Apply; }
   bool is_binary() const { return !is_unary(); }
   bool is_sign() const { return is_unary() && (m_func == Identity || m_func == Negate); }

   // operator precedence (priority)
   int rank() 
   {
      
      switch(m_operation) {
      case Nil: return 0;
      case Add: case Sub: return is_binary() ? 1 : 4;
      case Mul: case Div: return 2;
      case Pow: return 3;      
      case Apply: 
         if (m_func == FunctionId::Negate || m_func == FunctionId::Identity) return 4;
         return 5;
      }
      return 6;
   }
   
   char to_char()
   {
      switch (m_operation) {
      case Tag::Add: return '+';
      case Tag::Sub: return '-';
      case Tag::Mul: return '*';
      case Tag::Div: return '/';
      case Tag::Pow: return '^';
      case Tag::Apply: if (is_sign()) return (m_func == Identity) ? '+' : '-';
      }
      return '?';
   }
};


class UnaryNode;
class BinaryNode;
class TerminalNode;


// Visitor interface
class IVisitor {
public:
   virtual void visit(UnaryNode& node) = 0;
   virtual void visit(BinaryNode& node) = 0;
   virtual void visit(TerminalNode& node) = 0;
};



//----------------------------------------------------------------------------
class Node : public MemTracker {
public:
   enum NodeTag  { N_OPERATOR, N_VALUE};
   NodeTag    m_type;
   virtual void accept(IVisitor& visitor) = 0;
   Node() = delete;
   Node(NodeTag t) : m_type(t) {
   }
   virtual ~Node()
   {
   }

   // Travsers the tree with the specified visitor
   void traverse(IVisitor& visitor)
   {
      this->accept(visitor);
   }
};

// Node specialisations :
// AST leaf
class TerminalNode : public Node {
public:
   std::string m_value;
   TerminalNode(const std::string& value)
      : Node(N_VALUE)
      , m_value(value)
   {
      
   }
   void accept(IVisitor& visitor) override { visitor.visit(*this); }
};

class OperatorNode : public Node {
public:
   Operator m_operator;   
   OperatorNode(Operator& op) : Node(N_OPERATOR), m_operator(op) {}
   char to_char() { return m_operator.to_char(); }
};
// AST Unary operator
class UnaryNode : public Node {   
public: 
   FunctionId m_func;
   Node* m_child;
   UnaryNode(const FunctionId& func, Node* child)
      : Node(N_OPERATOR)
      , m_func(func)
      , m_child(child)
   {
   }
   void accept(IVisitor& visitor) override { visitor.visit(*this); }
   
   ~UnaryNode()
   {
      if (m_child) delete m_child;
   }   
   
   std::string function_name()
   {
      return mep_function_name(m_func);
   }

};
// AST binary operator
class BinaryNode : public OperatorNode {
public:  
   Node* m_left;
   Node* m_right;
   BinaryNode(Operator& op, Node* l, Node* r) 
      : OperatorNode(op)
      , m_left(l), m_right(r)
   {
   }
   void accept(IVisitor& visitor) override
   {
      visitor.visit(*this);
   }
   ~BinaryNode()
   {
      if (m_left)  delete m_left;
      if (m_right) delete m_right;
   }
};

/*
* AST traversal with visitor (see https://github.com/agentcooper/cpp-ast-example/tree/main)
       AST
        |
       Op(+)
      /   \
     /     \
 Number(5)  \
             Op(*)
             /   \
            /     \
           /       \
       Number(2)   Number(444)
*/

// Concrete Visitor to evaluate the AST
class EvaluteVisitor : public IVisitor {
   using VarTable = std::map<std::string, number_t>;
   VarTable vars; // need to be passed in
public:
   number_t result{ 0 };
   number_t collect(Node* node)
   {
      node->accept(*this);
      return result;
   }
   // variable replacement
   number_t lookup(const std::string& variable)
   {
      auto search = vars.find(variable);
      if (search != vars.end()) {
         return search->second;
      }
      return 1;
   }

   void visit(TerminalNode& node) override
   {
      result = 0;
      if(::isdigit(node.m_value[0])) {
         result = std::stod(node.m_value);     
      } else {
         result = lookup(node.m_value);
      }
      
   }
   void visit(UnaryNode& node) override
   {
      if (node.m_func == FunctionId::Negate) {
         result = -collect(node.m_child);
      } else if (node.m_func == FunctionId::Identity) {
         result = collect(node.m_child);
      } else {
         result = ::mep::call_math_function(node.m_func, collect(node.m_child));
      }
   }
   void visit(BinaryNode& node) override
   {
      double v1 = collect(node.m_left);
      double v2 = collect(node.m_right);
      switch (node.m_operator.m_operation) {
      case Operator::Add: result = v1 + v2; break;
      case Operator::Sub: result = v1 - v2; break;
      case Operator::Mul: result = v1 * v2; break;
      case Operator::Div: result = v1 / v2; break;
      // case Operator::Mod: result = v1 - v2 * (static_cast<int>(v1 / v2)); break;
      case Operator::Mod: result = std::fmod(v1, v2); break;
      case Operator::Pow: result = std::pow(v1, v2); break;
      }
   }
};


// Visitor to reconstruct the expression
class BeautifingVisitor : public IVisitor {
public:
   std::string result;
   std::string collect(Node* node)
   {
      node->accept(*this);
      return result;
   }

   void visit(TerminalNode& node) override
   {
      if( ::isdigit(node.m_value[0]) ) {
         double value = std::stod(node.m_value);
         std::ostringstream os;
         os << std::fixed << std::setprecision(2) << value;
         result = os.str();
      } else {
         result = node.m_value;
      }
   }
   void visit(UnaryNode& node) override
   {
      if (node.m_func == FunctionId::Identity) {
         result = collect(node.m_child);
      } else if (node.m_func == FunctionId::Negate) {
         result = "-"  + collect(node.m_child);
      } else {
         result = node.function_name() + "(" + collect(node.m_child) + ")";
      }         
         
   }
   void visit(BinaryNode& node) override
   {
      result = "(" + collect(node.m_left) + " " + node.to_char() + " " + collect(node.m_right) + ")";
   }
};

} // ns