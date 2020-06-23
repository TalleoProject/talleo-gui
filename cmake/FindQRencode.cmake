# Try to find QRencode headers and library.
#
# Usage of this module as follows:
#
#     find_package(QRencode)
#
# Variables used by this module, they can change the default behaviour and need
# to be set before calling find_package:
#
#  QRENCODE_ROOT_DIR         Set this variable to the root installation of
#                            QRencode if the module has problems finding the
#                            proper installation path.
#
# Variables defined by this module:
#
#  QRENCODE_FOUND            System has QRencode library/headers.
#  QRENCODE_LIBRARIES        The QRencode library.
#  QRENCODE_INCLUDE_DIRS     The location of QRencode headers.

find_path(QRENCODE_ROOT_DIR
    NAMES include/qrencode.h
)

find_library(QRENCODE_LIBRARIES
    NAMES qrencode
    HINTS ${QRENCODE_ROOT_DIR}/lib
)

find_path(QRENCODE_INCLUDE_DIRS
    NAMES qrencode.h
    HINTS ${QRENCODE_ROOT_DIR}/include
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(QRencode DEFAULT_MSG
    QRENCODE_LIBRARIES
    QRENCODE_INCLUDE_DIRS
)

mark_as_advanced(
    QRENCODE_ROOT_DIR
    QRENCODE_LIBRARIES
    QRENCODE_INCLUDE_DIRS
)
