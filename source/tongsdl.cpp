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

#include <map>

#include "tong.h"
#include "tongsdl.h"
#include "tongdebug.h"
#include "tongsdlhelper.h"

using namespace v8;

////////////////////////////////////////////////////////////////////////////////
namespace tong {
namespace sdl {

static bool				_sdl_ready = false;
static Handle<Object>	_sdl_instance;			// one and only SDL instance

////////////////////////////////////////////////////////////////////////////////

Local<Value> MakeSDLException ( const char* name )
{
	return Exception::Error( String::Concat(
								String::Concat(String::New(name), String::New(": ")),
								String::New(SDL_GetError())
							));
}

Handle<Value> ThrowSDLException ( const char* name )
{
	_sdl_ready = false;
	return ThrowException( MakeSDLException(name) );
}

////////////////////////////////////////////////////////////////////////////////
//	SDL JS Native bind

Handle<Value> Init ( const Arguments& args )
{
	HandleScope scope;

	tong_dbg( "[tong::sdl::Init] ! \n" );
	if (!(args.Length() == 1 && args[0]->IsNumber())) 
		return ThrowException( Exception::TypeError(String::New("SDL Invalid arguments: Expected Init(Number)")) );

	if ( SDL_Init(args[0]->Int32Value()) < 0 )
		return ThrowSDLException( __func__ );

	_sdl_ready = true;
	return Undefined();
}

Handle<Value> Quit ( const Arguments& args )
{
	HandleScope scope;

	if (!(args.Length() == 0)) 
		return ThrowException( Exception::TypeError(String::New("Invalid arguments: Expected Quit()")) );

	SDL_Quit();
	_sdl_ready = false;
	return Undefined();
}

Handle<Value> SetVideoMode ( const Arguments& args )
{
	HandleScope scope;

	if (!(args.Length() == 4 && args[0]->IsNumber() && args[1]->IsNumber() && args[2]->IsNumber() && args[3]->IsNumber())) 
		return ThrowException(Exception::TypeError(String::New("Invalid arguments: Expected SetVideoMode(Number, Number, Number, Number)")));

	int width	= (args[0]->Int32Value());
	int height	= (args[1]->Int32Value());
	int bpp		= (args[2]->Int32Value());
	int flags	= (args[3]->Int32Value());

	SDL_Surface* screen = SDL_SetVideoMode(width, height, bpp, flags);
	if (screen == NULL) 
		return ThrowSDLException(__func__);
	return scope.Close(WrapSurface(screen));
}

Handle<Value> FreeSurface ( const Arguments& args ) 
{
	HandleScope scope;

	if (!(args.Length() == 1 && args[0]->IsObject())) 
		return ThrowException( Exception::TypeError(String::New("Invalid arguments: Expected FreeSurface(Surface)")));

	// TODO: find a way to do this automatically by using GC hooks.  This is dangerous in JS land
	SDL_FreeSurface(UnwrapSurface(args[0]->ToObject()));
	args[0]->ToObject()->Set(String::New("DEAD"), Boolean::New(true));

	return Undefined();
}


////////////////////////////////////////////////////////////////////////////////////////////

Handle<Value> BlitSurface ( const Arguments& args )
{
	HandleScope scope;

	if (!(args.Length() == 4
		&& args[0]->IsObject()
		&& (args[1]->IsObject() || args[1]->IsNull())
		&& args[2]->IsObject()
		&& (args[3]->IsObject() || args[3]->IsNull()) )) 
	{
		return ThrowException(Exception::TypeError(String::New("Invalid arguments: Expected BlitSurface(Surface, Rect, Surface, Rect)")));
	}

	SDL_Surface*	src = UnwrapSurface(args[0]->ToObject());
	SDL_Surface*	dst = UnwrapSurface(args[2]->ToObject());
	SDL_Rect*		srcrect;
	SDL_Rect*		dstrect;
	SDL_Rect		srcrect_;
	SDL_Rect		dstrect_;


	if (args[1]->IsNull()) 
		srcrect = NULL;
	else if (args[1]->IsArray()) 
	{
		Handle<Object> arr1 = args[1]->ToObject();
		srcrect = &srcrect_;
		srcrect->x = arr1->Get(String::New("0"))->Int32Value();
		srcrect->y = arr1->Get(String::New("1"))->Int32Value();
		srcrect->w = arr1->Get(String::New("2"))->Int32Value();
		srcrect->h = arr1->Get(String::New("3"))->Int32Value();
	} 
	else 
		srcrect = UnwrapRect(args[1]->ToObject());

	if (args[3]->IsNull()) 
		dstrect = NULL;
	else if (args[3]->IsArray()) 
	{
		Handle<Object> arr2 = args[3]->ToObject();
		dstrect = &dstrect_;
		dstrect->x = arr2->Get(String::New("0"))->Int32Value();
		dstrect->y = arr2->Get(String::New("1"))->Int32Value();
		dstrect->w = arr2->Get(String::New("2"))->Int32Value();
		dstrect->h = arr2->Get(String::New("3"))->Int32Value();
	} 
	else 
		dstrect = UnwrapRect(args[3]->ToObject());

	//  if (srcrect) printf("srcrect = {x: %d, y: %d, w: %d, h: %d}\n", srcrect->x, srcrect->y, srcrect->w, srcrect->h);
	//  else printf("srcrect = null\n");
	//  if (dstrect) printf("dstrect = {x: %d, y: %d, w: %d, h: %d}\n", dstrect->x, dstrect->y, dstrect->w, dstrect->h);
	//  else printf("dstrect = null\n");

	if ( SDL_BlitSurface( src, srcrect, dst, dstrect) < 0 )
		return ThrowSDLException( __func__ );

	return Undefined();
}

Handle<Value> SoftStretch ( const Arguments& args )
{
	HandleScope scope;

	if (!(args.Length() == 4
		&& args[0]->IsObject()
		&& (args[1]->IsObject() || args[1]->IsNull())
		&& args[2]->IsObject()
		&& (args[3]->IsObject() || args[3]->IsNull()) )) 
	{
		return ThrowException(Exception::TypeError(String::New("Invalid arguments: Expected BlitSurface(Surface, Rect, Surface, Rect)")));
	}

	SDL_Surface*	src = UnwrapSurface(args[0]->ToObject());
	SDL_Surface*	dst = UnwrapSurface(args[2]->ToObject());
	SDL_Rect*		srcrect;
	SDL_Rect*		dstrect;
	SDL_Rect		srcrect_;
	SDL_Rect		dstrect_;


	if (args[1]->IsNull()) 
		srcrect = NULL;
	else if (args[1]->IsArray()) 
	{
		Handle<Object> arr1 = args[1]->ToObject();
		srcrect = &srcrect_;
		srcrect->x = arr1->Get(String::New("0"))->Int32Value();
		srcrect->y = arr1->Get(String::New("1"))->Int32Value();
		srcrect->w = arr1->Get(String::New("2"))->Int32Value();
		srcrect->h = arr1->Get(String::New("3"))->Int32Value();
	} 
	else 
		srcrect = UnwrapRect(args[1]->ToObject());

	if (args[3]->IsNull()) 
		dstrect = NULL;
	else if (args[3]->IsArray()) 
	{
		Handle<Object> arr2 = args[3]->ToObject();
		dstrect = &dstrect_;
		dstrect->x = arr2->Get(String::New("0"))->Int32Value();
		dstrect->y = arr2->Get(String::New("1"))->Int32Value();
		dstrect->w = arr2->Get(String::New("2"))->Int32Value();
		dstrect->h = arr2->Get(String::New("3"))->Int32Value();
	} 
	else 
		dstrect = UnwrapRect(args[3]->ToObject());

	//  if (srcrect) printf("srcrect = {x: %d, y: %d, w: %d, h: %d}\n", srcrect->x, srcrect->y, srcrect->w, srcrect->h);
	//  else printf("srcrect = null\n");
	//  if (dstrect) printf("dstrect = {x: %d, y: %d, w: %d, h: %d}\n", dstrect->x, dstrect->y, dstrect->w, dstrect->h);
	//  else printf("dstrect = null\n");

	if ( SDL_SoftStretch( src, srcrect, dst, dstrect) < 0 )
		return ThrowSDLException( __func__ );

	return Undefined();
}

Handle<Value> FillRect(const Arguments& args) 
{
	HandleScope scope;

	if (!(args.Length() == 3
			&& args[0]->IsObject()
			&& (args[1]->IsObject() || args[1]->IsNull())
			&& args[2]->IsNumber() )) 
	{
		return ThrowException( Exception::TypeError(String::New("Invalid arguments: Expected FillRect(Surface, Rect, Number)")) );
	}

	SDL_Surface* surface = UnwrapSurface(args[0]->ToObject());
	SDL_Rect	r;
	SDL_Rect*	rect;

	if (args[1]->IsNull()) 
		rect = NULL;
	else if (args[1]->IsArray()) 
	{
		Handle<Object> arr = args[1]->ToObject();
		r.x = arr->Get(String::New("0"))->Int32Value();
		r.y = arr->Get(String::New("1"))->Int32Value();
		r.w = arr->Get(String::New("2"))->Int32Value();
		r.h = arr->Get(String::New("3"))->Int32Value();
		rect = &r;
	} 
	else 
		rect = UnwrapRect(args[1]->ToObject());

	int color = args[2]->Int32Value();

	if ( SDL_FillRect (surface, rect, color) < 0 )
		return ThrowSDLException(__func__);

	return Undefined();
}

Handle<Value> CreateRGBSurface ( const Arguments& args )
{
	HandleScope scope;

	if ( !(args.Length() == 3 && args[0]->IsNumber() && args[1]->IsNumber() && args[2]->IsNumber()) )
		return ThrowException( Exception::TypeError(String::New("Invalid arguments: Expected CreateRGBSurface(Number, Number, Number)")) );

	unsigned int flags	= args[0]->Uint32Value();
	int width	= args[1]->Int32Value();
	int height	= args[2]->Int32Value();

	SDL_Surface *surface;
	int rmask, gmask, bmask, amask;

	/* SDL interprets each pixel as a 32-bit number, so our masks must depend
	on the endianness (byte order) of the machine */
//#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	//rmask = 0xff000000;
	//gmask = 0x00ff0000;
	//bmask = 0x0000ff00;
	//amask = 0x000000ff;
//#else
	rmask = 0x00ff0000;//0x000000ff;
	gmask = 0x0000ff00;
	bmask = 0x000000ff;//0x00ff0000;
	amask = 0xff000000;
//#endif
	surface = SDL_CreateRGBSurface( flags, width, height, 32, rmask, gmask, bmask, amask );
	if (surface == NULL) 
		return ThrowSDLException( __func__ );
	return scope.Close( WrapSurface( surface ) );
}

Handle<Value> DisplayFormatAlpha ( const Arguments& args ) 
{
	HandleScope scope;

	if (!(args.Length() == 1 && args[0]->IsObject() ) )
		return ThrowException( Exception::TypeError(String::New("Invalid arguments: Expected displayFormatAlpha(Surface)")) );

	SDL_Surface* surface = UnwrapSurface(args[0]->ToObject());
	SDL_Surface* surfacedf = SDL_DisplayFormatAlpha(surface);
	if ( surfacedf == NULL )
		return ThrowSDLException( __func__ );

	return scope.Close( WrapSurface( surfacedf ) );
}

Handle<Value> Flip ( const Arguments& args )
{
	HandleScope scope;

	if (!(args.Length() == 1 && args[0]->IsObject())) 
		return ThrowException( Exception::TypeError(String::New("Invalid arguments: Expected Flip(Surface)")) );

	SDL_Flip( UnwrapSurface(args[0]->ToObject()) );

	return Undefined();
}

Handle<Value> SetAlpha (const Arguments& args )
{
	HandleScope scope;

	if (!(args.Length() == 3 && args[0]->IsObject() && args[1]->IsNumber() && args[2]->IsNumber())) 
		return ThrowException( Exception::TypeError(String::New("Invalid arguments: Expected setAlpha(Surface, Number, Number)")) );

	SDL_Surface* surface = UnwrapSurface(args[0]->ToObject());
	unsigned int flags = args[1]->Uint32Value();
	int alpha = (args[2]->Int32Value()) & 0xff;

	if ( SDL_SetAlpha(surface, flags, alpha) < 0 )
		return ThrowSDLException( __func__ );

	return Undefined();
}



/////////////////////////////////////////////////////////////////////////////////////////
//
//	timer
//		we can wrap SDL_Timer but it'll make JS side complicate to compare the ID
//		so just cast it as an integer.
//

// for multiple timers
typedef struct TongJsTimer_
{
	v8::Persistent<v8::String>	timer_sym;
	SDL_TimerID					timerId;

} TongJsTimer;

typedef std::map<SDL_TimerID,TongJsTimer*>		TimerIdSymMapType;
typedef std::pair<SDL_TimerID,TongJsTimer*>		TimerIdSymPairType;

static TimerIdSymMapType			_timerIdSymMap;	// map of SDL_TimerID and Symbol map

static unsigned int timerCallback ( unsigned int interval, void* param )
{
	SDL_Event		tevent;

	// for testing...
	tevent.type			= SDL_USEREVENT;
	tevent.user.code	= 0;
	tevent.user.data1	= param;
	tevent.user.data2	= NULL;

	SDL_PushEvent( &tevent );

	return interval;
}

static SDL_TimerID _addTimer ( unsigned int interval, String::Utf8Value& eventHandler )
{
	v8::Local<v8::Value>		eventproc_val;
	v8::Local<v8::String>		eventproc_sym;
	v8::Local<v8::Object>		global = v8::Context::GetCurrent()->Global();

	eventproc_sym = v8::Local<v8::String>::New( v8::String::NewSymbol( *eventHandler ) );
	eventproc_val = global->Get(eventproc_sym);
	if ( !eventproc_val->IsFunction() )
	{
		tong_dbg( "Error: '%s' not found\n", *eventHandler );
		return false;
	}

	TongJsTimer*	ptimer;
	ptimer = new TongJsTimer;
	ptimer->timer_sym = v8::Persistent<v8::String>::New( v8::String::NewSymbol( *eventHandler ) );
	ptimer->timerId = SDL_AddTimer( interval, timerCallback, ptimer  );
	if ( ptimer->timerId != NULL )
	{
		tong_dbg( "_addTimer %s, %u\n", *eventHandler, (unsigned int)ptimer->timerId  );
		_timerIdSymMap.insert( TimerIdSymPairType( ptimer->timerId, ptimer ) );
	}
	return ptimer->timerId;
}

Handle<Value> SetTimer ( const Arguments& args )
{
	//	sdl.setTimer( 33, "onTimer" );
	v8::HandleScope scope;
	if ( args.Length() < 2 || (!args[0]->IsNumber()) )
		return scope.Close( ThrowException( v8::Exception::Error( v8::String::New("Type or handler missing"))));
	
	unsigned int interval = args[0]->Uint32Value();
	v8::String::Utf8Value strHandler( args[1] );
	tong_dbg( "[%s] (%u:%s)\r\n", "SetTimer", interval, *strHandler );
	if ( !strHandler.length() )
		return scope.Close( ThrowSDLException("Type or handler missing") );

	SDL_TimerID	timerId;
	timerId = _addTimer( interval, strHandler );
	if ( timerId == NULL )
		return scope.Close(ThrowSDLException("Handler not exist"));

	//return scope.Close( WrapSdlTimerId( timerId ) );
	return scope.Close( Number::New( (unsigned int)timerId ) );
}

Handle<Value> KillTimer ( const Arguments& args )
{
	v8::HandleScope scope;
	if ( !( args.Length() == 1 && args[0]->IsNumber() ) )
		return scope.Close(ThrowException(v8::Exception::Error(v8::String::New("Timer ID missing"))));

	//SDL_TimerID	timerid = UnwrapSdlTimerId(args[0]->ToObject());
	SDL_TimerID	timerid = (SDL_TimerID)(args[0]->Uint32Value());
	TimerIdSymMapType::iterator	iter;
	iter = _timerIdSymMap.find( timerid );
	if ( iter != _timerIdSymMap.end() )
	{
		if ( !SDL_RemoveTimer( timerid ) )
			tong_dbg( "SDL_RemoveTimer return fail, timerid is invalid\n" );

		TongJsTimer* ptimer;
		ptimer = iter->second;
		ptimer->timer_sym.Dispose();
		delete ptimer;
		_timerIdSymMap.erase( iter );
	}
	else
		tong_dbg( "SDL KillTimer ID not valid\n" );

	return Undefined();
}


/////////////////////////////////////////////////////////////////////////////////////////
//
// event handler,
//
static v8::Persistent<v8::String>		_event_keydown_sym;
static v8::Persistent<v8::String>		_event_keyup_sym;
static v8::Persistent<v8::String>		_event_mousemotion_sym;
static v8::Persistent<v8::String>		_event_mousebtndn_sym;
static v8::Persistent<v8::String>		_event_mousebtnup_sym;

//		
// testing hard coded for keydown
//
static bool _addEventHandler ( String::Utf8Value& eventName, String::Utf8Value& eventHandler )
{
	v8::Local<v8::Value>		eventproc_val;
	v8::Local<v8::String>		eventproc_sym;
	v8::Local<v8::Object>		global = v8::Context::GetCurrent()->Global();

	eventproc_sym = v8::Local<v8::String>::New(v8::String::NewSymbol( *eventHandler ));
	eventproc_val = global->Get(eventproc_sym);
	if ( !eventproc_val->IsFunction() )
	{
		tong_dbg( "Error: handler '%s' not found for event \n", *eventHandler, *eventName );
		return false;
	}

#define MATCH_EVENT_HANDLER0(NAME,SYMBOL) \
	if ( ! strcmp( *eventName, NAME ) ) \
		SYMBOL = v8::Persistent<v8::String>::New(v8::String::NewSymbol( *eventHandler ))

#define MATCH_EVENT_HANDLER1(NAME,SYMBOL) \
	else if ( ! strcmp( *eventName, NAME ) ) \
		SYMBOL = v8::Persistent<v8::String>::New(v8::String::NewSymbol( *eventHandler ))

	MATCH_EVENT_HANDLER0( "keydown",		_event_keydown_sym );
	MATCH_EVENT_HANDLER1( "keyup",			_event_keyup_sym );
	MATCH_EVENT_HANDLER1( "mousemotion",	_event_mousemotion_sym );
	MATCH_EVENT_HANDLER1( "mousebtndown",	_event_mousebtndn_sym );
	MATCH_EVENT_HANDLER1( "mousebtnup",		_event_mousebtnup_sym );

	/*
	if ( ! strcmp( *eventName, "keydown" ) )
		_event_keydown_sym = v8::Persistent<v8::String>::New(v8::String::NewSymbol( *eventHandler ));
	else if ( ! strcmp( *eventName, "keyup" ) )
		_event_keyup_sym = v8::Persistent<v8::String>::New(v8::String::NewSymbol( *eventHandler ));
	else if ( ! strcmp( *eventName, "mousemotion" ) )
		_event_mousemotion_sym = v8::Persistent<v8::String>::New(v8::String::NewSymbol( *eventHandler ));
	else if ( ! strcmp( *eventName, "mousebtndown" ) )
		_event_mousebtndn_sym = v8::Persistent<v8::String>::New(v8::String::NewSymbol( *eventHandler ));
	else if ( ! strcmp( *eventName, "mousebtnup" ) )
		_event_mousebtnup_sym = v8::Persistent<v8::String>::New(v8::String::NewSymbol( *eventHandler ));
	*/

	return true;
}

Handle<Value> AddListener(const v8::Arguments& args)
{
	v8::HandleScope scope;

	if (args.Length() < 2 )
		return scope.Close(ThrowException(v8::Exception::Error(v8::String::New("Type or handler missing"))));
	
	v8::String::Utf8Value strEventName(args[0]);
	v8::String::Utf8Value strHandler(args[1]);
	tong_dbg( "[%s] (%s:%s)\r\n", "AddListener", *strEventName, *strHandler );
	if ( !strEventName.length() || !strHandler.length() )
		return scope.Close(ThrowSDLException("Type or handler missing"));

	if ( !_addEventHandler( strEventName, strHandler ) )
		return scope.Close(ThrowSDLException("Handler not exist"));

	return v8::Undefined();
}

namespace IMG {

/////////////////////////////////////////////////////////////////////////////////////////
//
//	SDL Image
//
/////////////////////////////////////////////////////////////////////////////////////////
	
v8::Handle<v8::Value> Load ( const v8::Arguments& args )
{
	v8::HandleScope scope;

	if (!(args.Length() == 1 && args[0]->IsString())) 
		return ThrowException(Exception::TypeError(String::New("Invalid arguments: Expected IMG::Load(String)")));

	String::Utf8Value file(args[0]);
	SDL_Surface *image;

	image = IMG_Load(*file);
	if(!image) 
		return ThrowException( Exception::Error( String::Concat( String::New("IMG::Load failed: "), String::New(IMG_GetError()) )));

	return scope.Close( WrapSurface( image ) );
}

} // namespace IMG


namespace TTF {

/////////////////////////////////////////////////////////////////////////////////////////
//
//	SDL Text render
//
/////////////////////////////////////////////////////////////////////////////////////////

Handle<Value> Init ( const Arguments& args )
{
	HandleScope scope;

	if (!(args.Length() == 0)) 
		return ThrowException(Exception::TypeError(String::New("Invalid arguments: Expected TTF::Init()")));

	if (TTF_Init() < 0) 
		return ThrowException(Exception::Error(String::Concat(
					String::New("TTF::Init: "),
					String::New(TTF_GetError()) 
					)));

	return Undefined();
}

Handle<Value> Quit ( const Arguments& args )
{
	HandleScope scope;

	if (!(args.Length() == 0)) 
		return ThrowException(Exception::TypeError(String::New("Invalid arguments: Expected TTF::Quit()")));

	TTF_Quit();

	return Undefined();
}

Handle<Value> OpenFont ( const Arguments& args )
{
	HandleScope scope;

	if (!(args.Length() == 2 && args[0]->IsString() && args[1]->IsNumber())) 
		return ThrowException(Exception::TypeError(String::New("Invalid arguments: Expected TTF::OpenFont(fontfile, ptsize)")));

	String::Utf8Value file(args[0]);
	int ptsize = (args[1]->Int32Value());

	TTF_Font* font = TTF_OpenFont(*file, ptsize);
	if (font == NULL) 
		return ThrowException(Exception::Error(String::Concat(
					String::New("TTF::OpenFont: "),
					String::New(TTF_GetError())
					)));

	return scope.Close(WrapFont(font));
}

Handle<Value> CloseFont ( const Arguments& args )
{
	HandleScope scope;

	if (!(args.Length() == 1 && args[0]->IsObject() )) 
		return ThrowException(Exception::TypeError(String::New("Invalid arguments: Expected TTF::CloseFont(TTF_Font)")));

	TTF_Font* font = UnwrapFont(args[0]->ToObject());
	if (font == NULL) 
		return ThrowException(Exception::Error(String::Concat(
					String::New("TTF::CloseFont: "),
					String::New(TTF_GetError())
					)));

	TTF_CloseFont( font );

	return Undefined();
}

Handle<Value> RenderTextBlended ( const Arguments& args )
{
	HandleScope scope;

	if (!(args.Length() == 3 && args[0]->IsObject() && args[1]->IsString() && args[2]->IsNumber())) 
		return ThrowException(Exception::TypeError(String::New("Invalid arguments: Expected TTF::RenderTextBlended(Font, String, Number)")));

	SDL_PixelFormat* vfmt = SDL_GetVideoInfo()->vfmt;
	TTF_Font* font = UnwrapFont(args[0]->ToObject());
	String::Utf8Value text(args[1]);
	int colorCode = args[2]->Int32Value();

	Uint8 r, g, b;
	SDL_GetRGB(colorCode, vfmt, &r, &g, &b);

	SDL_Color color;
	color.r = r;
	color.g = g;
	color.b = b;

	SDL_Surface *resulting_text;
	resulting_text = TTF_RenderText_Blended(font, *text, color);
	if (!resulting_text)
		return ThrowException(Exception::Error(String::Concat(
					String::New("TTF::RenderTextBlended: "),
					String::New(TTF_GetError())
					)));

	return scope.Close(WrapSurface(resulting_text));
}

Handle<Value> RenderTextSolid ( const Arguments& args )
{
	HandleScope scope;

	if (!(args.Length() == 3 && args[0]->IsObject() && args[1]->IsString() && args[2]->IsNumber())) 
		return ThrowException(Exception::TypeError(String::New("Invalid arguments: Expected TTF::RenderTextBlended(Font, String, Number)")));

	SDL_PixelFormat* vfmt = SDL_GetVideoInfo()->vfmt;
	TTF_Font* font = UnwrapFont(args[0]->ToObject());
	String::Utf8Value text(args[1]);
	int colorCode = args[2]->Int32Value();

	Uint8 r, g, b;
	SDL_GetRGB(colorCode, vfmt, &r, &g, &b);

	SDL_Color color;
	color.r = r;
	color.g = g;
	color.b = b;

	SDL_Surface *resulting_text;
	resulting_text = TTF_RenderText_Solid(font, *text, color);
	if (!resulting_text)
		return ThrowException(Exception::Error(String::Concat(
					String::New("TTF::RenderTextSolid: "),
					String::New(TTF_GetError())
					)));

	return scope.Close(WrapSurface(resulting_text));
}

Handle<Value> RenderTextShaded ( const Arguments& args )
{
	HandleScope scope;

	if (!(args.Length() == 4 && args[0]->IsObject() && args[1]->IsString() && args[2]->IsNumber() && args[3]->IsNumber() )) 
		return ThrowException(Exception::TypeError(String::New("Invalid arguments: Expected TTF::RenderTextBlended(Font, String, fb color number, bg color number)")));

	SDL_PixelFormat* vfmt = SDL_GetVideoInfo()->vfmt;
	TTF_Font* font = UnwrapFont(args[0]->ToObject());
	String::Utf8Value text(args[1]);
	int colorCodeFg = args[2]->Int32Value();
	int colorCodeBg = args[3]->Int32Value();

	SDL_Color colorFg, colorBg;
	Uint8 r, g, b;

	SDL_GetRGB(colorCodeFg, vfmt, &r, &g, &b);
	colorFg.r = r;
	colorFg.g = g;
	colorFg.b = b;
	SDL_GetRGB(colorCodeBg, vfmt, &r, &g, &b);
	colorBg.r = r;
	colorBg.g = g;
	colorBg.b = b;

	SDL_Surface *resulting_text;
	resulting_text = TTF_RenderText_Shaded(font, *text, colorFg, colorBg);
	if (!resulting_text) 
		return ThrowException(Exception::Error(String::Concat(
					String::New("TTF::RenderTextShaded: "),
					String::New(TTF_GetError()) 
					)));

	return scope.Close(WrapSurface(resulting_text));
}

} // namespace TTF

/////////////////////////////////////////////////////////////////////////////////////////

//
//
//
void initialize ( Handle<Object> target )
{
	_sdl_ready = false;
	_sdl_instance = target;

	TONG_SET_METHOD( target, "init",				Init );
	TONG_SET_METHOD( target, "quit",				Quit );
	TONG_SET_METHOD( target, "blitSurface",			BlitSurface);
	TONG_SET_METHOD( target, "softStretch",			SoftStretch);
	TONG_SET_METHOD( target, "createRGBSurface",	CreateRGBSurface);
	TONG_SET_METHOD( target, "displayFormatAlpha",	DisplayFormatAlpha);
	TONG_SET_METHOD( target, "fillRect",			FillRect);
	TONG_SET_METHOD( target, "flip",				Flip);
	TONG_SET_METHOD( target, "setAlpha",			SetAlpha);
	TONG_SET_METHOD( target, "setVideoMode",		SetVideoMode);
	TONG_SET_METHOD( target, "freeSurface",			FreeSurface);

	TONG_SET_METHOD( target, "setTimer",			SetTimer );
	TONG_SET_METHOD( target, "killTimer",			KillTimer );

	//
	TONG_SET_METHOD( target, "addListener",			AddListener);

	//
	Local<Object> INIT = Object::New();
	target->Set(String::New("INIT"), INIT);
		INIT->Set(String::New("TIMER"),				Number::New(SDL_INIT_TIMER));
		INIT->Set(String::New("AUDIO"),				Number::New(SDL_INIT_AUDIO));
		INIT->Set(String::New("VIDEO"),				Number::New(SDL_INIT_VIDEO));
		INIT->Set(String::New("JOYSTICK"),			Number::New(SDL_INIT_JOYSTICK));
		INIT->Set(String::New("EVERYTHING"),		Number::New(SDL_INIT_EVERYTHING));
		INIT->Set(String::New("NOPARACHUTE"),		Number::New(SDL_INIT_NOPARACHUTE));

	Local<Object> IMG = Object::New();
	target->Set(String::New("IMG"), IMG);
		TONG_SET_METHOD( IMG,	"load",				IMG::Load );

	Local<Object> SURFACE = Object::New();
	target->Set(String::New("SURFACE"), SURFACE);
		SURFACE->Set(String::New("SWSURFACE"),		Number::New(SDL_SWSURFACE));
		SURFACE->Set(String::New("HWACCEL"),		Number::New(SDL_HWACCEL));
		SURFACE->Set(String::New("OPENGL"),			Number::New(SDL_OPENGL));
		SURFACE->Set(String::New("HWSURFACE"),		Number::New(SDL_HWSURFACE));

		SURFACE->Set(String::New("ANYFORMAT"),		Number::New(SDL_ANYFORMAT));
		SURFACE->Set(String::New("ASYNCBLIT"),		Number::New(SDL_ASYNCBLIT));
		SURFACE->Set(String::New("DOUBLEBUF"),		Number::New(SDL_DOUBLEBUF));
		SURFACE->Set(String::New("HWPALETTE"),		Number::New(SDL_HWPALETTE));
		SURFACE->Set(String::New("FULLSCREEN"),		Number::New(SDL_FULLSCREEN));
		SURFACE->Set(String::New("RESIZABLE"),		Number::New(SDL_RESIZABLE));
		SURFACE->Set(String::New("RLEACCEL"),		Number::New(SDL_RLEACCEL));
		SURFACE->Set(String::New("SRCALPHA"),		Number::New(SDL_SRCALPHA));
		SURFACE->Set(String::New("SRCCOLORKEY"),	Number::New(SDL_SRCCOLORKEY));
		SURFACE->Set(String::New("PREALLOC"),		Number::New(SDL_PREALLOC));

	Local<Object> TTF = Object::New();
	target->Set(String::New("TTF"), TTF);
		TONG_SET_METHOD(TTF, "init",				TTF::Init);
		TONG_SET_METHOD(TTF, "openFont",			TTF::OpenFont);
		TONG_SET_METHOD(TTF, "renderTextBlended",	TTF::RenderTextBlended);
		TONG_SET_METHOD(TTF, "renderTextSolid",		TTF::RenderTextSolid);
		TONG_SET_METHOD(TTF, "renderTextShaded",	TTF::RenderTextShaded);
		TONG_SET_METHOD(TTF, "closeFont",			TTF::CloseFont);
		TONG_SET_METHOD(TTF, "quit",				TTF::Quit);

}

/////////////////////////////////////////////////////////////////////////////////////////

void destroy( void )
{
	helper_destroy();
	//
	_event_keydown_sym.Dispose();
	_event_keyup_sym.Dispose();
	_event_mousemotion_sym.Dispose();
	_event_mousebtndn_sym.Dispose();
	_event_mousebtnup_sym.Dispose();
	//
	TimerIdSymMapType::iterator	iter;
	while ( (iter=_timerIdSymMap.begin()) != _timerIdSymMap.end() )
	{
		TongJsTimer* ptimer;
		ptimer = iter->second;
		ptimer->timer_sym.Dispose();
		delete ptimer;
		_timerIdSymMap.erase( iter );
	}
}

bool get_ready ( void )
{
	return _sdl_ready;
}

bool handle_event_key ( SDL_Event& sdlevent, bool down )
{
	v8::Local<v8::Object>	evt = v8::Object::New();
	v8::Local<v8::Object>	global = v8::Context::GetCurrent()->Global();

	evt->Set(String::New("scancode"),	Number::New(sdlevent.key.keysym.scancode));
	evt->Set(String::New("sym"),		Number::New(sdlevent.key.keysym.sym));
	evt->Set(String::New("mod"),		Number::New(sdlevent.key.keysym.mod));

	v8::Local<v8::Value>	args[1] = { evt };
	v8::Local<v8::Value>	handlerv;
	v8::Local<v8::Function>	handler;
				
	handlerv = global->Get(down ? _event_keydown_sym : _event_keyup_sym);
	handler  = v8::Local<v8::Function>::Cast(handlerv);

	v8::TryCatch try_catch;
	handler->Call( global, 1, args );
	if (try_catch.HasCaught()) 
	{
		tong::reportException(&try_catch);
	}
	return true;
}

bool handle_event_mousemotion ( SDL_Event& sdlevent )
{
	v8::Local<v8::Object>	evt = v8::Object::New();
	v8::Local<v8::Object>	global = v8::Context::GetCurrent()->Global();

	evt->Set(String::New("state"), Number::New(sdlevent.motion.state));
	evt->Set(String::New("which"), Number::New(sdlevent.motion.which));
	evt->Set(String::New("x"), Number::New(sdlevent.motion.x));
	evt->Set(String::New("y"), Number::New(sdlevent.motion.y));
	evt->Set(String::New("xrel"), Number::New(sdlevent.motion.xrel));
	evt->Set(String::New("yrel"), Number::New(sdlevent.motion.yrel));

	v8::Local<v8::Value>	args[1] = { evt };
	v8::Local<v8::Value>	handlerv;
	v8::Local<v8::Function>	handler;
				
	handlerv = global->Get( _event_mousemotion_sym );
	handler  = v8::Local<v8::Function>::Cast(handlerv);

	v8::TryCatch try_catch;
	handler->Call( global, 1, args );
	if (try_catch.HasCaught()) 
		tong::reportException(&try_catch);

	return true;
}

bool handle_event_mousebtn ( SDL_Event& sdlevent, bool btndown )
{
	v8::Local<v8::Object>	evt = v8::Object::New();
	v8::Local<v8::Object>	global = v8::Context::GetCurrent()->Global();

	evt->Set(String::New("button"), Number::New(sdlevent.button.button));
	evt->Set(String::New("which"), Number::New(sdlevent.button.which));
	evt->Set(String::New("x"), Number::New(sdlevent.button.x));
	evt->Set(String::New("y"), Number::New(sdlevent.button.y));

	v8::Local<v8::Value>	args[1] = { evt };
	v8::Local<v8::Value>	handlerv;
	v8::Local<v8::Function>	handler;
				
	handlerv = global->Get( btndown ? _event_mousebtndn_sym : _event_mousebtnup_sym );
	handler  = v8::Local<v8::Function>::Cast(handlerv);

	v8::TryCatch try_catch;
	handler->Call( global, 1, args );
	if (try_catch.HasCaught()) 
		tong::reportException(&try_catch);

	return true;
}

bool handle_event_timer ( SDL_Event& sdlevents )
{
	TongJsTimer*	ptimer = (TongJsTimer*)sdlevents.user.data1;
	if ( ptimer )
	{
		v8::Local<v8::Object>	evt = v8::Object::New();
		v8::Local<v8::Object>	global = v8::Context::GetCurrent()->Global();
		v8::Local<v8::Value>	handlerv;
		v8::Local<v8::Function>	handler;

		handlerv = global->Get( ptimer->timer_sym );
		handler  = v8::Local<v8::Function>::Cast(handlerv);

		//evt->Set(String::New("timerid"), WrapSdlTimerId(ptimer->timerId));
		evt->Set(String::New("timerid"), Number::New((unsigned int)(ptimer->timerId)));

		v8::Local<v8::Value>	args[1] = { evt };
		v8::TryCatch try_catch;
		handler->Call( global, 1, args );
		if (try_catch.HasCaught()) 
			tong::reportException(&try_catch);
	}
	else
		tong_dbg( "!!! handle_event_timer no timerid\n" );

	return true;
}

bool handle_events ( void )
{
	v8::Local<v8::Object>	global = v8::Context::GetCurrent()->Global();
	SDL_Event				sdlevents;
	int						peepcnt;

	SDL_PumpEvents();
	peepcnt = SDL_PeepEvents( &sdlevents, 1, SDL_GETEVENT, SDL_ALLEVENTS );
	if ( peepcnt > 0 )
	{
		switch ( sdlevents.type )
		{
		case SDL_KEYDOWN : 
			if ( !_event_keydown_sym.IsEmpty() )
				return handle_event_key( sdlevents, true );
			break;

		case SDL_KEYUP :
			if ( !_event_keyup_sym.IsEmpty() )
				return handle_event_key( sdlevents, false );
			break;

		case SDL_MOUSEMOTION:
			if ( !_event_mousemotion_sym.IsEmpty() )
				return handle_event_mousemotion( sdlevents );
			break;

		case SDL_MOUSEBUTTONDOWN:
			if ( !_event_mousebtndn_sym.IsEmpty() )
				return handle_event_mousebtn( sdlevents, true );
			break;

		case SDL_MOUSEBUTTONUP:
			if ( !_event_mousebtnup_sym.IsEmpty() )
				return handle_event_mousebtn( sdlevents, false );
			break;

		case SDL_USEREVENT : // for timer...
			return handle_event_timer( sdlevents );
		}
	}
	return false;
}


} // namespace sdl
} // namespace tong

/* vim: set ts=4 sw=4 tw=0: */