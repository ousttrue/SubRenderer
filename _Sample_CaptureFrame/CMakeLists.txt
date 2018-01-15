SET(PROJECTNAME _Sample_CaptureFrame)

FILE(GLOB SRCS *.cpp *.h *.hlsl)
#FILE(GLOB SHADERS *.hlsl)
#SET_PROPERTY(SOURCE 
#    ${SHADERS} 
#    PROPERTY VS_DEPLOYMENT_CONTENT 1
#    )
#SOURCE_GROUP("Shader Files" FILES ${SHADERS})
FILE(GLOB RESOURCES Resources/*)
ADD_EXECUTABLE(${PROJECTNAME} WIN32 
    ${SRCS}
    ${RESOURCES}
    )

TARGET_INCLUDE_DIRECTORIES(${PROJECTNAME} PUBLIC
    ${SUBRENDERER_INCLUDE}
    )

TARGET_LINK_LIBRARIES(${PROJECTNAME}
    ${LIBS}
    Mf
    Mfplat
    mfuuid
    strmiids
    winmm
    )

