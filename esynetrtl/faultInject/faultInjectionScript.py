#!/usr/bin/env python3
import os
import re
import sys
import random
from optparse import OptionParser
from builtins import print
from docutils.nodes import line
from pyverilog.vparser.parser import parse
import pyverilog.vparser.ast as vast
from pyverilog.ast_code_generator.codegen import ASTCodeGenerator

# Parameters
INFO = "Info: This program takes a to gate-level transformed verilog file " \
    "of a module, splits every wire inside of the verilog code and " \
    "inserts/implants a FaultInjectionUnit(also gate level transformed)) " \
    "between every wire pair. "\
    "It won't work with defines or macros in the Code!"
VERSION = "Version: 0.9"
HELP = "\nFor additional information enter: FunctionName -h"

# Global Variables
maxArgs = 3
verbose = False
MAKE_PORTS_FAULTY = False
ADD_FAULTY_PORT_SIGNALS = False
CREATE_SIM_GATE_LIBRARY = True


# Code Classes
class CodeModule:
    """Kind of struct to store gate data of target file"""

    def __init__(self, type, name, ports, wires):
        self.type = type
        # e.g. top-module, XOR2X1_HVT, WIRES(ofTopModule)
        self.name = name
        # e.g. U18, decoder, n14 - just the name given in the target code
        self.ports = ports
        # due to not knowing if a port is an input or an output of a module,
        # this variable is used for storing them + adding the arg
        # self.outputs = outputs  # e.g. a12[2:0], v, b2, y1
        # self.inputs = inputs
        # same as outputs e.g. a12[2:0], v, b2, y1, w2[3]
        self.wires = wires
        # same as outputs e.g. a12[2:0], v, b2, y1
        # - empty if module is not topmodule


class codeComponents:
    """Container for CodeModules"""

    def __init__(self):
        self.moduleList = []

    # Add function
    def addComponent(self, type, name, ports, wires):
        self.moduleList.append(CodeModule(type, name, ports, wires))
        return 0

    # Returns slot on a certain index
    def getModuleOnIndex(self, index):
        return self.moduleList[index]

    def getModuleNameOnIndex(self, index):
        """Returns name of module in on a certain index"""
        return self.moduleList[index].name

    def getModuleTypeOnIndex(self, index):
        """Returns type of module in on a certain index"""
        return self.moduleList[index].type

    # Returns list of indices, which contains the name
    # Search function
    def getListIndex_es_WithThisName(self, name):
        indexList = []
        for i in range(0, len(self.moduleList)):
            if self.moduleList[i].name is name:
                indexList.append(i)

        if len(indexList) is not 0:
            return indexList
        else:
            print("IndexList empty")
            return [0, 0, 0]

    def getModuleListLength(self):
        return len(self.moduleList)

        # def rmModuleFromList(self, index):
        # remove function


