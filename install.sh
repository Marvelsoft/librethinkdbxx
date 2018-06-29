#!/bin/bash

cp build/librethinkdb++* /opt1/lib/
mkdir -p /opt1/include/rethinkdb
cp src/*.h /opt1/include/rethinkdb/
