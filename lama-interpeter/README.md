# Lama interpeter
## Build
```bash
make
```
## Regression tests
For running tests I use `lamac` bytecode generation with -b option, so you should have lamac installed in your system.
```bash
make regression-all
```
Expression tests are not very representative and pretty slow, for speed check use `make regression`.
## Performance tests
```bash
make -C performance
```
Local results is following:
```
Sort
Sort iter       4.23
Sort (-i) rec   7.44
Sort (-s) rec   2.81
```
May fluctuate from run to run, but in general recursive implementation faster.