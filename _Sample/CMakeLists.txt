SET(PROJECTNAME _Sample)

FILE(GLOB SRCS *.cpp *.h *.hlsl)
#FILE(GLOB SHADERS *.hlsl)
#SET_PROPERTY(SOURCE 
#    ${SHADERS} 
#    PROPERTY VS_DEPLOYMENT_CONTENT 1
#    )
#SOURCE_GROUP("Shader Files" FILES ${SHADERS})
SET_SOURCE_FILES_PROPERTIES(${CMAKE_CURRENT_LIST_DIR}/SamplePixelShader.hlsl 
    PROPERTIES 
    VS_SHADER_TYPE Pixel 
    VS_SHADER_MODEL 5.0 
    VS_SHADER_ENTRYPOINT main 
    )
SET_SOURCE_FILES_PROPERTIES(${CMAKE_CURRENT_LIST_DIR}/SampleVertexShader.hlsl 
    PROPERTIES 
    VS_SHADER_TYPE Vertex 
    VS_SHADER_MODEL 5.0 
    VS_SHADER_ENTRYPOINT main 
    )

ADD_EXECUTABLE(${PROJECTNAME} WIN32 ${SRCS})

TARGET_INCLUDE_DIRECTORIES(${PROJECTNAME} PUBLIC
    ${SUBRENDERER_INCLUDE}
    )

TARGET_LINK_LIBRARIES(${PROJECTNAME}
    ${LIBS}
    winmm
    )

