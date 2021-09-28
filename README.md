# DLL

Disable ASLR for eqgame.exe

This has already been done for the eqgame.exe in the demoncia patcher
- Download https://blog.didierstevens.com/2010/10/17/setdllcharacteristics/
- setdllcharacteristics -d eqgame.exe



## zone data
- zone shortname list does not include clz, wall
offset: 0xAC7B80
airplane x0 then 22 x0, 0x5, 3 x0, 0x5, 3 0x0, 0x8, 3 0x0

- zone full name list (does include clz, wall)
offset: 0xA0AB90
interiorwalltest 3 x0, Wall Wall Wall, x0 x0
burnedwoods x0, The Burned Woods x0 x0 x0 x0
pomischief x0, The Plane of Mischief x0 x0 x0
