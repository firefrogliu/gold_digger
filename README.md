# gold_digger
This program takes a random number seed and generates a randomized picture according to the random number.

Light version of convolutional neural network Yolo v3 for objects detection is then applied on this randomized picture, yielding a series of detections.

The detections are fed to a SHA256 hash function, yielding a 256 bit string.

This repository supports:

* Linux
* both cuDNN >= 7.1.1
* CUDA >= 8.0

How to compile:
* To compile for CPU just do `make` on Linux with `GPU=0` in the `Makefile` 
* To compile for GPU set flag `GPU=1` in the `Makefile` 
    
    Required both [CUDA >= 8.0](https://developer.nvidia.com/cuda-toolkit-archive) and [cuDNN >= 7.1.1](https://developer.nvidia.com/rdp/cudnn-archive)

How to start:
* Download [google open image set](https://github.com/cvdfoundation/open-images-dataset#download-full-dataset-with-google-storage-transfer) to the `bin\testPics` directory 
* Enter the `bin` directory and run `./gold_digger {random_seed}`

