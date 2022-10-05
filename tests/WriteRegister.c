#include <stdio.h>
#include "SCP.h"
#include "Tests.h"

int main() {

    uint8_t hostBuffer[1024];
    uint8_t slaveBuffer[1024];
    uint8_t regData[] = {1, 2, 3, 4, 5, 6, 7, 8};

    Host linux;
    Slave stm;
    CreateHost(&linux, addr_linux, 18, hostBuffer, REQW);
    CreateSlave(&stm, slaveBuffer);

    linux.WriteData(&linux, regData, 8);

    uint8_t* data = linux.CreateRequest(&linux);
    print_bytes(data, REQUEST_SIZE(linux));
    for(int i = 0; ; i++) {

        stm.Read(&stm, *(data+i));
        if(stm.mode == finish)
            break;
    }
    if(stm.IsValid(&stm) == no_error){
        if(!REQ_TYPE(stm)){
             data = stm.CreateResponse(&stm);
            print_bytes(data, RESPONSE_SIZE(stm));
            for(int i = 0;; i++) {

                linux.Read(&linux, *(data+i));
                if(linux.mode == finish)
                    break;
            }
            if(IS_ERROR(linux)) {
                ShowError(*GET_DATA_PTR(linux));
                abort();
            }
            printf("\nData was write successfully:");
            ShowError(*GET_DATA_PTR(linux));

        }
    }
    if(!REQ_TYPE(stm)) {


    }
    return 0;
}
