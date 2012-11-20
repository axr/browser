set(BROWSER_PACKAGE_NAME "AXR Browser")
set(BROWSER_PACKAGE_PREFIX "axr-browser")

if(WIN32)
    set(AXR_PACKAGE_ICON "${CMAKE_SOURCE_DIR}\\\\src\\\\core\\\\share\\\\icons\\\\prototype.ico")
elseif(APPLE)
    set(AXR_PACKAGE_ICON "${CMAKE_SOURCE_DIR}/src/core/share/icons/prototype.icns")
else()
    set(AXR_PACKAGE_ICON "${CMAKE_SOURCE_DIR}/src/core/share/icons/prototype.png")
endif()

# Per-generator overrides
set(CPACK_PROJECT_CONFIG_FILE "${CMAKE_BINARY_DIR}/PackageOverrides.cmake")

# PackageMaker only allows .rtf, .rtfd, .html and .txt
configure_file("${CMAKE_SOURCE_DIR}/cmake/PackageOverrides.cmake.in" "${CPACK_PROJECT_CONFIG_FILE}")
configure_file("${AXR_README_FILE}" "${AXR_README_FILE_TXT}")
configure_file("${AXR_LICENSE_FILE}" "${AXR_LICENSE_FILE_TXT}")

# Package version
set(CPACK_PACKAGE_VERSION_MAJOR ${BROWSER_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${BROWSER_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${BROWSER_VERSION_PATCH})
set(CPACK_PACKAGE_VERSION_BUILD ${BROWSER_VERSION_BUILD})
set(CPACK_PACKAGE_VERSION ${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}.${CPACK_PACKAGE_VERSION_BUILD})

# CPack variables...
set(CPACK_PACKAGE_ICON "${AXR_PACKAGE_ICON}")
set(CPACK_PACKAGE_NAME "${BROWSER_PACKAGE_NAME}")
set(CPACK_PACKAGE_DESCRIPTION "TODO insert first paragraph from README")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "AXR Project reference browser")
set(CPACK_PACKAGE_VENDOR "${AXR_VENDOR}")
set(CPACK_PACKAGE_CONTACT "${AXR_CONTACT}")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${AXR_README_FILE_TXT}")
set(CPACK_RESOURCE_FILE_README "${AXR_README_FILE_TXT}")
set(CPACK_RESOURCE_FILE_LICENSE "${AXR_LICENSE_FILE_TXT}")
set(CPACK_STRIP_FILES ON)

# Friendly names, descriptions and groups for components
set(CPACK_COMPONENT_BROWSER_DISPLAY_NAME "AXR Browser")
set(CPACK_COMPONENT_BROWSER_DESCRIPTION "A browser-like application that can be used to test the AXR rendering engine and HSS language")
set(CPACK_COMPONENT_BROWSER_REQUIRED TRUE)

set(CPACK_COMPONENT_LIBRARIES_REQUIRED TRUE)
set(CPACK_COMPONENT_LIBRARIES_HIDDEN TRUE)

# Filenames
include(PackageFilenames)
set(CPACK_PACKAGE_FILE_NAME "${BROWSER_PACKAGE_PREFIX}-${BROWSER_VERSION_STRING}-${OS_CODE}-${ARCH_CODE}")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "${BROWSER_PACKAGE_PREFIX}-${BROWSER_VERSION_STRING}-src")
set(CPACK_SOURCE_IGNORE_FILES ".git" ".DS_Store" "thumbs.db" "CMakeLists.txt.user")

# Source package
list(APPEND CPACK_SOURCE_GENERATOR ZIP TGZ STGZ TZ TBZ2)

if(WIN32)
    # WiX packaging introduced in version 2.8.11... if we don't have that we'll fall back to NSIS
    if(${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION} VERSION_EQUAL 2.8.11 OR
       ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION} VERSION_GREATER 2.8.11)
        set(CPACK_GENERATOR "WIX")
    else()
        set(CPACK_GENERATOR "NSIS")
    endif()

    set(CPACK_NSIS_CONTACT "${AXR_CONTACT}")
    set(CPACK_NSIS_DISPLAY_NAME "${BROWSER_PACKAGE_NAME}")
    set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
    set(CPACK_NSIS_EXECUTABLES_DIRECTORY .) # NSIS assumes executables are in bin by default
    set(CPACK_NSIS_HELP_LINK "http://axr.vg")
    set(CPACK_NSIS_INSTALLED_ICON_NAME "axrbrowser.exe")
    set(CPACK_NSIS_MUI_ICON "${AXR_PACKAGE_ICON}")
    set(CPACK_NSIS_MUI_UNIICON "${AXR_PACKAGE_ICON}")
    set(CPACK_NSIS_PACKAGE_NAME "${BROWSER_PACKAGE_NAME}")
    set(CPACK_NSIS_URL_INFO_ABOUT "http://axr.vg")

    set(CPACK_WIX_PRODUCT_GUID "XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX") # Change on each release
    set(CPACK_WIX_UPGRADE_GUID "152C98EF-20E0-429C-B8F0-426876A9C3CF") # Permanent, never change this

    set(CPACK_PACKAGE_EXECUTABLES "axrbrowser" "AXR Browser") # Start Menu
