#include "SCP.h"
#include <string.h>

#define SET_1ST_BIT(x) (x |= 0x80)
#define RESERVED_REGISTER_COUNT 2
static const uint16_t ReservedRegistersAddr[] = {
        0x0000, 0xFFFF
};

static const unsigned short Crc16Table[256] = {
        0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
        0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
        0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
        0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
        0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
        0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
        0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
        0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
        0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
        0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
        0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
        0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
        0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
        0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
        0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
        0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
        0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
        0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
        0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
        0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
        0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
        0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
        0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
        0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
        0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
        0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
        0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
        0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
        0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
        0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
        0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
        0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040
};

static uint16_t Crc16( uint8_t *crc_arr, uint8_t crc_num);
static bool CheckType(uint8_t _type);
static bool CheckMPU(uint8_t _mpu);
static bool CheckRegisterAddr(uint16_t _addr);

//HOST
static uint8_t* CreateRequest(Host* _host);
static bool WriteHostData(Host* _host,  uint8_t* _data, size_t _dataLen);
static enum Error_code IsHostValid(Host* _host);
static void ReadHost(Host* _host, uint8_t _byte);

void CreateHost(Host* _host, uint8_t _mpu, uint16_t _register, uint8_t* _dataBuffer, enum Frame_type _req) {
    //Add buffer  length handler

    ChangeFrameType(_host, _req);

    _host->header.cmd0 = _mpu;
    _host->header.cmd1 = _register;
    _host->header.cmd2 = 0;
    _host->errorCode   = no_error;
    _host->mode = empty;

    _host->WriteData = &WriteHostData;
    _host->CreateRequest = &CreateRequest;
    _host->Read = &ReadHost;
    _host->IsValid = &IsHostValid;
    if(_dataBuffer)
        _host->buffer = _dataBuffer;
    else {
        _host->buffer = NULL;
        _host->errorCode = incorrect_data_value;
    }
}
static void ReadHost(Host* _host, uint8_t _byte){

    switch (_host->mode) {
        case empty: {
            if(_byte == (SOF & 0xff)) {
                _host->mode = sof;
                _host->frameSize = 1;
                _host->buffer[0] = _byte;
            }
        }  break;
        case sof: {
            if(_byte == (SOF >> 8)) {
                _host->mode = len;
                _host->buffer[_host->frameSize] = _byte;
                _host->frameSize++;
            } else {
                _host->mode = empty;
                _host->frameSize = 0;
            }
        }  break;
        case len: {
            if(_host->frameSize == SOF_SIZE) {
                _host->buffer[_host->frameSize] = _byte;
                _host->frameSize++;
            } else {
                _host->buffer[_host->frameSize] = _byte;
                _host->frameSize++;
                _host->mode = header;
            }
        }  break;
        case header: {
            switch (_host->frameSize) {
                case SOF_SIZE + LEN_SIZE://type
                    _host->header.type = _byte;
                case SOF_SIZE + LEN_SIZE + 1://cmd0
                    _host->header.cmd0 = _byte;
                case SOF_SIZE + LEN_SIZE + 2://cmd1(1)
                    _host->header.cmd1 = _byte;
                case SOF_SIZE + LEN_SIZE + 3://cmd1(2)
                    _host->header.cmd1 += _byte << 8;
                case SOF_SIZE + LEN_SIZE + 4://cmd2(1)
                    _host->header.cmd2 = _byte;
                    _host->buffer[_host->frameSize] = _byte;
                    _host->frameSize++;
                    break;
                default://cmd2(2)
                    _host->header.cmd2 += _byte << 8;
                    _host->buffer[_host->frameSize] = _byte;
                    _host->frameSize++;
                    _host->mode = data;
                    break;
            }
        }  break;
        case data: {
            if(_host->frameSize < (_host->header.cmd2 + HEADER_SIZE + SOF_SIZE + LEN_SIZE - 1)) {
                _host->buffer[_host->frameSize] = _byte;
                _host->frameSize++;
            } else {
                _host->buffer[_host->frameSize] = _byte;
                _host->frameSize++;
                _host->mode = fcs;
            }
        }  break;
        case fcs: {
            if(_host->frameSize < (_host->header.cmd2 + HEADER_SIZE + SOF_SIZE + LEN_SIZE + FCS_SIZE - 1)) {
                _host->buffer[_host->frameSize] = _byte;
                _host->frameSize++;
            } else {
                _host->buffer[_host->frameSize] = _byte;
                _host->frameSize++;
                _host->mode = finish;
            }
        }  break;
        case finish: {

        } break;
    }
}
static enum Error_code IsHostValid(Host* _host) {
    if(_host->mode != finish)
        return incorrect_frame_format;

