SKETCH=$1
PORT=$2
BOARD=$3
FLIGHT_COMPUTER_LIBS=~/Documents/Arduino/libraries/flight-computer
GROUND_RECEIVER_LIBS=~/Documents/Arduino/libraries/ground-receiver

if [ ! -d "$FLIGHT_COMPUTER_LIBS" ]; then
  mkdir "$FLIGHT_COMPUTER_LIBS"
else
  rm -rf "$FLIGHT_COMPUTER_LIBS"
  mkdir "$FLIGHT_COMPUTER_LIBS"
fi

if [ ! -d "$GROUND_RECEIVER_LIBS" ]; then
  mkdir "$GROUND_RECEIVER_LIBS"
else
  rm -rf "$GROUND_RECEIVER_LIBS"
  mkdir "$GROUND_RECEIVER_LIBS"
fi

cp flight-computer/libraries/*.{h,cpp} "$FLIGHT_COMPUTER_LIBS"
cp ground-computer/libraries/*.{h,cpp} "$GROUND_RECEIVER_LIBS"
cp -r flight-computer/libraries/external/ ~/Documents/Arduino/libraries/
cp -r ground-receiver/libraries/external/ ~/Documents/Arduino/libraries/

arduino-cli compile -b $BOARD $SKETCH && arduino-cli upload -p $PORT -b $BOARD $SKETCH
