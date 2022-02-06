COMPILER=../../../build/jack2llvm
LLC=/usr/local/bin/llc
GCC=/usr/local/gcc-8.2/bin/gcc

DRIVER_FILE=test.cpp
JACK_FILES=Base.jack Derived.jack Main.jack

all: ${DRIVER_FILE} ${JACK_FILES}
	# basic lexer & parser
	${COMPILER} ${JACK_FILES} > jack.ll
	${LLC} -filetype=obj jack.ll -o jack.o
	${GCC} ${DRIVER_FILE} jack.o -o main

ll: ${DRIVER_FILE} ${JACK_FILES}
	# basic lexer & parser
	${LLC} -filetype=obj jack.ll -o jack.o
	${GCC} ${DRIVER_FILE} jack.o -o main

clean:
	rm -rf *.ll *.o main
