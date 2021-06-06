# $Id: ftest3.mak,v 1.2 2019-08-08 09:45:01+03 dstef Exp root $

target=ftest3

CC=cc
RM=rm

$(target): $(target).c
	$(CC) -Wall -D_GNU_SOURCE=1 -D_BSD_SOURCE=1 -o $(target) $(target).c -pthread

static: $(target).c
	$(CC) -static -Wall -D_GNU_SOURCE=1 -D_BSD_SOURCE=1 -o $(target) $(target).c -pthread
	
clean:
	$(RM) -f $(target)
