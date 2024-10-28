FLAGS=-m32 -g2 -fstack-protector-all

all: byterun

%.o: %.c
	$(CC) $(FLAGS) -g -c $<

OBJ:=$(filter-out disasm.o, $(patsubst %.c,%.o,$(wildcard *.c)))

byterun: $(OBJ) ../runtime/runtime.a
	$(CC) $(FLAGS) -o byterun $(OBJ) ../runtime/runtime.a

disasm: disasm.o bytefile.o
	$(CC) $(FLAGS) -o disasm disasm.o bytefile.o ../runtime/runtime.a

clean:
	$(RM) *.a *.o *~ byterun disasm
