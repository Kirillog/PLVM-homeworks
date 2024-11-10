# Lama bytecode analyzer

### Build
```sh
cmake -B build && cmake --build build
```
### Run
```sh
./build/bytecode_analyzer
```
### Test
Sort.bc produces following output:
```
Count 29 of len 1:
DROP
Count 27 of len 1:
DUP
Count 21 of len 1:
ELEM
Count 16 of len 1:
CONST	1
Count 13 of len 2:
CONST	1
ELEM
...
```