/*
Copyright (c) 2012 sean93.park@gmail.com

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the 
"Software"), to deal in the Software without restriction, including 
without limitation the rights to use, copy, modify, merge, publish, 
distribute, sublicense, and/or sell copies of the Software, and to permit 
persons to whom the Software is furnished to do so, subject to 
the following conditions:

The above copyright notice and this permission notice shall be included 
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF 
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN 
NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR 
OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE 
USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

sdl = require("SDL");

var screen;
var contrun = 1;

var img_linux;
var img_moon;
var img_fonttext;
var jpeg_timerId_linux = 0;
var jpeg_timerId_moon = 0;


var fonttest;

function drawrandomrect()
{
    var     x, y, w, h;
    var     r, g, b, a;

    r = Math.floor( Math.random() * 256 );
	g = Math.floor( Math.random() * 256 );
	b = Math.floor( Math.random() * 256 );
	a = Math.floor( Math.random() * 256 );

	w = Math.floor( Math.random() * 128 );
	h = Math.floor( Math.random() * 64  );
    x = Math.floor( Math.random() * (screen.w-w) );
	y = Math.floor( Math.random() * (screen.h/2-h) );
	
    sdl.fillRect( screen, [x, y, w, h], (r << 0)+(g << 8)+(b << 16)+ (a << 24));
}

function drawjpeg( image )
{
    var     x,y,w,h;
    
    x = Math.floor( Math.random() * screen.h );
	y = Math.floor( Math.random() * (screen.h - image.h ) + 128 );
    w = Math.floor( Math.random() * image.w/2 + image.w/4);
    h = Math.floor( Math.random() * image.h/2 + image.h/4);

    w = image.w;
    h = image.h;

    if ( y+h >= screen.h ) y = screen.h - h;
    if ( x+w >= screen.w ) x = screen.w - w;
    
    sdl.setAlpha( screen, sdl.SURFACE.SRCALPHA, 0x80 );
    sdl.blitSurface( image, null, screen, [x,y,w,h] );
    //sdl.softStretch( image, null, screen, [x,y,w,h] );
}

function killtimers ( )
{
    if ( jpeg_timerId_moon ) sdl.killTimer( jpeg_timerId_moon );
    if ( jpeg_timerId_linux ) sdl.killTimer( jpeg_timerId_linux );
    jpeg_timerId_moon = 0;
    jpeg_timerId_linux = 0;
}

function quitprogram ( )
{
    killtimers();

    sdl.freeSurface( img_linux );
    sdl.freeSurface( img_moon );
    sdl.freeSurface( img_fonttext );
    img_linux = 0;
    img_moon = 0;
    img_fonttext = 0;
    sdl.TTF.closeFont( fonttest );
    fonttest = 0;

    sdl.quit();
    quit();
}

function onKeyDownHandler ( inputevent )
{
    print( "[KEY DN] " + "scancode(" + inputevent.scancode + "), sym(" + inputevent.sym + ")" );
}

function onKeyUpHandler ( inputevent )
{
    print( "[KEY UP] " + "scancode(" + inputevent.scancode + "), sym(" + inputevent.sym + ")" );
    if ( inputevent.sym == 27 )
    {
        print( "~~~ Exit..." );
        contrun = 0;
    }
    if ( inputevent.sym == 32 )
    {
        print( "~~~ kill timer..." );
        killtimers();
    }
}

function onMouseMotionHandler ( inputevent )
{
    print( "[MOUSE MV] " + "state(" + inputevent.state + "), which(" + inputevent.which + "), x,y(" 
            + inputevent.x + "," + inputevent.y + ")" );
}

function onMouseBtnDownHandler ( inputevent )
{
    print( "[MOUSE DN] " + "button(" + inputevent.button + "), which(" + inputevent.which + "), x,y(" 
            + inputevent.x + "," + inputevent.y + ")" );
}
function onMouseBtnUpHandler ( inputevent )
{
    print( "[MOUSE UP] " + "button(" + inputevent.button + "), which(" + inputevent.which + "), x,y(" 
            + inputevent.x + "," + inputevent.y + ")" );
}

function onTimer ( timerparam )
{
    if ( timerparam.timerid == jpeg_timerId_linux )
        drawjpeg( img_linux );
    else if ( timerparam.timerid == jpeg_timerId_moon )
    {
        drawjpeg( img_moon );
        drawjpeg( img_fonttext );        
    }
    sdl.flip(screen);
}

function callme_repeat ()
{
    drawrandomrect();
    sdl.flip(screen);

    if ( contrun )
    {
        process.nextTick( callme_repeat );
    }
    else
    {
        quitprogram();
    }
}

function callme_first ()
{
    // init sdl and stuff
    sdl.init( sdl.INIT.VIDEO | sdl.INIT.TIMER );
    screen = sdl.setVideoMode( 800,480,32,0);

    // register event handlers
    sdl.addListener( "keydown",        "onKeyDownHandler" );
    sdl.addListener( "keyup",          "onKeyUpHandler" );
    sdl.addListener( "mousemotion",    "onMouseMotionHandler" );
    sdl.addListener( "mousebtndown",   "onMouseBtnDownHandler" );
    sdl.addListener( "mousebtnup",     "onMouseBtnUpHandler" );

    img = sdl.IMG.load( "./tuxhalf.png" );
    img_linux = sdl.displayFormatAlpha( img );
    sdl.freeSurface( img );
    img = 0;

    img = sdl.IMG.load( "./2012_moon.png" );
    img_moon = sdl.displayFormatAlpha( img );
    sdl.freeSurface( img );
    img = 0;

    // text display test...
    sdl.TTF.init();
    //fonttest = sdl.TTF.openFont( "/usr/share/fonts/truetype/freefont/FreeSans.ttf", 32 );
    fonttest = sdl.TTF.openFont( "/usr/share/fonts/truetype/freefont/FreeSerif.ttf", 32 );

    //img_fonttext = sdl.TTF.renderTextBlended( fonttest, "Hello World", 0x80204080 );
    //img_fonttext = sdl.TTF.renderTextSolid( fonttest, "Hello World", 0x80204080 );
    img_fonttext = sdl.TTF.renderTextShaded( fonttest, "Hello World", 0x80204080, 0x80a05010 );
    
    drawrandomrect();
    sdl.flip(screen);

    jpeg_timerId_linux = sdl.setTimer( 333, "onTimer" );
    jpeg_timerId_moon  = sdl.setTimer( 500, "onTimer" );
    print( "timer : " + jpeg_timerId_linux + "," + jpeg_timerId_moon );

    process.nextTick( callme_repeat );

    print( ">>>> press ESC to quit...\n" );
}

process.nextTick( callme_first );
