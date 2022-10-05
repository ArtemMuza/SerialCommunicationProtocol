#ifndef TESTS_H
#define TESTS_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "../include/SCP.h"



void print_bytes(const unsigned char* _data, size_t len){

    if(!_data) {
        printf("NULL data\n");
        return;
    }
    printf("SOF: %d %d ", _data[1], _data[0]);
    printf("Len: %d %d ", _data[3], _data[2]);
    printf("Header: type: %d cmd0:%d cmd1:%d %d cmd2: %d %d", _data[4], _data[5], _data[7], _data[6], _data[9], _data[8]);
    printf("\nData: ");
    for(int i =10 ; i < len-2; i++) {
        printf("%d ", _data[i]);
    }
    printf("\nCSC: %d %d", _data[len-2], _data[len-1]);
};

void ShowError(enum Error_code _code) {

    printf("\n");
    switch (_code) {
        case no_error:
            printf("No error");
            break;
        case incorrect_frame_format:
            printf("Incorrect frame format loaded");
            break;
        case incorrect_type:
            printf("Incorrect type format loaded");
            break;
        case incorrect_mpu_address:
            printf("Incorrect mpu address loaded");
            break;
        case incorrect_register_address:
            printf("Incorrect reg address loaded");
            break;
        case incorrect_data_length:
            printf("Incorrect data len was detected");
            break;
        case incorrect_data_value:
            printf("Incorrect data val was detected");
            break;
        case  slave_data_integrity:
            printf("Incorrect crc16 pair loaded");
            break;
    }printf("\n");
}

#endif
