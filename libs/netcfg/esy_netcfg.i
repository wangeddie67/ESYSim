
%module py_esy_netcfg
%{
/* Includes the header in the wrapper code */
#include "esy_netcfg.h"
%}

/* Include STL/C++ Library */
%include "std_vector.i"

/* Parse the header file to generate wrappers */
%include "esy_netcfg_port.h"
%include "esy_netcfg_link.h"
%include "esy_netcfg.h"
