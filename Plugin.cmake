# ~~~
# Summary:      Local, non-generic plugin setup
# Copyright (c) 2020-2021 Mike Rossiter
# License:      GPLv3+
# ~~~

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.


# -------- Options ----------

set(OCPN_TEST_REPO
    "mike-rossiter/frcurrents-alpha"
    CACHE STRING "Default repository for untagged builds"
)
set(OCPN_BETA_REPO
    "opencpn/frcurrents-beta"
    CACHE STRING
    "Default repository for tagged builds matching 'beta'"
)
set(OCPN_RELEASE_REPO
    "opencpn/frcurrents-prod"
    CACHE STRING
    "Default repository for tagged builds not matching 'beta'"
)
#
#
# -------  Plugin setup --------
#
set(PKG_NAME frcurrents_pi)
set(PKG_VERSION  1.0.1)
set(PKG_PRERELEASE "")  # Empty, or a tag like 'beta'

set(DISPLAY_NAME frcurrents)    # Dialogs, installer artifacts, ...
set(PLUGIN_API_NAME frcurrents) # As of GetCommonName() in plugin API
set(PKG_SUMMARY "UK Currents")
set(PKG_DESCRIPTION [=[
Shows UK Currents.
]=])

set(PKG_AUTHOR "Mike Rossiter")
set(PKG_IS_OPEN_SOURCE "yes")
set(PKG_HOMEPAGE https://github.com/Rasbats/frcurrents_pi)
set(PKG_INFO_URL https://opencpn.org/OpenCPN/plugins/frcurrents.html)

set(SRC
            src/frcurrents_pi.h
            src/frcurrents_pi.cpp
            src/frcurrentsOverlayFactory.cpp
            src/frcurrentsOverlayFactory.h
            src/frcurrentsUIDialogBase.cpp
            src/frcurrentsUIDialogBase.h
            src/frcurrentsUIDialog.cpp
            src/frcurrentsUIDialog.h
            src/icons.h
            src/icons.cpp
            src/tcmgr.cpp
            src/tcmgr.h
            src/tinyxml.h
            src/tinyxml.cpp
            src/tinyxmlparser.cpp
            src/tinyxmlerror.cpp

        src/IDX_entry.cpp
        src/IDX_entry.h
        src/logger.cpp
        src/logger.h
        src/Station_Data.cpp
        src/Station_Data.h
        src/TC_Error_Code.h
        src/TCDataFactory.cpp
        src/TCDataFactory.h
        src/TCDataSource.cpp
        src/TCDataSource.h
        src/TCDS_Ascii_Harmonic.cpp
        src/TCDS_Ascii_Harmonic.h
        src/TCDS_Binary_Harmonic.cpp
        src/TCDS_Binary_Harmonic.h
        src/georef.cpp
        src/georef.h
        src/pidc.cpp
        src/pidc.h
        src/folder.xpm


)

set(PKG_API_LIB api-16)  #  A dir in opencpn-libs/ e. g., api-17 or api-16

macro(late_init)
  # Perform initialization after the PACKAGE_NAME library, compilers
  # and ocpn::api is available.

  add_definitions(-DocpnUSE_GL)

  if (QT_ANDROID)
    add_definitions(-DUSE_ANDROID_GLES2)
  endif ()

endmacro ()

macro(add_plugin_libraries)
  # Add libraries required by this plugin

  add_subdirectory("opencpn-libs/tinyxml")
  target_link_libraries(${PACKAGE_NAME} ocpn::tinyxml)

  add_subdirectory("${CMAKE_SOURCE_DIR}/opencpn-libs/plugingl")
  target_link_libraries(${PACKAGE_NAME} ocpn::plugingl)

  # The wxsvg library enables SVG overall in the plugin
  add_subdirectory("${CMAKE_SOURCE_DIR}/opencpn-libs/wxsvg")
  target_link_libraries(${PACKAGE_NAME} ocpn::wxsvg)

endmacro ()
