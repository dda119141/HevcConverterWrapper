# x265ConverterWrapper
Command line interface for converting any video file in a x265 (hevc) video file 

## Motivation

The majority of existing encoded or raw videos with high resolution occupy a lot of disk space. H.265 can compress video input data to 50% of its input size.
Within a given directory, this tool parses the video files.Then, it converts each retrieved video file to a x265 (hevc) encoded video file.
The conversion is performed by execution of the ffmpeg utility.

This tool can only successfully work if the ffmpeg utility has been successfully installed in the host.

## How to build this tool
In order to compile and build this utility, the conan package manager needs to be installed.

    1. pip install conan
    2. mkdir build && cd build && conan install ..
    3. cmake ..
    4. cmake --build .

## Requirements
- cmake >= 3.9
- c++ with c++17 support or later
- conan >= 1.9.1
- ffmpeg utility installed