    if(!CheckType(_host->header.type))
        _host->errorCode = incorrect_type;

    if(!CheckMPU(_host->header.cmd0))
        _host->errorCode = incorrect_mpu_address;

    if(!CheckRegisterAddr(_host->header.cmd1))
        _host->errorCode = incorrect_register_address;

    if (_host->header.type == REQR_CODE && _host->header.cmd2 == 0)
        _host->errorCode = incorrect_data_length;

    uint16_t crc = _host->buffer[_host->header.cmd2 + DATA_START_PLACE];
    crc += _host->buffer[_host->header.cmd2 + 1 + DATA_START_PLACE] << 8;

    if (crc != Crc16(_host->buffer + SOF_SIZE, _host->header.cmd2 + HEADER_SIZE + LEN_SIZE))
        _host->errorCode = slave_data_integrity;

    if(_host->frameSize < 12, _host->frameSize < GetHostPackageSize(_host))
        return incorrect_frame_format;

    return _host->errorCode;
}
static bool WriteHostData(Host* _host,  uint8_t* _data, size_t _dataLen) {

    if(_dataLen > (MAX_PROTOCOL_LEN - HEADER_SIZE) || !_data || !_host->buffer)
        return false;

    for(int i = 0; i < _dataLen; i++)
        _host->buffer[i + DATA_START_PLACE] = _data[i];
    _host->header.cmd2 = _dataLen;
    return true;
}
uint16_t GetHostPackageSize(Host* _host) {
    uint16_t size =  HEADER_SIZE;
    size += _host->header.cmd2;

    if(_host->header.type & REQR_CODE)
        return HEADER_SIZE;
    return size;
}
static uint8_t* CreateRequest(Host* _host) {
    if(!_host->buffer)
        return NULL;

    //SOF
    _host->buffer[0] = SOF & 0xff;//0xAA
    _host->buffer[1] = SOF >> 8;//0x55

    //Payload
    //Header
    _host->buffer[4] = _host->header.type;
    _host->buffer[5] = _host->header.cmd0;
    _host->buffer[6] = _host->header.cmd1 & 0xff;
    _host->buffer[7] = _host->header.cmd1 >> 8;

    if(_host->errorCode == no_error) {

        _host->buffer[8] = _host->header.cmd2 & 0xff;
        _host->buffer[9] = _host->header.cmd2 >> 8;

    } else {
        _host->header.type |= 0x80;
        _host->header.cmd2 = 1;
        _host->buffer[8] = 0x01;
        _host->buffer[9] = 0x00;
        _host->buffer[10] = _host->errorCode;
    }
    _host->buffer[2] = GetHostPackageSize(_host) & 0xff;//lsb
    _host->buffer[3] = GetHostPackageSize(_host) >> 8; //msb

    uint16_t crc = Crc16(_host->buffer + LEN_SIZE, _host->header.cmd2 + HEADER_SIZE + LEN_SIZE);
    _host->buffer[_host->header.cmd2 + DATA_START_PLACE] = crc & 0xff;
    _host->buffer[_host->header.cmd2 + DATA_START_PLACE + 1] = crc >> 8 ;


    return _host->buffer;
}

bool SetRegisterAddr(Host* _host, uint16_t _addr) {
    for(int i = 0; i < RESERVED_REGISTER_COUNT; i++) {
        if (_addr == ReservedRegistersAddr[i])
            return false;
    }

    _host->header.cmd1 = _addr;
    return true;
}
void ChangeFrameType(Host* _host, enum Frame_type _type){
    if(_type == REQR)
        _host->header.type = REQR_CODE;
    else
        _host->header.type = REQW_CODE;
}

