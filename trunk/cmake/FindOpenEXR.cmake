#
# Try to find OpenEXR's libraries, and include path.
# Once done this will define:
#
# OPENEXR_FOUND = OpenEXR found. 
# OPENEXR_INCLUDE_PATH = OpenEXR include directory.
# OPENEXR_LIBRARIES = libraries that are needed to use OpenEXR.
# 


IF (WIN32)
ELSE (WIN32)

	FIND_PATH(OPENEXR_INCLUDE_PATH ImfRgbaFile.h
		/usr/include
		/usr/local/include)
  
	FIND_LIBRARY(OPENEXR_HALF_LIBRARY 
		NAMES Half
		PATHS
			/usr/lib
			/usr/local/lib)
  
	FIND_LIBRARY(OPENEXR_IEX_LIBRARY 
		NAMES Iex
		PATHS
			/usr/lib
			/usr/local/lib)
  
	FIND_LIBRARY(OPENEXR_IMATH_LIBRARY
		NAMES Imath
		PATHS
			/usr/lib
			/usr/local/lib)
  
	FIND_LIBRARY(OPENEXR_ILMIMF_LIBRARY
		NAMES IlmImf
		PATHS
			/usr/lib
			/usr/local/lib)
  
ENDIF (WIN32)
  
IF (OPENEXR_INCLUDE_DIR AND OPENEXR_IMATH_LIBRARY AND OPENEXR_ILMIMF_LIBRARY AND OPENEXR_IEX_LIBRARY AND OPENEXR_HALF_LIBRARY)
	SET(OPENEXR_FOUND TRUE)
	SET(OPENEXR_LIBRARIES ${OPENEXR_IMATH_LIBRARY} ${OPENEXR_ILMIMF_LIBRARY} ${OPENEXR_IEX_LIBRARY} ${OPENEXR_HALF_LIBRARY} CACHE STRING "The libraries needed to use OpenEXR")
ENDIF (OPENEXR_INCLUDE_DIR AND OPENEXR_IMATH_LIBRARY AND OPENEXR_ILMIMF_LIBRARY AND OPENEXR_IEX_LIBRARY AND OPENEXR_HALF_LIBRARY)

MARK_AS_ADVANCED(
	OPENEXR_INCLUDE_DIR
	OPENEXR_LIBRARIES
	OPENEXR_ILMIMF_LIBRARY
	OPENEXR_IMATH_LIBRARY
	OPENEXR_IEX_LIBRARY
	OPENEXR_HALF_LIBRARY)
