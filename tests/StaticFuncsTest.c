#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdio.h>

#include "SCP.h"

#include <cmocka.h>

uint8_t hostBuffer[1024];
uint8_t slaveBuffer[1024];
uint8_t regData[] = {1, 2, 3, 4, 5, 6, 7, 8};
uint8_t ReadRequestExample[] = {
        0xAA, 0x55,
        0x06, 0x00,
        0x01, 0x01, 0x05, 0x00, 0x00, 0x00,
        0xFC, 0xFC
};
//tests
static void CheckerTest(void** state) {
    (void)state;

    //Type
    uint8_t correct = 0x01;
    uint8_t incorrect = 0x30;
    assert_true(CheckType(correct));
    assert_false(CheckType(incorrect));
    //MPU
    correct = 0x01;
    incorrect = 0x00;
    assert_true(CheckMPU(correct));
    assert_false(CheckMPU(incorrect));
    //Register
    assert_true(CheckMPU(correct));
    assert_false(CheckMPU(incorrect));
}
static void CrcTest(void** state) {
    (void)state;

    Host pc;
    Slave stm;
    CreateHost(&pc, hostBuffer, 1024, addr_linux, 18, REQW);
    CreateSlave(&stm, slaveBuffer, 1024);

    pc.WriteData(&pc, regData, 8);


    uint8_t* data = pc.CreateRequest(&pc);
    for(int i = 0; i < REQUEST_SIZE(pc); i++) {

        stm.Read(&stm, *(data+i));
        if( i != REQUEST_SIZE(pc) - 1)
            assert_int_not_equal(stm.IsValid(&stm), no_error);
    }
    assert_int_equal(stm.IsValid(&stm), no_error);

    uint16_t crc = Crc16(data + SOF_SIZE, pc.header.cmd2 + LEN_SIZE + HEADER_SIZE);
    uint16_t crc2 = data[DATA_START_PLACE + pc.header.cmd2];
    crc2 += data[DATA_START_PLACE + pc.header.cmd2 + 1] << 8;

    assert_int_equal(crc, crc2);
}
static void WriteTest(void** state) {
    (void) state;

    Header head = {0};
    assert_true(WriteData(&head, hostBuffer, regData, 8));
    assert_int_equal(head.cmd2, 8);
    assert_memory_equal(hostBuffer + DATA_START_PLACE, regData, 8);
    assert_false(WriteData(&head, hostBuffer, regData, 2048));
}
static void SerializeTest(void** state) {
    (void) state;

    Header head = { 0 };

    assert_false(Serialize(&head, NULL));

    head.type = REQR_CODE;
    head.cmd0 = 0x01;
    head.cmd1 = 0x05;
    uint8_t* data = Serialize(&head, hostBuffer);
    assert_memory_equal(data, ReadRequestExample, 12);
}
static void DeserializeTest(void** state) {
    (void) state;

    Header  head = {0};
    int frameSize = 0;

    for(int i = 0; i < 12; i++)
        DeserializeFrame(&head, slaveBuffer, ReadRequestExample[i], &frameSize);

    assert_int_equal(head.mode, finish);
    assert_int_equal(frameSize, 12);
    assert_memory_equal(slaveBuffer, ReadRequestExample, 12);
}
static void IsValidTest(void** state) {
    (void) state;

    Header head = { 0x01, 0x01, 0x10, 0x00};
    enum Error_code err = no_error;

    head.mode = empty;
    assert_int_equal(IsValid(&head, ReadRequestExample,100, 12), incorrect_frame_format);

    head.mode = finish;
    head.type = 0x30;
    assert_int_equal(IsValid(&head, ReadRequestExample,100, 12), incorrect_type);

    head.type = 0x01;
    head.cmd0 =0x00;
    assert_int_equal(IsValid(&head, ReadRequestExample,100, 12), incorrect_mpu_address);

    head.cmd0 = 0x01;
    head.cmd1 = 0x00;
    assert_int_equal(IsValid(&head, ReadRequestExample,100, 12), incorrect_register_address);

    head.cmd1 = 0x01;
    ReadRequestExample[11] = 0x00;
    assert_int_equal(IsValid(&head, ReadRequestExample,100, 12), slave_data_integrity);

    ReadRequestExample[11] = 0xFC;
    assert_int_equal(IsValid(&head, ReadRequestExample,100, 2), incorrect_frame_format);
}


int main(int argc, char** argv) {

    const struct UnitTest tests [] =
            {
                    unit_test(CheckerTest),
                    unit_test(CrcTest),
                    unit_test(WriteTest),
                    unit_test(SerializeTest),
                    unit_test(DeserializeTest),
                    unit_test(IsValidTest),
            };


    return run_tests(tests);
}