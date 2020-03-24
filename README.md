## NEW BUILD INSTRUCTIONS

``` bash
Install msys2 and then run:

# update repo data
pacman -Syu
# base
pacman -S base-devel git
# install Qt
pacman -S mingw-w64-i686-qt-creator mingw-w64-x86_64-qt-creator
# install GStreamer
pacman -S mingw64/mingw-w64-x86_64-gstreamer mingw64/mingw-w64-x86_64-gst-plugins-bad mingw64/mingw-w64-x86_64-gst-plugins-base mingw64/mingw-w64-x86_64-gst-plugins-good mingw64/mingw-w64-x86_64-gst-plugins-ugly mingw64/mingw-w64-x86_64-gst-rtsp-server mingw64/mingw-w64-x86_64-gst-libav

# Added new command lines.
# Install gcc (4.9.1)
pacman -S mingw-w64-x86_64-gcc

# Install ninja, cmake
pacman -S mingw-w64-x86_64-ninja mingw-w64-x86_64-cmake

2. 
Close the msys2 shell and add c:\msys64\mingw64\bin into the PATH environment variable.

Windows: Control Panel > System and Security > System > Advanced System Settings > Environment Variables... > Edit User variable Path

OPEN msys Mingw64
3. 
cd {directory path}
mkdir build && cd build
cmake -D CMAKE_CXX_COMPILER=g++.exe -G Ninja .. // debug
cmake -D CMAKE_CXX_COMPILER=g++.exe -DCMAKE_BUILD_TYPE=Release -G Ninja .. // release
cd ..
ninja 
```
