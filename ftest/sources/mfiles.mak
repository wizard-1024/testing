# $Id: mfiles.mak,v 1.1 2005-07-18 19:02:33+04 dstef Exp root $

target=mfiles

$(target): $(target).c
	cc -Wall -D_GNU_SOURCE=1 -D_BSD_SOURCE=1 -o $(target) $(target).c -pthread

clean:
	rm -f $(target)
