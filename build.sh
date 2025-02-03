#!/bin/bash

FLAGS="-DDEBUG=OFF -DCMAKE_BUILD_TYPE=Release"
RUN=false
TEST=false
VALGRIND=false
CPPCHECK=false
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
    -t|--test)
      TEST=true
      shift # past argument
      ;;
    -v|--valgrind)
      VALGRIND=true
      shift # past argument
      ;;
    -c|--cppcheck)
      CPPCHECK=true
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


if $CPPCHECK; then
    (cppcheck --enable=all --force --suppress=missingIncludeSystem --inconclusive --suppress=missingInclude --suppress=unusedFunction --check-level=exhaustive src gameengine)
else
  (mkdir bin -p && cd bin && cmake .. $FLAGS && cmake --build . -j)

  if [ $? -eq 0 ]; then
    if $TEST; then
        ./test.sh
    fi

    if [ $? -eq 0 ]; then

      if $VALGRIND; then
          (cd ./bin/Game && valgrind --leak-check=full --show-leak-kinds=all --show-reachable=yes --suppressions=../../valgrind.supp --error-limit=no --track-origins=yes --log-file=../../valgrind.log -s --gen-suppressions=all ./Game)
      elif $RUN; then
          ./run.sh
      fi
    fi
  fi
fi