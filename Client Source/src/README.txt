To build a full release:
Log in as Administrator of Sakkonet.
Hit the "Start" button, then select "Run"
When promtped for a name of a prgram to run, type in "cmd" and press <enter> which will cause a DOS window will appear on the desktop
In the DOS Window, type "cd d:\projects\NTCSS_CLIENT\src" and hit <return>
In the DOS Window, type "build release clean" and hit <return>

To build debug versions of everything, change "release" to "debug" in the command lines listed above.
Note that as of 7/02 several projects will not build executables in debug due to included library conflicts among other things.