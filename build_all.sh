#!/bin/bash

mkdir bin

pushd bin
clang -I "../untyped" -I "../lib" -I "../utils" -o untyped "../untyped/untyped.c"
popd

