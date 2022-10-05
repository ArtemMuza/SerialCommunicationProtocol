#include <stdio.h>
#include "SCP.h"
#include "Tests.h"

int main() {

    uint8_t hostBuffer[1024];
    uint8_t slaveBuffer[1024];
    uint8_t regData[] = {1, 2, 3, 4, 5, 6, 7, 8};

    Host linux;
    Slave stm;

    CreateHost(&linux, addr_linux, 18, hostBuffer, REQR);
    CreateSlave(&stm, slaveBuffer);

    printf("no_error check:");

   // stm.HandlingRequest(&stm, linux.CreateRequest(&linux), REQUEST_SIZE(linux));
    if (REQ_TYPE(stm)) {
        stm.WriteData(&stm, regData,8);
    }
   // linux.HandlingResponse(&linux, stm.CreateResponse(&stm), RESPONSE_SIZE(stm));

    ShowError(linux.errorCode);
    if(IS_ERROR(linux))
        abort();


    printf("\nincorrect_type check:");
    CreateHost(&linux, addr_linux, 18, hostBuffer, REQR);
    linux.header.type = 0x71;

  //  stm.HandlingRequest(&stm, linux.CreateRequest(&linux), REQUEST_SIZE(linux));
    if (REQ_TYPE(stm)) {
        stm.WriteData(&stm, regData,8);
    }
  //  linux.HandlingResponse(&linux, stm.CreateResponse(&stm), RESPONSE_SIZE(stm));

    ShowError(*GET_DATA_PTR(linux));
    if(!IS_ERROR(linux))
        abort();

    printf("\nincorrect_mpu_address check:");
    CreateHost(&linux, addr_linux, 18, hostBuffer, REQR);
    linux.header.cmd0 = 0xFF;

  //  stm.HandlingRequest(&stm, linux.CreateRequest(&linux), REQUEST_SIZE(linux));
    if (REQ_TYPE(stm)) {
        stm.WriteData(&stm, regData,8);
    }
  //  linux.HandlingResponse(&linux, stm.CreateResponse(&stm), RESPONSE_SIZE(stm));

    ShowError(*GET_DATA_PTR(linux));
    if(!IS_ERROR(linux))
        abort();

    printf("\nincorrect_register_address check:");
    CreateHost(&linux, addr_linux, 18, hostBuffer, REQR);
    linux.header.cmd1 = 0xFFFF;

  //  stm.HandlingRequest(&stm, linux.CreateRequest(&linux), REQUEST_SIZE(linux));
    if (REQ_TYPE(stm)) {
        stm.WriteData(&stm, regData,8);
    }
  //  linux.HandlingResponse(&linux, stm.CreateResponse(&stm), RESPONSE_SIZE(stm));

    ShowError(*GET_DATA_PTR(linux));
    if(!IS_ERROR(linux))
        abort();

    printf("\nincorrect_data_length check:");
    CreateSlave(&stm, slaveBuffer);
    CreateHost(&linux, addr_linux, 18, hostBuffer, REQR);
    linux.WriteData(&linux, regData, 8);

    //stm.HandlingRequest(&stm, linux.CreateRequest(&linux), REQUEST_SIZE(linux));
    if (REQ_TYPE(stm)) {
        stm.WriteData(&stm, regData,8);
    }
  //  linux.HandlingResponse(&linux, stm.CreateResponse(&stm), RESPONSE_SIZE(stm));

    ShowError(*GET_DATA_PTR(linux));
    if(!IS_ERROR(linux))
        abort();

    printf("\nincorrect_data_value check:");
    CreateSlave(&stm, NULL);
    if(stm.errorCode != no_error)
        ShowError(stm.errorCode);
    else
        abort();


    return 0;
}
