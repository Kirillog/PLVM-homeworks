FLAGS=-m32

all: byterun verifier disasm

%.o: %.c
	$(CC) $(FLAGS) -g -c $<

%.o: %.cpp
	$(CXX) $(FLAGS) -g -c $<

OBJ_C:=$(filter-out disasm.o verifier.o, $(patsubst %.c,%.o,$(wildcard *.c)))
OBJ_CXX:=$(patsubst %.cpp,%.o,$(wildcard *.cpp))

byterun: $(OBJ_C) verify.o ../runtime/runtime.a
	$(CXX) $(FLAGS) -o byterun $(OBJ_C) verify.o ../runtime/runtime.a

verifier: bytefile.o verifier.o verify.o ../runtime/runtime.a
	$(CXX) $(FLAGS) -o verifier verifier.o bytefile.o verify.o ../runtime/runtime.a

disasm: disasm.o bytefile.o ../runtime/runtime.a
	$(CC) $(FLAGS) -o disasm disasm.o bytefile.o ../runtime/runtime.a

clean:
	$(RM) *.a *.o *~ byterun disasm
