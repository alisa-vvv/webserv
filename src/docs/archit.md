
main
--call handleHttpRequest
--call handleHttpResponse


Http.cpp

recv() appends into httpBuffer
checkStatus() waits until full request is complete
parseRequest() once on the whole string

parsing layer

validation layer
invalid input? -> map to status message

response builder
valid input?
->no? build response, map to status builder (if not )
->yes?
	-> check hasextension 
		-> yes? call cgi response and exit to poll loop
		-> no? switch case http method, call handler
	->handlers
		->get
		->post
	->delete
->build response string
->send
	


handlehttpResponse.cpp
-setResponseCode
-getresponsestring
-sendresponsestring

