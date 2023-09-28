<?php
//phpinfo();
if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    if ($_FILES['fileInput']['error'] === UPLOAD_ERR_OK) {
        $uploadDir = 'uploads/';
        $uploadFile = $uploadDir . basename($_FILES['fileInput']['name']);

        if (move_uploaded_file($_FILES['fileInput']['tmp_name'], $uploadFile)) {
            echo 'File was successfully uploaded.';
        } else {
            echo 'File upload failed.';
        }
    } else {
        echo 'File upload error: ' . $_FILES['fileInput']['error'];
    }
}
?>
