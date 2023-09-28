<?php
// Access the custom environmental variables set in C++ using $_SERVER
$fileName = $_SERVER['FILENAME'];
//$fileName = $_FILES['FILENAME'];

// Get the value of FILENAME
//$fileName = getenv('FILENAME');

// Check if FILENAME is set and the file exists
if ($fileName && file_exists($fileName)) {
    // Generate the new file name with "_copy"
    $copyFileName = pathinfo($fileName, PATHINFO_FILENAME) . "_copy." . pathinfo($fileName, PATHINFO_EXTENSION);

    // Read data from the original file
    $originalData = file_get_contents($fileName);

    // Read data from stdin
    $inputData = file_get_contents('php://stdin');

    // Combine the original content and input data
    $combinedData = $originalData . $inputData;

    // Write the combined content to the copy file
    file_put_contents($copyFileName, $combinedData);

    echo "Copied file to: $copyFileName\n";
} else {
    echo "FILENAME is not set or the file does not exist.\n";
}

//$customVar2 = getenv('CUSTOM_VAR2');

echo "FILENAME: $fileName\n";
//echo "CUSTOM_VAR2: $customVar2\n";

echo "Hello, World!\n";
?>
