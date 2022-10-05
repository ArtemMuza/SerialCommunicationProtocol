#include <stdio.h>
#include "SCP.h"
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
    uint8_t* data = linux.CreateRequest(&linux);
    print_bytes(data, REQUEST_SIZE(linux));
    for(int i = 0; ; i++) {

        stm.Read(&stm, *(data+i));
        if(stm.mode == finish)
            break;
    }

    if(stm.IsValid(&stm) == no_error){
        if(REQ_TYPE(stm)){
            stm.WriteData(&stm, regData, 8);

            data = stm.CreateResponse(&stm);
            printf("\n");
            print_bytes(data, RESPONSE_SIZE(stm));
            for(int i = 0;; i++) {

                linux.Read(&linux, *(data+i));
                if(linux.mode == finish)
                    break;
            }

            if(IS_ERROR(linux)) {
                ShowError(*GET_DATA_PTR(stm));
                abort();
            }
            printf("\n\nData was read successfully:\n");
            for(int i =0; i < GET_DATA_LEN(linux); i++){
                printf("%d ",*(GET_DATA_PTR(linux) + i) );
            }
        }
    }

    return 0;
}
