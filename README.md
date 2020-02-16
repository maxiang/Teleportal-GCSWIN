## Build  

only support msys2.

install requirements:
``` bash
# update repo data
pacman -Syu
# base
pacman -S base-devel git
# install Qt
pacman -S mingw-w64-i686-qt-creator mingw-w64-x86_64-qt-creator
# install GStreamer
pacman -S mingw64/mingw-w64-x86_64-gstreamer mingw64/mingw-w64-x86_64-gst-plugins-bad mingw64/mingw-w64-x86_64-gst-plugins-base mingw64/mingw-w64-x86_64-gst-plugins-good mingw64/mingw-w64-x86_64-gst-plugins-ugly mingw64/mingw-w64-x86_64-gst-rtsp-server mingw64/mingw-w64-x86_64-gst-libav
# build

cd debug
cmake -DCMAKE_BUILD_TYPE=Debug -GNinja .. 
ninja

cd release
cmake -DCMAKE_BUILD_TYPE=Release -GNinja .. 
ninja
```
