project(SCP)

set(SCP_SOURCES
        SCP.c)

add_library( ${PROJECT_NAME} STATIC ${SCP_SOURCES} )