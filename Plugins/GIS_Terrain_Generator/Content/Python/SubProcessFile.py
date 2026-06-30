import subprocess
import sys
import os
from pathlib import Path

main_file_dir = os.path.join(Path(__file__).parent, "GIS_Data_Processor.py")
engine_python = (Path(sys.prefix) / "python.exe")

"""
    This file only exists because calling GIS_Data_Processor caused unreal to freeze up
    and creating a subprocess in c++ was more compilcated

    TO DO:
    Figure out a way to poll this
    possibly just make:

    def poll():
        if process.poll() is None:
            return false
        return true
    
    I feel like this would work but the way unrel has been treating python makes me think otherwise
    Since this would have to called from unreal c++, then theres the issue of if it hogs the main thread

"""

def main(file_path):
    process = subprocess.Popen([str(engine_python), main_file_dir, file_path],
                                creationflags=8)
    
    return process