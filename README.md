<h1>cursorEdit</h1>
A simple GUI for editing cursors for use with SDL.
This is a simple cursor editing program I wrote after my first year in University. The initial goal of this project was to help me understand how to use both C and C++ code together and learn how to use SDL's built in cursor system. I'm sure the code has many bugs because I was still a student when I developed this.
<h2>Compilation</h2>
```
sudo apt-get install build-essential libsdl1.2-dev libsdl-ttf2.0-dev
g++ -I /usr/include/SDL cursorEdit.cpp control.cpp -lSDL -lSDL_ttf -o cursorEdit
```
