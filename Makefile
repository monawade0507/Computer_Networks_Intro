#
# Makefile
# Computer Networking Programing Assignments
#

CXX = g++
LD = g++
CXXFLAGS = -g -pthread -std=c++11
LDFLAGS = -g -pthread

#
# Any libraries we might need.
#
LIBRARYS = -lpthread

dug: dug.o dug_help.o log.o
	${LD} ${LDFLAGS} dug.o dug_help.o log.o -o $@ ${LIBRARYS}

dug.o : dug.cc dug.h
	${CXX} -c ${CXXFLAGS} -o $@ $<

dug_help.o : dug_help.cc dug_help.h
	${CXX} -c ${CXXFLAGS} -o $@ $<

log.o : log.cc log.h
	${CXX} -c ${CXXFLAGS} -o $@ $<


#
# Please remember not to submit objects or binarys.
#
clean:
	rm -f core dug.o dug_help.o log.o dug

#
# This might work to create the submission tarball in the formal I asked for.
#
submit:
	rm -f core project2 dug.o dug_help.o log.o dug
	mkdir `whoami`
	cp Makefile README.txt *.h *.cc `whoami`
	tar cf `whoami`.tar `whoami`
