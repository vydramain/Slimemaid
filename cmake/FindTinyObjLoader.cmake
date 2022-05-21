# - Try to find TinyObjLoader library
# Once done this will define:
#   TOL_FOUND - System has TinyObjLoader 
#   TOL_INCLUDE_DIRS - The TinyObjLoader include directories
#   TOL_DEFINITIONS - Compiler switches requeired for using TinyObjLoader 


# Find the header
find_path(TOL_INCLUDE_DIR tiny_obj_loader.h
    PATHS "${CMAKE_SOURCE_DIR}/libraries/tinyobjloader")

# No compiler switches necessry
set(TOL_DEFINITIONS "")

# Handle _FOUND tags and print success/failure status
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(TinyObjLoader DEFAULT_MSG TOL_INCLUDE_DIR)

#Don't display in GUI
mark_as_advanced(TOL_INCLUDE_DIR)

#No dependencies, so proceed
set(TOL_INCLUDE_DIR ${TOL_INCLUDE_DIR})

