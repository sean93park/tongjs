/*
portion copy from https://github.com/joyent/node
portion copy from http://www.enlightenment.org elev8 project
*/
/*
Copyright Joyent, Inc. and other Node contributors.
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

#include <unistd.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "tong.h"
#include "tongsdl.h"
#include "tongversion.h"
#include "tongdebug.h"

namespace tong {

v8::Handle<v8::Value> exp_Print (const v8::Arguments& args)
{
	return tong::getTong()->int_Print( args );
}

v8::Handle<v8::Value> exp_Read(const v8::Arguments& args)
{
	return tong::getTong()->int_Read( args );
}

v8::Handle<v8::Value> exp_Load(const v8::Arguments& args)
{
	return tong::getTong()->int_Load( args );
}

v8::Handle<v8::Value> exp_Quit(const v8::Arguments& args)
{
	return tong::getTong()->int_Quit(args);
}

v8::Handle<v8::Value> exp_Require(const v8::Arguments& args)
{
	return tong::getTong()->int_Require(args);
}

v8::Handle<v8::Value> exp_V8Version(const v8::Arguments& args)
{
	return tong::getTong()->int_V8Version(args);
}

/////////////////////////////////////////////////////////////////////////////////////////

v8::Handle<v8::Value> exp_NeedTickCallback(const v8::Arguments& args)
{
	return tong::getTong()->int_NeedTickCallback(args);
}

/////////////////////////////////////////////////////////////////////////////////////////

#define SYSKEYDEF( VAR, NAME, NUMBER )	\
	VAR->Set( v8::String::New(NAME), v8::Integer::New(NUMBER))

// Creates a new execution environment containing the built-in
// functions.
v8::Persistent<v8::Context> tong::createSystemContext ( void )
{
	// Create a template for the global object.
	v8::Handle<v8::ObjectTemplate> global = v8::ObjectTemplate::New();

	// Bind the global functions~
	global->Set( v8::String::New("print"), 				v8::FunctionTemplate::New(exp_Print));
	global->Set( v8::String::New("read"), 				v8::FunctionTemplate::New(exp_Read));
	global->Set( v8::String::New("load"), 				v8::FunctionTemplate::New(exp_Load));
	global->Set( v8::String::New("quit"), 				v8::FunctionTemplate::New(exp_Quit));
	global->Set( v8::String::New("require"), 			v8::FunctionTemplate::New(exp_Require));
	
	global->Set( v8::String::New("v8version"), 			v8::FunctionTemplate::New(exp_V8Version));
	global->Set( v8::String::New("VERSION"), 			v8::String::New(TONG_VERSION_STRING));

	return v8::Context::New(NULL, global);
}


// set all process data passed to tongmain function
v8::Persistent<v8::Object> tong::setup_procss_param ( int argc, char* argv[] )
{
	v8::Local<v8::FunctionTemplate> process_template;
	v8::Persistent<v8::Object> process_param;

	process_template = v8::FunctionTemplate::New();
	process_param = v8::Persistent<v8::Object>::New(process_template->GetFunction()->NewInstance());

	// set global system constants
	process_param->Set(v8::String::NewSymbol("version"), v8::String::New(TONG_VERSION_STRING));

	// pass command line arguments
	int		i, j, option_end_index = 1;
	v8::Local<v8::Array> arguments = v8::Array::New(argc - option_end_index + 1);
	arguments->Set(v8::Integer::New(0), v8::String::New(argv[0]));
	for (j = 1, i = option_end_index; i < argc; j++, i++) 
		arguments->Set(v8::Integer::New(j), v8::String::New(argv[i]));
	process_param->Set(v8::String::NewSymbol("argv"), arguments);

	//
	process_param->Set(v8::String::NewSymbol("_needTickCallback"),	v8::FunctionTemplate::New(exp_NeedTickCallback)->GetFunction());

	return process_param;
}

// Reads a file into a v8 string.
v8::Handle<v8::String> tong::readFile(const char* name)
{
	FILE* file = fopen(name, "rb");
	if (file == NULL) return v8::Handle<v8::String>();

	fseek(file, 0, SEEK_END);
	int size = ftell(file);
	rewind(file);

	char* chars = new char[size + 1];
	chars[size] = '\0';
	for (int i = 0; i < size;) {
		int read = fread(&chars[i], 1, size - i, file);
		i += read;
	}
	fclose(file);
	v8::Handle<v8::String> result = v8::String::New(chars, size);
	delete[] chars;
	return result;
}

// The callback that is invoked by v8 whenever the JavaScript 'print'
// function is called.  Prints its arguments on stdout separated by
// spaces and ending with a newline.
v8::Handle<v8::Value> tong::int_Print (const v8::Arguments& args)
{
	bool first = true;

	tong_scriptmsg( "[SCRIPT] " );
	for (int i = 0; i < args.Length(); i++)
	{
		v8::HandleScope handle_scope;
		if (first) {
			first = false;
		}
		else {
			tong_scriptmsg( " " );
		}
		v8::String::Utf8Value str(args[i]);
		const char* cstr = toCString(str);
		tong_scriptmsg( "%s", cstr );
	}
	tong_scriptmsg( "\n" );
	fflush( stdout );
	return v8::Undefined();
}

// The callback that is invoked by v8 whenever the JavaScript 'read'
// function is called.  This function loads the content of the file named in
// the argument into a JavaScript string.
v8::Handle<v8::Value> tong::int_Read(const v8::Arguments& args)
{
	if (args.Length() != 1)
	{
		return v8::ThrowException(v8::String::New("Bad parameters"));
	}
	v8::String::Utf8Value file(args[0]);
	if (*file == NULL) {
		return v8::ThrowException(v8::String::New("Error loading file"));
	}
	v8::Handle<v8::String> source = readFile(*file);
	if (source.IsEmpty()) {
		return v8::ThrowException(v8::String::New("Error loading file"));
	}
	return source;
}

// The callback that is invoked by v8 whenever the JavaScript 'load'
// function is called.  Loads, compiles and executes its argument
// JavaScript file.
v8::Handle<v8::Value> tong::int_Load(const v8::Arguments& args)
{
	for (int i = 0; i < args.Length(); i++)
	{
		v8::HandleScope handle_scope;
		v8::String::Utf8Value file(args[i]);
		if (*file == NULL) {
			return v8::ThrowException(v8::String::New("Error loading file"));
		}
		v8::Handle<v8::String> source = readFile(*file);
		if (source.IsEmpty()) {
			return v8::ThrowException(v8::String::New("Error loading file"));
		}
		if (!executeString(source, v8::String::New(*file), false, false)) {
			return v8::ThrowException(v8::String::New("Error executing file"));
		}
	}
	return v8::Undefined();
}

// The callback that is invoked by v8 whenever the JavaScript 'quit'
// function is called.  Quits.
v8::Handle<v8::Value> tong::int_Quit(const v8::Arguments& args)
{
	// If not arguments are given args[0] will yield undefined which
	// converts to the integer value 0.
	int exit_code = args[0]->Int32Value();
	fflush(stdout);
	fflush(stderr);
	//exit(exit_code);
	m_exitcode = exit_code;
	m_doexitmainloop = true;
	return v8::Undefined();
}

//
//
//
#define MODLOAD_ENV					"TONG_MODPATH"
#define MODLOAD_ENV_DEFAULT_DIRS	".:./lib:/usr/local/lib:/usr/lib"
#define	MODLOAD_PREFIX				"lib"
#define	MODLOAD_SUFFIX				".tong"
#define	MODLOAD_SUFFIX_JS			".js"

static v8::Persistent<v8::Object> module_cache;

static char * find_module_file_name ( char *module_name, const char *prefix, const char *type )
{
	char *modpath = getenv(MODLOAD_ENV);
	char default_modpath[] = MODLOAD_ENV_DEFAULT_DIRS;

	if (!modpath) modpath = default_modpath;
    
	for (char *token, *rest, *ptr = modpath; (token = strtok_r(ptr, ":", &rest)); ptr = rest)
	{
		char full_path[PATH_MAX];

		if (snprintf(full_path, PATH_MAX - 1, "%s/%s%s%s", token, prefix, module_name, type) < 0)
			return NULL;

		if (!access(full_path, R_OK))
			return strdup(full_path);
	}
    
	return NULL;
}

inline static char * find_native_module_file_name ( char *module_name )
{
	tong_dbg( "[%s] (%s)\r\n", "find_native_module_file_name", module_name );
	return find_module_file_name( module_name, MODLOAD_PREFIX, MODLOAD_SUFFIX );
}

inline static char * find_js_module_file_name ( char *module_name )
{
	tong_dbg( "[%s] (%s)\r\n", "find_js_module_file_name", module_name );

	char *  modfile;
	modfile = find_module_file_name(module_name, "", MODLOAD_SUFFIX_JS );
	if ( modfile ) return modfile;
	return find_module_file_name(module_name, "", "" );
}


bool tong::module_native_load ( char *module_name, v8::Handle<v8::Object> name_space, ContextUseRule )
{
	char *file_name = find_native_module_file_name(module_name);
	if (!file_name) return false;
	tong_dbg( "Loading native module: %s\r\n", file_name );

	void *handle = dlopen(file_name, RTLD_LAZY);
	if (!handle)
	{
		tong_dbg( "Could not dlopen(%s): %s\r\n", file_name, dlerror() );
		free(file_name);
		return false;
	}

	void (*init_func)(v8::Handle<v8::Object> name_space);
	init_func = (void (*)(v8::Handle<v8::Object>))dlsym(handle, "RegisterModule");
	if (!init_func)
	{
		tong_dbg( "Could not dlsym(%p, RegisterModule): %s\r\n", handle, dlerror());
		free(file_name);
		dlclose(handle);
		return false;
	}

	tong_dbg( "Initializing native module: %s\n", file_name);
	init_func(name_space);

	name_space->Set(v8::String::NewSymbol("__dl_handle"), v8::External::Wrap(handle));
	name_space->Set(v8::String::NewSymbol("__file_name"), v8::String::New(file_name));

	free(file_name);
	return true;
}

bool tong::module_js_load ( char *module_name, Handle<Object> name_space, ContextUseRule context_use_rule )
{
	char *file_name = find_js_module_file_name(module_name);
	bool return_value = false;

	tong_dbg( "[module_js_load] %s > %s\n", module_name, file_name );
	if (!file_name) return false;

	Handle<String> mod_source = readFile(file_name);
	if (mod_source.IsEmpty())
	{
		free(file_name);
		return false;
	}
	//
	HandleScope			handle_scope;
	Persistent<Context> mod_context;
	
	/*
	if (context_use_rule == CREATE_NEW_CONTEXT)
	{
		// Fix: global type
		v8::Handle<v8::ObjectTemplate> global = v8::ObjectTemplate::New();
		global->Set(String::NewSymbol("exports"), name_space);
		mod_context = Context::New(NULL, global);
		mod_context->Enter();
	}
	*/

	TryCatch try_catch;
	Local<Script> mod_script = Script::Compile(mod_source->ToString(), String::New(file_name));
	if (try_catch.HasCaught())
	{
		tong_dbg( "[module_js_load] error...\n" );
		goto end;
	}
	mod_script->Run();
	if (try_catch.HasCaught())
	{
		goto end;
	}

	name_space->Set(String::NewSymbol("__file_name"), String::New(file_name));
	// FIXME: How to wrap mod_context so that t can be Disposed() properly?

	return_value = true;

