KEY_FILE=key_file
KEY_HASH_FILE=key_hash_file
ASSET_FILE=movie.mp4
AES_BLOCK_SIZE=16
ASSET_HASH_FILE=asset_hash_file
CIPHER_FILE=cipher_file
EXECUTABLE=bin

SOURCE="asset-swap.oc asset-swap.c"
UTIL="../obliv-c/test/oblivc/common/util.c aes.oc sha.oc"
CFLAGS="-lssl -lcrypto -DREMOTE_HOST=localhost -O3 -I"

set -e

ASSET_SIZE=`stat --printf=%s ${ASSET_FILE}`
ASSET_SIZE_ROUNDED=$((${ASSET_SIZE} + (${AES_BLOCK_SIZE} - 1 - \
  ((${ASSET_SIZE} - 1) % ${AES_BLOCK_SIZE}))))

sed -i "/ASSET_PLAIN_SIZE/c\#define ASSET_PLAIN_SIZE ${ASSET_SIZE_ROUNDED}" asset-swap.h
sed -i "/ASSET_CIPHER_SIZE/c\#define ASSET_CIPHER_SIZE ${ASSET_SIZE_ROUNDED}" asset-swap.h

../obliv-c/bin/oblivcc ${CFLAGS} . ${SOURCE} ${UTIL} -o ${EXECUTABLE}

python encrypt-file.py --key ${KEY_FILE} --plaintext ${ASSET_FILE} --cipher ${CIPHER_FILE}
python hash-file.py --preimage ${ASSET_FILE} --hash ${ASSET_HASH_FILE}
python hash-file.py --preimage ${KEY_FILE} --hash ${KEY_HASH_FILE}

PORT=`cat port`
echo $((PORT + 1)) > port

# increase stack size to accommodate big files
# this ugly hack alone makes this non-production ready
# multiplier chosen experimentally
ORIGINAL_STACK_SIZE=`ulimit -s`
ulimit -Ss $((1000*${ASSET_SIZE_ROUNDED}))

./${EXECUTABLE} ${PORT} 1 ${CIPHER_FILE} ${ASSET_HASH_FILE} ${KEY_HASH_FILE} &
./${EXECUTABLE} ${PORT} 2 ${ASSET_FILE} ${KEY_FILE}

# revert stack size and C source to original
ulimit -Ss ${ORIGINAL_STACK_SIZE}
sed -i "/ASSET_PLAIN_SIZE/c\#define ASSET_PLAIN_SIZE 1024" asset-swap.h
sed -i "/ASSET_CIPHER_SIZE/c\#define ASSET_CIPHER_SIZE 1024" asset-swap.h

rm ${EXECUTABLE} ${CIPHER_FILE} ${ASSET_HASH_FILE} ${KEY_HASH_FILE}
