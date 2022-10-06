project(SCP)

set(SCP_SOURCES
        SCP.c)

set(CMOCKA_SOURCES
        cmocka/cmocka.c)

add_library( ${PROJECT_NAME} STATIC ${SCP_SOURCES} )
add_library( cmocka STATIC ${CMOCKA_SOURCES})