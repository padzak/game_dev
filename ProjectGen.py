import os
import platform
import sys

buildEnvArg = sys.argv[1]

premakePath = os.path.dirname(os.path.abspath(__file__)) + "/vendor/bin/premake/"

platformSystem = platform.system();
osCall = ""

if platformSystem == "Windows":
	osCall = premakePath + platformSystem + "/premake5.exe"

if platformSystem == "Darwin":
	osCall =  premakePath + platformSystem + "/premake5"
	os.system("chmod 777 " + osCall )
	
if platformSystem == "Linux":
	osCall = premakePath + platformSystem + "/premake5"
	os.system("chmod 777 " + osCall )

osCall = osCall + " " + buildEnvArg
os.system(osCall)
