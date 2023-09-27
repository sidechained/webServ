# cgiPrepareEnvironment/main_from_struct.cpp

- get the upload.php script to run to succesful completion (moves the temp file to hardcoded location)
- understand how to populate the $_FILES superglobal correctly - send snippet via stdin and execute?
"$_FILES = array('myfile' => array('name' => 'fileToUpload', 'tmp_name' => 'temp.txt', 'error' => '0'))"
- handle "error code" issue - how to pass an integer into the script or convert to one from a string?
- DONE: make the PHP script give back an exit status (success or failure)
- implement a fork timeout in case the PHP script gets stuck in a loop (advice from Cosmo)

# PostRequestParser class

- DONE: check if our real world Firefox request is parsed correctly
- DONE: PostRequestParser now creates a separate file for each part of the multipart POST request

- implement prechecks - see step 3 of cgiPhpNotes.md
	-- 1. check config.location("key").methods allows POST request, if not then return "405 Method Not Allowed"
	-- 2. check file postUploadRequest.resource extension is .php, if not continue to server_sandbox code
	-- 3. is the client body size < max allowed client body size, if not return "413 Payload Too Large"	
- read only the amount specified by the content length field
