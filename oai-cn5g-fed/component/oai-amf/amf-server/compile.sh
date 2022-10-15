DIR="build"
if [ -d "$DIR" ]; then
  echo "Removing old ${DIR}..."
  sudo rm -r ${DIR}
fi
mkdir ${DIR}
cmake -B build -S .
cmake --build build
# ./build/lmf_client_run 172.17.0.2:9080/determineLocation
# docker cp lmf_client/build/lmf_client_run oai-amf:/home
