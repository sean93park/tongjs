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

#include <stdio.h>
#include <stdarg.h>
#include "tong.h"
#include "tongdebug.h"

namespace tong {

#if defined(TONG_DEBUG) && (TONG_DEBUG)

#pragma message "TONG_DEBUG is ON"

#define dbg_printf_bufsize		2048

static char dbg_printf_buf[dbg_printf_bufsize];

int tong_dbg ( const char * format, ... )
{
    va_list		ap;
    
	va_start(ap, format);
    vsnprintf( dbg_printf_buf, dbg_printf_bufsize, format, ap );
    fputs( dbg_printf_buf, stderr );

    va_end(ap);

    return 0;
}
#else

#pragma message "TONG_DEBUG is OFF"

#endif 

//

#if defined(TONG_SCRIPTMSG) && (TONG_SCRIPTMSG)

#pragma message "TONG_SCRIPTMSG is ON"

#define script_printf_bufsize		2048

static char script_printf_buf[script_printf_bufsize];

int tong_scriptmsg ( const char * format, ... )
{
    va_list		ap;
    
	va_start(ap, format);
    vsnprintf( script_printf_buf, script_printf_bufsize, format, ap );
    fputs( script_printf_buf, stdout );

    va_end(ap);

    return 0;
}
#else

#pragma message "TONG_SCRIPTMSG is OFF"

#endif 


} // namespace tong


