All credit to the keylogger specific portions goes to SCOTPAUL, at https://github.com/SCOTPAUL/keylog 

To compile, call "make".

To run the keylogger, the following command must be run:

sudo taskset -c 2 ./keylog

The keylogger must run with admin privileges to access the keyboard event file. 


