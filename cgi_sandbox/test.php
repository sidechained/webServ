<?php
// Access the custom environmental variables set in C++ using $_SERVER

// Get the value of FILENAME
$fileName = getenv('FILENAME');

// Check if FILENAME is set and the file exists
if ($fileName && file_exists($fileName)) {
    // Generate the new file name with "_copy"
    $copyFileName = pathinfo($fileName, PATHINFO_FILENAME) . "_copy." . pathinfo($fileName, PATHINFO_EXTENSION);

    // Make a copy of the file
    if (copy($fileName, $copyFileName)) {
        echo "Copied file to: $copyFileName\n";
    } else {
        echo "Failed to copy file.\n";
    }
} else {
    echo "FILENAME is not set or the file does not exist.\n";
}

//$customVar2 = getenv('CUSTOM_VAR2');

echo "FILENAME: $fileName\n";
//echo "CUSTOM_VAR2: $customVar2\n";

echo "Hello, World!\n";
?>

