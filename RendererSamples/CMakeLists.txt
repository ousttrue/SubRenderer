SET(PROJECTNAME RendererSamples)

FILE(GLOB SRCS 
    *.cpp 
    *.h
    ${CMAKE_CURRENT_LIST_DIR}/../imgui/imgui.h
    ${CMAKE_CURRENT_LIST_DIR}/../imgui/imgui.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../imgui/imgui_internal.h
    ${CMAKE_CURRENT_LIST_DIR}/../imgui/imgui_draw.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../imgui/imgui_demo.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../imgui/stb_rect_pack.h
    ${CMAKE_CURRENT_LIST_DIR}/../imgui/stb_textedit.h
    ${CMAKE_CURRENT_LIST_DIR}/../imgui/stb_truetype.h
    ${CMAKE_CURRENT_LIST_DIR}/../imgui/examples/directx11_example/imgui_impl_dx11.h
    ${CMAKE_CURRENT_LIST_DIR}/../imgui/examples/directx11_example/imgui_impl_dx11.cpp
    )

ADD_LIBRARY(${PROJECTNAME} SHARED ${SRCS})

TARGET_COMPILE_DEFINITIONS(${PROJECTNAME} PUBLIC
    BUILD_RENDERERSAMPLES=1
    )

TARGET_LINK_LIBRARIES(${PROJECTNAME} 
    RendererToolkit
    d3d11
    d2d1
    dwrite
    )

