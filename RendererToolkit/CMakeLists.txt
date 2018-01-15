SET(PROJECTNAME RendererToolkit)

FILE(GLOB SRCS 
    *.cpp 
    *.h
    )
#IF(${CMAKE_SYSTEM_NAME} STREQUAL WindowsStore)
#    LIST(REMOVE_ITEM SRCS
#        ${CMAKE_CURRENT_LIST_DIR}/RenderWindow.h
#        ${CMAKE_CURRENT_LIST_DIR}/RenderWindow.cpp
#        )
#ENDIF()

ADD_LIBRARY(${PROJECTNAME} SHARED ${SRCS})

TARGET_COMPILE_DEFINITIONS(${PROJECTNAME} PUBLIC
    BUILD_RENDERERTK=1
    )

TARGET_LINK_LIBRARIES(${PROJECTNAME} 
    d3d11
    d3dcompiler
    dxgi
    dxguid
    )