# Code Functions
def getTargetComponentList(targetFile, include, define):
    # Takes verilog file name, splits it to use it for the top module name
    def getTargetFileName(verilogFile):
        name = verilogFile.split('.')

        # Additional Information for Systemverilog Files
        if 'sv' in name[1]:
            print(
                "This program may not work correct if the target file is a "
                "Systemverilog file (recognized by ending)!")

        return name[0]

    # Preperations for pyverilog parsing
    tmp_fileList = []
    tmp_fileList.append(targetFile)
    targetFileName = getTargetFileName(targetFile)

    ast, directives = parse(
        tmp_fileList, preprocess_include=include, preprocess_define=define)

    if verbose:
        ast.show()

    # Code container, which will be filled with all the necessary data for
    # the faulty code
    componentData = codeComponents()

    topModuleInputs = []
    topModuleOutputs = []
    topModuleWires = []

    # AST Data Mining Loop
    for i in range(0, ast.description.definitions[0].items.__len__()):

        # "Decl" part stands for the top module, so it contains inputs,
        # outputs and wires of the top module
        if "Decl" in str(ast.description.definitions[0].items[i]):
            # If inputs are found...
            for j in range(0,
                           ast.description.definitions[0].items[i].list.__len__()):

                if "Input" in str(ast.description.definitions[0].items[i].list):
                    inputName = ast.description.definitions[0].items[i].list[j].name
                    inputPortDirection = "Input"

                    # To catch if input is a vector
                    if ast.description.definitions[0].items[i].list[j].width is not None:
                        inputWidthMSB = ast.description.definitions[0].items[i].list[j].width.msb.value
                        inputWidthLSB = ast.description.definitions[0].items[i].list[j].width.lsb.value
                        if verbose:
                            print(
                                "Decl: TargetModule-Input: " + inputName + "[" + inputWidthMSB + ":" + inputWidthLSB + "]")
                        topModuleInputs.append((inputPortDirection, '!top-port',
                                                inputName + "[" + inputWidthMSB + ":" + inputWidthLSB + "]", "vector"))
                    else:
                        if verbose:
                            print("Decl: TargetModule-Input: " + inputName)
                        topModuleInputs.append((inputPortDirection, '!top-port', inputName, "singular"))

                # If outputs are found...
                elif "Output" in str(ast.description.definitions[0].items[i].list):
                    outputName = ast.description.definitions[0].items[i].list[j].name
                    outputPortDirection = "Output"

                    # To catch if output is a vector
                    if ast.description.definitions[0].items[i].list[j].width is not None:
                        outputWidthMSB = ast.description.definitions[0].items[i].list[j].width.msb.value
                        outputWidthLSB = ast.description.definitions[0].items[i].list[j].width.lsb.value
                        if verbose:
                            print(
                                "Decl: TargetModule-Output: " + outputName + "[" + outputWidthMSB + ":" + outputWidthLSB + "]")
                        topModuleOutputs.append((outputPortDirection, '!top-port',
                                                 outputName + "[" + outputWidthMSB + ":" + outputWidthLSB + "]", "vector"))
                    else:
                        if verbose:
                            print("Decl: TargetModule-Input: " + outputName)
                        topModuleOutputs.append((outputPortDirection, '!top-port', outputName, "singular"))

                # If wires are found
                elif ("Wire" or "WireArray") in str(ast.description.definitions[0].items[i].list):

                    wireName = ast.description.definitions[0].items[i].list[j].name

                    # To catch if wire is a vector
                    if ast.description.definitions[0].items[i].list[j].width is not None:
                        wireWidthMSB = ast.description.definitions[0].items[i].list[j].width.msb.value
                        wireWidthLSB = ast.description.definitions[0].items[i].list[j].width.lsb.value
                        topModuleWires.append((wireName + "[" + wireWidthMSB + ":" + wireWidthLSB + "]", "vector"))
                    else:
                        topModuleWires.append((wireName, "singular"))

                else:
                    print("Unknown pyverilog ast decl object: %s" % (str(ast.description.definitions[0].items[i])))
                    sys.exit(2)

            # If the next list position is not a Decl the data gets stored in the codeComponent-object
            # This is necessary or else the "last" data part would be lost
            if "InstanceList" in str(ast.description.definitions[0].items[(i + 1)]):
                componentData.addComponent((str(ast.description.definitions[0].name), "topmodule"), targetFileName,
                                           topModuleOutputs + topModuleInputs, topModuleWires)

        # The "InstanceList" contains all the declared modules of the top module
        elif "InstanceList" in str(ast.description.definitions[0].items[i]):
            if verbose:
                print("InstanceList")
            moduleType = (ast.description.definitions[0].items[i].instances[0].module, "gate")
            moduleName = ast.description.definitions[0].items[i].instances[0].name
            modulePorts = []

            if verbose:
                print("Instance: Type: %s, Name: %s" % (moduleType, moduleName))

            for m in range(0, ast.description.definitions[0].items[i].instances[0].portlist.__len__()):
                modulePortName = ast.description.definitions[0].items[i].instances[0].portlist[m].portname

                # If it is a pointer port, special preperations are needed to save it adequate
                if "Pointer" in str(ast.description.definitions[0].items[i].instances[0].portlist[m].argname):
                    modulePortArgName = ast.description.definitions[0].items[i].instances[0].portlist[
                        m].argname.var.name
                    modulePortArgPointer = ast.description.definitions[0].items[i].instances[0].portlist[
                        m].argname.ptr.value
                    modulePortArg = modulePortArgName + "[" + modulePortArgPointer + "]"
                    portType = "pointer"

                # If it is a normal port...
                else:
                    modulePortArg = ast.description.definitions[0].items[i].instances[0].portlist[m].argname.name
                    portType = "singular"

                modulePorts.append((None, modulePortName, modulePortArg, portType))

            componentData.addComponent(moduleType, moduleName, modulePorts, None)

        else:
            print("getTargetComponentList: Unknown pyverilog ast object: %s" % (
                str(ast.description.definitions[0].items[i])))
            # sys.exit(2)

    return componentData


