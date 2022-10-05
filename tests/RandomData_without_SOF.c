#include <stdio.h>
#include "../include/SCP.h"
#include "Tests.h"


int main() {

    uint8_t hostBuffer[1024];
    uint8_t slaveBuffer[1024];
    uint8_t regData[] = {1, 2, 3, 4, 5, 6, 7, 8};
    uint8_t randomData[] = { 1,9, 5, 0xA5, 0x55, 0x23, 0x89, 0x91, 0x80, 0x49, 0x12, 0x48, 0x98, 0x65, 0x34, 0x34};

    //Creating Host | Slave
    Host linux;
    Slave stm;
    CreateHost(&linux, addr_linux, 18, hostBuffer, REQR);
    CreateSlave(&stm, slaveBuffer);

    printf("Frame log:\n");
    print_bytes( linux.CreateRequest(&linux), REQUEST_SIZE(linux));

    stm.HandlingRequest(&stm, randomData,16);
    printf("\n");


    print_bytes( stm.CreateResponse(&stm), RESPONSE_SIZE(stm));
    linux.HandlingResponse(&linux, stm.CreateResponse(&stm), RESPONSE_SIZE(stm));

    if(IS_ERROR(linux)) {
        ShowError(*GET_DATA_PTR(stm));
        return 0;
    }
    abort();
}
