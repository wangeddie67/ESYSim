#!/usr/bin/python3

import sys
sys.path.append( '../../build' )

import trafficgen

file_name = '../test.trace'

trafficgen.dumpTrafficTrace( file_name )

