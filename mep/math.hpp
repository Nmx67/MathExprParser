#pragma once

#include <mep/mep_export.h>
#include <string>

namespace mep {

using number_t = double;
class MepFuntionNotSupported : public MepException 
{
public:
   MepFuntionNotSupported() 
      : MepException("unsupported") 
   {}
};

enum FunctionId {
   Identity,  // f(x) = +x
   Negate,    // f(x) = -x
   Abs,
   Sin, Cos, Tan,
   Asin, Acos, Atan,
   Exp, Log, Log10
};

FunctionId MEP_EXPORTS mep_lookup_function(const std::string& fname);
std::string MEP_EXPORTS mep_function_name(const FunctionId& fname);

} // ns