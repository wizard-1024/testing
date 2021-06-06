# $Id: ftest.mak,v 1.3 2003-10-10 15:13:26+04 dstef Exp root $

target=ftest

$(target): $(target).c
	cc -Wall -D_GNU_SOURCE=1 -D_BSD_SOURCE=1 -o $(target) $(target).c

clean:
	rm -f $(target)
