import time

#add the start line ()
# request URI HTTPVERSION
# GET /foo HTTP/1.1
# HTTPV CODE CODESTRING
# HTTP/1.1 201 OK but cooler

time.sleep(1)
# headers
print("Content-type: text/html", end="\r\n")
# end-of-header marker
print(end="\r\n")
# body
print("Hello, world!")
