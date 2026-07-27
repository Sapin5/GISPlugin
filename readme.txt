GIS Terrain Generator

A plugin for unreal engine 5 that can generate terrain based on a submitted GeoTIFF file

The plugin uses Unreals built in widget system called "Slate". 
The intent is for the plugin to do the following:

1. Allow user to select and preview a GeoTIFF file           <- DONE
  - The plugin will create a low resolution preview of the selected file
    to ensure its the correct file.  

2. Generate a tiled higher resolution map of the GeoTIFF preview map <- WORK IN PROGRESS
  - Upon confirming the tile selection the plugin will generate two variations of the map
    A High Resolution version (4K) and a low resolution version for the tiles
  - The tiled map should allow users to select an area. This will load up the linked 
    high resolution area to appear for the user to use

3. Allow users to highlight an area to snapshot and export   <- NOT STARTED
  - The higher resolution version will have some tools available to highlight, select,
    and export

4. Generate terrain from exported snapshot <- NOT STARTED
  - Hook into unreal engines terrain system and generate terrain. The tools for editing
   terrain should be available afterwards for users to manipulate the generated terrain


READ THE FOLLOWING PLEASE PLEASE PLEASE
=========================================================================================
So there is a bug, if you're on the tiled map screen and grab any of the corners of the 
screen and move them around to resize the window. There's a good chance the program 
will stall/freeze.

I am not entirely sure why it occurs, but I have done some digging and found a
potential cause

- Slate refreshes whenever window size is changed, so there could be a delay on slates
 update when the window refreshes. This causes something to reference a null pointer 
 which triggers a breakpoint. 

- After more days of digging, I came across some resources that said this is potentially
an unreal engine rendering issue. I have no idea how to mitigate this.


If you encounter this bug. There is a good chance your program will NOT launch next time
you try to build. 

Unreal will start the build, but it will hang and never complete. You can Stop Visual 
Studio by launching task manager and looking for msbuild and ending that task (There is
no other way to stop it without restarting the computer).

At this point something has corrupted, dont even bother trying to find the corrupted file

Do not try to launch again. Instead open the folder where the project is located.
Select the Binaries, Saved, .VS, .VSconfig, Intermediate, and .sln and delete them.
You will then have to select the .uproject, right click -> more options -> generate 
visual studio project files. This will regenerate the .sln and all relevant files.

You can launch the .sln once it is done and launch the project as normal.
It may take a bit longer to build. 
If it does not, delete the same folders, restart computer and regenerate files. 
=========================================================================================