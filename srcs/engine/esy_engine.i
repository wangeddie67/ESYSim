
%module esy_engine

%{
/* Includes the header in the wrapper code */
#include "esy_event_queue.h"
%}

/* Include STL/C++ Library */
%include "std_shared_ptr.i"
%shared_ptr( EsyEvent )
%include "std_multiset.i"
namespace std {
    %template(EventSet) multiset<shared_ptr<EsyEvent> >;
}

%rename(esy_eventptr_less) operator<(const EsyEventPtr &, const EsyEventPtr & );

/* Parse the header file to generate wrappers */
%include "esy_event_queue.h"

