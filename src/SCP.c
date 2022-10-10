#include "SCP.h"
#include <string.h>

#define SET_1ST_BIT(x) (x |= 0x80)
#define UNSET_1ST_BIT(x) (x &= 0x7F)

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
#ifndef UNIT_TESTS
static uint16_t Crc16( uint8_t *crc_arr, uint8_t crc_num);
static bool CheckType(uint8_t _type);
static bool CheckMPU(uint8_t _mpu);
static bool CheckRegisterAddr(uint16_t _addr);
#endif
static enum Error_code     IsValid(Header* _header, uint8_t* _buffer, size_t _bufferSize, int _frameSize) {

    _header->errorCode = no_error;
    if(!_buffer || _bufferSize < 6)
        return incorrect_data_length;

    if(!CheckType(_header->type))
        _header->errorCode = incorrect_type;

    if(!CheckMPU(_header->cmd0))
        _header->errorCode = incorrect_mpu_address;

    if(!CheckRegisterAddr(_header->cmd1))
        _header->errorCode = incorrect_register_address;

    uint16_t crc = _buffer[_header->cmd2 + DATA_START_PLACE];
    crc += _buffer[_header->cmd2 + 1 + DATA_START_PLACE] << 8;


    if( _bufferSize < _header->len)
        return incorrect_data_length;

    if (crc != Crc16(_buffer + SOF_SIZE, _header->len + LEN_SIZE))
        _header->errorCode = slave_data_integrity;

    if(_header->mode != finish)
        _header->errorCode = incorrect_frame_format;

    if(_header->errorCode != no_error)
        SET_1ST_BIT(_header->type);
    else
        UNSET_1ST_BIT(_header->type);
    if(_frameSize < 12 || _frameSize < (_header->cmd2 + 6))
        return incorrect_frame_format;

