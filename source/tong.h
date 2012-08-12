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

#ifndef __hong_header__
#define __hong_header__

#include <v8.h>

#ifdef _WIN32
#	ifndef BUILDING_TONG_EXTENSION
#   	define TONG_EXTERN __declspec(dllexport)
# 	else
#   	define TONG_EXTERN __declspec(dllimport)
# 	endif
#else
# 	define TONG_EXTERN /* nothing */
#endif

#define TONG_SET_METHOD(obj, name, callback)                              \
  obj->Set(v8::String::NewSymbol(name),                                   \
           v8::FunctionTemplate::New(callback)->GetFunction())

#define TONG_PSYMBOL(s) \
  v8::Persistent<v8::String>::New(v8::String::NewSymbol(s))



namespace tong {

enum ContextUseRule {
	CREATE_NEW_CONTEXT,
	USE_CURRENT_CONTEXT
};


class tong {

public:
	tong();
	virtual ~tong();

public:
	static tong*	createInstance ( void );
	static tong*	getTong(void);

	static const char* 	toCString(const v8::String::Utf8Value& value);
	static void 	reportException(v8::TryCatch* try_catch);

protected:
	static tong*	m_tongSingletone;
	int				m_exitcode;
	//
	bool			m_doexitmainloop;
	bool			m_need_tick_cb;

public:
	void 			destroy( void );
    bool			init( int argc, char* argv[] );
    bool			initSurface( void );
    bool 			run( int argc, char* argv[] );
	void			stopabnormal( void );

protected:
	// maybe SDL here
	v8::Persistent<v8::String>	m_tick_callback_sym;

protected:
	v8::Persistent<v8::Context>
					createSystemContext ( void );
    v8::Handle<v8::String>
					readFile(const char* name);
	v8::Persistent<v8::Object>		
					setup_procss_param ( int argc, char* argv[] );
	void			callTick ( v8::Persistent<v8::Object> process );
	void			emitExit( v8::Handle<v8::Object> process );

	bool 			executeString(v8::Handle<v8::String> source, v8::Handle<v8::Value> name,
									bool print_result, bool report_exceptions);
	bool 			start_tong ( void );

	void			init_module_cache ( void );
	void			destroy_module_cache ( void );

public:
    v8::Handle<v8::Value> 			int_Print (const v8::Arguments& args);
    v8::Handle<v8::Value> 			int_Read(const v8::Arguments& args);
    v8::Handle<v8::Value> 			int_Load(const v8::Arguments& args);
    v8::Handle<v8::Value> 			int_Quit(const v8::Arguments& args);
	v8::Handle<v8::Value> 			int_Require(const v8::Arguments& args);
	v8::Handle<v8::Value>			int_NeedTickCallback(const v8::Arguments& args);
    v8::Handle<v8::Value> 			int_V8Version(const v8::Arguments& args);

protected:
	bool		module_native_load ( char *module_name, v8::Handle<v8::Object> name_space, ContextUseRule );
	bool		module_js_load ( char *module_name, v8::Handle<v8::Object> name_space, ContextUseRule context_use_rule );
	bool		load_module_with_type_hints ( v8::Handle<v8::String> module_name, v8::Local<v8::Object> name_space, 
											ContextUseRule context_use_rule );
	v8::Handle<v8::Value>	load_module ( v8::Handle<v8::String> module_name, ContextUseRule context_use_rule );

};



} // namespace tong

#endif // __hong_header__
