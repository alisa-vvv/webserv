


Todo

ticket00
Item: Fix validate layer with allowed methods from config file
Description: we need from the config file the allowed methods and we need to check to set the methods not allowed message 

ticket01
Item: Fix client max body size
Description: We need to give a max body size and we need to set the macro. This will be given in config file

ticket02:
Item: Throwing exception on invalid headers
Description: we probably dont need to throw exception here and we just need to set response?

ticket03:
Item: URI should check root
Description: Directory where the requested file should be located (e.g., if URL /kapouet is rooted to /tmp/www, URL /kapouet/pouic/toto/pouet will search for /tmp/www/pouic/toto/pouet). We probably need to change the URL in URIvalidation

ticket04:
Item: Response handler must match the server and location block and serve specific paths etc
Description: If there are multiple ports, with 8080 inly allowing get and 8034 allowing all -> we need
to validate server/port

ticket05:
Item: route / to index
Description: So we always have an index

ticket06:
Item: autoindex
Descrption: generate html directory


ticket07
Item: http redirections
Description: When a client requests a URL, the web server responds with a specific 3xx status code and a Location header containing the new URL. The browser reads this and instantly fetches the new destination.
//We just return 301 and the browser will make another get request based on the response route

ticket08:
do not handle 1.1