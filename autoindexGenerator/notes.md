when we set 'autoindex: true' in the server for a given location the server will serve an index for that location
a route may be a file or a file path, but autoindex seems to make sense only in context of a file path
in NGINX location is a versatile directive that can match various types of URLs, including specific paths, files, or patterns
when it specifies a path, the rules will also apply to all subpaths

for now, i will make it work only with the specified path
later we can make it work for subpaths too

when autoindex is true, when a GET request is sent with the path as a resource, the server should server a response of the current directory listing as a html page

how can we create the index file from 

how does an index.html look for a given 

there is nothing to say we should be able to navigate, is is enough that the listing is generated and served

see https://github.com/cclaude42/webserv/tree/master/srcs/autoindex
