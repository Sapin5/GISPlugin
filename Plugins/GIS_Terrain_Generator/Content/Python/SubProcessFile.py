import subprocess
import sys
import os
from pathlib import Path

main_file_dir = os.path.join(Path(__file__).parent, "GIS_Data_Processor.py")
engine_python = (Path(sys.prefix) / "python.exe")
process = None
test = None

"""
    This file only exists because calling GIS_Data_Processor caused unreal to freeze up
    and creating a subprocess in c++ was more compilcated

    TO DO:
    Figure out a way to poll this
"""

def main(file_path):
    global process, test
    print("hi")
    process = subprocess.Popen([str(engine_python), main_file_dir, file_path],
                                creationflags=8)
    
    return process

# This is not working. 
def poll():
    global process, test
    if process is None:
        return None
    if process.poll() is None:
        return False
    return True