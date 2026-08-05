# GIS Terrain Generator



A plugin for unreal engine 5 that can generate terrain based on a submitted GeoTIFF file



The plugin uses Unreals built in widget system called "Slate". 

The intent is for the plugin to do the following:



1\. Allow user to select and preview a GeoTIFF file           <------------ DONE

&#x20; - The plugin will create a low resolution preview of the selected file

&#x20;   to ensure its the correct file.  



2\. Generate a tiled higher resolution map of the GeoTIFF preview map <---- WORK IN PROGRESS

&#x20; - Upon confirming the tile selection the plugin will generate two variations of the map

&#x20;   A High Resolution version (4K) and a low resolution version for the tiles

&#x20; - The tiled map should allow users to select an area. This will load up the linked 

&#x20;   high resolution area to appear for the user to use



3\. Allow users to highlight an area to snapshot and export   <----------- NOT STARTED

&#x20; - The higher resolution version will have some tools available to highlight, select,

&#x20;   and export



4\. Generate terrain from exported snapshot <----------------------------- NOT STARTED

&#x20; - Hook into unreal engines terrain system and generate terrain. The tools for editing

&#x20;  terrain should be available afterwards for users to manipulate the generated terrain







### READ THE FOLLOWING PLEASE PLEASE PLEASE



The following can be mitigated by disabling access violations.

Go to Debug -> Windows -> Exception Settings -> Win32 Exceptions -> uncheck 0xc0000005 Access violations

When resizing the window, it may hang for a second and then you can continue to resize.

=========================================================================================================

In Slate, during the draws there is a hitch that can occur when resizing the nomadic window by stretching

or squishing along the X and Y axis. The hitch will cause a windows access exception which will freeze 

Unreal Engine. You can hit continue in visual studio and the engine should continue running.







There is another potential bug when this occurs in which you kill the process. What will happen is that

the engine will stop running but upon trying to launch again there is a chance that the program will get

stuck launching. The only way to stop it from here is to open task manager, find msbuild and stop it

then you can stop unreal engine. If this does not work then you will have to restart your computer.



At this point something has corrupted



Do not try to launch again. Instead open the folder where the project is located.

Select the Binaries, Saved, .VS, .VSconfig, Intermediate, and .sln and delete them.

You will then have to select the .uproject, right click -> more options -> generate 

visual studio project files. This will regenerate the .sln and all relevant files.



You can launch the .sln once it is done and launch the project as normal.

It may take a bit longer to build. 

If it does not, delete the same folders, restart computer and regenerate files.

=========================================================================================================

