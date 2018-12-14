#!/bin/bash

dir=$(dirname $0)

cmd() {
        echo "$@"
        "$@"
}

echo P4:
cmd cloc --force-lang=C++,p4 $dir/../p4/*.p4
echo ----

echo Firewall:
cmd cloc $(find $dir/../hls/ -maxdepth 1 -name '*.[hc]pp' -not -name main.cpp)
echo ----

echo Library:
cmd cloc $dir/../hls/ntl/
echo ----