//SLAVE
static bool WriteSlaveData(Slave* _slave,  uint8_t* _data, size_t _dataLen);
static uint8_t* CreateResponse(Slave*);
static void ReadSlave(Slave* _slave, uint8_t _byte);
static enum Error_code IsSlaveValid(Slave* _slave);

void CreateSlave(Slave* _slave, uint8_t* _dataBuffer) {

    _slave->header.cmd2 = 0;
    _slave->errorCode   = no_error;
    _slave->frameSize   = 0;
    _slave->mode = empty;

    _slave->WriteData = &WriteSlaveData;
    _slave->CreateResponse = &CreateResponse;
    _slave->Read = &ReadSlave;
    _slave->IsValid = &IsSlaveValid;
    if(_dataBuffer)
        _slave->buffer = _dataBuffer;
    else {
        _slave->buffer = NULL;
        _slave->errorCode = incorrect_data_value;
    }
}
static void ReadSlave(Slave* _slave, uint8_t _byte) {

    switch (_slave->mode) {
        case empty: {
            if(_byte == (SOF & 0xff)) {
                _slave->mode = sof;
                _slave->frameSize = 1;
                _slave->buffer[0] = _byte;
            }
        }  break;
        case sof: {
            if(_byte == (SOF >> 8)) {
                _slave->mode = len;
                _slave->buffer[_slave->frameSize] = _byte;
                _slave->frameSize++;
            } else {
                _slave->mode = empty;
                _slave->frameSize = 0;
            }
        }  break;
        case len: {
            if(_slave->frameSize == SOF_SIZE) {
                _slave->buffer[_slave->frameSize] = _byte;
                _slave->frameSize++;
            } else {
                _slave->buffer[_slave->frameSize] = _byte;
                _slave->frameSize++;
                _slave->mode = header;
            }
        }  break;
        case header: {
            switch (_slave->frameSize) {
                case SOF_SIZE + LEN_SIZE://type
                    _slave->header.type = _byte;
                case SOF_SIZE + LEN_SIZE + 1://cmd0
                    _slave->header.cmd0 = _byte;
                case SOF_SIZE + LEN_SIZE + 2://cmd1(1)
                    _slave->header.cmd1 = _byte;
                case SOF_SIZE + LEN_SIZE + 3://cmd1(2)
                    _slave->header.cmd1 += _byte << 8;
                case SOF_SIZE + LEN_SIZE + 4://cmd2(1)
                    _slave->header.cmd2 = _byte;
                    _slave->buffer[_slave->frameSize] = _byte;
                    _slave->frameSize++;
                    break;
                default://cmd2(2)
                    _slave->header.cmd2 += _byte << 8;
                    _slave->buffer[_slave->frameSize] = _byte;
                    _slave->frameSize++;
                    _slave->mode = data;
                    break;
            }
        }  break;
        case data: {
            if(_slave->frameSize < (_slave->header.cmd2 + HEADER_SIZE + SOF_SIZE + LEN_SIZE - 1)) {
                _slave->buffer[_slave->frameSize] = _byte;
                _slave->frameSize++;
            } else {
                _slave->buffer[_slave->frameSize] = _byte;
                _slave->frameSize++;
                _slave->mode = fcs;
            }
        }  break;
        case fcs: {
            if(_slave->frameSize < (_slave->header.cmd2 + HEADER_SIZE + SOF_SIZE + LEN_SIZE + FCS_SIZE - 1)) {
                _slave->buffer[_slave->frameSize] = _byte;
                _slave->frameSize++;
            } else {
                _slave->buffer[_slave->frameSize] = _byte;
                _slave->frameSize++;
                _slave->mode = finish;
            }
        }  break;
    }
}
static enum Error_code IsSlaveValid(Slave* _slave) {

    if(_slave->mode != finish)
        return incorrect_frame_format;

    if(!CheckType(_slave->header.type))
        _slave->errorCode = incorrect_type;

    if(!CheckMPU(_slave->header.cmd0))
        _slave->errorCode = incorrect_mpu_address;

