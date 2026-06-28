from pathlib import Path
import sys
import os
from pip._internal.cli.main import main as pip_main

# Folder paths
current_folder = Path(__file__).parent
lib_folder = os.path.join(current_folder, 'lib')
cache_folder = os.path.join(current_folder, 'pip_cache')
root_folder = Path(__file__).resolve().parents[4]
gis_folder = os.path.join(root_folder, 'Content\\GIS\\GeoTIFF')
image_folder = os.path.join(root_folder, 'Content\\GIS\\Images')

# Packages that will be used by parser
PACKAGES = ["numpy", "rasterio", "pillow", "unreal"]

# Create library folder and install libraries
if not (os.path.exists(lib_folder)) or not os.listdir(lib_folder):
    os.makedirs(lib_folder, exist_ok=True)
    pip_main(["install", *PACKAGES, "--target", lib_folder, "--cache-dir", cache_folder])
    print(f"{PACKAGES} have been installed")

if not(os.path.exists(gis_folder)):
    os.makedirs(gis_folder)

if not(os.path.exists(image_folder)):
    os.makedirs(image_folder)

# Add libraries to python path so all other files can use them
sys.path.insert(0, lib_folder)