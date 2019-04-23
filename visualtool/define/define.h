#ifndef DEFINE_H
#define DEFINE_H

#include "qt_include.h"

// pi
#define PI 3.141592654

// macro enumerate type
typedef long int MACROENUM;

// axis macro
#define AX_NUM	2
#define AX_X	0
#define AX_Y	1

// port direction macro
#define PD_LOCAL	0
#define PD_NORTH	0
#define PD_SOUTH	0
#define PD_EAST		0
#define PD_WEST		0

// network configuration macro
#define NC_NETWORK_SIZE		0
#define NC_PORT_NUMBER		1
#define NC_VC_CHANNEL		2

#define RT_NAME_COL     0
#define RT_NAME_STR     QString("Name")
#define RT_VALUE_COL    1
#define RT_VALUE_STR    QString("Value")

// argument tree column string and index
#define AT_FILENAME	"networkcfg.xml"
#define AT_HEAD		"argumentconfiguration"
#define AT_VERSION	"1.0"
#define AT_TITLE	"title"
#define AT_ITEM_STR	"argu"
#define AT_SEQ		0
#define AT_SEQ_STR	"Seq"
#define AT_NAME		1
#define AT_NAME_STR	"Name"
#define AT_VALUE	2
#define AT_VALUE_STR	"Value"
#define AT_MODIFY_STR	"Modify"
#define AT_MODIFY_YES	"1"
#define AT_MODIFY_NO	"0"
#define AT_NOTE		3
#define AT_NOTE_STR	"Note"
#define AT_CHILD_STR	"Child"
#define AT_CHILD_EMPTY	"0"
#define AT_TYPE_STR "Type"
#define AT_ENUM_STR "enum"
#define AT_NETWORK_STR   "NetVar"

#define AT_TYPE_ENUM_STR "Enum"
#define AT_TYPE_INT_STR "Int"
#define AT_TYPE_BOOL_STR    "Bool"

#define TC_FILENAME	"argucfg.xml"
#define TC_HEAD		"argumentconfiguration"
#define TC_VERSION_XML	"1.0"
#define TC_NAME     "name"
#define TC_VERSION  "version"
#define TC_TITLE	"title"
#define TC_EXEC		"exec"
#define TC_COMPILE  "compile"
#define TC_PARALLEL	"parallel"
#define TC_PARALLEL_DISABLE	"disable"
#define TC_PARALLEL_YES		"yes"
#define TC_PARALLEL_NO		"no"
#define TC_PARALLEL_CFG	"paracfg"
#define TC_PARALLEL_OUTARG	"paraoutarg"
#define TC_ITEM_STR	"argu"
#define TC_SEQ_STR  "Seq"
#define TC_ARGU		0
#define TC_ARGU_STR	"Argument"
#define TC_LOOP		1
#define TC_LOOP_STR	"Loop"
#define TC_VALUE	2
#define TC_VALUE_STR	"Value"
#define TC_NOTE		3
#define TC_NOTE_STR	"Note"
#define TC_TYPE_STR "Type"
#define TC_LOOP_VAR	"(loop variable)"
#define TC_DIR "Direction"
#define TC_FATHER "Father"

// animiation state
#define AS_START	"Start"
#define AS_STOP		"Stop"
#define AS_RUN		"Run"
#define AS_FINISH 	"Finish"

#define AS_TIMER	1000

#define VC_STATE	0
#define VC_BUFFER	1
#define VC_DIRCTION	2

#define ROUTER_FAULT	1
#define LINK_FAULT	2

#endif
