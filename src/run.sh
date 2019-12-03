KEY_FILE=key_file
KEY_HASH_FILE=key_hash_file
ASSET_FILE=movie.mp4
ASSET_HASH_FILE=asset_hash_file
CIPHER_FILE=cipher_file
EXECUTABLE=bin

SOURCE="asset-swap.oc asset-swap.c"
UTIL="../obliv-c/test/oblivc/common/util.c aes.oc sha.oc"
CFLAGS="-lssl -lcrypto -DREMOTE_HOST=localhost -O3 -I"

set -e

../obliv-c/bin/oblivcc ${CFLAGS} . ${SOURCE} ${UTIL} -o ${EXECUTABLE}

python encrypt-file.py --key ${KEY_FILE} --plaintext ${ASSET_FILE} --cipher ${CIPHER_FILE}
python hash-file.py --preimage ${ASSET_FILE} --hash ${ASSET_HASH_FILE}
python hash-file.py --preimage ${KEY_FILE} --hash ${KEY_HASH_FILE}

PORT=`cat port`
echo $((PORT + 1)) > port
./${EXECUTABLE} ${PORT} 1 ${CIPHER_FILE} ${ASSET_HASH_FILE} ${KEY_HASH_FILE} &
./${EXECUTABLE} ${PORT} 2 ${ASSET_FILE} ${KEY_FILE}
rm ${EXECUTABLE} ${CIPHER_FILE} ${ASSET_HASH_FILE} ${KEY_HASH_FILE}
