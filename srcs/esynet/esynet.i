/*
 * File name : esy_engine.i
 * Function : SWIG interface file for simulation engine.
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

%module esynet

%{
/* Includes the header in the wrapper code */
#include "esynet_sim.h"
%}

/* Include STL/C++ Library */
%include "stdint.i"
%include "std_string.i"

/* Parse the header file to generate wrappers */
%include "esynet_sim.h"
