KEY_FILE=key
ASSET_FILE=movie.mp4
CIPHER_FILE=cipher

SOURCE=asset-swap.oc
UTIL=../obliv-c/test/oblivc/common/util.c
CFLAGS="-lssl -lcrypto -DREMOTE_HOST=localhost -O3 -I"

python encrypt-file.py --key ${KEY_FILE} --plaintext ${ASSET_FILE} --cipher ${CIPHER_FILE}

../obliv-c/bin/oblivcc ${CFLAGS} . ${SOURCE} asset-swap.c ${UTIL}

PORT=`cat port`
echo $((PORT + 1)) > port
./a.out ${PORT} 1 ${CIPHER_FILE} &
./a.out ${PORT} 2 ${ASSET_FILE}
rm a.out ${CIPHER_FILE}
