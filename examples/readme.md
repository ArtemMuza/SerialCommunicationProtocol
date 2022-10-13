# Examples
## Build instruction

Select
```bash
cd <Selected example dir>
mkdir build && cd build
```
Cmake generating
 - Use Makefiles generator (-G "Unix/MinGW Makefiles")
```bash
cmake ..
```
Build
```bash
cmake --build . -- -j 4
````
Output file: example_name.elf


# SCP_uart_example

Usage of Serial Communication Protocol between two uart

Settings:
   - USART2 - Outputing logs (PA2-TX, PA3-RX)
   - USART4 - (PA0 - TX , PA1 - RX) (Host)
   - USART5 - (PC12 - TX, PD2 - RX) (Slave)
