
#  CJSON_FOUND - System has cJSON
#  CJSON_INCLUDE_DIR - The cJSON include directories
#  CJSON_LIBRARY - The library needed to use cJSON

FIND_PATH( CJSON_INCLUDE_DIR
        NAMES
        cjson
        PATHS
        /usr/local/include
        /usr/include
)

find_library(CJSON_LIBRARY
        NAMES
        libcjson
        cjson
        PATHS
        /usr/local/lib
        /usr/lib
)

include(FindPackageHandleStandardArgs)

# handle the QUIETLY and REQUIRED arguments and set CJSON_FOUND to TRUE
# if all listed variables are TRUE

find_package_handle_standard_args(CJSON  DEFAULT_MESSAGE
        CJSON_LIBRARY CJSON_INCLUDE_DIR)
mark_as_advanced( CJSON_INCLUDE_DIR CJSON_LIBRARY )