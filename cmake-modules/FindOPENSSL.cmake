
#  OPENSSL_FOUND - System has Openssl
#  OPENSSL_INCLUDE_DIRS - The Openssl include directories
#  OPENSSL_LIBRARIES - The libraries needed to use Openssl

FIND_PATH( OPENSSL_INCLUDE_DIR
        NAMES
        openssl/ssl.h
        PATHS
        /usr/local/include
        /usr/include
        "C:\\Program Files (x86)\\Microsoft Visual Studio\\2017\\Enterprise\\VC\\Tools\\MSVC\\14.14.26428\\include"
)

find_library(CRYPTO_LIBRARY
        NAMES
        libcrypto
        crypto
        PATHS
        /usr/local/lib
        /usr/lib
        /usr/lib/x86_64-linux-gnu
        "C:\\Program Files (x86)\\Microsoft Visual Studio\\2017\\Enterprise\\VC\\Tools\\MSVC\\14.14.26428\\lib\\x86"
)

find_library(SSL_LIBRARY
        NAMES
        libssl
        ssl
        PATHS
        /usr/local/lib
        /usr/lib
        /usr/lib/x86_64-linux-gnu
)

set(OPENSSL_LIBRARIES ${CRYPTO_LIBRARY} ${SSL_LIBRARY})
set(OPENSSL_INCLUDE_DIRS ${OPENSSL_INCLUDE_DIR} )

include(FindPackageHandleStandardArgs)

# handle the QUIETLY and REQUIRED arguments and set OPENSSL_FOUND to TRUE
# if all listed variables are TRUE

find_package_handle_standard_args(OPENSSL  DEFAULT_MESSAGE
        OPENSSL_LIBRARIES OPENSSL_INCLUDE_DIR)
mark_as_advanced(OPENSLL_INCLUDE_DIR OPENSSL_LIBRARIES )