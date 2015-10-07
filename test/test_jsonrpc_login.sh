#!/bin/sh

curl -X POST -d '{"id":1,"method":"login","params":["kpeo","123"]}' -H 'content-type: application/json' http://127.0.0.1:8090/rpc

