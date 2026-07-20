import time

#add the start line ()
# request URI HTTPVERSION
# GET /foo HTTP/1.1
# HTTPV CODE CODESTRING
# HTTP/1.1 200 OK

"HTTP/1.1 300 WHATEVER\r\nContent-type: text/html\r\n\r\nHello, world!" # this string can be send as is

# this string just needs to check the HTTP version, and the first header needs to be replaced:
"GET /foo HTTP/1.1\r\nContent-type: text/html\r\n\r\nHello, world!" # replace first header with "HTTP.1.1 200 OK\r\n"
#so the result should be:
"HTTP/1.1 200 OK\r\nContent-type: text/html\r\n\r\nHello, world!"

time.sleep(1)
# headers
print("HTTP/1.1 300 WHATEVER", end="\r\n")
print("Content-type: text/html", end="\r\n")
# end-of-header marker
print(end="\r\n")
# body
print("Hello, world!")
