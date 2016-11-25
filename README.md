# JSON to C++
This program will take either a json file or a URL to a web service and build C++ classes to work with that data.  By default it will create the serialization linkage for the JsonLink library that is part of https://github.com/beached/parse_json .

# Requirements
CMake >= 2.8.12 https://cmake.org/
Boost >= 1.58 https://www.boost.com/
Curl https://curl.haxx.se/

Both Boost and Curl are often packaged within a Linux distribution and available for Windows/Mac

# Building
The build system uses cmake.  Often a build can be accomplished by creating a build folder in the project source folder and typing 
```
cmake ..
cmake --build .
```
# Running
To output the C++ code to the terminal one just needs to type json_to_cpp_bin --in_file jsonfile.json

```
Command line options
Options:
  --help                  print option descriptions
  --in_file arg           json source file path or url
  --out_file arg          output c++ file
  --use_jsonlink arg (=1) Use JsonLink serializaion/deserialization
```
