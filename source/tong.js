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

function tongmain ( process )
{
    global = this;

	// print( "Global Version = " + VERSION + ", v8 = " + v8version() );
    // print( "Version = " + process.version );

    process.processNextTick = function()  
    {
        var nextTickQueue = [];

        process._tickCallback = function() 
        {
            var l = nextTickQueue.length;
            if (l === 0) return;

            var q = nextTickQueue;
            nextTickQueue = [];

            try 
            {
                for (var i = 0; i < l; i++) q[i]();
            }
            catch (e) 
            {
                if (i + 1 < l) 
                {
                    nextTickQueue = q.slice(i + 1).concat(nextTickQueue);
                }
                if (nextTickQueue.length) 
                {
                    process._needTickCallback();
                }
                throw e; // process.nextTick error, or 'error' event on first tick
            }
        };

        process.nextTick = function(callback) 
        {
            nextTickQueue.push(callback);
            process._needTickCallback();
        };
    };

    process.processEventHandlers = function() 
    {

    };
    
    process.globalVariables = function() 
    {
        global.process = process;
    };

    process.globalVariables();
    process.processNextTick();
    process.processEventHandlers();

    for ( i=0; process.argv[i] ;i++ )
    {
        print( "argv[" + i + "] = " + process.argv[i] );
        if ( i ) 
            require( process.argv[i] );
    }
}
