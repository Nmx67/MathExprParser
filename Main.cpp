// Parse and evaluate a mathematical expression from string
/*

*/
#include <cctype>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <queue>
#include <stack>
#include <string>


#include <stdio.h>
#include <string.h>


#include <mep/mep.hpp>


#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"


#ifdef BUILD_MEP_AS_DLL
# define toto toto
#endif
#ifdef _WIN32
#define strtok_r strtok_s
#endif





mep::number_t Test(const char* text)
{
   using namespace mep;
   std::cout << "------------------------" << std::endl;
   std::cout << "Input = " << text << std::endl;
   Parser parser;
   try {
      AST* ast = parser.parse(text);
      try {
         BeautifingVisitor printer;
         ast->traverse(printer);
         std::cout << "Exp = " << printer.result << std::endl;

         EvaluteVisitor evaluator;
         ast->traverse(evaluator);
         return evaluator.result;
      } catch (EvaluatorException& e) {
         std::cout << text << " \t " << e.what() << std::endl;
      }      
      delete ast;
   } catch (ParserException& e) {
      std::cout << text << " \t " << e.what() << std::endl;
      throw;
   }
}



std::vector<std::string> test_cases = {
   // basic 
   "- + -1",
   "1+2",
   "2^2^2",
   " % ",
   "  6 + c",
   "- + -1",
   "6-2-2",
   "2-7",
   "5*3",
   "8/4",
   // mixed additive and multiplicative expressions
   "2 -4 +6 -1 -1- 0 +8", // = 10
   "2*3 - 4*5 + 6/3", // = -12
   "2*3*4/8 -   5/2*4 +  6 + 0/3   ", // = -1
   "1 -1   + 2   - 2   +  4 - 4 +    6", // = 6
   // syntax error
   " 5 + + 6",
   "-5 + 2",
   // Divide by 0
   "5/0",
   " 2 - 1 + 14/0 + 7",
   // complex expressions enclosed in parenthesis
   "(5 + 2*3 - 1 + 7 * 8)", // = 66
   "(67 + 2 * 3 - 67 + 2/1 - 7)", // = 1
   // expressions with many subexpressions enclosed in parenthesis
   "(2) + (17*2-30) * (5)+2 - (8/2)*4", // = 8
    "(5*7/5) + (23) - 5 * (98-4)/(6*7-42)", // --> inf
   // nested parenthesis
   "(((((5)))))",
   "(( ((2)) + 4))*((5))",
   // should raise an error on unbalanced parenthesis
   "  6 + c",
   "  7 & 2",
   " % ",
   "2 + (5 * 2",
   "(((((4))))",
   "((2)) * ((3",
   "((9)) * ((1)"
};



TEST_CASE("Testing mep ")
{
   CHECK(doctest::Approx(1.3812).epsilon(0.001) == Test("sin(x) + cos(y)"));
   CHECK(   1 == Test("--1"));
   
   CHECK(1 == Test("- + -1"));
   CHECK(3== Test("1+2"));
   CHECK(16== Test("2^2^2"));

   
   CHECK(7== Test("  6 + c"));
   
   CHECK(2== Test("6-2-2"));
   CHECK(-5== Test("2-7"));
   CHECK(15== Test("5*3"));
   CHECK(2== Test("8/4"));
   // mixed additive and multiplicative expressions
   CHECK(10 == Test("2 -4 +6 -1 -1- 0 +8"));
   CHECK(-12== Test("2*3 - 4*5 + 6/3"));
   CHECK(-1 == Test("2*3*4/8 -   5/2*4 +  6 + 0/3   "));
   CHECK(6== Test("1 -1   + 2   - 2   +  4 - 4 +    6"));
   // syntax error
   CHECK(11 == Test(" 5 + + 6"));
   CHECK(-3== Test("-5 + 2"));
   CHECK(Test("5/3") == doctest::Approx(1.66).epsilon(0.01));
   
   // Divide by 0
   CHECK(true == std::isinf(Test("5/0")));
   CHECK(std::isinf(Test(" 2 - 1 + 14/0 + 7")));

   // complex expressions enclosed in parenthesis
   CHECK(66== Test("(5 + 2*3 - 1 + 7 * 8)"));
   CHECK(1== Test("(67 + 2 * 3 - 67 + 2/1 - 7)"));
   // expressions with many subexpressions enclosed in parenthesis
   CHECK(8 == Test("(2) + (17*2-30) * (5)+2 - (8/2)*4"));
   CHECK(std::isinf(Test("(5*7/5) + (23) - 5 * (98-4)/(6*7-42)")));
   // nested parenthesis
   CHECK(5== Test("(((((5)))))"));
   CHECK(30== Test("(( ((2)) + 4))*((5))"));
   
   CHECK(23== Test("2 + (7 * 3)"));   
   CHECK(6== Test("((2)) * ((3))"));

   // should raise an error on unbalanced parenthesis
   CHECK_THROWS_AS(Test("(((((4))))"), mep::ParserException);
   CHECK_THROWS_AS(Test("((5)) * ((1)"), mep::ParserException);
   CHECK_THROWS_AS(Test(" % "), mep::ParserException);
   CHECK_THROWS_AS(Test("  7 & 2"), mep::ParserException);

}


int main_old()
{
  
   for (const auto& exp : test_cases) {
      std::cout << "---------------------------" << std::endl;
      std::cout << "Evaluating: " << exp << std::endl;
      try {
         Test(exp.c_str());
         mep::MemTracker::show_mem_stats();
      } catch (std::exception& e) {
         std::cout << e.what() << std::endl;
      }
      std::cout << "---------------------------" << std::endl;
   }

   return EXIT_SUCCESS;

#if 0
   MathParser paser;
   std::string expr;
   double v;
   expr  = "3 + 4 * 2";
   std::cout << " Evaluation " << expr << " : " << std::endl;

   v = paser.evaluateExpression(expr);
   std::cout << "Result = " << v << std::endl;


   expr = "-(-3 + 4) * -2 / (1 + 1)";
   std::cout << " Evaluation " << expr << " : " << std::endl;
   v = paser.evaluateExpression(expr);
   std::cout << "Result = " << v << std::endl;
#endif
   return EXIT_SUCCESS;
}
