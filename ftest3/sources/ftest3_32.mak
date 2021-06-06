# $Id: ftest3_32.mak,v 1.2 2019-08-18 10:25:33+03 dstef Exp root $

target=ftest3
LIB32=/usr/lib32
CC=cc
RM=rm

ALL: $(target) $(target)-static


$(target): $(target).c
	$(CC) -m32 -Wall -D_GNU_SOURCE=1 -D_BSD_SOURCE=1 \
	-L$(LIB32) -B$(LIB32) \
	-o $(target) $(target).c -pthread

$(target)-static: $(target).c
	$(CC) -static -m32 -Wall -D_GNU_SOURCE=1 -D_BSD_SOURCE=1 \
	-L$(LIB32) -B$(LIB32) \
	-o $(target)-static $(target).c -pthread


run:  $(target)
	LD_LIBRARY_PATH=/usr/lib32 ./$(target)


run-static: $(target)-static
	./$(target)-static


clean:
	$(RM) -f $(target)
	$(RM) -f $(target)-static
