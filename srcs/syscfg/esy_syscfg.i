
%module esy_syscfg
%{
/* Includes the header in the wrapper code */
#include "esy_netcfg.h"
%}

/* Include STL/C++ Library */
%include "std_vector.i"
namespace std {
   %template(LongVector) vector<long>;
   %template(EsyNetCfgLinkVector) vector<EsyNetCfgLink>;
}

/* Parse the header file to generate wrappers */
%include "esy_netcfgport.h"
%include "esy_netcfglink.h"
%include "esy_netcfg.h"
