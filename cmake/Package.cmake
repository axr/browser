set(BROWSER_PACKAGE_NAME "AXR Browser")
set(BROWSER_PACKAGE_PREFIX "axr-browser")

if(WIN32)
    set(AXR_PACKAGE_ICON "${AXR_CORE_SOURCE_DIR}\\\\share\\\\icons\\\\prototype.ico")
elseif(APPLE)
    set(AXR_PACKAGE_ICON "${AXR_CORE_SOURCE_DIR}/share/icons/prototype.icns")
else()
    set(AXR_PACKAGE_ICON "${AXR_CORE_SOURCE_DIR}/share/icons/prototype.png")
endif()

# Per-generator overrides
set(CPACK_PROJECT_CONFIG_FILE "${CMAKE_BINARY_DIR}/PackageOverrides.cmake")
configure_file("${CMAKE_SOURCE_DIR}/cmake/PackageOverrides.cmake.in" "${CPACK_PROJECT_CONFIG_FILE}")

# PackageMaker only allows .rtf, .rtfd, .html and .txt
configure_file("${AXR_README_FILE}" "${AXR_README_FILE_TXT}")
configure_file("${AXR_LICENSE_FILE}" "${AXR_LICENSE_FILE_TXT}")

# Package version
set(CPACK_PACKAGE_VERSION_MAJOR ${BROWSER_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${BROWSER_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${BROWSER_VERSION_PATCH})
set(CPACK_PACKAGE_VERSION_BUILD ${BROWSER_VERSION_BUILD})
set(CPACK_PACKAGE_VERSION ${BROWSER_VERSION_STRING_REAL})

# CPack variables...
set(CPACK_PACKAGE_ICON "${AXR_PACKAGE_ICON}")
set(CPACK_PACKAGE_NAME "${BROWSER_PACKAGE_NAME}")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${AXR_COMMON_SOURCE_DIR}/description.txt")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "AXR Project reference browser")
set(CPACK_PACKAGE_VENDOR "${AXR_VENDOR}")
set(CPACK_PACKAGE_CONTACT "${AXR_CONTACT}")
set(CPACK_RESOURCE_FILE_README "${AXR_README_FILE_RTF}")
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
package_file_name(CPACK_PACKAGE_FILE_NAME "${BROWSER_PACKAGE_PREFIX}" "${BROWSER_VERSION_STRING}")
src_package_file_name(CPACK_SOURCE_PACKAGE_FILE_NAME "${BROWSER_PACKAGE_PREFIX}" "${BROWSER_VERSION_STRING}")
set(CPACK_SOURCE_IGNORE_FILES ".git" ".DS_Store" "thumbs.db" "CMakeLists.txt.user")

# Source package
list(APPEND CPACK_SOURCE_GENERATOR ZIP TGZ)

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
    set(CPACK_NSIS_HELP_LINK "${AXR_WEB_URL}")
    set(CPACK_NSIS_INSTALLED_ICON_NAME "axrbrowser.exe")
    set(CPACK_NSIS_MUI_ICON "${AXR_PACKAGE_ICON}")
    set(CPACK_NSIS_MUI_UNIICON "${AXR_PACKAGE_ICON}")
    set(CPACK_NSIS_PACKAGE_NAME "${BROWSER_PACKAGE_NAME}")
    set(CPACK_NSIS_URL_INFO_ABOUT "${AXR_WEB_URL}")

    # This is black magic. CPack has no way to let us place BrandingText in the
    # main section of the .nsi file... but SetCompressor IS in the right spot,
    # so we'll inject our own property after this one by using a newline
    set(CPACK_NSIS_COMPRESSOR "lzma
BrandingText '${AXR_VENDOR}'")

    set(CPACK_WIX_PRODUCT_GUID "XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX") # Change on each release
    set(CPACK_WIX_UPGRADE_GUID "152C98EF-20E0-429C-B8F0-426876A9C3CF") # Permanent, never change this

    set(CPACK_PACKAGE_EXECUTABLES "axrbrowser" "AXR Browser") # Start Menu
    set(CPACK_PACKAGE_ICON "${AXR_COMMON_SOURCE_DIR}\\\\nsis-header.bmp")
    set(CPACK_PACKAGE_INSTALL_DIRECTORY "${BROWSER_PACKAGE_NAME}")
endif()

if(APPLE)
    set(CPACK_GENERATOR "DragNDrop")
    set(CPACK_DMG_VOLUME_NAME "${BROWSER_PACKAGE_NAME} ${BROWSER_VERSION_STRING}")
    set(CPACK_DMG_BACKGROUND_IMAGE "${CMAKE_BINARY_DIR}/backgroundImage.tiff")
    set(CPACK_DMG_DS_STORE "${CMAKE_SOURCE_DIR}/cmake/DS_Store")

    execute_process(COMMAND sips -s format tiff -s formatOptions normal "${CMAKE_SOURCE_DIR}/cmake/dmg-background.png" -s dpiWidth 72 -s dpiHeight 72 --out "${CMAKE_BINARY_DIR}/backgroundImage.tiff" WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
endif()

if(NOT WIN32 AND NOT APPLE)
    set(CPACK_GENERATOR "TGZ")
endif()

include(PackageLinux)

if(DPKG_FOUND)
    file(READ "${CPACK_PACKAGE_DESCRIPTION_FILE}" CPACK_PACKAGE_DESCRIPTION)

    # Finicky Debian control file formatting...
    string(REPLACE "\n\n" "\n.\n" CPACK_PACKAGE_DESCRIPTION "${CPACK_PACKAGE_DESCRIPTION}")
    string(REPLACE "\n" "\n " CPACK_PACKAGE_DESCRIPTION "${CPACK_PACKAGE_DESCRIPTION}")
    string(STRIP "${CPACK_PACKAGE_DESCRIPTION}" CPACK_PACKAGE_DESCRIPTION)

    list(APPEND CPACK_GENERATOR "DEB")
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6, libaxr")
    set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "${CPACK_PACKAGE_DESCRIPTION_SUMMARY}\n ${CPACK_PACKAGE_DESCRIPTION}")
    set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "${AXR_WEB_URL}")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "${AXR_VENDOR} <${AXR_CONTACT}>")
    set(CPACK_DEBIAN_PACKAGE_NAME "axr") # Needed for the CPACK_DEB_COMPONENT_INSTALL fix
    set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
    set(CPACK_DEBIAN_PACKAGE_SECTION "web")
    set(CPACK_DEBIAN_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION}")

    install(FILES "${CMAKE_BINARY_DIR}/copyright" DESTINATION share/doc/${BROWSER_PACKAGE_PREFIX} RENAME copyright COMPONENT browser)
    install(FILES "${CMAKE_BINARY_DIR}/deb/changelog.gz" DESTINATION share/doc/${BROWSER_PACKAGE_PREFIX} COMPONENT browser)
endif()

if(RPMBUILD_FOUND)
    list(APPEND CPACK_GENERATOR "RPM")
    set(CPACK_RPM_CHANGELOG_FILE "${CMAKE_BINARY_DIR}/rpm/changelog")
    set(CPACK_RPM_PACKAGE_ARCHITECTURE "${ARCH_CODE}")
    set(CPACK_RPM_PACKAGE_GROUP "Applications/Internet")
    set(CPACK_RPM_PACKAGE_LICENSE "GPL")
    set(CPACK_RPM_PACKAGE_NAME "axr") # Needed for the CPACK_RPM_COMPONENT_INSTALL fix
    set(CPACK_RPM_PACKAGE_RELEASE 1) # Don't change this
    set(CPACK_RPM_PACKAGE_REQUIRES "axr")
    set(CPACK_RPM_PACKAGE_URL "${AXR_WEB_URL}")
    set(CPACK_RPM_PACKAGE_VENDOR "${CPACK_PACKAGE_VENDOR}")
    set(CPACK_RPM_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION}")
endif()

if(DPKG_FOUND OR RPMBUILD_FOUND)
    # In tandem with CPACK_DEB/RPM_COMPONENT_INSTALL in PackageOverrides.cmake.in
    # Otherwise the package gets named incorrectly since CPack appends the component name
    set(CPACK_PACKAGE_NAME "axr")

    # To ensure only one package gets built on DEB/RPM
    set(CPACK_COMPONENT_BROWSER_GROUP browser)
    set(CPACK_COMPONENT_FREEDESKTOP_GROUP browser)

    set(CMAKE_INSTALL_PREFIX /usr)
endif()