    return _header->errorCode;
}
static bool WriteData(Header* _header, uint8_t* _buffer, uint8_t* _data, size_t _dataLen) {
    if(_dataLen > (MAX_PROTOCOL_LEN - HEADER_SIZE) || !_data || !_buffer)
        return false;

    for(int i = 0; i < _dataLen; i++)
        _buffer[i + DATA_START_PLACE] = _data[i];
    _header->cmd2 = _dataLen;
    return true;
}
static uint8_t* Serialize(Header* _header, uint8_t* _buffer) {
    if(!_buffer)
        return NULL;

    //SOF
    _buffer[0] = SOF & 0xff;//0xAA
    _buffer[1] = SOF >> 8;//0x55

    //Payload
    //Header
    _buffer[4] = _header->type;
    _buffer[5] = _header->cmd0;
    _buffer[6] = _header->cmd1 & 0xff;
    _buffer[7] = _header->cmd1 >> 8;

    if(_header->errorCode == no_error) {

        _buffer[8] = _header->cmd2 & 0xff;
        _buffer[9] = _header->cmd2 >> 8;
    } else {
        _header->type |= 0x80;
        _header->cmd2 = 1;
        _buffer[8] = 0x01;
        _buffer[9] = 0x00;
        _buffer[10] = _header->errorCode;
    }
    _buffer[2] = (_header->cmd2 + HEADER_SIZE) & 0xff;//lsb
    _buffer[3] = (_header->cmd2 + HEADER_SIZE) >> 8; //msb

    uint16_t crc = Crc16(_buffer + SOF_SIZE, _header->cmd2 + HEADER_SIZE + LEN_SIZE);
    _buffer[_header->cmd2 + DATA_START_PLACE] = crc & 0xff;
    _buffer[_header->cmd2 + DATA_START_PLACE + 1] = crc >> 8 ;

    return _buffer;
}
static void     DeserializePayload(Header* _header, uint8_t* _buffer, uint8_t _byte, int* _frameSize) {
    switch (_header->mode) {
        case header: {
            switch (*_frameSize) {
                case SOF_SIZE + LEN_SIZE://type
                    _header->type = _byte;
                case SOF_SIZE + LEN_SIZE + 1://cmd0
                    _header->cmd0 = _byte;
                case SOF_SIZE + LEN_SIZE + 2://cmd1(1)
                    _header->cmd1 = _byte;
                case SOF_SIZE + LEN_SIZE + 3://cmd1(2)
                    _header->cmd1 += _byte << 8;
                case SOF_SIZE + LEN_SIZE + 4://cmd2(1)
                    _header->cmd2 = _byte;
                    _buffer[*_frameSize] = _byte;
                    (*_frameSize)++;
                    break;
                default://cmd2(2)
                    _header->cmd2 += _byte << 8;
                    _buffer[*_frameSize] = _byte;
                    (*_frameSize)++;
                    _header->mode = data;
                    break;
            }
        }  break;
        case data: {
                _buffer[*_frameSize] = _byte;
                (*_frameSize)++;
        }  break;
    }
}
static void     DeserializeFrame(Header* _header, uint8_t* _buffer, uint8_t _byte, int* _frameSize) {
    switch (_header->mode) {
        case empty: {
            _header->errorCode = no_error;
            if(_byte == (SOF & 0xff)) {
                _header->mode = sof;
                *_frameSize = 1;
                _buffer[0] = _byte;
            }
        }  break;
        case sof: {
            _header->errorCode = incorrect_frame_format;
            if(_byte == (SOF >> 8)) {
                _header->mode = len;
                _buffer[*_frameSize] = _byte;
                (*_frameSize)++;
            } else {
                _header->mode = empty;
                *_frameSize = 0;
            }
        }  break;
        case len: {
            if(*_frameSize == SOF_SIZE) {
                _buffer[*_frameSize] = _byte;
                (*_frameSize)++;
                _header->len = _byte;
            } else {
                _buffer[*_frameSize] = _byte;
                (*_frameSize)++;
                _header->mode = header;
                _header->len += _byte << 8;
            }
        }  break;
        case fcs: {
            if(*_frameSize < (_header->len + SOF_SIZE + LEN_SIZE + FCS_SIZE - 1)) {
                _buffer[*_frameSize] = _byte;
                (*_frameSize)++;
            } else {
                _buffer[*_frameSize] = _byte;
                (*_frameSize)++;
                _header->mode = finish;
                _header->errorCode = IsValid(_header, _buffer, *_frameSize, *_frameSize);
            }
        }  break;
        case finish: {

            _header->mode = empty;
            DeserializeFrame(_header, _buffer, _byte,  _frameSize);
        } break;
        default: {

            if (*_frameSize < ( _header->len + LEN_SIZE + SOF_SIZE))
                DeserializePayload(_header, _buffer, _byte, _frameSize);
            else {
                _header->mode = fcs;
                DeserializeFrame(_header, _buffer, _byte, _frameSize);
            }
        } break;
    }
}



//HOST
static uint8_t* CreateRequest(Host* _host);
static bool WriteHostData(Host* _host,  uint8_t* _data, size_t _dataLen);
static enum Error_code IsHostValid(Host* _host);
static void ReadHost(Host* _host, uint8_t _byte);

void CreateHost(Host* _host, uint8_t* _dataBuffer, size_t _bufferSize, uint8_t _mpu, uint16_t _register, enum Frame_type _req) {


    ChangeFrameType(_host, _req);

    _host->header.cmd0 = _mpu;
    _host->header.cmd1 = _register;
    _host->header.cmd2 = 0;
    _host->header.errorCode   = no_error;
    _host->header.mode = empty;
    _host->bufferSize = _bufferSize;
    _host->frameSize = 0;

    _host->WriteData = &WriteHostData;
    _host->CreateRequest = &CreateRequest;
    _host->Read = &ReadHost;
    _host->IsValid = &IsHostValid;
    if(_dataBuffer && (_bufferSize > 6 && _bufferSize < MAX_PROTOCOL_LEN + 1))
        _host->buffer = _dataBuffer;
    else {
        _host->buffer = NULL;
        _host->header.errorCode = incorrect_data_length;
    }
}
static void ReadHost(Host* _host, uint8_t _byte){
    if(_host->frameSize >= _host->bufferSize){
        _host->header.errorCode = incorrect_data_length;
        return;
    }
    DeserializeFrame(&_host->header, _host->buffer, _byte, &_host->frameSize);
}
static enum Error_code IsHostValid(Host* _host) {

