#pragma once

#include <string>
#include <iostream>
#include <exception>

#ifdef BUILD_MEP_AS_DLL
# define MEP_EXPORTS __declspec(dllexport)
#else
# define MEP_EXPORTS
#endif




namespace mep {

using MepException = std::runtime_error;

//----------------------------------------------------------------------------

struct MemTracker {
   static int nb_created;
   static int nb_destroyed;

   MemTracker() {
      ++nb_created; 
   }
   MemTracker(const MemTracker&) {
      ++nb_created;
   }
protected:
   ~MemTracker() { ++nb_destroyed; }
public:
   static void MEP_EXPORTS show_mem_stats();
};

} // ns