def getReferenceComponentList(targetModuleComponents, gateLibraryFile):
    # This function will store all the used gates in a file to be able to
    # find out about the direction of the ports of the gates

    """Searching for gate names and returns them as a list"""

    def getGateNames(targetModuleComponents):

        foundGates = []

        for i in range(0, targetModuleComponents.getModuleListLength()):
            if verbose:
                print(targetModuleComponents.getModuleTypeOnIndex(i))
            if (targetModuleComponents.getModuleTypeOnIndex(i)[0] not in foundGates) and (
                        targetModuleComponents.getModuleTypeOnIndex(i)[1] != "topmodule"):
                foundGates.append(targetModuleComponents.getModuleTypeOnIndex(i)[0])

        return foundGates

    # Checking which gates are needed from the gateLibraryFile/which gates appear in the targetFile
    relevantGates = getGateNames(targetModuleComponents)

    def getMinimumGateCode(gate, gateLibraryFile):
        codeLines = []
        workFile = open(gateLibraryFile, 'r')

        continueRead = False
        skipLines = False
        whiteList = ('module', 'endmodule', 'input', 'output')

        for num, line in enumerate(workFile, 1):
            if ("module " + gate) in line:
                if verbose:
                    print(gate)
                    print('found at line:', num)
                continueRead = True

            # Ignore specify/endspecify in the library file
            if continueRead:
                if any(s in line for s in whiteList):
                    skipLines = False
                else:
                    skipLines = True

            if continueRead and not skipLines:
                codeLines.append(line)

            if "endmodule" in line:
                continueRead = False

        workFile.close()

        return codeLines

    def getGateCode(gate, gateLibraryFile):
        codeLines = []
        workFile = open(gateLibraryFile, 'r')

        continueRead = False

        for num, line in enumerate(workFile, 1):
            if ("module " + gate) in line:
                if verbose:
                    print(gate)
                    print('found at line:', num)
                continueRead = True

            if continueRead:
                codeLines.append(line)

            if "endmodule" in line:
                continueRead = False

        workFile.close()

        return codeLines

    # Temporary file to store date, will be deleted afterwards
    tmp_fileName = "___tmp_" + str(random.randint(a=0, b=999))
    tmp_gateFile = open(tmp_fileName, "w+")
    global CREATE_SIM_GATE_LIBRARY

    # Additional file creation for gates
    if CREATE_SIM_GATE_LIBRARY:
        simGateLibraryName = targetModuleComponents.moduleList[0].type[0] + '_simGateLibrary.v'   # Hardcoded topModule name
        simGateLibraryFile = open(simGateLibraryName, 'w+')


    # This loop writes the used gates into file which gets parsed by pyverilog
    for gatename in relevantGates:
        if verbose:
            print(gatename)
        gateMinimumCode = getMinimumGateCode(gatename, gateLibraryFile)
        if CREATE_SIM_GATE_LIBRARY:
            simGateCode = getGateCode(gatename, gateLibraryFile)

        for line in range(0, len(gateMinimumCode)):
            if verbose:
                print(gateMinimumCode[line])
            tmp_gateFile.write(gateMinimumCode[line])

        tmp_gateFile.write("\n")

        # Writing additional gate code file
        if CREATE_SIM_GATE_LIBRARY:
            for line in range(0, len(simGateCode)):
                if verbose:
                    print(simGateCode[line])
                simGateLibraryFile.write(simGateCode[line])
            simGateLibraryFile.write('\n')


    tmp_gateFile.close()

    # Closing additional gate code file
    if CREATE_SIM_GATE_LIBRARY:
        simGateLibraryFile.close()

    fileList = [tmp_fileName]
    # Parsing by Pyverilog
    ast, directives = parse(fileList)
    # Deleting tmp file
    os.remove(tmp_fileName)

    # Creating a new object, to store gate data
    libraryData = codeComponents()

    # Transfering gate data into new object libraryData
    for i in range(0, ast.description.definitions.__len__()):
        if "ModuleDef" in str(ast.description.definitions[i]):
            gateType = (ast.description.definitions[i].name, 'gate')
            gatePorts = []

            for j in range(0, ast.description.definitions[i].items.__len__()):
                for k in range(0, ast.description.definitions[i].items[j].list.__len__()):
                    if "Output" in str(ast.description.definitions[i].items[j].list[k]):
                        gatePortDirection = "Output"
                        gatePortName = ast.description.definitions[i].items[j].list[k].name
                        gatePortArg = None
                    elif "Input" in str(ast.description.definitions[i].items[j].list[k]):
                        gatePortDirection = "Input"
                        gatePortName = ast.description.definitions[i].items[j].list[k].name
                        gatePortArg = None
                    else:
                        print("Unknown port configuration found in gate library: ",
                              str(str(ast.description.definitions[i].items[j].list[k])))
                        sys.exit(3)

                    gatePorts.append((gatePortDirection, gatePortName, gatePortArg))

            libraryData.addComponent(gateType, "Gate", gatePorts, None)

    # Transfering Data Back
    return libraryData


def getPortDirection(targetComponents, gateComponents):
    # This functions writes the port direction of the gates into our component
    # file, with the help of the previous function, which provided the gates

    def getGateData(type, gateComponents):
        #  Searches for the wanted gate and returns it
        i = 0
        foundGate = False

        # will/should always break while loop, won't use True/False variable
        while not foundGate:

            if (type == gateComponents.moduleList[i].type[0]):
                break

            i += 1

        return gateComponents.moduleList[i]

    # Loops through the gates of the target File
    for i in range(1, targetComponents.getModuleListLength()):  # Start by 1 due to skipping the topModule in the list
        # print(targetComponents.moduleList[i].type)
        gateData = getGateData(targetComponents.moduleList[i].type[0], gateComponents)

        # Adds the port direction data by
        for j in range(0, targetComponents.moduleList[i].ports.__len__()):
            # print(targetComponents.moduleList[i].ports[j])
            # print(targetComponents.moduleList[i].ports[j][1])
            # print([p[1] for p in gateData.ports].index(targetComponents.moduleList[i].ports[j][1]))
            portIndex = [p[1] for p in gateData.ports].index(targetComponents.moduleList[i].ports[j][1])
            # print(gateData.ports[portIndex][0])
            # print(targetComponents.moduleList[i].ports[j][0])

            # The Tuple Data gets reconstructed with the new information about the port direction
            tmp_tuple = (gateData.ports[portIndex][0], targetComponents.moduleList[i].ports[j][1],
                         targetComponents.moduleList[i].ports[j][2], targetComponents.moduleList[i].ports[j][3])
            # print(tmp_tuple)
            targetComponents.moduleList[i].ports[j] = tmp_tuple

    return targetComponents