end:
	free(file_name);

	/*
	if (context_use_rule == CREATE_NEW_CONTEXT)
		mod_context->Exit();
	*/

	return return_value;
}


bool tong::load_module_with_type_hints ( v8::Handle<v8::String> module_name, v8::Local<v8::Object> name_space, 
											ContextUseRule context_use_rule )
{
	v8::String::Utf8Value module_name_utf(module_name);

	tong_dbg( "[%s] (%s)\r\n", "load_module_with_type_hints", *module_name_utf );
	if ( !strcasecmp( *module_name_utf, "sdl" ) )
	{
		sdl::initialize( name_space );
		return true;
	}

	if ( module_native_load(*module_name_utf, name_space, context_use_rule) )
		return true;
	return module_js_load(*module_name_utf, name_space, context_use_rule);
		
}

v8::Handle<v8::Value> tong::load_module ( v8::Handle<v8::String> module_name, ContextUseRule context_use_rule )
{
	v8::HandleScope scope;

	if ( module_cache->HasOwnProperty(module_name))
		return scope.Close(module_cache->Get(module_name));
	
	v8::Local<v8::Object> name_space = (context_use_rule == CREATE_NEW_CONTEXT) ?
								v8::Object::New() : 
								v8::Context::GetCurrent()->Global();

	if (load_module_with_type_hints(module_name, name_space, context_use_rule))
	{
		module_cache->Set(module_name, v8::Persistent<v8::Object>::New(name_space));
		return scope.Close(name_space);
	}
	v8::Local<v8::String> msg = v8::String::Concat(v8::String::New("Cannot load module: "), module_name);
	return scope.Close(ThrowException(v8::Exception::Error(msg)));
}

