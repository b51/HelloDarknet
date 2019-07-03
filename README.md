# HelloDarknet
A test function of darknet

## Build
``` bash
$ git clone https://github.com/b51/HelloDarknet

Build darknet.so from darknet first
$ git clone https://github.com/pjreddie/darknet
$ cd darknet
$ make -j4
$ cp libdarknet.so ~/HelloDarknet/lib/

Build HelloDarknet
$ cd ~/HelloDarknet
$ mkdir build
$ cd build
$ cmake ..
$ make -j4
```

## Run
``` bash
./HelloDarknet cfg_file model_file image_to_detect 1 0.2
```
