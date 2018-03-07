
#include "profiler.h"

#ifdef USE_PROFILING
#include <dollar.hpp>
#include <Remotery.h>
#include <fstream>
using namespace std;
void dump_profiling_info()
{
//    std::ofstream ofs("/tmp/trevi_profiling.json");
//    dollar::text(std::cout);                      // report stats to std::cout in text format; see also csv(), tsv() and markdown()
//    dollar::chrome(ofs); // write tracing results to a json file (for chrome://tracing embedded profiler)
//    dollar::clear();
//    ofs.close();
}
#else
void dump_profiling_info()
{

}
#endif
