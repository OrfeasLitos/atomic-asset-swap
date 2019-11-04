SOURCE=add_fourth.oc
UTIL=../obliv-c/test/oblivc/common/util.c
CFLAGS="-DREMOTE_HOST=localhost -O3 -I"
../obliv-c/bin/oblivcc ${CFLAGS} . ${SOURCE} add.c ${UTIL}

PORT=`cat port`
echo $((PORT + 1)) > port
./a.out ${PORT} 1 15 &
./a.out ${PORT} 2 6
rm a.out
