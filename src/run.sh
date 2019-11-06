SOURCE=asset-swap.oc
UTIL=../obliv-c/test/oblivc/common/util.c
CFLAGS="-lssl -lcrypto -DREMOTE_HOST=localhost -O3 -I"
../obliv-c/bin/oblivcc ${CFLAGS} . ${SOURCE} asset-swap.c ${UTIL}

PORT=`cat port`
echo $((PORT + 1)) > port
./a.out ${PORT} 1 &
./a.out ${PORT} 2 movie.mp4
rm a.out
