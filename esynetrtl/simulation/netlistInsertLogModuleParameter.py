#!/usr/bin/python
import re

netlist = open('/home/junkaizhan/workspace/ftnoc/temp/mapped/m_Assembled_Router.v', 'r')
target  = open('/home/junkaizhan/workspace/ftnoc/temp/mapped/m_Assembled_Router_test.v', 'w')
count = 0;
for line in netlist:
	if line.find('m_LogModule') >= 0 :
		print('>>>>>>>>>>>>>>>> Find m_LogModule', count)
		newline = "m_LogModule #(.P_LOCAL_CHANNEL(" + str(count) + "))"
		line = line.replace( "m_LogModule", newline )
		count = count + 1;
	target.write(line)

print('>>>>>>>>>>>>>>>>>>>>  Finish script  <<<<<<<<<<<<<<<<<<<<')

target.flush()
netlist.close()
target.close()