def getChangedWireNames(workModule):
    # This function will generate the necessary wire list for the final faulty
    # file, by splitting every wire into an a-part and b-part

    # Attention: the ingoing Postfix is used for wires which go into a module,
    # the other way round with the outgoing postfix
    # Careful this postfixes are confusing
    outgoingPostfix = '_a'
    ingoingPostfix = '_b'
    topModulePortList = []
    topModuleWireList = []
    global MAKE_PORTS_FAULTY
    global ADD_FAULTY_PORT_SIGNALS

    def setNewWireName(workTuple, wirePostfix):
        # This function adds the defined postfix to the port plus it handles a propable appearing pointer, but won't handle a vector
        # Todo Make it work for vectors [3:2], [7:0],...

        if "[" in workTuple[2]:
            tmpName = workTuple[2].split('[')
            # print(tmpName)
            newName = tmpName[0] + wirePostfix + "[" + tmpName[1]  # Feels like ugly splitting but it works
        else:
            newName = workTuple[2] + wirePostfix

        return (workTuple[0], workTuple[1], newName, workTuple[3])

    # Adding the inputs and outputs of the topmodule to the faulty wire list only if MAKE_PORTS_FAULTY is set true, the names of the module inputs and outputs stay the same but the wires, which will enter/leave the fault modules are changed
    # This as well creates a list of a port original inputs/outputs
    for l in range(0, workModule.moduleList[0].ports.__len__()):
        if "Input" in workModule.moduleList[0].ports[l][0]:
            # print("Input")
            if 'singular' in workModule.moduleList[0].ports[l][3]:
                topModulePortList.append(workModule.moduleList[0].ports[l][2])
            elif 'vector' in workModule.moduleList[0].ports[l][3]:
                prepVector = getPreparedVector(workModule.moduleList[0].ports[l][2])
                topModulePortList.append(prepVector[0])
            if ADD_FAULTY_PORT_SIGNALS:
                topModuleWireList.append((setNewWireName(workModule.moduleList[0].ports[l], ingoingPostfix)[2:4]))
        elif "Output" in workModule.moduleList[0].ports[l][0]:
            # print("Output")
            if 'singular' in workModule.moduleList[0].ports[l][3]:
                topModulePortList.append(workModule.moduleList[0].ports[l][2])
            elif 'vector' in workModule.moduleList[0].ports[l][3]:
                prepVector = getPreparedVector(workModule.moduleList[0].ports[l][2])
                topModulePortList.append(prepVector[0])
            if ADD_FAULTY_PORT_SIGNALS:
                topModuleWireList.append((setNewWireName(workModule.moduleList[0].ports[l], outgoingPostfix)[2:4]))
        else:
            print("Problem during port splitting!")
            sys.exit(4)

    # Add clock for fault modules
    workModule.moduleList[0].ports.append(('Input', '!top-port', "faultClock", 'singular'))

    # Changing the wires of the topmodule
    for k in range(0, workModule.moduleList[0].wires.__len__()):
        wireName = workModule.moduleList[0].wires[k][0]

        if 'singular' in workModule.moduleList[0].wires[k][1]:
            topModuleWireList.append(((wireName + outgoingPostfix), workModule.moduleList[0].wires[k][1]))
            topModuleWireList.append(((wireName + ingoingPostfix), workModule.moduleList[0].wires[k][1]))
        elif 'vector' in workModule.moduleList[0].wires[k][1]:
            prepVector = getPreparedVector(workModule.moduleList[0].wires[k][0])
            topModuleWireList.append((prepVector[0] + outgoingPostfix + '[' +prepVector[1] + ':' + prepVector[2] + ']', workModule.moduleList[0].wires[k][1]))
            topModuleWireList.append((prepVector[0] + ingoingPostfix + '[' +prepVector[1] + ':' + prepVector[2] + ']', workModule.moduleList[0].wires[k][1]))
        else:
            print("Problem during wire splitting!")
            sys.exit(4)


    workModule.moduleList[0].wires = topModuleWireList

    def checkForAllowance(candidate, whitelist):
        # Depending on the MAKE_PORTS_FAULTY variable it allows to change the top module wires
        if 'singular' in candidate[3]:
            if candidate[2] in whitelist:
                if not MAKE_PORTS_FAULTY:
                    return False
        elif 'pointer' in candidate[3]:
            prepPointer = getPreparedPointer(candidate[2])
            # print(prepPointer)
            if prepPointer[0] in whitelist:
                if not MAKE_PORTS_FAULTY:
                    return False

        return True

    # Changing the wire names for the gates into input -> b and output -> a
    for i in range(1, workModule.moduleList.__len__()):  # starting by 1 to skip the topmodule
        for j in range(0, workModule.moduleList[i].ports.__len__()):
            if "Input" in workModule.moduleList[i].ports[j][0]:
                # print("Input")
                if checkForAllowance(workModule.moduleList[i].ports[j], topModulePortList):
                    workModule.moduleList[i].ports[j] = setNewWireName(workModule.moduleList[i].ports[j],
                                                                       ingoingPostfix)
            elif "Output" in workModule.moduleList[i].ports[j][0]:
                # print("Output")
                if checkForAllowance(workModule.moduleList[i].ports[j], topModulePortList):
                    workModule.moduleList[i].ports[j] = setNewWireName(workModule.moduleList[i].ports[j],
                                                                       outgoingPostfix)
            else:
                print("Problem during wire splitting!")
                sys.exit(4)

    return workModule


