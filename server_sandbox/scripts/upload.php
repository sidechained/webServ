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

//// Loop through each part to process the file uploads
//foreach ($parts as $part) {
//    // Skip empty parts
//    if (trim($part) == '') continue;

//    // Extract content disposition and file name
//    if (preg_match('/Content-Disposition: form-data; name="fileToUpload"; filename="(.+)"(.+)/s', $part, $matches)) {
//        $fileName = trim($matches[1]);
//		echo $fileName;
//        $content = trim($matches[2]);
//		echo "\n\n content is: $content\n" ;

//        // Save the content to a file in the upload folder
//        file_put_contents("$uploadPath/$fileName", $content);
//    }
//}

// Initialize variables to store Content-Type and content
$contentType = '';
$fileContent = '';

// Loop through each part to process the file uploads
foreach ($parts as $part) {
    // Skip empty parts
    if (trim($part) == '') continue;

    // Check if the part contains Content-Type
    if (strpos($part, 'Content-Type:') !== false) {
        // Extract Content-Type
        $contentType = trim(preg_replace('/Content-Type: (.+)/', '$1', $part));
		echo "\n\n content is: $contentType\n" ;
    } else {
        // This part contains file content
        $fileContent .= trim($part);
		echo "\n\n content is: $fileContent\n" ;
    }
}

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
