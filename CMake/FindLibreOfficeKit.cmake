find_path(LibreOfficeKit_INCLUDE_DIR LibreOfficeKit/LibreOfficeKit.hxx
          HINTS
          /usr/include
          /usr/local/include
          /opt/local/include/
)

find_library(LibreOfficeKit_LIBRARIES NAMES libreofficekitgtk
             HINTS
             /usr/lib
             /usr/lib64
             /usr/local/lib
             /opt/local/lib
)

find_path(LibreOffice_LOPATH NAMES soffice
             HINTS
             /usr/lib/libreoffice/program
             /usr/lib64/libreoffice/program
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LibreOfficeKit
                                  REQUIRED_VARS LibreOfficeKit_INCLUDE_DIR LibreOfficeKit_LIBRARIES
)

if(LibreOffice_LOPATH)
    SET(LibreOfficeKit_FOUND "YES")
endif()

mark_as_advanced(LibreOfficeKit_INCLUDE_DIR
                 LibreOfficeKit_LIBRARIES)
