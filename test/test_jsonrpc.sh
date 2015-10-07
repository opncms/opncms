#!/bin/sh

curl -X POST -d '{"id":1,"method":"system.listMethods","params":[]}' -H 'content-type: application/json' http://127.0.0.1:8090/rpc
