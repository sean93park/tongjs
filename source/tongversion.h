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

#ifndef __tong_version_header__
#define __tong_version_header__

#define TONG_MAJOR_VERSION			0
#define TONG_MINOR_VERSION			0
#define TONG_PATCH_VERSION			1

#define TONG_VERSION_IS_RELEASE		1

#ifndef TONG_STRINGIFY
#define TONG_STRINGIFY(n) 			TONG_STRINGIFY_HELPER(n)
#define TONG_STRINGIFY_HELPER(n)	#n
#endif

#if TONG_VERSION_IS_RELEASE
# define TONG_VERSION_STRING  TONG_STRINGIFY(TONG_MAJOR_VERSION) "." \
                              TONG_STRINGIFY(TONG_MINOR_VERSION) "." \
                              TONG_STRINGIFY(TONG_PATCH_VERSION)
#else
# define TONG_VERSION_STRING  TONG_STRINGIFY(TONG_MAJOR_VERSION) "." \
                              TONG_STRINGIFY(TONG_MINOR_VERSION) "." \
                              TONG_STRINGIFY(TONG_PATCH_VERSION) "-pre"
#endif

#define TONG_VERSION		"v" TONG_VERSION_STRING

#endif // __tong_version_header__
