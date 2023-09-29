<?php

// // Read the HTTP request from stdin
// $request = '';
// while ($line = fgets(STDIN)) {
//     $request .= $line;
// }

// // Split the request into headers and body
// list($headers, $body) = explode("\r\n\r\n", $request, 2);

// Extract the POST data from the body
// if ($_SERVER['REQUEST_METHOD'] === 'POST') {
//     parse_str($body, $postParams);
// } else {
//     // No POST data found
//     exit("No POST data found in the request.");
// }

// Extract the "name" and "color" variables
$name = isset($postParams['name']) ? $postParams['name'] : '';
$color = isset($postParams['color']) ? $postParams['color'] : '';

// Check if the name is "marco"
// if ($name !== 'marco') {
//     // Name is not "marco," set an error code and exit
//     //http_response_code(400); // Set a 400 Bad Request status code
//     exit(5);
// }

// Generate a custom HTML response
$htmlResponse = "<html>
<head>
    <title>Form Submission Result</title>
</head>
<body>
    <h1>Form Submission Result</h1>
    <p>Name: $name</p>
    <p>Color: $color</p>
</body>
</html>";

// Send the HTTP response headers
header("Content-Type: text/html");
header("Content-Length: " . strlen($htmlResponse));

// Output the HTML response to stdout
echo $htmlResponse;
