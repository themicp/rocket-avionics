SKETCH=$1
PORT=$2
BOARD=$3
LIBS=~/Documents/Arduino/libraries/$SKETCH

if [ ! -d "$LIBS" ]; then
  mkdir "$LIBS"
else
  rm -rf ~/Documents/Arduino/libraries/$SKETCH
  mkdir "$LIBS"
fi
cp $SKETCH/libraries/*.{h,cpp} ~/Documents/Arduino/libraries/$SKETCH
cp -r $SKETCH/libraries/external/ ~/Documents/Arduino/libraries/

arduino-cli compile -b $BOARD $SKETCH && arduino-cli upload -p $PORT -b $BOARD $SKETCH
