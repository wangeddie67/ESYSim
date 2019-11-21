/*
 * File name : esy_packetgen.i
 * Function : SWIG interface file for packet generator.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 *
 * Copyright (C) 2017, Junshi Wang <wangeddie67@gmail.com>
 */

%module esy_packetgen

%{
/* Includes the header in the wrapper code */
#include "esy_packetgenerator.h"
%}

/* Include STL/C++ Library */
%include "stdint.i"
%include "std_string.i"
%include "std_shared_ptr.i"
%shared_ptr( EsyNetworkPacket )
%include "std_vector.i"
namespace std {
    %template(LongVector) vector<long>;
    %template(PacketVector) vector<shared_ptr<EsyNetworkPacket> >;
}

/* Parse the header file to generate wrappers */
%include "esy_packetgenerator_fun.h"
%include "esy_packetgenerator.h"

