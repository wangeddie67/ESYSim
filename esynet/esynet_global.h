/*
This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor,
Boston, MA  02110-1301, USA.

---
Copyright (C) 2015, Junshi Wang <>
*/

#ifndef ESYNET_GLOBAL_H
#define ESYNET_GLOBAL_H

/* including library file */
#include <vector>

/* namespace */
using namespace std;

/*************************************************
  Name Space :
    esynet

  Description :
    global define of namespace

  Types :
    VCStateType : enum
    VCShareType : enum
    VCUsageType : enum
    
    double : double
    AddrType : vector< long >
    VCType   : pair< long, long >
    AtomType : unsigned long long
    DataType : vector< AtomType >
    
  Variables :
    VC_NULL : const VCType
    
  Macro define :
    BUFF_BOUND
    WIRE_DELAY
    PIPE_DELAY
    CREDIT_DELAY
    REPORT_PERIOD
    S_ELPS
    ATOM_WIDTH
    ZERO
    MAX_64
    CORR_EFF
    POWER_NOM
*************************************************/
namespace esynet
{
/* VC state machenism */
enum VCStateType 
{
	VCS_INIT,       /* initialization */
	VCS_ROUTING,    /* wait for routing determine */
	VCS_VC_AB,      /* wait for virtual channel arbitration */
	VCS_SW_AB,      /* wait for switch arbitration */
	VCS_SW_TR,      /* transport a paciet through switch */
};
/* VC state type */
enum VCShareType
{
	VC_SHARE,
	VC_MONO
};
/* VC usage state */
enum VCUsageType
{
	VC_USED,    /* assigned */
	VC_FREE     /* free */
};

enum RoutingAlg
{
	RA_XY,
	RA_TXY,
	RA_DYXY,
	RA_FTLR,
	RA_HIPFAR,
	RA_CORERESCUER,
	RA_ERESCUER,
	RA_ALTERTEST,
	RA_NONBLOCK,
    RA_FREESLOT,
};

enum TrafficProfile
{
	TP_UNIFORM,
	TP_TRANSPOSE1,
	TP_TRANSPOSE2,
	TP_BITREVERSAL,
	TP_BUTTERFLY,
	TP_SHUFFLE,
};

enum ECCMethod
{
	ECC_HM74,
	ECC_HM128,
	ECC_HM2116,
	ECC_HM3832,
};

enum ECCStrategy
{
	ECC_H2H,
	ECC_E2E,
	ECC_COUNT,
	ECC_CROSS,
	ECC_SLOPE,
	ECC_SQUARE,
};

enum BISTMethod
{
	BIST_TARRA,
	BIST_BLOCKING,
	BIST_NONBLOCK,
    BIST_FREESLOT,
};

/* build-in self-test state */
enum BISTRouterState {
	BIST_ROUTER_NORMAL,	/* normal state */
	BIST_ROUTER_EMPTY,	/* empty state */
	BIST_ROUTER_TESTING,	/* testing state */
	BIST_ROUTER_RECOVER,	/* recover state */
	BIST_ROUTER_FREE,	/* free slot state */
	BIST_ROUTER_BLOCK,	/* block state */
};

enum BISTPortState {
	BIST_PORT_NORMAL,
	BIST_PORT_EMPTY,
	BIST_PORT_FREE,
	BIST_PORT_BLOCK
};

/* Type define */
typedef vector< long > AddrType;
typedef pair< long, long > VCType;
typedef unsigned long long AtomType;
typedef vector< AtomType > DataType;
#define VC_NULL VCType(-1, -1)

/* Macro define */
#define BUFF_BOUND_ 100
#define WIRE_DELAY_ 0.9
#define PIPE_DELAY_ 1.0
#define CREDIT_DELAY_ 1.0
#define REPORT_PERIOD_ 2000
#define S_ELPS_ 0.00000001
#define ATOM_WIDTH_ 64    //bus width for power model 
#define ZERO_ 0
#define MAX_64_ 0xffffffffffffffffLL
#define CORR_EFF_ 0.8
#define POWER_NOM_ 1

/*
#define BUFF_BOUND_ 100
#define WIRE_DELAY_ 2.0	 	0.9
#define PIPE_DELAY_ 1.0		1.0
#define CREDIT_DELAY_ 2.0 	1.0
#define REPORT_PERIOD_ 2000
#define S_ELPS_ 0.00000001
#define ATOM_WIDTH_ 64
#define ZERO_ 0
#define MAX_64_ 0xffffffffffffffffLL
#define CORR_EFF_ 0.8
#define POWER_NOM_ 1e9
*/
}

#endif
