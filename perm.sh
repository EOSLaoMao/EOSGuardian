#!/bin/bash

ACCOUNT=$1
PKEY=$2
API=${3:-http://localhost:8888}

cleos set account permission $ACCOUNT guardianperm '{"threshold": 1,"keys": [],"accounts": [{"permission":{"actor":"'$ACCOUNT'","permission":"eosio.code"},"weight":1}]}'  "active" -p $ACCOUNT@active
cleos set action permission $ACCOUNT eosio delegatebw guardianperm -p $ACCOUNT@active
cleos set action permission $ACCOUNT eosio.token transfer guardianperm -p $ACCOUNT@active

cleos set account permission $ACCOUNT safeperm '{"threshold": 1,"keys": [{"key": "'$PKEY'","weight": 1}],"accounts": [{"permission":{"actor":"'$ACCOUNT'","permission":"eosio.code"},"weight":1}]}'  "active" -p $ACCOUNT@active
cleos set action permission $ACCOUNT $ACCOUNT safedelegate safeperm -p $ACCOUNT@active
cleos set action permission $ACCOUNT $ACCOUNT safetransfer safeperm -p $ACCOUNT@active
