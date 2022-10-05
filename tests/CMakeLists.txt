project(tests)


add_executable(ReadRegister ReadRegister.c)
target_link_libraries(ReadRegister  SCP)

add_executable(WriteRegister WriteRegister.c)
target_link_libraries(WriteRegister  SCP)

add_executable(ErrorCheck ErrorCheck.c)
target_link_libraries(ErrorCheck  SCP)

add_executable(EmptyBufferTest EmptyBufferTest.c)
target_link_libraries(EmptyBufferTest SCP)

add_executable(RandomData RandomData_with_SOF.c)
target_link_libraries(RandomData SCP)

add_executable(RealRandomData RandomData_without_SOF.c)
target_link_libraries(RealRandomData SCP)

add_test(NAME ReadRegister COMMAND ReadRegister)
add_test(NAME WriteRegister COMMAND WriteRegister)
add_test(NAME ErrorCheck COMMAND ErrorCheck)
add_test(NAME EmptyBufferTest COMMAND EmptyBufferTest)
add_test(NAME RandomData COMMAND RandomData)
add_test(NAME RealRandomData COMMAND RealRandomData)