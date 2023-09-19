We will use CGI to handle file uploads via a html form, using PHP on the server side.
File uploads are enabled by route, so if 'upload' is set in config file for a given route, uploads will be placed in that location, otherwise they are not allowed

From eval sheet:
- the CGI should be run in the correct directory for relative path file access
- check with GET and POST methods (download files, upload files)
- test with files containing errors

1. On the client side, we attempt to upload a file using a html form such 'responses/fileUploadClientForm.html', where 
- form action="upload.php"
- method="post"
- enctype="multipart/form-data"
Clicking 'upload' on the form will produce a POST request such as in 'POSTrequest.txt'

2. The request will be received by the server.

a. Check if response file extension is /cgiUploader/upload.php, if so the script can be run
b. setup the following environment variables to pass info between the server and CGI script

CGI_REQUEST_METHOD
CGI_FILE_TO_UPLOAD
CGI_MAX_CLIENT_BODY_SIZE_MB
CGI_POST_ALLOWED - dict of route, true - take from config
CGI_UPLOAD_DIR - dict of route, path - take from config - if not in dict, upload not allowed

c. Prepare POST data in the format that PHP expects

- unchunk the request body by boundary, copying the chunks to a temporarily file in /tmp
- (this location must not be accessible by the server directly)
- the CGI will expect EOF as end of the body

d. Fork and execve the php script i.e.

$ php /path/to/your/php_script.php path/to/your/file.txt

e. In the script itself

Check if the following conditions are met
- it is a valid HTTP/1.1 POST request (CGI_REQUEST_METHOD)
- POST requests are allowed for this route
- uploads are allowed in the config for this route
- the request body is less than the client_max_body_size_mb in the config (CGI_MAX_CLIENT_BODY_SIZE_MB)
-- if not return a HTTP 413 - Request Entity Too Large

responses will be served by our C code

This will move the file from the /tmp directory to the given upload directory
- ? If no content_length is returned from the CGI, EOF will mark the end of the returned data
- ? The CGI should be run in the correct directory for relative path file access.

If the script runs exit with status code EXIT_SUCCESS (0) otherwise EXIT_FAILURE (1)

6. Wait for cgi script to finish (waitpid) and act accordingly

- if EXIT_SUCCESS return a 'HTTP Status Code 200' response, with HTML body with success message
- if EXIT_FAILURE return a 'Bad Resquest 400' response

7. Send Response: After running the PHP script, we send either a `200 OK` or `400 Bad Request` HTTP response based on the exit status (0) or (1). We may also wish to capture output of the PHP script to provide an individualised error message to the Bad Request response

# Q: our CGI has to support both GET and POST methods. Upload will be POST but what will we use GET for?

# Q: why do we (need to) use multipart/form-data?

multipart/form-data allows for the transmission of binary data, such as images, audio, video, or any other type of file

File Uploads: When a user selects a file to upload using an HTML form, the data associated with that file needs to be encoded in a way that can be transmitted via HTTP. multipart/form-data facilitates this transmission.

Form Fields and Files in One Request: It allows for the inclusion of both regular form fields (e.g., text, numbers) and files within a single HTTP request, making it possible to submit a mixture of textual and binary data simultaneously.

Structured Encoding: The data is structured into multiple parts (hence "multipart"), each representing a separate piece of information, such as a file or a form field.

Boundary Delimiters: Data within multipart/form-data is separated using unique boundary delimiters, ensuring proper parsing and identification of different parts of the data.

Using multipart/form-data allows servers to interpret and handle the form data appropriately, distinguishing between regular form fields and file uploads in the HTTP request.

# Meaning of: Because you won’t call the CGI directly, use the full path as PATH_INFO.

This means that any additional path information in the URL after the script name should be treated as PATH_INFO and passed to the CGI script for processing.

For example, if the URL is http://example.com/cgi-bin/my_script/some/additional/path, "some/additional/path" would be treated as the PATH_INFO.

In summary, this guidance is suggesting that if the CGI script is not accessed directly via a URL, but rather through a different method, any additional path information in the URL should be interpreted as PATH_INFO and passed to the CGI script for appropriate handling. This is a way to provide input or context to the CGI script based on the URL's path structure.

# example PHP scripts for upload

Example: https://www.w3schools.com/Php/php_file_upload.asp