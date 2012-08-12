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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h> /* PATH_MAX */
#include <assert.h>
#include <locale.h>
#include <signal.h>
#include <SDL.h>
#include <unistd.h>

#include "tong.h"
#include "tongsdl.h"
#include "tongdebug.h"
#include "tongversion.h"

namespace tong {

tong*	tong::m_tongSingletone = NULL;

tong::tong()
{
	m_exitcode = 0;
	m_doexitmainloop = false;
	m_need_tick_cb = false;
}

tong::~tong()
{
}

tong * tong::createInstance ( void )
{
	if ( m_tongSingletone == NULL )
		m_tongSingletone = new tong();
	return m_tongSingletone;
}

void tong::destroy ( void )
{
	if ( m_tongSingletone )
	{
		delete m_tongSingletone;
	}
	m_tongSingletone = NULL;
}

tong* tong::getTong ( void )
{
	return m_tongSingletone;
}

bool tong::init( int argc, char* argv[] )
{
	// init 
    return true;
}

/////////////////////////////////////////////////////////////////////////////////////////

const char* tong::toCString(const v8::String::Utf8Value& value)
{
	return *value ? *value : "<string conversion failed>";
}

void tong::reportException ( v8::TryCatch* try_catch )
{
	v8::HandleScope handle_scope;
	v8::String::Utf8Value exception(try_catch->Exception());
	const char* exception_string = toCString(exception);
	v8::Handle<v8::Message> message = try_catch->Message();

	if (message.IsEmpty())
	{
		// V8 didn't provide any extra information about this error; just
		// print the exception.
		if ( exception_string ) 
			tong_dbg( "%s\n", exception_string );
		else
			tong_dbg( "%s\n", "!!! real bad exception !!!\n" );
	}
	else
	{
		v8::String::Utf8Value filename(message->GetScriptResourceName());
		v8::String::Utf8Value sourceline(message->GetSourceLine());
		v8::String::Utf8Value stack_trace(try_catch->StackTrace());

		const char* filename_string = toCString(filename);
		const char* sourceline_string = toCString(sourceline);
		int linenum = message->GetLineNumber();
		int start = message->GetStartColumn();

		// Print (filename):(line number): (message).
		tong_dbg( "%s:%i: ??\n", filename_string, linenum ); //, exception_string);

		// Print line of source code.
		tong_dbg( "%s\n", sourceline_string);

		// Print wavy underline (GetUnderline is deprecated).
		for (int i = 0; i < start; i++) printf(" ");
		int end = message->GetEndColumn();
		for (int i = start; i < end; i++) tong_dbg( "^" );
		tong_dbg( "\n" );

		if (stack_trace.length() > 0) {
			const char* stack_trace_string = toCString(stack_trace);
			tong_dbg( "%s\n", stack_trace_string );
		}
	}
	tong_dbg( "\n" );
	if ( m_tongSingletone )
		m_tongSingletone->stopabnormal();
}


// Executes a string within the current v8 context.
bool tong::executeString ( v8::Handle<v8::String> source,
                   	   	   v8::Handle<v8::Value> name,
                   	   	   bool print_result,
                   	   	   bool report_exceptions)
{
	v8::HandleScope handle_scope;
	v8::TryCatch try_catch;
	v8::Handle<v8::Script> script;

	script = v8::Script::Compile(source, name);
	if (script.IsEmpty())
	{
		// Print errors that happened during compilation.
		if (report_exceptions)
			reportException(&try_catch);
		return false;
	}

	v8::Handle<v8::Value> result = script->Run();
	if (result.IsEmpty()) {
		assert(try_catch.HasCaught());
		// Print errors that happened during execution.
		if (report_exceptions)
			reportException(&try_catch);
		return false;
	}
	assert(!try_catch.HasCaught());
	if ( print_result )
	{
		if ( !result->IsUndefined() )
		{
			// If all went well and the result wasn't undefined then print
			// the returned value.
			v8::String::Utf8Value str(result);
			const char* cstr = toCString(str);
			tong_dbg( "%s\n", cstr );
		}
	}
	return true;
}

bool tong::start_tong ( void )
{
	const char* str = "tong.js";

	v8::TryCatch try_catch;
	v8::Handle<v8::String> file_name;
	v8::Handle<v8::String> source;

	// Use all other arguments as names of files to load and run.
	file_name = v8::String::New(str);
	source = readFile(str);
	if (source.IsEmpty())
	{
		tong_dbg( "Error reading '%s'\n", str);
		return false;
	}
	tong_dbg( "[%s #1] %d ok\n", __FILE__, __LINE__ );
	executeString( source, file_name, true, true );
	tong_dbg( "[%s#2 ] %d ok\n", __FILE__, __LINE__ );

	return true;
}

void tong::stopabnormal ( void )
{
	m_doexitmainloop = true;
}


void tong::emitExit( v8::Handle<v8::Object> process ) 
{
	// process.emit('exit')
	v8::Local<v8::Value> emit_v = process->Get(v8::String::New("emit"));
	assert(emit_v->IsFunction());
	v8::Local<v8::Function> emit = v8::Local<v8::Function>::Cast(emit_v);
	v8::Local<v8::Value> args[] = { v8::String::New("exit") };
	
	v8::TryCatch try_catch;
	emit->Call(process, 1, args);
	if (try_catch.HasCaught()) 
	{
		reportException(&try_catch);
	}
}

void tong::callTick ( v8::Persistent<v8::Object> process )
{
	// Avoid entering a V8 scope.
	if (!m_need_tick_cb) return;
	m_need_tick_cb = false;

	//tong_dbg( "[tong::callTick] !\n" );

	/*
	if (uv_is_active((uv_handle_t*) &tick_spinner)) {
	uv_idle_stop(&tick_spinner);
	uv_unref(uv_default_loop());
	}
	*/

	v8::HandleScope scope;

	if (m_tick_callback_sym.IsEmpty()) 
		m_tick_callback_sym = v8::Persistent<v8::String>::New(v8::String::NewSymbol("_tickCallback"));

	v8::Local<v8::Value> cb_v = process->Get(m_tick_callback_sym);
	if (!cb_v->IsFunction()) 
	{
		tong_dbg( "tong::callTick, no process._tickCallback() defined.\n" );
		return;
	}
	v8::Local<v8::Function> cb = v8::Local<v8::Function>::Cast(cb_v);

	v8::TryCatch try_catch;
	cb->Call(process, 0, NULL);
	if (try_catch.HasCaught()) 
	{
		reportException(&try_catch);
	}
}

bool tong::run ( int argc, char* argv[] )
{
	tong_dbg( "tongMain::run()...\n" );
	//
	v8::V8::Initialize();
	v8::V8::SetFlagsFromCommandLine(&argc, argv, true);
	
	v8::HandleScope handle_scope;
	v8::Persistent<v8::Context> context;

	context = createSystemContext();
	if ( context.IsEmpty() )
	{
		tong_dbg( "Error creating context\n");
		return false;
	}
	context->Enter();
	init_module_cache();

	// 1. execute the tong.js, unlike node.js return is not used
	// instead we've created JS context and will use that context
	if ( !start_tong() )
	{
		tong_dbg( "[%s] %d failed\n", __FILE__, __LINE__ );
		context->Exit();
		context.Dispose();
		v8::V8::Dispose();
		return false;
	}

	// 2. now we're going to call tongmain() the entry function of tong with one parameter "process"
	// this parameter will be link between system and JavaScript world.
	v8::Context::Scope		context_scope(context);
	v8::Local<v8::Object>	global = v8::Context::GetCurrent()->Global();
	v8::Persistent<v8::Object> process_param;

	process_param = setup_procss_param( argc, argv );

	{
		v8::Local<v8::Value>	args[1] = { v8::Local<v8::Value>::New(process_param) };
		v8::Handle<v8::String>	tongmain_name;
		v8::Handle<v8::Value>	tongmain_val;
		v8::Handle<v8::Function> tongmain_func;
		v8::Handle<v8::Value>	result;

		// call "tongmain() with tongmain_param, tongmain(tongmain_param)
		tongmain_name = v8::String::New( "tongmain" );
		tongmain_val = global->Get(tongmain_name);
		if (!tongmain_val->IsFunction())
		{
			tong_dbg( "Error: Script should declare 'tongmain' global function.\n");
			return false;
		}
		tongmain_func = v8::Handle<v8::Function>::Cast(tongmain_val);
		result = tongmain_func->Call( global, 1, args );
	}

	//
	bool		bcont = true;
	int			wait_count;

	tong_dbg( "tong::run loop... may have to ctrl+c to exit\n" );
	wait_count = 200;
	while ( bcont )
	{
		if ( sdl::get_ready() )
		{
			sdl::handle_events();
		}
		else
		{
			usleep(10000);
			wait_count--;
			if ( !wait_count )
			{
				tong_dbg( "tong::run, no sdl so quit\n" );
				break;
			}
		}

		if ( m_need_tick_cb )
			callTick( process_param );

		if ( m_doexitmainloop )
		{
			// this flag is set when quit is called
			break;
		}
	}

	//emitExit( process_param );
	sdl::destroy();
	destroy_module_cache();

	context->Exit();
	context.Dispose();

	//v8::V8::Dispose();	<-- 2012.07.08: with V8 version 3.12.9, this call makes 'Segment fault' when program exits.

	return true;
}

} // namespace tong


int main( int argc, char *argv[] )
{
     tong::tong*	tongmain;

	 tongmain = tong::tong::createInstance();
     if ( tongmain->init( argc, argv ))
     {
   		 tongmain->run( argc, argv );
     }
     tongmain->destroy();
     return 0;
}
