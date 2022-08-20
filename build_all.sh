#!/bin/bash

mkdir bin

pushd bin
clang -o untyped -I "../untyped" -I "../lib" -I "../utils" "../untyped/untyped.c"
popd
