CPPSRC= $(wildcard *.cpp)
DEST= $(CPPSRC:.cpp=.spdl)

default:	$(DEST)

%.spdl:	%.cpp
	cpp $< >$@


