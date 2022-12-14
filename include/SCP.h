#ifndef SCP_H
#define SCP_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define SOF 0x55AA //Start of Frame

#define REQ_TYPE(x) (x.header.type & 0x01) // True - read, False- write
#define IS_ERROR(x) (x.header.type & 0x80)

#define GET_DATA_PTR(x)  ((x.buffer ) ? (x.buffer + 10) : NULL)
#define GET_DATA_LEN(x) (x.header.cmd2)
#define GET_REG_ADDR(x) (x.header.cmd1)
#define RESPONSE_SIZE(slave) (GetSlavePackageSize(&slave) + 6)
#define REQUEST_SIZE(host) (GetHostPackageSize(&host) + 6)

#define MAX_PROTOCOL_LEN 1024
#define DATA_START_PLACE 10
#define HEADER_SIZE      6
#define SOF_SIZE         2
#define LEN_SIZE         2
#define FCS_SIZE         2
#define REQR_CODE        0x01
#define REQW_CODE        0x02

enum Error_code {
    no_error,
    incorrect_frame_format,
    incorrect_type,
    incorrect_mpu_address,
    incorrect_register_address,
    incorrect_data_length,
    incorrect_data_value,
    slave_data_integrity,
    slave_timeout,
    slave_busy,
};

enum mpu_addr {
    addr_linux  = 1,
    addr_main,
    addr_ventilation_1,
    addr_ventilation_2,
    addr_ventilation_3,
    addr_supervisor = 254,
};

enum Frame_type{
    REQR = 1, //Request to read a register
    REQW,     //Request to write a register
};

enum Work_mode {
    empty,
    sof,
    len,
    header,
    data,
    fcs,
    finish
};

typedef struct Header{

    uint16_t    len;
    uint8_t     type;
    uint8_t     cmd0; //MPU_ADDR
    uint16_t    cmd1; //REGISTER_ADDR
    uint16_t    cmd2; //Data_length

    enum Error_code errorCode;
    enum Work_mode  mode;
}Header;

typedef struct Host{
    Header header;
    uint8_t* buffer;
    size_t   bufferSize;
    int frameSize;

    uint8_t* (*CreateRequest)(struct Host*);
    bool     (*WriteData)(struct Host*,  uint8_t*, size_t);
    void     (*Read)(struct Host*, uint8_t);
    enum Error_code (*IsValid)(struct Host*);
}Host;

void        CreateHost(Host* _host, uint8_t* _dataBuffer, size_t _bufferSize, uint8_t _mpu, uint16_t _register, enum Frame_type _req);
uint16_t    GetHostPackageSize(Host* _host);
bool        SetRegisterAddr(Host* _host, uint16_t _addr);
void        ChangeFrameType(Host* _host, enum Frame_type _type);


typedef struct Slave{
    Header header;
    uint8_t* buffer;
    size_t   bufferSize;
    int frameSize;

    uint8_t* (*CreateResponse)(struct Slave*);
    bool     (*WriteData)(struct Slave*, uint8_t*, size_t);
    void     (*Read)(struct Slave*, uint8_t);
    enum Error_code (*IsValid)(struct Slave*);
}Slave;

void CreateSlave(Slave* _slave, uint8_t* _dataBuffer, size_t _bufferSize);
uint16_t GetSlavePackageSize(Slave* _slave);


#ifdef UNIT_TESTS
#define STATIC

uint16_t Crc16( uint8_t *crc_arr, uint8_t crc_num);
bool CheckType(uint8_t _type);
bool CheckMPU(uint8_t _mpu);
bool CheckRegisterAddr(uint16_t _addr);

bool WriteData(Header* _header, uint8_t* _buffer, uint8_t* _data, size_t _dataLen);
uint8_t* Serialize(Header* _header, uint8_t* _buffer);
void     DeserializeFrame(Header* _header, uint8_t* _buffer, uint8_t _byte, int* _frameSize);
enum Error_code     IsValid(Header* _header, uint8_t* _buffer, size_t _bufferSize, int _frameSize);
void     DeserializePayload(Header* _header, uint8_t* _buffer, uint8_t _byte, int* _frameSize);

#else
#define STATIC static
#endif

#endif