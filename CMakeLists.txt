IF("${CMAKE_SYSTEM_NAME}" STREQUAL WindowsStore)
    SET(SLN_NAME "RendererToolkit[UWP]")
ELSE()
    SET(SLN_NAME "RendererToolkit")
ENDIF()
CMAKE_MINIMUM_REQUIRED(VERSION 3.4)
PROJECT(${SLN_NAME}) # .sln


##############################################################################
# common settins
##############################################################################
INCLUDE(${CMAKE_CURRENT_LIST_DIR}/cmake-precompiled-header/PrecompiledHeader.cmake)

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

ADD_DEFINITIONS(
    -DWIN32=1
    -DUNICODE=1
    -D_UNICODE=1
    )

INCLUDE_DIRECTORIES(
    ${CMAKE_CURRENT_LIST_DIR}/imgui
    ${CMAKE_CURRENT_LIST_DIR}/RendererToolkit
    ${CMAKE_CURRENT_LIST_DIR}/RendererSamples
    )

IF("${CMAKE_SYSTEM_NAME}" STREQUAL WindowsStore)
    SET(CMAKE_C_FLAGS "/ZW /EHsc /await /std:c++latest")
    SET(CMAKE_CXX_FLAGS ${CMAKE_C_FLAGS})
    ADD_DEFINITIONS(
        -DUWP=1
        )
    INCLUDE_DIRECTORIES(
        ${CMAKE_CURRENT_LIST_DIR}/cppwinrt/10.0.15063.0
        )
ENDIF()


##############################################################################
# SUBDIRS
##############################################################################
SET(LIBS
    RendererToolkit
    RendererSamples
    )

SUBDIRS(
    RendererToolkit
    RendererSamples
    _Sample
    )

IF("${CMAKE_SYSTEM_NAME}" STREQUAL WindowsStore)
    SUBDIRS(
        CaptureFrameGrabber
        _Sample_CaptureFrame
        _Sample_Hololens
        )
ENDIF()

