#include <mep.hpp>

#include <map>
#include <functional>
#include <math.h>
/*

*/


namespace mep {
int MemTracker::nb_created = 0;
int MemTracker::nb_destroyed = 0;

void MEP_EXPORTS MemTracker::show_mem_stats()
{
   std::cout << "Nb allocated = " << MemTracker::nb_created << std::endl;
   std::cout << "Nb freed = " << MemTracker::nb_destroyed << std::endl;
}

} // ns


//----------------------------------------------------------------------------

 
   