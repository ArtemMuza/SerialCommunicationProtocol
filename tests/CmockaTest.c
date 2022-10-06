#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdio.h>

#include "SCP.h"
#include "Tests.h"
#include <cmocka.h>

uint8_t hostBuffer[1024];
uint8_t slaveBuffer[1024];
uint8_t regData[] = {1, 2, 3, 4, 5, 6, 7, 8};

//tests
static void ReadTest(void** state){
    (void) state;

    //Creating Host | Slave
    Host linux;
    Slave stm;
    CreateHost(&linux, addr_linux, 18, hostBuffer, REQR);
    CreateSlave(&stm, slaveBuffer);


    uint8_t* data = linux.CreateRequest(&linux);

    for(int i = 0; i < REQUEST_SIZE(linux) ; i++) {
        stm.Read(&stm, *(data+i));
        if (i != REQUEST_SIZE(linux) - 1)
            assert_int_not_equal(stm.IsValid(&stm), no_error);
    }
    assert_int_equal(stm.IsValid(&stm), no_error);

    if(REQ_TYPE(stm)){
        stm.WriteData(&stm, regData, 8);

        data = stm.CreateResponse(&stm);

        for(int i = 0; i < RESPONSE_SIZE(stm) ; i++) {
            linux.Read(&linux, *(data+i));
            if (i != RESPONSE_SIZE(stm) -1)
                assert_int_not_equal(linux.IsValid(&linux), no_error);
        }

        assert_int_equal(linux.IsValid(&linux), no_error);
        assert_false(IS_ERROR(linux));
        assert_memory_equal(GET_DATA_PTR(linux), regData, 8);

    }
}
static void WriteTest(void** state) {
    (void) state;

    Host linux;
    Slave stm;
    CreateHost(&linux, addr_linux, 18, hostBuffer, REQW);
    CreateSlave(&stm, slaveBuffer);

    linux.WriteData(&linux, regData, 8);

    uint8_t* data = linux.CreateRequest(&linux);
    for(int i = 0; i < REQUEST_SIZE(linux); i++) {

        stm.Read(&stm, *(data+i));
        if( i != REQUEST_SIZE(linux) - 1)
            assert_int_not_equal(stm.IsValid(&stm), no_error);
    }
    assert_int_equal(stm.IsValid(&stm), no_error);

    if(!REQ_TYPE(stm)){
        data = stm.CreateResponse(&stm);
        for(int i = 0; i < RESPONSE_SIZE(stm); i++) {

            linux.Read(&linux, *(data+i));
            if( i != RESPONSE_SIZE(stm) - 1)
                assert_int_not_equal(linux.IsValid(&linux), no_error);
        }
        assert_false(IS_ERROR(linux));
        assert_int_equal(*(GET_DATA_PTR(linux)), no_error);
    }
}
static void RandomDataFirst(void** state) {
    (void) state;

    hostBuffer[0] = 0x23;
    hostBuffer[1] = 0x35;
    hostBuffer[2] = 0xF2;
    hostBuffer[3] = 0xE3;
    hostBuffer[4] = 0x21;
    hostBuffer[5] = 0x44;
    hostBuffer[6] = 0xA3;
    hostBuffer[7] = 0x55;
    hostBuffer[8] = 0x01;
    hostBuffer[9] = 0x02;


    Host linux;
    Slave stm;
    CreateHost(&linux, addr_linux, 18, hostBuffer + 10, REQW);
    CreateSlave(&stm, slaveBuffer);

    linux.WriteData(&linux, regData, 8);

    uint8_t* data = linux.CreateRequest(&linux)  - 10;
    for(int i = 0; i < REQUEST_SIZE(linux) + 10 ; i++) {
        stm.Read(&stm, *(data+i));
        if ( i != REQUEST_SIZE(linux) + 9)
            assert_int_not_equal(stm.IsValid(&stm), no_error);
    }
    assert_int_equal(stm.IsValid(&stm), no_error);
    if(!REQ_TYPE(stm)){
        data = stm.CreateResponse(&stm);
        for(int i = 0; i < RESPONSE_SIZE(stm); i++) {
            linux.Read(&linux, *(data+i));
            if ( i != RESPONSE_SIZE(stm) - 1)
                assert_int_not_equal(linux.IsValid(&linux), no_error);
        }
        assert_int_equal(linux.IsValid(&linux), no_error);
        assert_false(IS_ERROR(linux));
        assert_int_equal(*(GET_DATA_PTR(linux)), no_error);
    }

}
static void CallError(void** state) {

    //Creating Host | Slave
    Host linux;
    Slave stm;
    CreateHost(&linux, addr_linux, 0x0000, hostBuffer, REQR);
    CreateSlave(&stm, slaveBuffer);

    uint8_t* data = linux.CreateRequest(&linux);

    for(int i = 0; i < REQUEST_SIZE(linux); i++) {
        stm.Read(&stm, *(data+i));
        if( i != REQUEST_SIZE(linux))
            assert_int_not_equal(stm.IsValid(&stm), no_error);
    }
    assert_int_equal(stm.IsValid(&stm), incorrect_register_address);

    if(REQ_TYPE(stm)){
        stm.WriteData(&stm, regData, 8);

        data = stm.CreateResponse(&stm);

        for(int i = 0; i < RESPONSE_SIZE(stm); i++) {

            linux.Read(&linux, *(data+i));
            if( i != RESPONSE_SIZE(stm) - 1)
                assert_int_not_equal(linux.IsValid(&linux), no_error);
        }
        assert_true(IS_ERROR(linux));
        assert_int_equal(*(GET_DATA_PTR(linux)), incorrect_register_address);

    }
}
static void RandomDataSOF(void** state) {

    hostBuffer[0] = 0x23;
    hostBuffer[1] = 0x35;
    hostBuffer[2] = 0xF2;
    hostBuffer[3] = 0xE3;
    hostBuffer[4] = 0x21;
    hostBuffer[5] = 0x44;
    hostBuffer[6] = 0xAA;
    hostBuffer[7] = 0x55;
    hostBuffer[8] = 0x01;
    hostBuffer[9] = 0x02;


    Host linux;
    Slave stm;
    CreateHost(&linux, addr_linux, 18, hostBuffer + 10, REQW);
    CreateSlave(&stm, slaveBuffer);

    linux.WriteData(&linux, regData, 8);

    uint8_t* data = linux.CreateRequest(&linux)  - 10;
    for(int i = 0; i < REQUEST_SIZE(linux); i++) {

        stm.Read(&stm, *(data+i));
        if( i != REQUEST_SIZE(linux) - 1)
            assert_int_not_equal(stm.IsValid(&stm), no_error);
    }
    assert_false(stm.IsValid(&stm) == no_error);

    data = stm.CreateResponse(&stm);
    for(int i = 0; i < RESPONSE_SIZE(stm); i++) {

        linux.Read(&linux, *(data+i));
        if( i != RESPONSE_SIZE(stm) - 1)
            assert_int_not_equal(linux.IsValid(&linux), no_error);
    }
    assert_true(IS_ERROR(linux));
}


int main(int argc, char** argv) {

    const struct UnitTest tests [] =
            {
            unit_test (ReadTest),
            unit_test (WriteTest),
            unit_test(RandomDataFirst),
            unit_test(CallError),
            unit_test(RandomDataSOF),
            };


    return run_tests(tests);
}