<?php
// Access the custom environmental variables set in C++ using $_SERVER


// thsi works as well the same!!
//$customVar1 = $_SERVER['CUSTOM_VAR1'];
//$customVar2 = $_SERVER['CUSTOM_VAR2'];

$customVar1 = getenv('CUSTOM_VAR1');
$customVar2 = getenv('CUSTOM_VAR2');

echo "CUSTOM_VAR1: $customVar1\n";
echo "CUSTOM_VAR2: $customVar2\n";

echo "hello World!\n"
?>
