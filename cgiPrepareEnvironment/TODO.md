# main_from_struct.cpp

- get the upload.php script to run to succesful completion (moves the temp file to hardcoded location)
- understand how to populate the $_FILES superglobal correctly - send snippet via stdin and execute?
"$_FILES = array('myfile' => array('name' => 'fileToUpload', 'tmp_name' => 'temp.txt', 'error' => '0'))"
- handle "error code" issue - how to pass an integer into the script or convert to one from a string?
- make the PHP script give back an exit status (success or failure)
- implement a fork timeout in case the script gets stuck in a loop (advice from Cosmo)

# PostRequestParser class

- check if our real world Firefox request is parsed correctly
- adapt PostRequestParser to create a separate file for each part in the multipart file
- implement prechecks - see step 3 of cgiPhpNotes.md
- read only the amount specified by content length