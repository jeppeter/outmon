set RELEASE_ARCH=x86
set VC_SETVAR_ARCH=amd64_x86
cscript runmake.vbs makefile.nmake clean
cscript runmake.vbs makefile.nmake all