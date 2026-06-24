import init_unreal
import subprocess
import sys
import os
from pathlib import Path

main_file_dir = os.path.join(Path(__file__).parent, "GIS_Data_Processor.py")

"""
    This file only exists because calling GIS_Data_Processor caused unreal to freeze up
    and creating a subprocess in c++ was more compilcated

    May change in the future, I dont like having another 
"""

def main(file_path):
    subprocess.Popen(["python", main_file_dir, file_path])