FLAGS.C = gcc -c -Wall -ansi -pedantic 

FLAGS.G = gcc -g -Wall -ansi -pedantic 

OBJECTS = assembler.o pre_assembler.o first_pass.o second_pass.o manage_bit_line.o error_messages.o structure.o

HEADERS = constant.h pre_assembler.h first_pass.h second_pass.h manage_bit_line.h error_messages.h structure.h

assembler: $(OBJECTS)
	$(FLAGS.G) $(OBJECTS) -lm -o assembler

assembler.o: assembler.c $(HEADERS)
	$(FLAGS.C) assembler.c -o assembler.o

structure.o: structure.c $(HEADERS)
	$(FLAGS.C) structure.c -o structure.o

pre_assembler.o: pre_assembler.c $(HEADERS)
	$(FLAGS.C) pre_assembler.c -o pre_assembler.o

first_pass.o: first_pass.c $(HEADERS)
	$(FLAGS.C) first_pass.c -o first_pass.o

second_pass.o: second_pass.c $(HEADERS)
	$(FLAGS.C) second_pass.c -o second_pass.o

manage_bit_line.o: manage_bit_line.c $(HEADERS) 
	$(FLAGS.C) manage_bit_line.c -lm -o manage_bit_line.o

error_messages.o: error_messages.c $(HEADERS)
	$(FLAGS.C) error_messages.c -o error_messages.o
