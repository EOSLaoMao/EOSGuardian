#!/bin/bash

PROJECT=eosguardian
CONTAINER=$PROJECT-eos-dev

docker ps | grep $CONTAINER
if [ $? -ne 0 ]; then
    echo "Run eos dev env "
    docker run --name $CONTAINER -dit --rm -v  `(pwd)`:/$PROJECT eoslaomao/eos-dev:1.2.3
fi

docker exec $CONTAINER eosiocpp -g /$PROJECT/$PROJECT.abi /$PROJECT/$PROJECT.cpp
docker exec $CONTAINER eosiocpp -o /$PROJECT/$PROJECT.wast /$PROJECT/$PROJECT.cpp 
docker cp ../$PROJECT nodeosd:/
