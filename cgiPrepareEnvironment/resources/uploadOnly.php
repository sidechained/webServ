<?php
$sourcePath = "/nfs/homes/gbooth/42cursus/5_3_webserv_repo/cgiPrepareEnvironment/resources/tmp1.txt";
$destinationPath = "/nfs/homes/gbooth/42cursus/5_3_webserv_repo/cgiPrepareEnvironment/resources/tmp2.txt";

if (move_uploaded_file($sourcePath, $destinationPath)) {
    echo "File moved successfully.";
} else {
    echo "Error moving the file.";
}
?>