def getPreparedPointer(pointerString):
    if ':' in pointerString:
        # Splits the pointer to extract the MSB and LSB - shouldn't be possible in our code because we don't have vector wires/gates with vector inputs - everything is singular
        splittedString = re.split('\[|:|\]', pointerString)
        #  (Name, MSB, LSB) of vector
        preparedPointer = (splittedString[0], splittedString[1], splittedString[0])
    else:
        splittedString = re.split('\[|\]', pointerString)
        #  (Name, Pointer) of vector
        preparedPointer = (splittedString[0], splittedString[1])

    return preparedPointer


def getPreparedVector(vectorString):
    # Splits the vector to extract the MSB and LSB
    splittedString = re.split('\[|:|\]', vectorString)
    #  (Name, MSB, LSB) of vector
    preparedVector = (splittedString[0], splittedString[1], splittedString[2])

    return preparedVector


def createFaultyOutputFile(preparedModules, faultModuleFile):
    # This file creates the faulty output file
    # If this code is used as reference for pyverilog, be very cautious about
    # the exact use of it and a good debugger is very useful
    # Todo I still have struggle with the fault module injection, so if vectors
    # appear or similar things it could become diffcult -> it fails
    print("Start creating faulty verilog file!")

    # Contains all the verilog components of this module
    portMembers = []
    itemMembersPortInput = []
    itemMembersPortInputVector = []
    itemMembersPortOutput = []
    itemMembersPortOutputVector = []
    itemMembersWire = []
    itemMembersWireVector = []
    itemMembersFaultModule = []
    itemMembersGate = []
    itemMembers = []
    emptyList = []

    # The following code create transforms all the modifies parts into
    # pyverilog understandable code
    for element in range(0, preparedModules.getModuleListLength()):

        if 'topmodule' in preparedModules.moduleList[element].type[1]:
            if verbose:
                print("Found Topmodule!")
            outputName = preparedModules.moduleList[element].type[0]

            # Handling the Ports
            for port in range(0, preparedModules.moduleList[element].ports.__len__()):
                if "Input" in preparedModules.moduleList[element].ports[port][0]:
                    # print("Input")
                    if 'singular' in preparedModules.moduleList[element].ports[port][3]:
                        itemMembersPortInput.append(vast.Input(name=preparedModules.moduleList[element].ports[port][2], width=None, signed=False))
                        portMembers.append(vast.Port(name=preparedModules.moduleList[element].ports[port][2], type=None, width=None))
                    elif 'vector' in preparedModules.moduleList[element].ports[port][3]:
                        # For initialization
                        prepVector = getPreparedVector(preparedModules.moduleList[element].ports[port][2])
                        width = vast.Width(msb=vast.IntConst(prepVector[1]), lsb=vast.IntConst(prepVector[2]))
                        itemMembersPortInputVector.append(vast.Decl([vast.Input(name=prepVector[0], width=width)]))
                        # For port definition
                        portMembers.append(vast.Port(name=prepVector[0], type=None, width=None))
                    else:
                        print("createFaultyOutputFile - topmodule - ports: Unknown port type found!")

                elif "Output" in preparedModules.moduleList[element].ports[port][0]:
                    # print("Output")
                    if 'singular' in preparedModules.moduleList[element].ports[port][3]:
                        portMembers.append(vast.Port(name=preparedModules.moduleList[element].ports[port][2], type=None, width=None))
                        itemMembersPortOutput.append(vast.Output(name=preparedModules.moduleList[element].ports[port][2], width=None, signed=False))

                    elif 'vector' in preparedModules.moduleList[element].ports[port][3]:
                        # For initialization
                        prepVector = getPreparedVector(preparedModules.moduleList[element].ports[port][2])
                        width = vast.Width(msb=vast.IntConst(prepVector[1]), lsb=vast.IntConst(prepVector[2]))
                        itemMembersPortOutputVector.append(vast.Decl([vast.Output(name=prepVector[0], width=width)]))
                        # For port definition
                        portMembers.append(vast.Port(name=prepVector[0], type=None, width=None))

                    else:
                        print("createFaultyOutputFile - topmodule - ports: Unknown port type found!")
                else:
                    print("createFaultyOutputFile - topmodule - ports: Unknown port found!")
                    sys.exit(8)

            # Handling the wires
            for wire in range(0, preparedModules.moduleList[element].wires.__len__()):
                if 'singular' in preparedModules.moduleList[element].wires[wire][1]:
                    wire = vast.Wire(name=preparedModules.moduleList[element].wires[wire][0])
                    itemMembersWire.append(wire)

                # TODO this is kind of dangerous, because it seems that there is pointer, wirearray, wire and the use is always similar
                elif 'vector' in preparedModules.moduleList[element].wires[wire][1]:
                    prepVector = getPreparedVector(preparedModules.moduleList[element].wires[wire][0])
                    # length = vast.Length(msb=vast.IntConst(prepVector[1]), lsb=vast.IntConst(prepVector[2]))
                    # wireArray = vast.WireArray(name=prepVector[0], length=length, width=None)  # WireArray can also consume width but this seems distinct from the IO ports not to be used for the vector width
                    width = vast.Width(msb=vast.IntConst(prepVector[1]), lsb=vast.IntConst(prepVector[2]))
                    wire = vast.Wire(name=prepVector[0], width=width)
                    itemMembersWireVector.append(vast.Decl(list=[wire]))
                else:
                    print("createFaultyOutputFile - topmodule - wires: Unknown wire found!")
                    sys.exit(8)


        # Relevant for the Gates
        elif 'gate' in preparedModules.moduleList[element].type[1]:
            moduleName = preparedModules.moduleList[element].name
            moduleType = preparedModules.moduleList[element].type[0]
            modulePortList = []

            # Port Handling
            for port in range(0, preparedModules.moduleList[element].ports.__len__()):
                if 'singular' in preparedModules.moduleList[element].ports[port][3]:
                    argName = vast.Identifier(name=preparedModules.moduleList[element].ports[port][2])
                    portArg = vast.PortArg(argname=argName, portname=preparedModules.moduleList[element].ports[port][1])
                    modulePortList.append(portArg)
                elif 'pointer' in preparedModules.moduleList[element].ports[port][3]:
                    prepPointer = getPreparedPointer(preparedModules.moduleList[element].ports[port][2])
                    argName = vast.Pointer(var=vast.Identifier(prepPointer[0]), ptr=vast.IntConst(prepPointer[1]))
                    portArg = vast.PortArg(argname=argName, portname=preparedModules.moduleList[element].ports[port][1])
                    modulePortList.append(portArg)
                else:
                    print("createFaultyOutputFile - gates - ports: Unknown port type found!")
                    sys.exit(9)

            # Instance Handling
            moduleInstance = vast.Instance(module=moduleType, name=moduleName, portlist=modulePortList,
                                           parameterlist=emptyList, array=None)

            moduleInstanceList = vast.InstanceList(module=moduleType, instances=[moduleInstance],
                                                   parameterlist=emptyList)

            itemMembersGate.append(moduleInstanceList)

        else:
            print("createFaultyOutputFile: Error unknown module-type appeared")
            sys.exit(7)

    # Adding the fault modules
    # Getting the name from the faultModule File, which is an argument of this script
    print(
        "Attention, only the name of the faultModule-file is used, anything else is hardcoded in the script (amount of IOs and so on)")
    print(
        "We expect, that you use Questa/Modelsim for simulation, so it makes no difference, if the fault module is a Verilog or Systemverilog file")

    print(
        "The inputs of the topmodule won't be connected to fault modules, the necessity of the function needs to be evaluated")

    faultModuleName = (((faultModuleFile.split('/'))[-1]).split('.'))[0]
    print('Faultmodule name: %s' % faultModuleName)

    global MAKE_PORTS_FAULTY
    if MAKE_PORTS_FAULTY:
        print("The function to involve the ports of the top module has to be evaluated and if necessary implemented!")

    # Implementing Fault Modules - this is hardcoded, must be modified if a different kind of fault modules is used
    # TODO a code block which handles the inport outport so that there is a clean cut between the ports and the wires

    # Additional values for the module parameters
    moduleID = ("MODULEID", "")
    faultParametersByScript = False
    prob_c2f = ("PROB_C2F", "32'd42949672")
    prob_f2c = ("PROB_F2C", "32'd3865470565")
    seed = ("SEED", "32'd1234")
    random.seed(a=None, version=2)
    randomSeed = True

    for i in range(0, preparedModules.moduleList[0].wires.__len__()):


        # Wire Preperation
        if 'singular' in preparedModules.moduleList[0].wires[i][1]:


            wireName = preparedModules.moduleList[0].wires[i][0]
            if "_a" == wireName[(len(wireName) - 2):]:
                faultPortList = []
                # Module
                wirePair = (
                    preparedModules.moduleList[0].wires[i][0],
                    preparedModules.moduleList[0].wires[i + 1][0])
                moduleName = wirePair[0][:(len(wirePair[0]) - 2)] + '_faulty'
                # Clock
                argName = vast.Identifier(name='faultClock')
                portArg = vast.PortArg(argname=argName, portname='CLK')
                faultPortList.append(portArg)
                # Inport
                argName = vast.Identifier(name=wirePair[0])
                portArg = vast.PortArg(argname=argName, portname='in')
                faultPortList.append(portArg)
                # Outport
                argName = vast.Identifier(name=wirePair[1])
                portArg = vast.PortArg(argname=argName, portname='out')
                faultPortList.append(portArg)

                # Parameter Handling - If we want to add certain parameters to the file, these lines need to be modified
                parameterList = []
                moduleID = (moduleID[0], wirePair[0][:(len(wirePair[0]) - 2)])
                parameterList.append(vast.ParamArg(argname=vast.StringConst(moduleID[1]), paramname=moduleID[0]))

                # Condition query if the testbench shall set the parameters (so no global defines are used
                if faultParametersByScript:
                    parameterList.append(vast.ParamArg(argname=vast.IntConst(prob_c2f[1]), paramname=prob_c2f[0]))
                    parameterList.append(vast.ParamArg(argname=vast.IntConst(prob_f2c[1]), paramname=prob_f2c[0]))

                # Randomizing seed
                if randomSeed:
                    seed = ("SEED", "32'd" + str(random.randint(a=1, b=10000)))
                parameterList.append(vast.ParamArg(argname=vast.IntConst(seed[1]), paramname=seed[0]))

                # Instance Handling
                moduleInstance = vast.Instance(module=faultModuleName, name=moduleName, portlist=faultPortList,
                                               parameterlist=emptyList, array=None)

                moduleInstanceList = vast.InstanceList(module=faultModuleName, instances=[moduleInstance],
                                                       parameterlist=parameterList)

                itemMembersFaultModule.append(moduleInstanceList)  # FaultModule initialization


        elif 'vector' in preparedModules.moduleList[0].wires[i][1]:
            # Preparing the vector
            prepVector = getPreparedVector(preparedModules.moduleList[0].wires[i][0])
            wireName_a =  prepVector[0]
            vectorMSB = prepVector[1]
            vectorLSB = prepVector[2]
            # print(preparedModules.moduleList[0].wires[i][0])

            if "_a" == wireName_a[(len(wireName_a) - 2):]:
                # Must be after if else it would possible take a wrong wire
                wireName_b = getPreparedVector(preparedModules.moduleList[0].wires[i + 1][0])[0]

                for j in range(int(vectorLSB), int(vectorMSB) + 1):         # +1 that the last bit is also included

                    faultPortList = []
                    # Module
                    wirePair = (wireName_a , wireName_b)
                    moduleName = wirePair[0][:(len(wireName_a[0]) - 2)] + str(j) + 'b_faulty'
                    # Clock
                    argName = vast.Identifier(name='faultClock')
                    portArg = vast.PortArg(argname=argName, portname='CLK')
                    faultPortList.append(portArg)
                    # Inport
                    argName = vast.Pointer(var=vast.Identifier(wireName_a), ptr=vast.IntConst(j))
                    portArg = vast.PortArg(argname=argName, portname='in')
                    faultPortList.append(portArg)
                    # Outport
                    argName = vast.Pointer(var=vast.Identifier(wireName_b), ptr=vast.IntConst(j))
                    portArg = vast.PortArg(argname=argName, portname='out')
                    faultPortList.append(portArg)

                    # Parameter Handling - If we want to add certain parameters to the file, these lines need to be modified
                    parameterList = []
                    moduleID =  (moduleID[0], wirePair[0][:(len(wirePair[0]) - 2)] + '_' + str(j))
                    parameterList.append(vast.ParamArg(argname=vast.StringConst(moduleID[1]), paramname=moduleID[0]))

                    # Condition query if the testbench shall set the parameters (so no global defines are used)
                    if faultParametersByScript:
                        parameterList.append(vast.ParamArg(argname=vast.IntConst(prob_c2f[1]), paramname=prob_c2f[0]))
                        parameterList.append(vast.ParamArg(argname=vast.IntConst(prob_f2c[1]), paramname=prob_f2c[0]))

                    # Randomizing seed
                    if randomSeed:
                        seed = ("SEED", "32'd" + str(random.randint(a=1, b=10000)))

                    parameterList.append(vast.ParamArg(argname=vast.IntConst(seed[1]), paramname=seed[0]))

                    # Instance Handling
                    moduleInstance = vast.Instance(module=faultModuleName, name=moduleName, portlist=faultPortList,
                                                   parameterlist=emptyList, array=None)

                    moduleInstanceList = vast.InstanceList(module=faultModuleName, instances=[moduleInstance],
                                                           parameterlist=parameterList)

                    itemMembersFaultModule.append(moduleInstanceList)  # FaultModule initialization
        else:
            print("Implement fault modules: Error appeared during instantiation in wire section!")
            sys.exit(9)

    # Last preperations steps
    # Ports
    itemMembers.extend(itemMembersPortInputVector)
    # itemMembers.extend(itemMembersPortInput)                      # In this way for every signal is a new line used
    itemMembers.append(vast.Decl(list=itemMembersPortInput))        # In this way it is more "bulk" like

    itemMembers.extend(itemMembersPortOutputVector)
    # itemMembers.extend(itemMembersPortOutput)
    itemMembers.append(vast.Decl(list=itemMembersPortOutput))
    # Wires
    itemMembers.extend(itemMembersWireVector)
    #itemMembers.extend(itemMembersWire)
    itemMembers.append(vast.Decl(list=itemMembersWire))
    # Module Declaration
    itemMembers.extend(itemMembersGate)
    itemMembers.extend(itemMembersFaultModule)


    # Wire Arrays

    # Actual code generation
    portList = vast.Portlist(portMembers)
    ast = vast.ModuleDef(name=outputName, paramlist=None, portlist=portList, items=itemMembers)

    codegen = ASTCodeGenerator()
    result = codegen.visit(ast)

    if(verbose):
        print(result)
    # Writing to the code to output file
    outputFile = open((outputName + ".v"), 'w')
    outputFile.write(result)
    outputFile.close()
    print("Finished writing process!")

    return 0


