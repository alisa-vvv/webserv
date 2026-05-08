Pseudocode

Http receives from socket and responds

Receive request
1. receive bytes with recv until \r\n\r\n
* it is received as char* so i dont need a translation layer
2. while receiving, put the data into a buffer. check if complete
-if content length is the same
-if \r\n\r\n received
3. if complete, parse it

Send response


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
