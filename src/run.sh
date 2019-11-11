KEY_FILE=key
ASSET_FILE=movie.mp4
CIPHER_FILE=cipher
EXECUTABLE=bin

SOURCE="asset-swap.oc asset-swap.c"
UTIL="../obliv-c/test/oblivc/common/util.c aes.oc"
CFLAGS="-lssl -lcrypto -DREMOTE_HOST=localhost -O3 -I"

python encrypt-file.py --key ${KEY_FILE} --plaintext ${ASSET_FILE} --cipher ${CIPHER_FILE}

../obliv-c/bin/oblivcc ${CFLAGS} . ${SOURCE} ${UTIL} -o ${EXECUTABLE}

if [[ -f "${EXECUTABLE}" ]]; then
  PORT=`cat port`
  echo $((PORT + 1)) > port
  ./${EXECUTABLE} ${PORT} 1 ${CIPHER_FILE} &
  ./${EXECUTABLE} ${PORT} 2 ${ASSET_FILE}
  rm ${EXECUTABLE} ${CIPHER_FILE}
fi
