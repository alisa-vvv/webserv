
Every connection/client creates an Http Object - this is specific for each request. 

This object goes through a linear pipeline: Parse → Route → Validate → Handle → Execute → Build Response String.

So when we get the string from recv....

Parsing - httpParser,cpp- send the string to parseRequest - parse it into parts
Routing - setRequestConfig.cpp - match the request to server/location block
Validation - httpValidate.cpp - validate the parsed parts, the allowed methods from routing, uri, permissions, detect file extensions
Handle Response - buildResponse.cpp - dispatcher
Method execution - handleGet/Post/Delete/Error/AutoindexReturn - execute the logic
Build Response String - buildResponse.cpp - build the string, ready for send