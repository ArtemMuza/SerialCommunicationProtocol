project(SCP)

set(SCP_SOURCES
        SCP.c)

add_library( ${PROJECT_NAME} STATIC ${SCP_SOURCES} )

add_definitions(-DUNIT_TESTS)

add_library(SCPtest STATIC ${SCP_SOURCES})