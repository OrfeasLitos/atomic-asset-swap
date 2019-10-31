OBLIVCPATH=../obliv-c
UTIL=$(OBLIVCPATH)/test/oblivc/common/util.c
NAME=add
CFLAGS=-DREMOTE_HOST=localhost -O3

./a.out: $(NAME).oc $(NAME).c $(UTIL) $(OBLIVCPATH)/_build/libobliv.a
	$(OBLIVCPATH)/bin/oblivcc $(CFLAGS) -I . $(NAME).oc $(NAME).c $(UTIL)
