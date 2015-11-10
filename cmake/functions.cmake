macro(ChangeStaticRuntimeLib)
	message(STATUS "change runtime static")
	foreach(flag_var
	        CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
	        CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
	   if(${flag_var} MATCHES "/MD")
	      string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
	   endif(${flag_var} MATCHES "/MD")
	   if(${flag_var} MATCHES "/MDd")
	   	  string(REGEX REPLACE "/MDd" "/MTd" ${flag_var} "${${flag_var}}")
	   endif(${flag_var} MATCHES "/MDd")
endforeach(flag_var)	
endmacro(ChangeStaticRuntimeLib)

macro(CheckMSVC)
if (MSVC)
	if(${MSVC_VERSION} LESS 1600)
		message(FATAL_ERROR "must visual studio 2010 later")
	endif()
else()
	message(FATAL_ERROR "this is for msvc running")
endif(MSVC)
endmacro(CheckMSVC)

macro(Check32Bit)
if(${CMAKE_CL_64} EQUAL "1")
	message(FATAL_ERROR "must run into 32bits compiler")
endif()
endmacro(Check32Bit)