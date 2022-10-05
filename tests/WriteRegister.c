#include <stdio.h>
#include "../include/SCP.h"
#include "Tests.h"

int main() {

    uint8_t hostBuffer[1024];
    uint8_t slaveBuffer[1024];
    uint8_t regData[] = {1, 2, 3, 4, 5, 6, 7, 8};
    Host linux;
    Slave stm;
    CreateHost(&linux, addr_linux, 18, hostBuffer, REQW);
    CreateSlave(&stm, slaveBuffer);
    printf("Frame log:\n");
    linux.WriteData(&linux, regData, 8);

    print_bytes( linux.CreateRequest(&linux), REQUEST_SIZE(linux));

    stm.HandlingRequest(&stm, linux.CreateRequest(&linux), REQUEST_SIZE(linux));

    printf("\n");

    if(!REQ_TYPE(stm)) {
        print_bytes( stm.CreateResponse(&stm), RESPONSE_SIZE(stm));
        linux.HandlingResponse(&linux, stm.CreateResponse(&stm), RESPONSE_SIZE(stm));
        if(IS_ERROR(linux)) {
            ShowError(*GET_DATA_PTR(linux));
            abort();
        }
        printf("\nData was write successfully:");
        ShowError(*GET_DATA_PTR(linux));

    }
    return 0;
}
