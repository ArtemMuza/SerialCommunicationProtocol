project(tests C)

if (TESTS)
    add_executable(CmockaTest CmockaTest.c)
    target_include_directories(CmockaTest PUBLIC ${CMOCKA_INCLUDE_DIR})
    target_link_libraries(CmockaTest  ${CMOCKA_STATIC_LIB})
    target_link_libraries(CmockaTest  SCP)
    add_test(NAME CmockaTest COMMAND CmockaTest)

    add_executable(StaticTest StaticFuncsTest.c)
    target_include_directories(StaticTest PUBLIC ${CMOCKA_INCLUDE_DIR})
    target_link_libraries(StaticTest  ${CMOCKA_STATIC_LIB})
    target_link_libraries(StaticTest  SCP)
    add_test(NAME StaticTest COMMAND StaticTest)
endif ()




