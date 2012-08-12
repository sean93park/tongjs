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

	$ tongjs tongsdl.js

you'll have to run on X with SDL installed. It'll show colorful rectangle boxes, 
images and some text.

### How to build

TBD

### Supported platforms ?

I'm working with Ubuntu 10.10 and should work with other Linux distributions.
And though not tested it might work with Mac OS X.

### Plans

It's a beginning and there's long way to go, adding system functions, 
other event system besides SDL, maybe libuv, other graphics layer, maybe cairo or skia.
Well, time depends but want to support MS-Windows and Mac OS X also.

### Etc

Codes were from and reference from all around the internet and especially from
https://github.com/creationix/node-sdl, it was a great SDL reference with v8.