#!/bin/bash

FLAGS="-DDEBUG=OFF -DCMAKE_BUILD_TYPE=Release"
RUN=false

while [[ $# -gt 0 ]]; do
  case $1 in
    -d|--debug)
      FLAGS="-DDEBUG=ON -DCMAKE_BUILD_TYPE=Debug"
      shift # past argument
      ;;
    -r|--run)
      RUN=true
      shift # past argument
      ;;
    -*|--*)
      echo "Unknown option $1"
      exit 1
      ;;
    *)
      POSITIONAL_ARGS+=("$1") # save positional arg
      shift # past argument
      ;;
  esac
done

(mkdir bin -p && cd bin && cmake .. $FLAGS && cmake --build . -j)

if [ $? -eq 0 ]; then
  if $RUN; then
      ./run.sh
  fi
fi