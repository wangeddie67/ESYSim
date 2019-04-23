#!/usr/bin/python

import sys
import math

def fileCheck(filePath):
	try:
		open(filePath, 'r')
		return 1
	except IOError:
		print("ERROR@InputChecking: File \"%s\" does not appear to exist." % filePath)
		return 0

def transformFile(inputFile):
	# Reference list
	refListEvent = ['logtime', 'logevent', 'src', 'src_pc', 'src_vc', 'dst', 'dst_pc', 'dst_vc', 'flitID', 'flitsize', 'flittype', 'flitsrc/oldstate', 'flitdst/newstate', 'flitflag', 'flitStartTime']
	refListFlit = ['flit_id', 'flit_size', 'flit_type', 'flit_src', 'flit_dst', 'flit_flag', 'flit_starttime']
	refListFlitDataInject = ['flit_id', 'flit_size', 'flit_src', 'flit_dst', 'flit_flag', 'flit_starttime']

	# Counting variables
	counter_processedlines = 0
	counter_datapackets = 0
	counter_logevents = 0
	counter_stateevents = 0
	counter_invalidstateevents = 0
	counter_scriptlogevents = 0

	clkperiod = 2
	print("Clock period = " + str(clkperiod) + " ns\n")
	workFile = open(inputFile, 'r')

	# Initializing the list for the ids, which are numbers displayed by a 12 bit vector
	id_container = []
	id_list = [[] for _ in range(2**12)]

	print("Starting Packet IDs import process.")
	# Collecting Event Loop
	for line in workFile:
		# print(lineCounting)

		# Split the line and remove the '#'
		currentLine = line.split()[0:]

		# Skip empty lines
		if (len(currentLine) == 0):
			continue

		# Store PacketData in array
		if(currentLine[0] == 'PacketInjectLog:'):
			counter_datapackets = counter_datapackets + 1
			# Changing the value from time to cycle
			currentLine[6] = str(math.trunc(int(currentLine[6])/clkperiod))
			# Add the data to the corresponding id
			id_list[int(currentLine[1])].append(currentLine[1:])

	print("\nFinished Packet ID imported process.")


	# Reseting read pointer
	workFile.seek(0)
	# Creating output list
	tmp_list = []
	# Active ID list - points at the momentan used variant of the ID data
	id_active_list = [0 for _ in range(2**12)]

	print("\nStarting Data modification process")
	# Adding ID Loop
	for line in workFile:
		counter_processedlines = counter_processedlines + 1

		# Split the line and remove the '#'
		currentLine = line.split()[0:]

		# Skip empty lines
		if (len(currentLine) == 0):
			continue

		# Skip PacketInject messages
		if(currentLine[0] == "PacketInjectLog:"):
			continue

		if (currentLine[0] == "PacketEjectLog:"):
			# Incrementing the active packet id pointer
			id_active_list[int(currentLine[2])] = id_active_list[int(currentLine[2])] + 1
			continue



		if(currentLine[0] == 'NoCLog:'):
			counter_logevents = counter_logevents + 1
			# Expecting that the processing of the PacketData has already happened
			currentLine[1] = str(math.trunc(int(currentLine[1])/clkperiod))

			# Adding the id data to the message, by choosing the corresponding id plus the right now active packet id
			currentLine.extend(id_list[int(currentLine[10])][id_active_list[int(currentLine[10])]])
			currentLine.pop(10)
			tmp_list.append(currentLine[1:])

		elif(currentLine[0] == 'StateLog:'):
			counter_stateevents = counter_stateevents + 1
			# Expecting that the PacketDataInject has already happened
			if currentLine[11] is 'x':
				counter_invalidstateevents = counter_invalidstateevents + 1
				continue
			else:
				currentLine[1] = str(math.trunc(int(currentLine[1])/clkperiod))

				# Extend StateLog with fake id date so that the further changes are stil possible
				currentLine.extend(['0', '0', '0', '0', '0', '0'])

				# Put state changes to right position
				currentLine[15] = currentLine[11]
				currentLine[16] = currentLine[12]

				# Popping irrelevant parts
				currentLine.pop(12)
				currentLine.pop(11)
				currentLine.pop(10)
				tmp_list.append(currentLine[1:])

		else:
			counter_scriptlogevents = counter_scriptlogevents + 1
			print("# SCRIPTLOG: Unknown sequence found: \"" + (' '.join(currentLine[0:])) + "\"")

	workFile.close()

	print("\nFinished data modifcation process")

	print("\nStart creating output file")
	# Write Part
	# -4 to erase .txt file ending
	outputFilename = inputFile[:len(inputFile)-4] + '_modified.eventt'
	workfile = open(outputFilename, 'w+')
	print("\nOutputfilename: " + outputFilename)

	# junkaizhan add
	workfile.write('0'+'                             '+'\n')

	for element in range(0, len(tmp_list)):
		# Change order of type, id, size
		flittype, flitid, flitsize = tmp_list[element][8], tmp_list[element][9], tmp_list[element][10]
		# Change flit encoding to VisualNOC style (10)
		if flittype == '2':
			# VisualNOC HeadFlit
			flittype = '0'
		elif flittype == '3':
			# VisualNOC BodyFlit
			flittype = '1'
		elif flittype == '1':
			# VisualNOC TailFlit
			flittype = '2'
		elif flittype == '0':
			# Dummy elif to keep 0 values 0
			flittype = '0'
		else:
			print(flittype)
			print("ERROR: Unknown FlitType found!")
			sys.exit(1)

		# Put everything back together in the correct order
		tmp_list[element][8], tmp_list[element][9], tmp_list[element][10] = flitid, flitsize, flittype

		# Change to VisualNoC states
		def changestate2VN(state):
			if(state == '1'):
				return '0'
			elif(state == '2'):
				return '1'
			elif(state == '3'):
				return '4'
			else:
				print("Error in changestate2V: " + state)
				sys.exit(1)

		if tmp_list[element][1] == "40":
			# print(tmp_list[element])
			tmp_list[element][11] = str(changestate2VN(tmp_list[element][11]))
			tmp_list[element][12] = str(changestate2VN(tmp_list[element][12]))
			# print(tmp_list[element])

		# Write to file
		# if(tmp_list[element][8] == '239'):
		# if(tmp_list[element][8] == '239' and tmp_list[element][1] == '20' ):
		#	 print(tmp_list[element])
		workfile.write('\t'.join(tmp_list[element]) + '\n')
	
	# junkaizhan add
	workfile.flush()
	workfile.seek(0)
	workfile.write(str(tmp_list[element][0]))
	workfile.flush()

	print("\nFinished writing output file")

	print('\n')
	print('Stats:')
	print('Lines of PacketData events: ' + str(counter_datapackets))
	print('Lines of log events: ' + str(counter_logevents))
	print('Lines of state events: ' + str(counter_stateevents) + ' (' + str(counter_invalidstateevents) + ' of them invalid)')
	print('Lines of scriptlog events: ' + str(counter_scriptlogevents))
	print('Sum of events: ' + str(counter_scriptlogevents + counter_datapackets + counter_logevents + counter_stateevents))
	print('\n')
	print('Length of input file: ' + str(counter_processedlines))
	print('Length of output file: ' + str(len(tmp_list)))
	print('Sum of log and state events minus the invalid states: ' + str(counter_logevents + counter_stateevents - counter_invalidstateevents))


def main(argv):
	print('\n')
	print('\t ++++++++++++++++++++++++++++++++++++++++')
	print('\t |		Script started		|')
	print('\t ++++++++++++++++++++++++++++++++++++++++')
	print('\n')
	print('\n')
	if(len(argv) is 0):
		print('\nThis programs purpose is to modify the Questasim logfile in that way that VisualNoC can us it.')
		print('No file for further processing found!')
		sys.exit()
	else:
		testfile = fileCheck(argv[0])
	if(testfile):
		print("File \""+ argv[0] +" \"gets processed...\n")
	else:
		print('No file for further processing found!')
		sys.exit()
	transformFile(argv[0])
	print('\n')
	print('\t ++++++++++++++++++++++++++++++')
	print('\t | Script successful finished |')
	print('\t ++++++++++++++++++++++++++++++')
	sys.exit(0)


if __name__ == "__main__":
	main(sys.argv[1:])
