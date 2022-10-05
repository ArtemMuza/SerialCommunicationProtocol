#include <stdio.h>
#include "../include/SCP.h"
#include "Tests.h"


int main() {

    uint8_t hostBuffer[1024];
    uint8_t slaveBuffer[1024];
    uint8_t regData[] = {1, 2, 3, 4, 5, 6, 7, 8};

    //Creating Host | Slave
    Host linux;
    Slave stm;
    CreateHost(&linux, addr_linux, 18, hostBuffer, REQR);
    CreateSlave(&stm, slaveBuffer);

    printf("Frame log:\n");
    print_bytes( linux.CreateRequest(&linux), REQUEST_SIZE(linux));

    stm.HandlingRequest(&stm, linux.CreateRequest(&linux), REQUEST_SIZE(linux));
    printf("\n");

    if(REQ_TYPE(stm)){
        stm.WriteData(&stm, regData, 8);

        print_bytes( stm.CreateResponse(&stm), RESPONSE_SIZE(stm));
        linux.HandlingResponse(&linux, stm.CreateResponse(&stm), RESPONSE_SIZE(stm));

        if(IS_ERROR(linux)) {
            ShowError(*GET_DATA_PTR(stm));
            abort();
        }
        printf("\nData was read successfully:\n");
        for(int i =0; i < GET_DATA_LEN(linux); i++){
            printf("%d ",*(GET_DATA_PTR(linux) + i) );
        }
    }




    return 0;
}
