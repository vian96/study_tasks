# This program prints bad apple video on my cpu

This program uses [ASCII version of bad apple](https://github.com/Chion82/ASCII_bad_apple) with little changes. It works with versions 0.3.3 and 0.3.5 but it may also work with the later ones.

## Running

You have to compile and run `generator.cpp` to create `badapple.casm` file which you should compile with `asm` and run `badapple.boac` from cpu (or you can simply run `badapple.bat`). See [config](#Config) section below for settings of CPU.

## Config

Image width is 100 pixels, image height is 30 pixels, delay in print function is approximately 60 ms.

## Using generator for your own purposes

Buffer size is size of one frame **including** `\n` symbols, frame len is count of pixels in one frame (or buffer size minus number of `\n`). 

