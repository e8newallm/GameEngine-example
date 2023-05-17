bin=$1
if [ -z $1 ]
  then
    bin="TheGame"
fi

./build.sh
if [ $? -eq 0 ];
then
  (cd ./bin/bin && clear && ./$bin)
fi