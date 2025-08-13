#!/bin/bash

# clean up the repo
DIR_PROJECT_ROOT=$(pwd)/'..'

find ${DIR_PROJECT_ROOT}/images -name "*.raw" | xargs rm
rm -rf ${DIR_PROJECT_ROOT}/experiments/simbricks/out

# build image
docker build --tag mjccjm/nexdsim:sosp25_ae --file ${DIR_PROJECT_ROOT}/docker/Dockerfile ${DIR_PROJECT_ROOT}
