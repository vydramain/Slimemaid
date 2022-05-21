# - Try to find STBImage library
# Once done this will define:
#   STBI_FOUND - System has STBImage
#   STBI_INCLUDE_DIRS - The STBImage include directories
#   STBI_DEFINITIONS - Compiler switches requeired for using STBImage


# Find the header
find_path(STBI_INCLUDE_DIR stb_image.h
    PATHS "${CMAKE_SOURCE_DIR}/libraries/stb_image")

# No compiler switches necessry
set(STBI_DEFINITIONS "")

# Handle _FOUND tags and print success/failure status
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(STBImage DEFAULT_MSG STBI_INCLUDE_DIR)

#Don't display in GUI
mark_as_advanced(STBI_INCLUDE_DIR)

#No dependencies, so proceed
set(STBI_INCLUDE_DIR ${SBTI_INCLUDE_DIR})

