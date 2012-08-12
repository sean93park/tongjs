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

#ifndef __tongsdl_helper_header__
#define __tongsdl_helper_header__

#include <v8.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include "tong.h"
#include "tongsdl.h"
#include "tongdebug.h"

#define TONG_SET_METHOD(obj, name, callback)						\
			obj->Set(v8::String::NewSymbol(name),					\
			v8::FunctionTemplate::New(callback)->GetFunction())



namespace tong {
namespace sdl {

Handle<Object>		WrapSurface ( SDL_Surface* surface );
SDL_Surface*		UnwrapSurface(Handle<Object> obj);

Handle<Object>		WrapRect(SDL_Rect* rect);
SDL_Rect*			UnwrapRect(Handle<Object> obj);

Handle<Object>		WrapPixelFormat(SDL_PixelFormat* pixelformat);
SDL_PixelFormat*	UnwrapPixelFormat(Handle<Object> obj);

Handle<Object>		WrapSdlTimerId(SDL_TimerID timerid );
SDL_TimerID			UnwrapSdlTimerId(Handle<Object> obj);

Handle<Object>		WrapFont(TTF_Font* font);
TTF_Font*			UnwrapFont(Handle<Object> obj);

void				helper_destroy( void );

} // namespace sdl
} // namespace tong

#endif //__tongsdl_helper_header__
