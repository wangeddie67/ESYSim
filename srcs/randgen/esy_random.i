
%module esy_random
%{
/* Includes the header in the wrapper code */
#include "esy_random.h"
%}

/* Parse the header file to generate wrappers */
%include "esy_random.h"

