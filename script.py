import subprocess
from os import system, name
from sys import argv
from re import search
import subprocess


def change_file(fileName, n_threads):
    with open(fileName) as inputFile:
        data = inputFile.readlines()
    for i in range(len(data)):
        if search('n', data[i]):
            data[i] = f"n_threads = {n_threads}\n"
            break
    with open(fileName, 'w') as outputFile:
        outputFile.writelines(data)


cmd = 'cmake -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -DCMAKE_BUILD_TYPE:STRING=Release -H./ -B./build -G "Unix Makefiles" && cmake --build ./build --config Release --target all'
if __name__ == "__main__":
    system(cmd)
    fileName = argv[1] if len(argv) > 1 else "example.conf"
    cmd = ('build\\intCalculator.exe' if name == 'nt' else './build/intCalculator') + \
        f" {fileName}"
    # system(cmd)
    abs_er = 0.01
    times = []
    res = []
    timed_res = 0
    N = argv[2] if len(argv) > 2 else 10
    for n_threads in range(1, 13):
        print(f"Testing for {n_threads} thread(s)")
        mini = float('inf')
        change_file(fileName, n_threads)
        local_res = []
        for trial in range(N):
            process = subprocess.Popen(
                cmd.strip().split(), stdout=subprocess.PIPE)
            while True:
                output = process.stdout.readline()
                if process.poll() is not None:
                    break
                if output:
                    # s = str(output.strip())
                    s = output.strip()
                    if b'mcs' in s:
                        timed_res = int(s.split()[-1][:-3])
                    if b'Result' in s:
                        local_res.append(float(s.split()[-1]))
                    print(s)
                    # try:
                    #     timed_res = int(search("(\d+)(?=mcs)", s).group(1))
                    # except:
                    #     pass
                    # if search('(?<=Result: ).*$', s) is not None:
                    #     tmp = (search('(?<=Result: ).*$', s))
                    #     # local_res.append(float(search('(?<=Result: ).*$', s)))
                    #     print("here")
                    #     pass
                    if mini > timed_res and timed_res>0:
                        mini = timed_res
                    # if s!=b'':
                    # print(s)
        for i in range(len(local_res)-1):
            if abs(local_res[i]-local_res[i+1]) > abs_er:
                print(
                    f"Incorrect result while calculating with {n_threads} threads.")
        res.append(local_res[0])
        times.append(mini)

    for i in range(len(res)-1):
        if abs(res[i]-res[i+1]) > abs_er:
            print(
                f"Results differ while calculating with {i+1} and {i+2} threads.")
    for i in range(len(times)):
        print(f'Minimal time for {i+1} thread(s) is {times[i]} mcs')
