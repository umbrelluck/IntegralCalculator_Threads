import subprocess
from os import system, name
import re


cmd = 'cmake -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -DCMAKE_BUILD_TYPE:STRING=Release -H./ -B./build -G "Unix Makefiles" && cmake --build ./build --config Release --target all'
if __name__ == "__main__":
    system(cmd)
    fileName = ""
    cmd = 'build\\intCalculator.exe' if name == 'nt' else './build/intCalculator' + \
        f" {fileName}"
    system(cmd)
