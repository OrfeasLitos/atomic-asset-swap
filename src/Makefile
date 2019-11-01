OBLIVCPATH=../obliv-c
UTIL=$(OBLIVCPATH)/test/oblivc/common/util.c
NAME=add
SUBNAME=add_fourth
CFLAGS=-DREMOTE_HOST=localhost -O3

./a.out: $(SUBNAME).oc $(NAME).c $(UTIL) $(OBLIVCPATH)/_build/libobliv.a
	$(OBLIVCPATH)/bin/oblivcc $(CFLAGS) -I . $(SUBNAME).oc $(NAME).c $(UTIL)
