#	Alan Witkowski - CMake module to find irrlicht
#
#	Input:
# 		IRRLICHT_ROOT - Environment variable that points to  irrlicht root directory
#
#	Output:
#		IRRLICHT_FOUND - Set to true if irrlicht was found
#		IRRLICHT_INCLUDE_DIRS - Path to irrlicht.h
#		IRRLICHT_LIBRARIES - Contains the list of irrlicht libraries
#

set(IRRLICHT_FOUND false)

# find include path
find_path(
		IRRLICHT_INCLUDE_DIRS
	NAMES
		irrlicht.h
	HINTS
		ENV IRRLICHT_ROOT
                ${IRRLICHT_ROOT}
	PATHS
		/usr
		/usr/local
                .
                ..
                ../..
                ../../..
	PATH_SUFFIXES
		include
		irrlicht
)

if(WIN32)
        if(MSVC)
                set(IRRLICHT_SOURCE_LIB_SUFFIX "/lib/Win32-visualstudio")
        else(MSVC)
                set(IRRLICHT_SOURCE_LIB_SUFFIX "/lib/Win32-gcc")
        endif(MSVC)
else(WIN32)
        if(APPLE)
                set(IRRLICHT_SOURCE_LIB_SUFFIX "/lib/Win32-gcc")
        else(APPLE)
                set(IRRLICHT_SOURCE_LIB_SUFFIX "/lib/Win32-gcc")
        endif(APPLE)
endif(WIN32)

# find libraries
find_library(
		IRRLICHT_LIBRARIES Irrlicht
	HINTS
		ENV IRRLICHT_ROOT
                ${IRRLICHT_ROOT}
	PATHS
		/usr/lib
		/usr/local/lib
	PATH_SUFFIXES
                ${IRRLICHT_SOURCE_LIB_SUFFIX}
                Irrlicht
                irrlicht
)

# handle QUIET and REQUIRED
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Irrlicht REQUIRED_VARS IRRLICHT_LIBRARIES IRRLICHT_INCLUDE_DIRS)

# advanced variables only show up in gui if show advanced is turned on
mark_as_advanced(IRRLICHT_INCLUDE_DIRS IRRLICHT_LIBRARIES)
