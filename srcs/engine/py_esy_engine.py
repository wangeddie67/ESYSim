#
# File name : py_esy_engine.py
# Function : Python wrapper of simulation engine.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor,
# Boston, MA  02110-1301, USA.
#
# Copyright (C) 2017, Junshi Wang <wangeddie67@gmail.com>

##
# @ingroup ESY_SIM_ENGINE
# @package engine.py_esy_engine
# Python wrapper of simulation engine.

##
# @ingroup ESY_SIM_ENGINE
# @file py_esy_engine.py
# @brief Python wrapper of simulation engine.

from typing import List
import argparse
from esy_engine import EsyEvent, \
                       EsyEventQueue

##
# @ingroup ESY_SIM_ENGINE
# @brief Add options for simulation engine
# @param[in,out] parser Argument parser entity
def addOptions( parser : argparse.ArgumentParser ) :
    argument_group = parser.add_argument_group(
        description='Simulation Engine' )

    argument_group.add_argument( '--sim_length',
        help='Simulation length',
        type=int,
        default=10000 )

##
# @ingroup ESY_SIM_ENGINE
# @brief Build simulation engine
# @param[in] args Argument namespace
# @return Generated event queue entity
def buildEngine( args ) :
    event_queue = EsyEventQueue( args.sim_length )
    return event_queue

