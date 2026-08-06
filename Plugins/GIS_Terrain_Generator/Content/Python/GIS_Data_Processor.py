# import init_unreal
import sys
import os
from pathlib import Path

current_folder = Path(__file__).parent
lib_folder = os.path.join(current_folder, 'lib')
sys.path.insert(0, lib_folder)


import rasterio
import math
import numpy as np
import tempfile
import shutil

from PIL import Image
from collections import namedtuple
from rasterio.windows import Window as Window


# Creating "Structs" to keep data passed coherent and simple
Segment = namedtuple('Segment', ['width', 'height', 'subdivisions'])
Stats = namedtuple('Stats',['min', 'max'])
Shape = namedtuple('Shape', ['rows', 'cols'])

output_dir_high_res = os.path.join(Path(__file__).parents[1], "raster_segments_high")
output_dir_low_res = os.path.join(Path(__file__).parents[1], "raster_segments_low")


def outputFolders():
    """
    Create output directories, clearing any existing contents first
    """
    if os.path.exists(output_dir_high_res):
        shutil.rmtree(output_dir_high_res)
    os.makedirs(output_dir_high_res, exist_ok=True)

    if os.path.exists(output_dir_low_res):
        shutil.rmtree(output_dir_low_res)
    os.makedirs(output_dir_low_res, exist_ok=True)



def getWindowSize(totalWidth, totalHeight):
    """
    Gets window size for segmentation and data parsing

    This logic likely needs to be reworked, I threw this together really fast
    Likely just add padding to the edges if the geoTIFF is not even
    """
    windowHeight = totalHeight
    windowWidth  = totalWidth

    maxSize = 4096

    widthSubs = math.ceil(totalWidth / maxSize)
    heightSubs = math.ceil(totalHeight / maxSize)

    subdivisions = max(widthSubs, heightSubs)

    # Starts with 2 subdisvions by default
    subdivisions = max(2, subdivisions)

    windowHeight = max(256, round(totalWidth / subdivisions))
    windowWidth  = max(256, round(totalHeight / subdivisions))

    return Segment(windowWidth, windowHeight, subdivisions)



def getMinMax(data):
    """
    Finds the min and max values in data set 
    """
    # Read data statistics once
    globalStats = data.stats(indexes=1)[0]
    globalMin = globalStats.min
    globalMax = globalStats.max
    del globalStats
    return Stats(globalMin, globalMax)



def processData(data, windowInfo, globalStats):
    """
    Processes the data and creates raster segments
    Outputs the segments to folder for them to be later stitched
    together in order to create larger image within unreal
    """
    # noramalizing into 16bit
    rangeDiff = globalStats.max-globalStats.min

    total_cols = math.ceil(data.width / windowInfo.width)

    for i, rowOff in enumerate(range(0, data.height, windowInfo.height)):
        for j, colOff in enumerate(range(0, data.width, windowInfo.width)):

            # Find first windows width
            # Find first windows height 
            wWidth  = min(windowInfo.width, data.width - colOff)
            wHeight = min(windowInfo.height, data.height - rowOff)

            # Create first window of data
            window = Window(colOff, rowOff, wWidth, wHeight)
            
            # Extract first window from data set, this is uncompressed 
            wData = data.read(1, window=window)

            # Clipping data to prevent weird outliers
            clippedData = np.clip(wData, globalStats.min, globalStats.max)

            if rangeDiff == 0:
                rangeDiff = 1

            normalize = ((clippedData - globalStats.min) / rangeDiff * np.iinfo(np.uint16).max)
            normalize16Bit = np.asarray(normalize, dtype=np.uint16)

            # Saving output attempt
            img = Image.fromarray(normalize16Bit)

            linear_index = i * total_cols + j
            
            filenameHigh = os.path.join(output_dir_high_res, f"{linear_index}.png")
            img.save(filenameHigh, dpi=(600, 600))


            filenameLow = os.path.join(output_dir_low_res, f"{linear_index}.png")
            lowImg = img.resize((256, 256), Image.Resampling.BILINEAR)
            lowImg.save(filenameLow)



def generateRaster(file_path):
    # Open file and load data into memory
    # Also closes
    with rasterio.open(file_path) as data:
        
        # Creates window size for segmenting
        windowInfo = getWindowSize(data.width, data.height)

        # Get dataset min max
        globalStats = getMinMax(data)
        processData(data, windowInfo, globalStats)



def lowResolutionPreview(file_path):
    """
    Create a low resolution preview of GeoTIFF
    """

    with rasterio.open(file_path) as src:
        data = src.read(1, out_shape=(512, 512))

    data = data.astype(np.float32)
    data_min, data_max = np.nanmin(data), np.nanmax(data)
    normalized = ((data - data_min) / (data_max - data_min) * 255).astype(np.uint8)

    img = Image.fromarray(normalized)
    img.thumbnail((512, 512))

    if img.mode != "L":
        img = img.convert("L")

    tmp = tempfile.NamedTemporaryFile(suffix=".jpg", delete=False)
    img.save(tmp, format="JPEG", quality=70)
    tmp.close()

    return tmp.name



def main(file_path):
    """
    Loads, process, and outputs GeoTIFF raster

    attempted multi threading and subprocess, neither worked. 
    will be moving over to unreal for this most likely
    """
    outputFolders()
    generateRaster(file_path)



if __name__ == "__main__":
    main(sys.argv[1])
    
