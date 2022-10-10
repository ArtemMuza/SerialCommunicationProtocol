# # SerialCommunicationProtocol
## Install & build
Build :
```bash
git clone https://github.com/ArtemMuza/SerialCommunicationProtocol.git
cd SerialCommunicationProtocol
mkdir build && cd build
cmake ..
make
```
Library out directory:
```
SerialCommunicationProtocol/build/build/lib/libSCP.a
```
Tests out directory:
```
SerialCommunicationProtocol/build/build/tests/
```

## How to use
- Allocating buffers for Host|Slave (max_length = 1024)
```c++
uint8_t hostBuffer[1024];
uint8_t slaveBuffer[1024];
```
- Create Host|Slave objects
```c++
    Host pc;
    Slave stm;
```
- Set buffer & parameters to Host|Slave objects
```c++
    CreateHost(&pc, addr_linux, 18, hostBuffer, REQR);
    CreateSlave(&stm, slaveBuffer);
```
Read Script:
```c++
    uint8_t* data = pc.CreateRequest(&pc);

    for(int i = 0; i < REQUEST_SIZE(pc) ; i++) 
        stm.Read(&stm, *(data+i));
    if(stm.IsValid(&stm) != no_error)
        exit(1);
    if(REQ_TYPE(stm)) { //Check read or write request was
        // get data from register addr:
        //GET_REG_ADDR(stm);
        stm.WriteData(&stm, regData, 8);

        data = stm.CreateResponse(&stm);
        for(int i = 0; i < RESPONSE_SIZE(stm) ; i++) 
            pc.Read(&pc, *(data + i));
        if(pc.IsValid(&pc) == no_error) {}
        //Now we get data in  GET_DATA_PTR(linux)
        //And them lenght in  GET_DATA_LEN(linux)
    }
```
Write Script:
```c++
    pc.WriteData(&pc, regData, 8);

    uint8_t* data = pc.CreateRequest(&pc);
    for(int i = 0; i < REQUEST_SIZE(pc); i++) 
        stm.Read(&stm, *(data+i));
    if(stm.IsValid(&stm) != no_error)
        exit(1);
    if(!REQ_TYPE(stm)){
        data = stm.CreateResponse(&stm);
        for(int i = 0; i < RESPONSE_SIZE(stm); i++) 
            pc.Read(&pc, *(data + i));
        switch(pc.IsValid(&pc) {}
        //Now we have error code in pc data, no_error if all is ok
    }
```