def main():
    errorCode = 1

    def showVersion():
        print(INFO)
        print(VERSION)
        print(HELP)
        sys.exit()

    def showUsage():
        print('================= Usage =================')
        print('Usage: FunctionName [Options] '
              '<Gate-Data-File> <fault-module-file> <target-File>')
        print('\nArguments:')
        print('\t<Gate-Data-File>'
              '\t: Contains all the gates appearing in the files to find out '
              'which port is input/ouput!')
        print('\t<Fault-Module-File>'
              '\t: This module gets placed between every wire of the target '
              'file!')
        print('\t<Target-Module-File>'
              '\t: This module gets modified with the fault modules and '
              'should be transformed to gate level')
        print('\tAttention \t\t: All files are expected to be on gate-level '
              'and formated like they are provided from the ICT!')
        print('\ne.g.: FunctionName -v file_X_gate_level.sv '
              'file_Y_gate_level.v')

    # Declare additional command line options
    usage = "usage: %prog [options] "\
        "<Gate-Library-File> <Fault-Module-File> <Target-Module-File>"
    optparser = OptionParser(usage=usage)
    optparser.add_option("--version", action="store_true",
                         dest="showVersion", default=False,
                         help="show the version")
    optparser.add_option("-v", "--verbose", action="store_true",
                         dest="verbose", default=False,
                         help="activates additional text output")
    optparser.add_option("-u", "--usage", action="store_true",
                         dest="showUsage", default=False,
                         help="additional information for usage")
    optparser.add_option("-I", "--include", action="append",
                         dest="parserInclude", default=[],
                         help="include path for pyverilog parser")
    optparser.add_option("-D", "--define", action="append",
                         dest="parserDefine", default=[],
                         help="Macro Definition for pyverilog parser")
    # Parse options
    (options, args) = optparser.parse_args()

    # Start of program
    if options.showVersion:
        showVersion()

    if options.showUsage:
        showUsage()
        sys.exit(0)

    # Program start
    filelist = args
    if (len(filelist) < maxArgs):
        print("Error: Too few arguments (args == %i)!" % maxArgs)
        sys.exit(errorCode)
    elif (len(filelist) > maxArgs):
        print("Error: Too many arguments (args == %i)!" % maxArgs)
        sys.exit(errorCode)
    else:
        print(filelist)
        for f in filelist:
            if not os.path.exists(f):
                raise IOError("File not found: " + f)

        # First arg is gate-file
        gateLibraryFile = args[0]

        # Second arg is fault module
        faultModuleFile = args[1]

        # Third arg is target module
        targetModuleFile = args[2]

        if len(filelist) == 0:
            showVersion()

    # Getting the modules of the targetFile as a object list
    targetModuleComponents = getTargetComponentList(
        targetModuleFile, options.parserInclude, options.parserDefine)

    # Filtering the modules, depending on the used gates in the target code
    gateLibraryComponents = getReferenceComponentList(
        targetModuleComponents, gateLibraryFile)

    # Resolve if ports are in-/outputs
    targetModuleFinished = getPortDirection(
        targetModuleComponents, gateLibraryComponents)

    # Splitting the wires into a and b pieces (inbetween will come the fault
    # module) and also renames the module ports
    preparedModules = getChangedWireNames(targetModuleFinished)

    # Adds faulty to top module name to make changes visible and for naming
    # the output file
    preparedModules.moduleList[0].type = (
        preparedModules.moduleList[0].type[0], 'topmodule')
    createFaultyOutputFile(preparedModules, faultModuleFile)

    print("Program successful finished! Exit now!")

    sys.exit(0)


"""
TODO 7, this code works with singular and pointer wires/ports but
i won't correclty work with vectors
# TODO is necessary to detect signed or not? NO
# TODO data mining must handle all input variations? NO
# TODO there won't be any instance of a module which has 0 ports...i think
# TODO reduce redundancy of code, there a passages where the code is testing
the third time for the same problem
"""

if __name__ == '__main__':
    main()