    if (_host->header.type == REQR_CODE && _host->header.cmd2 == 0 && _host->header.mode == finish)
        _host->header.errorCode = incorrect_data_length;

    return IsValid(&_host->header, _host->buffer, _host->bufferSize, _host->frameSize);
}
static bool WriteHostData(Host* _host,  uint8_t* _data, size_t _dataLen) {
    return WriteData(&_host->header, _host->buffer, _data, _dataLen);
}
uint16_t GetHostPackageSize(Host* _host) {

    if(_host->header.type & REQR_CODE)
        return HEADER_SIZE;
    return HEADER_SIZE + _host->header.cmd2;
}
static uint8_t* CreateRequest(Host* _host) {

    if(_host->header.type & REQR_CODE) {
        _host->WriteData(_host, _host->buffer, 0);
    }

    return Serialize(&_host->header, _host->buffer);
}

bool SetRegisterAddr(Host* _host, uint16_t _addr) {
    if(CheckRegisterAddr(_addr))
    {
        _host->header.cmd1 = _addr;
        return true;
    }
    return false;
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

void CreateSlave(Slave* _slave, uint8_t* _dataBuffer, size_t _bufferSize) {

    _slave->header.cmd2 = 0;
    _slave->header.errorCode   = no_error;
    _slave->frameSize   = 0;
    _slave->header.mode = empty;
    _slave->bufferSize = _bufferSize;
    _slave->frameSize = 0;

    _slave->WriteData = &WriteSlaveData;
    _slave->CreateResponse = &CreateResponse;
    _slave->Read = &ReadSlave;
    _slave->IsValid = &IsSlaveValid;
    if(_dataBuffer && (_bufferSize > 6 && _bufferSize < MAX_PROTOCOL_LEN + 1))
        _slave->buffer = _dataBuffer;
    else {
        _slave->buffer = NULL;
        _slave->header.errorCode = incorrect_data_value;
    }
}
static void ReadSlave(Slave* _slave, uint8_t _byte) {
    if(_slave->frameSize >= _slave->bufferSize){
        _slave->header.errorCode = incorrect_data_length;
        return;
    }
    DeserializeFrame(&_slave->header, _slave->buffer, _byte, &_slave->frameSize);
}
static enum Error_code IsSlaveValid(Slave* _slave) {

    if (_slave->header.type == REQR_CODE && _slave->header.cmd2 != 0 && _slave->header.mode == finish)
        _slave->header.errorCode = incorrect_data_length;

    return IsValid(&_slave->header,  _slave->buffer, _slave->bufferSize, _slave->frameSize);
}
static bool WriteSlaveData(Slave* _slave,  uint8_t* _data, size_t _dataLen) {
    return WriteData(&_slave->header, _slave->buffer, _data, _dataLen);
}
uint16_t GetSlavePackageSize(Slave* _slave) {

    if(_slave->header.type & REQW_CODE || _slave->header.errorCode != no_error)
        return HEADER_SIZE + 1;
    return HEADER_SIZE + _slave->header.cmd2;
}
static uint8_t* CreateResponse(Slave* _slave) {

    if(_slave->header.type & REQW_CODE) {
        uint8_t err = _slave->header.errorCode;
        _slave->WriteData(_slave, &err, 1);
    }
    return Serialize(&_slave->header, _slave->buffer);
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