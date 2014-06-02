# Simple HTTP Server#
This adaptation of the default HTTP server provided in the libevent library.

It uses doc root as *"/var/www/html"* to serve the html files. If the request 
is made with location /sync then it would use synchronous workers to process
request. Example:  

   curl http://localhost:4242/sync/index.html  
   
This would use worker pool to fetch the index.html. Similarly: 
   curl http://localhost:4242/async/index.html  

would return the index.html using async task base processing.

## Uses concepts ##
GExectorService and DeferredTask

#HTTP sync async server#
Provides introduction to low level GExecutor task interfaces. Functionally, it
behaves similar to the Simple HTTP Server example.