v8::Handle<v8::Value> tong::int_Require(const v8::Arguments& args)
{
	v8::HandleScope scope;
	if (args.Length() < 1 )
		return scope.Close(ThrowException(v8::Exception::Error(v8::String::New("Module name missing"))));
	
	v8::String::Utf8Value strModName(args[0]);
	tong_dbg( "[%s] (%s)\r\n", "tong::int_Require", *strModName );
	if ( !strModName.length() )
		return scope.Close(ThrowException(v8::Exception::Error(v8::String::New("Module name missing"))));

	return scope.Close( load_module( args[0]->ToString(), CREATE_NEW_CONTEXT ) );
}

v8::Handle<v8::Value> tong::int_NeedTickCallback(const v8::Arguments& args) 
{
	v8::HandleScope scope;
	
	m_need_tick_cb = true;
	/*
	// TODO: this tick_spinner shouldn't be necessary. An ev_prepare should be
	// sufficent, the problem is only in the case of the very last "tick" -
	// there is nothing left to do in the event loop and libev will exit. The
	// ev_prepare callback isn't called before exiting. Thus we start this
	// tick_spinner to keep the event loop alive long enough to handle it.
	if (!uv_is_active((uv_handle_t*) &tick_spinner)) 
	{
		uv_idle_start(&tick_spinner, Spin);
		uv_ref(uv_default_loop());
	}
	*/
	return Undefined();
}


v8::Handle<v8::Value> tong::int_V8Version(const v8::Arguments& args)
{
	return v8::String::New(v8::V8::GetVersion());
}

void tong::init_module_cache ( void )
{
	module_cache = v8::Persistent<v8::Object>::New(v8::Object::New());
}

void tong::destroy_module_cache ( void )
{
	module_cache.Dispose();
}


}
