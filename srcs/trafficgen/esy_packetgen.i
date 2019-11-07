
%module esy_packetgen

%{
/* Includes the header in the wrapper code */
#include "esy_packetgenerator.h"
%}

/* Include STL/C++ Library */
%include "std_shared_ptr.i"
%shared_ptr( EsyNetworkPacket )
%include "std_vector.i"
namespace std {
    %template(LongVector) vector<long>;
    %template(PacketVector) vector<shared_ptr<EsyNetworkPacket> >;
}

/* Parse the header file to generate wrappers */
//%include "esy_traffic.pb.h"
%include "esy_packetgenerator_fun.h"
%include "esy_packetgenerator.h"

