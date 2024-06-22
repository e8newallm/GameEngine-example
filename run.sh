bin=$1

if [ -z $1 ]
then
  bin="Game"
  dir="Game"
else
  dir="bin"
fi

./build.sh
if [ $? -eq 0 ];
then
  (cd ./bin/$dir && clear && ./$bin ${@:2})
fi