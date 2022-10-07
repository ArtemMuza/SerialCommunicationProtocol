#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdio.h>

#include "SCP.h"
#include <cmocka.h>

uint8_t hostBuffer[1024];
uint8_t slaveBuffer[1024];
uint8_t regData[] = {1, 2, 3, 4, 5, 6, 7, 8};

//tests
static void ReadTest(void** state){
    (void) state;

    //Creating Host | Slave
    Host pc;
    Slave stm;
    CreateHost(&pc, addr_linux, 18, hostBuffer, REQR);
    CreateSlave(&stm, slaveBuffer);


    uint8_t* data = pc.CreateRequest(&pc);

    for(int i = 0; i < REQUEST_SIZE(pc) ; i++) {
        stm.Read(&stm, *(data+i));
        if (i != REQUEST_SIZE(pc) - 1)
            assert_int_not_equal(stm.IsValid(&stm), no_error);
    }
    assert_int_equal(stm.IsValid(&stm), no_error);

    if(REQ_TYPE(stm)){
        stm.WriteData(&stm, regData, 8);

        data = stm.CreateResponse(&stm);

        for(int i = 0; i < RESPONSE_SIZE(stm) ; i++) {
            pc.Read(&pc, *(data + i));
            if (i != RESPONSE_SIZE(stm) -1)
                assert_int_not_equal(pc.IsValid(&pc), no_error);
        }

        assert_int_equal(pc.IsValid(&pc), no_error);
        assert_false(IS_ERROR(pc));
        assert_memory_equal(GET_DATA_PTR(pc), regData, 8);

    }
}
static void WriteTest(void** state) {
    (void) state;

    Host pc;
    Slave stm;
    CreateHost(&pc, addr_linux, 18, hostBuffer, REQW);
    CreateSlave(&stm, slaveBuffer);

    pc.WriteData(&pc, regData, 8);

    uint8_t* data = pc.CreateRequest(&pc);
    for(int i = 0; i < REQUEST_SIZE(pc); i++) {

        stm.Read(&stm, *(data+i));
        if( i != REQUEST_SIZE(pc) - 1)
            assert_int_not_equal(stm.IsValid(&stm), no_error);
    }
    assert_int_equal(stm.IsValid(&stm), no_error);

    if(!REQ_TYPE(stm)){
        data = stm.CreateResponse(&stm);
        for(int i = 0; i < RESPONSE_SIZE(stm); i++) {

            pc.Read(&pc, *(data + i));
            if( i != RESPONSE_SIZE(stm) - 1)
                assert_int_not_equal(pc.IsValid(&pc), no_error);
        }
        assert_false(IS_ERROR(pc));
        assert_int_equal(*(GET_DATA_PTR(pc)), no_error);
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


    Host pc;
    Slave stm;
    CreateHost(&pc, addr_linux, 18, hostBuffer + 10, REQW);
    CreateSlave(&stm, slaveBuffer);

    pc.WriteData(&pc, regData, 8);

    uint8_t* data = pc.CreateRequest(&pc) - 10;
    for(int i = 0; i < REQUEST_SIZE(pc) + 10 ; i++) {
        stm.Read(&stm, *(data+i));
        if ( i != REQUEST_SIZE(pc) + 9)
            assert_int_not_equal(stm.IsValid(&stm), no_error);
    }
    assert_int_equal(stm.IsValid(&stm), no_error);
    if(!REQ_TYPE(stm)){
        data = stm.CreateResponse(&stm);
        for(int i = 0; i < RESPONSE_SIZE(stm); i++) {
            pc.Read(&pc, *(data + i));
            if ( i != RESPONSE_SIZE(stm) - 1)
                assert_int_not_equal(pc.IsValid(&pc), no_error);
        }
        assert_int_equal(pc.IsValid(&pc), no_error);
        assert_false(IS_ERROR(pc));
        assert_int_equal(*(GET_DATA_PTR(pc)), no_error);
    }

}
static void CallError(void** state) {

    (void) state;
    //Creating Host | Slave
    Host pc;
    Slave stm;
    CreateHost(&pc, addr_linux, 0x0000, hostBuffer, REQR);
    CreateSlave(&stm, slaveBuffer);

    uint8_t* data = pc.CreateRequest(&pc);

    for(int i = 0; i < REQUEST_SIZE(pc); i++) {
        stm.Read(&stm, *(data+i));
        if( i != REQUEST_SIZE(pc))
            assert_int_not_equal(stm.IsValid(&stm), no_error);
    }
    assert_int_equal(stm.IsValid(&stm), incorrect_register_address);

    if(REQ_TYPE(stm)){
        stm.WriteData(&stm, regData, 8);

        data = stm.CreateResponse(&stm);

        for(int i = 0; i < RESPONSE_SIZE(stm); i++) {

            pc.Read(&pc, *(data + i));
            if( i != RESPONSE_SIZE(stm) - 1)
                assert_int_not_equal(pc.IsValid(&pc), no_error);
        }
        assert_int_not_equal(pc.IsValid(&pc), no_error);
        assert_true(IS_ERROR(pc));
        assert_int_equal(*(GET_DATA_PTR(pc)), incorrect_register_address);

    }
}
static void RandomDataSOF(void** state) {

    (void)state;
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


    Host pc;
    Slave stm;
    CreateHost(&pc, addr_linux, 18, hostBuffer + 10, REQW);
    CreateSlave(&stm, slaveBuffer);

    pc.WriteData(&pc, regData, 8);

    uint8_t* data = pc.CreateRequest(&pc) - 10;
    for(int i = 0; i < REQUEST_SIZE(pc); i++) {

        stm.Read(&stm, *(data+i));
        if( i != REQUEST_SIZE(pc) - 1)
            assert_int_not_equal(stm.IsValid(&stm), no_error);
    }
    assert_false(stm.IsValid(&stm) == no_error);

    data = stm.CreateResponse(&stm);
    for(int i = 0; i < RESPONSE_SIZE(stm); i++) {

        pc.Read(&pc, *(data + i));
        if( i != RESPONSE_SIZE(stm) - 1)
            assert_int_not_equal(pc.IsValid(&pc), no_error);
    }
    assert_true(IS_ERROR(pc));
}
static void SetterTest(void** state) {

    Host pc;
    Slave stm;
    CreateHost(&pc, addr_linux, 18, hostBuffer, REQR);
    CreateSlave(&stm, slaveBuffer);


    assert_true(SetRegisterAddr(&pc, 0x20));
    assert_int_equal(pc.header.cmd1, 0x20);
    ChangeFrameType(&pc, REQW);
    assert_int_equal(pc.header.type, REQW_CODE);
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