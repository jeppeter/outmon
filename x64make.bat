set RELEASE_ARCH=x64
set VC_SETVAR_ARCH=amd64
set _IsNativeEnvironment=true
cscript verify_vs.vbs
cscript runmake.vbs makefile.nmake clean
cscript runmake.vbs makefile.nmake all