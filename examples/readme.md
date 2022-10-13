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
