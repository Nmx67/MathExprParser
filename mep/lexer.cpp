#include <mep/mep.hpp>
#include <mep/lexer.hpp>


namespace mep {

char Token::to_char() const
{
   switch (tag) {
   case TokenType::T_UNARY_OP: return 'u';
   case TokenType::T_BINARY_OP: return 'b';
   case TokenType::T_TERM: return 'v';
   case TokenType::T_LP: return '(';
   case TokenType::T_RP: return ')';
   case TokenType::T_EOF: return '\0';
   }
   return '?';
}

std::ostream& operator<<(std::ostream& os, const mep::Token& tok)
{
   if (tok.is_operator()) {

   } else {

   }

   // using std::operator<<;
   os << "TOK{";
   os << tok.to_char();
   os << '}';
   os << std::endl;
   return os;
}

} // ns