#   set(CPACK_PACKAGE_ICON "<something>.bmp")
    set(CPACK_PACKAGE_INSTALL_DIRECTORY "${BROWSER_PACKAGE_NAME}")
endif()

if(APPLE)
    set(CPACK_GENERATOR "DragNDrop")
    set(CPACK_DMG_VOLUME_NAME "${BROWSER_PACKAGE_NAME} ${BROWSER_VERSION_STRING}")
    set(CPACK_OSX_PACKAGE_VERSION ${CMAKE_OSX_DEPLOYMENT_TARGET})
endif()

if(DPKG_FOUND)
    set(CPACK_GENERATOR "DEB")
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libaxr")
    set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "${CPACK_PACKAGE_DESCRIPTION_SUMMARY}\n ${CPACK_PACKAGE_DESCRIPTION}")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "${AXR_VENDOR} <${AXR_CONTACT}>")
    set(CPACK_DEBIAN_PACKAGE_NAME "${BROWSER_PACKAGE_PREFIX}")
    set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
    set(CPACK_DEBIAN_PACKAGE_SECTION "web")
    set(CPACK_DEBIAN_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION}") # add +lenny1 for example, for distro-specific pkgs

    # Because we have to mess with component packaging in a weird way,
    # we need to name the Debian package *just* "axr", and -browser
    # will be appended by the component package file
    set(CPACK_PACKAGE_NAME "axr")
endif()

if(RPMBUILD_FOUND)
    set(CPACK_GENERATOR "RPM")
    set(CPACK_RPM_PACKAGE_DEPENDS "qt ( >= 4.6), qt-x11 ( >= 4.6 )")
    set(CPACK_RPM_PACKAGE_DESCRIPTION "${CPACK_PACKAGE_DESCRIPTION}")
    set(CPACK_RPM_PACKAGE_GROUP "Development/Libraries")
    set(CPACK_RPM_PACKAGE_LICENSE "GPLv3 License")
    set(CPACK_RPM_PACKAGE_NAME "${BROWSER_PACKAGE_PREFIX}")
    set(CPACK_RPM_PACKAGE_PROVIDES "axr-sdk")
    set(CPACK_RPM_PACKAGE_REQUIRES "qt >= 4.8")
    set(CPACK_RPM_PACKAGE_SUMMARY "${CPACK_PACKAGE_DESCRIPTION_SUMMARY}")
    set(CPACK_RPM_PACKAGE_VENDOR "${CPACK_PACKAGE_VENDOR}")
    set(CPACK_RPM_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION}") # long version string specifically?
endif()

if(DPKG_FOUND)
    # Configure and install Debian copyright file
    execute_process(COMMAND date -R OUTPUT_VARIABLE DATE_R OUTPUT_STRIP_TRAILING_WHITESPACE)
    configure_file("${CMAKE_SOURCE_DIR}/common/debian-copyright.txt" "${CMAKE_BINARY_DIR}/copyright")
    install(FILES "${CMAKE_BINARY_DIR}/copyright" DESTINATION share/doc/${BROWSER_PACKAGE_PREFIX} RENAME copyright COMPONENT browser)

    # Configure and install changelog file
    file(COPY "${CMAKE_SOURCE_DIR}/CHANGELOG.md" DESTINATION "${CMAKE_BINARY_DIR}")
    file(RENAME "${CMAKE_BINARY_DIR}/CHANGELOG.md" "${CMAKE_BINARY_DIR}/changelog")
    file(REMOVE "${CMAKE_BINARY_DIR}/changelog.gz")
    execute_process(COMMAND gzip -9 changelog WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")
    install(FILES "${CMAKE_BINARY_DIR}/changelog.gz" DESTINATION share/doc/${BROWSER_PACKAGE_PREFIX} COMPONENT browser)
endif()
