Pseudocode

Http receives from socket and responds

General
1. Create http class
header class? 
body class?
priv:
method
target uri
http version
content-type

pub:
getmethod
2. Create enum for error messages 
https://developer.mozilla.org/en-US/docs/Web/HTTP/Reference/Status

Receive request
1. receive bytes with recv until \r\n\r\n
* it is received as char* so i dont need a translation layer
2. while receiving, put the data into a buffer. check if complete
-if content length is the same
-if \r\n\r\n received
3. if complete, parse it (parse request)

Parse request line
1. Extract the method used (get, post)
	- if not either of those 3, ERROR: 405
2. Extract the target (absolute path and absolute form)
	- validate
3. extract the http version
	- validate
4. Extract content-type
5. Extract content length
6. E
Send response
1. create enum for http status codes
2. headers - build valid http headers
3. send response back to client

To do
Receive:
-Make class for parsed http
	-private vars path, contentlength, IP, status code
-Make getters and setters
-CHeck for which protocols can be processed?
-Make validator for every var (get vs dsjdns) or check if i always get it correct?

Test:
-Print everything

Respond
-Check all the http response codes, make enum class



use assert + do unit tests
make test 
doctest.h // google 