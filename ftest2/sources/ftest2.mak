# $Id: ftest2.mak,v 1.1 2006-06-25 00:09:56+04 dstef Exp root $

target=ftest2

$(target): $(target).c
	cc -Wall -D_GNU_SOURCE=1 -D_BSD_SOURCE=1 -o $(target) $(target).c -pthread

static: $(target).c
	cc -static -Wall -D_GNU_SOURCE=1 -D_BSD_SOURCE=1 -o $(target) $(target).c -pthread
	
clean:
	rm -f $(target)
