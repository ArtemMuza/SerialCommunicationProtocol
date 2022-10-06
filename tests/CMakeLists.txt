project(tests)


add_executable(CmockaTest CmockaTest.c)
target_include_directories(CmockaTest PUBLIC ${CMOCKA_INCLUDE_DIR})
target_link_libraries(CmockaTest  ${CMOCKA_STATIC_LIB})
target_link_libraries(CmockaTest  SCP)

add_test(NAME CmockaTest COMMAND CmockaTest)

