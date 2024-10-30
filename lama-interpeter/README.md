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