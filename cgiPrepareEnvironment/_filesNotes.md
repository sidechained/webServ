HTML forms that use the POST method send their encoded information using the standard input.
You can use the CONTENT_LENGTH environment variable to determine the number of bytes to read in.

From https://github.com/cclaude42/webserv/blob/master/srcs/cgi/CgiHandler.cpp I can see that body is 

// write body to 
write(fdIn, _body.c_str(), _body.size());
lseek(fdIn, 0, SEEK_SET);
dup2(fdIn, STDIN_FILENO);

// the question is what does body contain? the absolute raw payload or also the headers? Maybe php can actually parse these headers and I don't need to do it

// how can they execute like this?
execve(scriptName.c_str(), nll, env);


https://www.php.net/manual/en/features.file-upload.post-method.php