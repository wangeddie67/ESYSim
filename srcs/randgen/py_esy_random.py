#
# File name : py_esy_syscfg.py
# Function : XML interface of system configuration structure.
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

from typing import List
import argparse

def addOptions( parser : argparse.ArgumentParser ) :
    argument_group = parser.add_argument_group(
        description='Random generator' )

    argument_group.add_argument( '--random_seed', '-s',
        help='Random seed',
        type=int,
        default=1 )

