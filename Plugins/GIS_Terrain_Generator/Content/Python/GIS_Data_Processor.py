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
import io
import tempfile
import time

from PIL import Image
from collections import namedtuple
from rasterio.windows import Window as Window


# Creating "Structs" to keep data passed coherent and simple
Segment = namedtuple('Segment', ['width', 'height', 'subdivisions'])
Stats = namedtuple('Stats',['min', 'max'])
Shape = namedtuple('Shape', ['rows', 'cols'])

output_dir = os.path.join(Path(__file__).parents[1], "raster_segments")


def outPutFolders():
    """
    Create output directories
    """
    if not os.path.exists(output_dir):
        os.makedirs(output_dir, exist_ok=True)



def getWindowSize(totalWidth, totalHeight):
    """
    Gets window size for segmentation and data parsing

    This logic likely needs to be reworked, I threw this together really fast
    Likely just add padding to the edges if the geoTIFF is not even
    """
    windowHeight = totalHeight
    windowWidth  = totalWidth

    # Starts with 2 subdisvions by default
    subdivisions = 2

    # Find optimal bounds for data parsing
    while windowWidth > 4096 or windowHeight > 4096:
        # 256 acts as a minimum size for png output
        windowWidth  = round(totalWidth/subdivisions) if round(totalWidth/subdivisions) > 256 else 256
        windowHeight = round(totalHeight/subdivisions) if round(totalHeight/subdivisions) > 256 else 256
        subdivisions+=1

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



def processData(data, totalRowsAndCols, windowInfo, globalStats):
    """
    Processes the data and creates raster segments
    Outputs the segments to folder for them to be later stitched
    together in order to create larger image within unreal
    """
    # noramalizing into 16bit
    rangeDiff = globalStats.max-globalStats.min

    for i, rowOff in enumerate(range(0, data.height, windowInfo.height)):
        for j, colOff in enumerate(range(0, data.width, windowInfo.width)):

            # current = i * totalRowsAndCols.cols + j + 1
            # total = totalRowsAndCols.cols*totalRowsAndCols.rows
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
            filename = os.path.join(output_dir, f"segment_row_{i}_col_{j}.png")
            img = Image.fromarray(normalize16Bit)
            img.save(filename, dpi=(600, 600))



def generateRaster(file_path):
    # Open file and load data into memory
    # Also closes
    with rasterio.open(file_path) as data:
        
        # Creates window size for segmenting
        windowInfo = getWindowSize(data.width, data.height)

        # Total rows and columns
        totalRowsAndCols = Shape(math.ceil(data.width / windowInfo.width),
                                math.ceil(data.height / windowInfo.height))


        # Get dataset min max
        globalStats = getMinMax(data)
        processData(data, totalRowsAndCols, windowInfo, globalStats)



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
    outPutFolders()
    generateRaster(file_path)



if __name__ == "__main__":
    main(sys.argv[1])
    
