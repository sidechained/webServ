<?php

function removeAfterDoubleHyphen($inputString) {
	$result = strstr($inputString, '--', true); 
	return $result !== false ? $result : $inputString;
}

$uploadPath = $_SERVER['UPLOAD_PATH'];
$boundary = $_SERVER['BOUNDARY'];

echo $uploadPath;
// Create the directory if it does not exist
if (is_dir($uploadPath)) {
	echo "upload path exists <br>";
} else {
	echo "upload path does not exist <br>";
	echo "creating upload path <br>";
	mkdir($uploadPath, 0777, true);
}

 // Read the HTTP request from stdin
 $request = '';
 while ($line = fgets(STDIN)) {
     $request .= $line;
 }

// Split the request into parts using the boundary
$parts = explode("--$boundary", $request);

// Initialize variables to store Content-Type and content
$content = '';
$fileName = '';
$fileContent = '';
$body = '';

// Loop through each part to process the file uploads
foreach ($parts as $part) {
	$part = str_replace($boundary, '', $part);

	// Extract name and filename using regular expressions
	if (preg_match('/name="([^"]+)"/', $part, $matches)) {
		$name = $matches[1];
	}
	
	if (preg_match('/filename="([^"]+)"/', $part, $matches)) {
		$fileName = $matches[1];
	}
	
	// Remove the first two lines
	$part = preg_replace('/^(.*\n){3}/', '', $part);
	
	// Remove the last two lines
	$part = preg_replace('/\n-----------------------------[^\n]+--$/', '', $part);
	
	// Remove leading and trailing newlines from the body
	$body = trim($part);

	$filePath = $uploadPath . "/" . $fileName;

	$body = removeAfterDoubleHyphen($body);

	//$body = preg_replace('/\n[^\n]*$/', '', $body);

	// Open the file for writing
	$file = fopen($filePath, "w");
	
	if ($file) {
		// Write "hello world" to the file
		fwrite($file, $body);
	
		// Close the file
		fclose($file);
	
		//echo "File '$fileName' has been created and saved to '$uploadPath'.";
	} else {
		echo "Failed to create the file.";
	}
}

// Define the file name and upload path
//$fileName = "example.txt"; // Change this to your desired file name
//$uploadPath = "/path/to/your/upload/directory/"; // Change this to your desired directory path

// Create the full file path


// Generate a custom HTML response
$htmlResponse = "<html>
<head>
    <title>Form Submission Result</title>
</head>
<body>
    <h1>Upload success!</h1>
    <a href='/uploads'>Click here to view your files</a>
</body>
</html>";

// Send the HTTP response headers
//header("Content-Type: text/html");
//header("Content-Length: " . strlen($htmlResponse));

// Output the HTML response to stdout
echo $htmlResponse;
