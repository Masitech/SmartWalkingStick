<?php
//get the post variables
$value = $_POST['ledpwmval'];
$unit = $_POST['unit'];
$column = $_POST['column'];

//connect to the database
$servername = "localhost";
$username = "*****";
$password = "****";
$dbname = "*****";

// Create connection
$conn = mysqli_connect($servername, $username, $password, $dbname);
// Check connection
if (!$conn) {
    die("Connection failed: " . mysqli_connect_error());
}

//update the value
$sql = "UPDATE ArduinoJS SET LedPWM = '{$value}' WHERE ID = 1";
$result = $conn->query($sql);
echo "changed";

//go back to the interface
header("location: main.php");
 ?>
