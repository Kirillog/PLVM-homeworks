# Lama interpeter
## Build
Required `gcc-multilib` and `g++-multilib` to be installed.
```bash
make
```
## Regression tests
For running tests I use `lamac` bytecode generation with -b option, so you should have lamac installed in your system.
```bash
make regression
```
```bash
make -C performance
```
Local results is following:
```
Sort
Sort iter (ver) 0.00
Sort iter (run) 4.05
Sort (-i) rec   7.38
Sort (-s) rec   3.01
```
May fluctuate from run to run, but in general recursive implementation faster, and verification doesn't spend any significant time.