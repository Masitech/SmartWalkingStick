<?php
$servername = "localhost";
$username = "aruuhntg";
$password = "74z9oiTI7p";

// Create connection
$conn = new mysqli($servername, $username, $password);

// Check connection
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
} 
echo "Connected successfully";
?>