    if(!CheckRegisterAddr(_slave->header.cmd1))
        _slave->errorCode = incorrect_register_address;

    if (_slave->header.type == REQR_CODE && _slave->header.cmd2 != 0)
        _slave->errorCode = incorrect_data_length;

    uint16_t crc = _slave->buffer[_slave->header.cmd2 + DATA_START_PLACE];
    crc += _slave->buffer[_slave->header.cmd2 + 1 + DATA_START_PLACE] << 8;

    if (crc != Crc16(_slave->buffer + LEN_SIZE, _slave->header.cmd2 + HEADER_SIZE + LEN_SIZE))
        _slave->errorCode = slave_data_integrity;

    if(_slave->frameSize < 12 || _slave->frameSize < GetSlavePackageSize(_slave))
        return incorrect_frame_format;

    return _slave->errorCode;
}
static bool WriteSlaveData(Slave* _slave,  uint8_t* _data, size_t _dataLen) {

    if(_dataLen > MAX_PROTOCOL_LEN - HEADER_SIZE || !_data || !_slave->buffer)
        return false;

    for(int i = 0; i < _dataLen; i++)
        _slave->buffer[i + DATA_START_PLACE] = _data[i];
    _slave->header.cmd2 = _dataLen;
    return true;
}
uint16_t GetSlavePackageSize(Slave* _slave) {
    uint16_t size =  HEADER_SIZE;
    size += _slave->header.cmd2;
    if(_slave->header.type & REQW_CODE || _slave->errorCode != no_error)
        return HEADER_SIZE + 1;
    return size;
}
static uint8_t* CreateResponse(Slave* _slave) {
    if(!_slave->buffer)
        return NULL;

    //SOF
    _slave->buffer[0] = SOF & 0xff;//0xAA
    _slave->buffer[1] = SOF >> 8;//0x55
    //DATA len

    //Payload
    //Header
    _slave->buffer[4] = _slave->header.type;
    _slave->buffer[5] = _slave->header.cmd0;
    _slave->buffer[6] = _slave->header.cmd1 & 0xff;
    _slave->buffer[7] = _slave->header.cmd1 >> 8;

    if(_slave->errorCode == no_error) {
        if(_slave->header.type & REQR_CODE){

            _slave->buffer[8] = _slave->header.cmd2 & 0xff;
            _slave->buffer[9] = _slave->header.cmd2 >> 8;

        } else {

            _slave->header.cmd2 = 1;
            _slave->buffer[8] = 0x01;//Size = 1 because it will contain only err code
            _slave->buffer[9] = 0x00;

            _slave->buffer[10] = _slave->errorCode;

        }

    } else {
        SET_1ST_BIT(_slave->buffer[4]);
        _slave->header.cmd2 = 1;
        _slave->buffer[8] = 0x01;//Size = 1 because it will contain only err code
        _slave->buffer[9] = 0x00;

        _slave->buffer[10] = _slave->errorCode;
    }
    _slave->buffer[2] = GetSlavePackageSize(_slave) & 0xff;
    _slave->buffer[3] = GetSlavePackageSize(_slave) >> 8;

    uint16_t crc = Crc16(_slave->buffer + LEN_SIZE, GetSlavePackageSize(_slave) + 2);
    _slave->buffer[_slave->header.cmd2 + DATA_START_PLACE] = crc & 0xff;
    _slave->buffer[_slave->header.cmd2 + 1 + DATA_START_PLACE] = crc >> 8;

    return _slave->buffer;
}

static uint16_t Crc16( uint8_t *crc_arr, uint8_t crc_num)
{
    uint16_t crc = 0xFFFF;

    while (crc_num--)
        crc = (crc >> 8) ^ Crc16Table[(crc & 0xFF) ^ *crc_arr++];

    return crc;
}

static bool CheckType(uint8_t _type) {
    return !(_type & 0x70);
}
static bool CheckMPU(uint8_t _mpu) {
    if(_mpu == 0x00 || _mpu == 0xFF)
        return false;
    return true;
}
static bool CheckRegisterAddr(uint16_t _addr){
    if(_addr == 0x0000 || _addr == 0xFFFF)
        return false;
    return true;
}