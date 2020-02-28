import subprocess
from os import system
import re


cmd = "cmake -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -DCMAKE_BUILD_TYPE:STRING=Release -H./ -B./build -G \"Unix Makefiles\" && cmake --build ./build --config Release --target all"
if __name__ == "__main__":
    system(cmd)
    cmd="./build/intCalculator " 
    system(cmd)