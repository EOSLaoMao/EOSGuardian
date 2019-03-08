#!/bin/bash

IMG_NAME=eoslaomao/eos-dev:1.4.0
PROJECT_NAME=eosguardian
WORKING_DIR=eosguardian

docker ps | grep $PROJECT_NAME-eos-dev

if [ $? -ne 0 ]; then
    echo "Run eos dev env "
    docker run --name $PROJECT_NAME-eos-dev -dit --rm -v  `(pwd)`:/$PROJECT_NAME $IMG_NAME
fi

docker exec $PROJECT_NAME-eos-dev eosio-cpp --contract $PROJECT_NAME \
    -abigen /$WORKING_DIR/src/eosguardian.cpp \
    -o $PROJECT_NAME.wasm \
    -I /contracts/eosio.token/include \
    -I /contracts/eosio.system/include \

docker exec $PROJECT_NAME-eos-dev cp /$PROJECT_NAME.abi /$PROJECT_NAME.wasm /$PROJECT_NAME/

docker cp ../$PROJECT_NAME nodeosd:/
