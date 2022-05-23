export STREAMER_PATH= $HOME/Documents/mjpg/mjpg-streamer/mjpg-streamer-experimental/
export LD_LIBRARY_PATH=$STREAMER_PATH
SSTREAMER_PATH/mjpg_streamer -i "input_raspicam.so -fps 10 -vf" -o "output_http.so -p 8080 -w $STREAMER_PATH/www"
