# Packaging info currently for RPM

# Fix Relocatable paths -  https://github.com/axomem/nucleus/issues/45

if(NUCLEUS_PACKAGE_INCLUDED)
    message(STATUS "PACKAGE was already included for ${APP_NAME}")
    message(FATAL_ERROR "At present packaging can only be included once.")
endif()
set(NUCLEUS_PACKAGE_INCLUDED ${APP_NAME})

message(STATUS "Including Packaging configuration - run `make package` to build package")

set(CPACK_PACKAGE_NAME ${APP_NAME})
set(CPACK_PACKAGE_VERSION ${VERSION})
set(CPACK_PACKAGE_RELEASE 1)
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_PACKAGE_RELEASE}.${CMAKE_SYSTEM_PROCESSOR}")
#set(CPACK_PACKAGE_CONTACT "Name")
set(CPACK_PACKAGE_VENDOR "axomem.io")

if(WIN32)
    set(CPACK_GENERATOR "NSIS64")
else()
    set(CPACK_GENERATOR "RPM")

    set(CPACK_PACKAGING_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
    set(CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION /usr /usr/local /usr/local/bin /etc /etc/${APPNAME} /usr/lib
            /usr/lib/systemd /usr/lib/systemd/system)

    #configure_file("${CMAKE_CURRENT_SOURCE_DIR}/setup/thingbase.rpm.spec" "${CMAKE_CURRENT_BINARY_DIR}/thingbase.rpm.spec" @ONLY IMMEDIATE)
    #set(CPACK_RPM_USER_BINARY_SPECFILE "${CMAKE_CURRENT_BINARY_DIR}/thingbase.rpm.spec")

endif()


include(CPack)
