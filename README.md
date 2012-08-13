Welcome to tongjs
======

### What is tongjs?

Tongjs is an application execution environment that executes JavaScript applications.
Its based on googles V8 engine with SDL graphics and events.


### What is it for?

I started this for a hobby to learn how to use v8 and dig inside node, 
so I have no particular reason where to use or what to use, but someone may !


### How to play with this ?

there's a tongsdl.js file what you can try.

	tongjs tongsdl

you'll have to run on X with SDL installed. It'll show colorful rectangle boxes, 
images and some text.

### Prerequisite

Below packages are needed to build and run

  - cmake
  - libv8-dev
  - libsdl1.2-dev
  - libsdl-ttf2.0-dev
  - libsdl-image1.2-dev

You should check below font file exist
  
  - /usr/share/fonts/truetype/freefont/FreeSerif.ttf


### How to build and run test program

	mkdir build
	cd build
	cmake ..
	make
	./tongjs tongsdl

to disable debug and script messages

    cmake .. -DENABLE_DEBUG=0 -DENABLE_SCRIPTMSG=0
	make
### Supported platforms ?

I worked with Ubuntu 10.10 and 12.04 and I guess it work with other Linux distributions.
And though not tested it might work with Mac OS X.


### Plans

It's a beginning and there's long way to go, adding system functions, 
other event system besides SDL, maybe libuv, other graphics layer, maybe cairo or skia.
Well, time depends but want to support MS-Windows and Mac OS X also.


### License

MIT License : http://opensource.org/licenses/mit-license.php


### Etc

Codes were from and reference from all around the internet and especially from
https://github.com/creationix/node-sdl, it was a great SDL reference with v8 and 
http://www.enlightenment.org/ elev8 project.
