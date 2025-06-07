#include <mep/math.hpp>
#include <functional>
#include <map>


namespace mep {

FunctionId MEP_EXPORTS mep_lookup_function(const std::string& fname)
{
   if (fname == "abs") {
      return FunctionId::Abs;
   } else if (fname == "sin") {
      return FunctionId::Sin;
   } else if (fname == "cos") {
      return FunctionId::Cos;
   } else if (fname == "tan") {
      return FunctionId::Tan;
   } else if (fname == "asin") {
      return FunctionId::Asin;
   } else if (fname == "acos") {
      return FunctionId::Acos;
   } else if (fname == "atan") {
      return FunctionId::Atan;
   } else if (fname == "exp") {
      return FunctionId::Exp;
   } else if (fname == "log") {
      return FunctionId::Log;
   } else if (fname == "log10") {
      return FunctionId::Log10;
   }
   throw MepFuntionNotSupported();
}

std::string MEP_EXPORTS mep_function_name(const FunctionId& id)
{
   switch (id) {
   case FunctionId::Abs:   return "abs";
   case FunctionId::Sin:   return "sin";
   case FunctionId::Cos:   return "cos";
   case FunctionId::Tan:   return "tan";
   case FunctionId::Asin:  return "asin";
   case FunctionId::Acos:  return "acos";
   case FunctionId::Atan:  return "atan";
   case FunctionId::Exp:   return "exp";
   case FunctionId::Log:   return "log";
   case FunctionId::Log10: return "log10";
   }
   return "???";
}


using MathFunction = std::function<number_t(const number_t)>;

std::map <FunctionId, MathFunction> function_map =
{
   {FunctionId::Identity,  [](const double& x)->double { return x; }},
   {FunctionId::Negate,    [](const double& x)->double { return -x; }},
   {FunctionId::Abs,       [](const double& x)->double { return (x > 0) ? x : -x; }},
   {FunctionId::Sin,       [](const double& x)->double { return ::sin(x); }},
   {FunctionId::Cos,       [](const double& x)->double { return ::cos(x); }},
   {FunctionId::Tan,       [](const double& x)->double { return ::tan(x); }},
   {FunctionId::Asin,      [](const double& x)->double { return ::asin(x); } },
   {FunctionId::Acos,      [](const double& x)->double { return ::acos(x); } },
   {FunctionId::Atan,      [](const double& x)->double { return ::atan(x); } },
   {FunctionId::Exp,       [](const double& x)->double { return ::exp(x); } },
   {FunctionId::Log,       [](const double& x)->double { return ::log(x); } },
   {FunctionId::Log10,     [](const double& x)->double { return ::log10(x); } }
};


number_t MEP_EXPORTS call_math_function(const FunctionId& id, const number_t& param)
{
   MathFunction func = function_map[id];

   return func(param);
}

} // ns