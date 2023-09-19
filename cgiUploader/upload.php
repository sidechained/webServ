<?php
// Check if the form was submitted via POST
if ($_SERVER["REQUEST_METHOD"] === "POST") {
    // Check if the "fileToUpload" field exists in the form data
    if (isset($_FILES["fileToUpload"])) {
        $file = $_FILES["fileToUpload"];

        // Check for file upload errors
        if ($file["error"] === UPLOAD_ERR_OK) {
            // Specify the upload directory (make sure it exists)
            $uploadDir = "uploads/";

            // Create a unique filename for the uploaded file
            $uniqueFilename = uniqid() . "_" . $file["name"];
            $destination = $uploadDir . $uniqueFilename;

            // Move the uploaded file to the destination directory
            if (move_uploaded_file($file["tmp_name"], $destination)) {
                echo "File uploaded successfully.";
            } else {
                echo "Error moving the uploaded file.";
            }
        } else {
            echo "Error uploading file. Error code: " . $file["error"];
        }
    } else {
        echo "File field not found in the form.";
    }
} else {
    echo "Invalid request method.";
}
?>