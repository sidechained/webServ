<?php

$uploadPath = $_SERVER['UPLOAD_PATH'];
$boundary = $_SERVER['BOUNDARY'];

//echo "path = $uploadPath";
//echo "path = $boundary";

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
	echo "part<br>";
	echo $part;

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

	
	// Output the results
	echo "Name: $name<br>";
	echo "Filename: $fileName<br>";
	echo "Body:<br>$body<br>";
	$filePath = $uploadPath . "/" . $fileName;
	$body = str_replace(["\r", "--"], "", $body);
	echo "Body after:<br>$body<br>";
	
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
    <h1>Form Submission Result</h1>
    <p>Upload folder: $uploadPath</p>
    <p>boundary: $boundary</p>
    <p>Request: $request</p>
</body>
</html>";

// Send the HTTP response headers
//header("Content-Type: text/html");
//header("Content-Length: " . strlen($htmlResponse));

// Output the HTML response to stdout
echo $htmlResponse;
