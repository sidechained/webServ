<?php


// Extract the "name" and "color" variables
$name = getenv('NAME');
$color = getenv('COLOR');


 //Check if the name is "marco"
 if ($name !== 'marco') {
     // Name is not "marco," set an error code and exit
     //http_response_code(400); // Set a 400 Bad Request status code
     exit(5);
 }


// Generate a custom HTML response
$htmlResponse = "<html>
<head>
    <title>Form Submission Result</title>
</head>
<body style=\"color: $color;\">
    <h1>Form Submission Result</h1>
    <p>Name: $name</p>
    <p>Color: $color</p>
</body>
</html>";

// Send the HTTP response headers
//header("Content-Type: text/html");
//header("Content-Length: " . strlen($htmlResponse));

// Output the HTML response to stdout
echo $htmlResponse;
