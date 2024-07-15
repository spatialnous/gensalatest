# SPDX-FileCopyrightText: 2017-2019 Christian Sailer
# SPDX-FileCopyrightText: 2017-2024 Petros Koutsolampros
#
# SPDX-License-Identifier: GPL-3.0-or-later

import os.path
import cmdlinewrapper
import difflib
import pprint

import runhelpers


class DepthmapRunner():
    def __init__(self, runFunc, binary ):
        self.__runFunc = runFunc
        self.__binary = binary
        
    def runDepthmap(self, cmdWrapper, runDir, extraArgs = []):
        args = [self.__binary]
        args.extend(cmdWrapper.toCmdArray())
        args.extend(extraArgs);
        return self.__runFunc(runDir, args)
        
    def getVersion(self, runDir):
        if (os.path.isfile(os.path.join(runDir, self.__binary))):
            return runhelpers.runExecutable(runDir, [self.__binary, "-v"])[1]
        else:
            return ""
        
    def getHelpText(self, runDir):
        if (os.path.isfile(os.path.join(runDir, self.__binary))):
            return runhelpers.runExecutable(runDir, [self.__binary, "-h"])[1]
        else:
            return ""
        
    def canIgnoreDisplayData(self, runDir):
        return "-idd" in self.getHelpText(runDir)
        
    def canMimicVersion(self, runDir):
        return "-mmv" in self.getHelpText(runDir)

def diffBinaryFiles(file1, file2):
    with open(file1, "rb") as f:
        content1 = f.read()
    with open(file2, "rb") as f:
        content2 = f.read()
    gen = difflib.diff_bytes(difflib.unified_diff, [content1], [content2])
    return not(list(gen))

class DepthmapRegressionRunner():
    def __init__(self, runFunc, baseBinary, testBinary, workingDir):
        self.__baseRunner = DepthmapRunner(runFunc, baseBinary)
        self.__testRunner = DepthmapRunner(runFunc, testBinary)
        self.__workingDir = workingDir
        self.__extraArgsSet = False
        self.__extraTestArgs = []

    def makeBaseDir(self, name):
        return os.path.join(self.__workingDir, name + "_base")

    def makeTestDir(self, name):
        return os.path.join(self.__workingDir, name + "_test")

    def runTestCase(self, name, cmds):
        runhelpers.prepareDirectory(self.makeBaseDir(name))
        runhelpers.prepareDirectory(self.makeTestDir(name))
        return self.runTestCaseImpl(name, cmds)

    def defExtraArgs(self, rundir):
        self.__baseCanIDD = self.__baseRunner.canIgnoreDisplayData(rundir)
        self.__testCanIDD = self.__testRunner.canIgnoreDisplayData(rundir)
        self.__extraBaseArgs = []
        self.__extraTestArgs = []
        if self.__baseCanIDD and self.__testCanIDD:
            self.__extraBaseArgs = ["-idd"]
            self.__extraTestArgs = ["-idd"]
            print("Both test and base binaries can drop display data. Dropping");
        elif self.__testRunner.canMimicVersion(rundir):
            self.__baseVersion = self.__baseRunner.getVersion(rundir).strip()
            self.__extraTestArgs = ["-mmv", self.__baseVersion]
            print("Test binary can mimic older versions. Mimicking base's \"" + self.__baseVersion + "\"");
            
    def runTestCaseImpl(self, name, cmds):
        baseDir = self.makeBaseDir(name)
        if not self.__extraArgsSet:
            self.defExtraArgs(baseDir)
            self.__extraArgsSet = True
        for step,cmd in enumerate(cmds):
            (baseSuccess, baseOut) = self.__baseRunner.runDepthmap(cmd, baseDir)
            if not baseSuccess:
                print("Baseline run failed at step " + str(step) + " with arguments " + pprint.pformat(cmd.toCmdArray()))
                print(baseOut)
                return (False, "Baseline run failed at step: " + str(step))

        testDir = self.makeTestDir(name)
        for step,cmd in enumerate(cmds):
            (testSuccess, testOut) = self.__testRunner.runDepthmap(cmd, testDir, self.__extraTestArgs)
            if not testSuccess:
                print("Test run failed at step " + str(step) + " with arguments " + pprint.pformat(cmd.toCmdArray()))
                print(testOut)
                return (False, "Test run failed at step: " + str(step))

        baseFile = os.path.join(baseDir, cmds[-1].outfile)
        testFile = os.path.join(testDir, cmds[-1].outfile)
        if not os.path.exists(baseFile):
            message = "Baseline output {0} does not exist".format(baseFile)
            print (message)
            return (False, message)
        if not os.path.exists(testFile):
            message = "Test output {0} does not exist".format(testFile)
            print(message)
            return (False, message)
        
        if not diffBinaryFiles(baseFile, testFile):
            message = "Test outputs differ"
            print (message)
            return (False, message)

        return (True, "")


