/*
portion copy from https://github.com/creationix/node-sdl
*/
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


#include "tongsdlhelper.h"
#include "tongdebug.h"

using namespace v8;

////////////////////////////////////////////////////////////////////////////////

namespace tong {
namespace sdl {

////////////////////////////////////////////////////////////////////////////////
// Wrap/Unwrap SDL_Surface

static Persistent<ObjectTemplate> surface_template_;

Handle<Value> GetSurfaceFlags(Local<String> name, const AccessorInfo& info) 
{
	SDL_Surface* surface = UnwrapSurface(info.Holder());
	return Number::New(surface->flags);
}
Handle<Value> GetSurfaceFormat(Local<String> name, const AccessorInfo& info) 
{
	HandleScope scope;
	SDL_Surface* surface = UnwrapSurface(info.Holder());
	return scope.Close(WrapPixelFormat(surface->format));
}
Handle<Value> GetSurfaceWidth(Local<String> name, const AccessorInfo& info) 
{
	SDL_Surface* surface = UnwrapSurface(info.Holder());
	return Number::New(surface->w);
}
Handle<Value> GetSurfaceHeight(Local<String> name, const AccessorInfo& info) 
{
	SDL_Surface* surface = UnwrapSurface(info.Holder());
	return Number::New(surface->h);
}
Handle<Value> GetSurfacePitch(Local<String> name, const AccessorInfo& info) 
{
	SDL_Surface* surface = UnwrapSurface(info.Holder());
	return Number::New(surface->pitch);
}
Handle<Value> GetSurfaceRect(Local<String> name, const AccessorInfo& info) 
{
	HandleScope scope;
	SDL_Surface* surface = UnwrapSurface(info.Holder());
	return scope.Close(WrapRect(&surface->clip_rect));
}

Handle<ObjectTemplate> MakeSurfaceTemplate ( void ) 
{
	HandleScope handle_scope;

	Handle<ObjectTemplate> result = ObjectTemplate::New();
	result->SetInternalFieldCount(1);

	// Add accessors for some of the fields of the surface.
	result->SetAccessor(String::NewSymbol("flags"),		GetSurfaceFlags );
	result->SetAccessor(String::NewSymbol("format"),	GetSurfaceFormat );
	result->SetAccessor(String::NewSymbol("w"),			GetSurfaceWidth );
	result->SetAccessor(String::NewSymbol("h"),			GetSurfaceHeight );
	result->SetAccessor(String::NewSymbol("pitch"),		GetSurfacePitch);
	result->SetAccessor(String::NewSymbol("clip_rect"),	GetSurfaceRect);

	// Again, return the result through the current handle scope.
	return handle_scope.Close(result);
}

Handle<Object> WrapSurface ( SDL_Surface* surface )
{
	// Handle scope for temporary handles.
	HandleScope handle_scope;

	// Fetch the template for creating JavaScript wrappers.
	// It only has to be created once, which we do on demand.
	if (surface_template_.IsEmpty()) 
	{
		Handle<ObjectTemplate> raw_template = MakeSurfaceTemplate();
		surface_template_ = Persistent<ObjectTemplate>::New(raw_template);
	}
	Handle<ObjectTemplate> templ = surface_template_;

	// Create an empty wrapper.
	Handle<Object> result = templ->NewInstance();

	// Wrap the raw C++ pointer in an External so it can be referenced
	// from within JavaScript.
	Handle<External> request_ptr = External::New(surface);

	// Store the request pointer in the JavaScript wrapper.
	result->SetInternalField(0, request_ptr);

	// Return the result through the current handle scope.  Since each
	// of these handles will go away when the handle scope is deleted
	// we need to call Close to let one, the result, escape into the
	// outer handle scope.
	return handle_scope.Close(result);
}

SDL_Surface* UnwrapSurface ( Handle<Object> obj )
{
	Handle<External> field = Handle<External>::Cast(obj->GetInternalField(0));
	void* ptr = field->Value();
	return static_cast<SDL_Surface*>(ptr);
}

////////////////////////////////////////////////////////////////////////////////
// Wrap/Unwrap Rect

static Persistent<ObjectTemplate> rect_template_;

Handle<Value> GetRectX ( Local<String> name, const AccessorInfo& info )
{
	SDL_Rect* rect = UnwrapRect(info.Holder());
	return Number::New(rect->x);
}
Handle<Value> GetRectY ( Local<String> name, const AccessorInfo& info )
{
	SDL_Rect* rect = UnwrapRect(info.Holder());
	return Number::New(rect->y);
}
Handle<Value> GetRectW ( Local<String> name, const AccessorInfo& info )
{
	SDL_Rect* rect = UnwrapRect(info.Holder());
	return Number::New(rect->w);
}
Handle<Value> GetRectH ( Local<String> name, const AccessorInfo& info )
{
	SDL_Rect* rect = UnwrapRect(info.Holder());
	return Number::New(rect->h);
}

Handle<ObjectTemplate> MakeRectTemplate ( void ) 
{
	HandleScope handle_scope;

	Handle<ObjectTemplate> result = ObjectTemplate::New();
	result->SetInternalFieldCount(1);

	// Add accessors for some of the fields of the rect.
	result->SetAccessor(String::NewSymbol("x"), GetRectX);
	result->SetAccessor(String::NewSymbol("y"), GetRectY);
	result->SetAccessor(String::NewSymbol("w"), GetRectW);
	result->SetAccessor(String::NewSymbol("h"), GetRectH);

	// Again, return the result through the current handle scope.
	return handle_scope.Close(result);
}

Handle<Object> WrapRect ( SDL_Rect* rect )
{
	// Handle scope for temporary handles.
	HandleScope handle_scope;

	// Fetch the template for creating JavaScript wrappers.
	// It only has to be created once, which we do on demand.
	if (rect_template_.IsEmpty()) 
	{
		Handle<ObjectTemplate> raw_template = MakeRectTemplate();
		rect_template_ = Persistent<ObjectTemplate>::New(raw_template);
	}
	Handle<ObjectTemplate> templ = rect_template_;

	// Create an empty wrapper.
	Handle<Object> result = templ->NewInstance();

	// Wrap the raw C++ pointer in an External so it can be referenced
	// from within JavaScript.
	Handle<External> request_ptr = External::New(rect);

	// Store the request pointer in the JavaScript wrapper.
	result->SetInternalField(0, request_ptr);

	// Return the result through the current handle scope.  Since each
	// of these handles will go away when the handle scope is deleted
	// we need to call Close to let one, the result, escape into the
	// outer handle scope.
	return handle_scope.Close(result);
}

SDL_Rect* UnwrapRect ( Handle<Object> obj )
{
	Handle<External> field = Handle<External>::Cast(obj->GetInternalField(0));
	void* ptr = field->Value();
	return static_cast<SDL_Rect*>(ptr);
}

////////////////////////////////////////////////////////////////////////////////
// Wrap/Unwrap PixelFormat

static Persistent<ObjectTemplate> pixelformat_template_;

Handle<Value> GetFormatBits ( Local<String> name, const AccessorInfo& info )
{
	SDL_PixelFormat* format = UnwrapPixelFormat(info.Holder());
	return Number::New(format->BitsPerPixel);
}
Handle<Value> GetFormatBytes ( Local<String> name, const AccessorInfo& info )
{
	SDL_PixelFormat* format = UnwrapPixelFormat(info.Holder());
	return Number::New(format->BytesPerPixel);
}
Handle<Value> GetFormatColorkey ( Local<String> name, const AccessorInfo& info )
{
	SDL_PixelFormat* format = UnwrapPixelFormat(info.Holder());
	return Number::New(format->colorkey);
}
Handle<Value> GetFormatAlpha ( Local<String> name, const AccessorInfo& info )
{
	SDL_PixelFormat* format = UnwrapPixelFormat(info.Holder());
	return Number::New(format->alpha);
}

Handle<ObjectTemplate> MakePixelFormatTemplate ( void ) 
{
	HandleScope handle_scope;

	Handle<ObjectTemplate> result = ObjectTemplate::New();
	result->SetInternalFieldCount(1);

	// Add accessors for some of the fields of the pixelformat.
	result->SetAccessor(String::NewSymbol("bitsPerPixel"), GetFormatBits);
	result->SetAccessor(String::NewSymbol("bytesPerPixel"), GetFormatBytes);
	result->SetAccessor(String::NewSymbol("colorkey"), GetFormatColorkey);
	result->SetAccessor(String::NewSymbol("alpha"), GetFormatAlpha);

	// Again, return the result through the current handle scope.
	return handle_scope.Close(result);
}

Handle<Object> WrapPixelFormat ( SDL_PixelFormat* pixelformat )
{
	// Handle scope for temporary handles.
	HandleScope handle_scope;

	// Fetch the template for creating JavaScript wrappers.
	// It only has to be created once, which we do on demand.
	if (pixelformat_template_.IsEmpty()) 
	{
		Handle<ObjectTemplate> raw_template = MakePixelFormatTemplate();
		pixelformat_template_ = Persistent<ObjectTemplate>::New(raw_template);
	}
	Handle<ObjectTemplate> templ = pixelformat_template_;

	// Create an empty wrapper.
	Handle<Object> result = templ->NewInstance();

	// Wrap the raw C++ pointer in an External so it can be referenced
	// from within JavaScript.
	Handle<External> request_ptr = External::New(pixelformat);

	// Store the request pointer in the JavaScript wrapper.
	result->SetInternalField(0, request_ptr);

	// Return the result through the current handle scope.  Since each
	// of these handles will go away when the handle scope is deleted
	// we need to call Close to let one, the result, escape into the
	// outer handle scope.
	return handle_scope.Close(result);
}

SDL_PixelFormat* UnwrapPixelFormat ( Handle<Object> obj )
{
	Handle<External> field = Handle<External>::Cast(obj->GetInternalField(0));
	void* ptr = field->Value();
	return static_cast<SDL_PixelFormat*>(ptr);
}

////////////////////////////////////////////////////////////////////////////////
//	Wrap/Unwrap SDL_TimerID

static Persistent<ObjectTemplate> sdltimer_template_;

Handle<ObjectTemplate> MakeSdlTimerTemplate ( void ) 
{
	HandleScope handle_scope;
	Handle<ObjectTemplate> result = ObjectTemplate::New();
	result->SetInternalFieldCount(1);
	return handle_scope.Close(result);
}

Handle<Object> WrapSdlTimerId ( SDL_TimerID timerid )
{
	HandleScope handle_scope;

	if (sdltimer_template_.IsEmpty()) 
	{
		Handle<ObjectTemplate> raw_template = MakeSdlTimerTemplate();
		sdltimer_template_ = Persistent<ObjectTemplate>::New(raw_template);
	}
	Handle<ObjectTemplate> templ = sdltimer_template_;
	Handle<Object> result = templ->NewInstance();
	Handle<External> request_ptr = External::New(timerid);
	result->SetInternalField(0, request_ptr);
	return handle_scope.Close(result);
}

SDL_TimerID UnwrapSdlTimerId ( Handle<Object> obj )
{
	Handle<External> field = Handle<External>::Cast(obj->GetInternalField(0));
	void* ptr = field->Value();
	return static_cast<SDL_TimerID>(ptr);
}

////////////////////////////////////////////////////////////////////////////////

static Persistent<ObjectTemplate> font_template_;

Handle<ObjectTemplate> MakeFontTemplate() 
{
	HandleScope handle_scope;

	Handle<ObjectTemplate> result = ObjectTemplate::New();
	result->SetInternalFieldCount(1);

	// Add accessors for some of the fields of the font.
	//  result->SetAccessor(String::NewSymbol("flags"), GetFontFlags);
	//  result->SetAccessor(String::NewSymbol("format"), GetFontFormat);
	//  result->SetAccessor(String::NewSymbol("w"), GetFontWidth);
	//  result->SetAccessor(String::NewSymbol("h"), GetFontHeight);
	//  result->SetAccessor(String::NewSymbol("pitch"), GetFontPitch);
	//  result->SetAccessor(String::NewSymbol("clip_rect"), GetFontRect);

	// Again, return the result through the current handle scope.
	return handle_scope.Close(result);
}


Handle<Object> WrapFont(TTF_Font* font) 
{
	// Handle scope for temporary handles.
	HandleScope handle_scope;

	// Fetch the template for creating JavaScript http request wrappers.
	// It only has to be created once, which we do on demand.
	if (font_template_.IsEmpty()) 
	{
		Handle<ObjectTemplate> raw_template = MakeFontTemplate();
		font_template_ = Persistent<ObjectTemplate>::New(raw_template);
	}
	Handle<ObjectTemplate> templ = font_template_;

	// Create an empty http request wrapper.
	Handle<Object> result = templ->NewInstance();

	// Wrap the raw C++ pointer in an External so it can be referenced
	// from within JavaScript.
	Handle<External> request_ptr = External::New(font);

	// Store the request pointer in the JavaScript wrapper.
	result->SetInternalField(0, request_ptr);

	// Return the result through the current handle scope.  Since each
	// of these handles will go away when the handle scope is deleted
	// we need to call Close to let one, the result, escape into the
	// outer handle scope.
	return handle_scope.Close(result);
}

TTF_Font* UnwrapFont(Handle<Object> obj) 
{
	Handle<External> field = Handle<External>::Cast(obj->GetInternalField(0));
	void* ptr = field->Value();
	return static_cast<TTF_Font*>(ptr);
}

////////////////////////////////////////////////////////////////////////////////


void helper_destroy ( void )
{
	surface_template_.Dispose();
	rect_template_.Dispose();
	pixelformat_template_.Dispose();
	sdltimer_template_.Dispose();
	font_template_.Dispose();
}

} // namespace sdl
} // namespace tong

/* vim: set ts=4 sw=4 tw=0: */