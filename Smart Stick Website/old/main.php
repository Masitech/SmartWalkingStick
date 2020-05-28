<?php
//all of this code used for refreshing the page
$page = $_SERVER['PHP_SELF'];
$sec  = "12";
?>
<!DOCTYPE html>
<html>
<head>
<meta http-equiv="refresh" content="<?php
echo $sec;
?>;URL='<?php
echo $page;
?>'">
<title>NodeJs - Ardunio Communication Project</title>
      <style>

@charset "UTF-8";
@import url(https://fonts.googleapis.com/css?family=Open+Sans:300,400,700);
@import url(https://fonts.googleapis.com/css?family=Ubuntu:300,400,700);

body {
  font-family: 'Ubuntu', Open Sans, sans-serif;
  font-weight: 300;
  line-height: 1.42em;
  color:#A7A1AE;
  background-color:#1F2739;
}

h1 {
  font-size:3em; 
  font-weight: 300;
  line-height:1em;
  text-align: center;
  color: #4DC3FA;
}

h2 {
  font-size:1em; 
  font-weight: 300;
  text-align: center;
  display: block;
  line-height:1em;
  padding-bottom: 2em;
  color: #FB667A;
}

h2 a {
  font-weight: 700;
  text-transform: uppercase;
  color: #FB667A;
  text-decoration: none;
}

.blue { color: #185875; }
.yellow { color: #89f442; }

.container th h1 {
    font-weight: bold;
	font-size: 2em;
    text-align: left;
  color: #185875;
}

.container td {
	  font-weight: normal;
	  font-size: 1.8em;
  -webkit-box-shadow: 0 2px 2px -2px #0E1119;
	   -moz-box-shadow: 0 2px 2px -2px #0E1119;
	        box-shadow: 0 2px 2px -2px #0E1119;
}

.container {
	  text-align: left;
	  overflow: hidden;
	  width: 80%;
	  margin: 0 auto;
  display: table;
  padding: 0 0 8em 0;
}

.container td, .container th {
	  padding-bottom: 2%;
	  padding-top: 2%;
      padding-left:2%;  
}

/* Background-color of the odd rows */
.container tr:nth-child(odd) {
	  background-color: #323C50;
}

/* Background-color of the even rows */
.container tr:nth-child(even) {
	  background-color: #2C3446;
}

.container th {
	  background-color: #1F2739;
}

.container td:first-child { color: #FB667A; }

.container tr:hover {
   background-color: #464A52;
-webkit-box-shadow: 0 6px 6px -6px #0E1119;
	   -moz-box-shadow: 0 6px 6px -6px #0E1119;
	        box-shadow: 0 6px 6px -6px #0E1119;
}

.container td:hover {
  background-color: #1F2739;
  color: #403E10;
  font-weight: bold;
  
  box-shadow: #7F7C21 -1px 1px, #7F7C21 -2px 2px, #7F7C21 -3px 3px, #7F7C21 -4px 4px, #7F7C21 -5px 5px, #7F7C21 -6px 6px;
  transform: translate3d(6px, -6px, 0);
  
  transition-delay: 0s;
	  transition-duration: 0.4s;
	  transition-property: all;
  transition-timing-function: line;
    }
    form {
  background: none;
  margin: 0px 5% 0;
  padding: 0px;
  width: 360px;
}

input {
  display: inline-block;
  font-size: 1em;
  width: 310px;
  min-width: 100px;
  margin: 10px 10px;
  padding: 10px 8px 10px 8px;
  border-radius: 5px;
  box-shadow: inset 0 1px 2px rgba(0,0,0, .55), 0px 1px 1px rgba(255,255,255,.5);
  border: 1px solid #666;
}

input {
  opacity: 0.5;
}

input:hover,
input:focus {
  opacity: .7;
  color:#08c;
  border: 1px solid #08c;
   box-shadow: 0px 1px 0px rgba(255,255,255,.25),inset 0px 3px 6px rgba(0,0,0,.25);
}

input[type="text"]:focus,
input[type="password"]:focus {
  box-shadow: inset 0 1px 2px rgba(255,255,255, .35), 0px 1px 15px rgba(0,246,255,.5);
  border: 1px solid #08c;
  outline: none;
}

input[type="submit"] {
  font-size: 1em;
  appearance: none;
  opacity: .90;
  width: auto;
  background: #08c;
  box-shadow: inset 0 1px 2px rgba(255,255,255, .35), 0px 1px 6px rgba(0,246,255,.5);
  border: 1px solid #0a5378;
  border-radius: 4px;
  color: #eee;
  cursor: pointer;
  text-shadow:0px -1px 0px rgba(0,0,0,.5);
}

input[type="submit"]:hover {
  background: #08c;
  width:Auto;
  border: 1px solid #0a5378;
  border-radius: 3px;
  box-shadow: inset 0px 3px 16px rgba(0,0,0,.25),0px 1px 10px rgba(255,255,255,.5),inset 0px -1px 2px rgba(255,255,255,.35);
  text-shadow:0px 1px 1px rgba(0,0,0,.65);
  -webkit-transition: all 0.40s ease-out;
  transition: all 0.40s ease-out;
}

}
    </style>
    <script>
  window.console = window.console || function(t) {};
</script>
  <script>
  if (document.location.search.match(/type=embed/gi)) {
    window.parent.postMessage("resize", "*");
  }
</script>

</head>
<body translate="no">
<?php
$servername = "localhost";
$username   = "aruuhntg_ma";
$password   = "Masi244144";
$dbname     = "aruuhntg_MA1";

// Create connection
$conn = mysqli_connect($servername, $username, $password, $dbname);
// Check connection
if (!$conn) {
    die("Connection failed: " . mysqli_connect_error());
}

$sql    = "SELECT * FROM ArduinoJS";
$result = $conn->query($sql);

//print some nice text at the top
echo "<h1><span class='blue'>&lt;</span>NodeJs - Ardunio  <span class='blue'>&gt;</span> <span class='yellow'>Communication Project</pan></h1>";
echo "<h2>Made By <a rel='nofollow' rel='noreferrer' href='http://www.mahmed.tech' target='_blank'>Masum Ahmed</a></h2>";

//draw the table
echo "<table class = 'container'>
<thread>
<tr>
<th><h1>ID</h1></th>
<th><h1><h1>LED PWM</h1></th>
<th><h1>POT VALUE</h1></th>
</tr></thread>";
//loop through the table and print the data into the table
while ($row = mysqli_fetch_array($result)) {
    
    echo "<tbody><tr>";
    $unit_id = $row['ID'];
    echo "<td>" . $row['ID'] . "</td>";
    $column      = "LedPWM";
    $current_LED = $row['LedPWM'];
    echo "<td><form action= change_SQL.php method= 'post'>
<input type='text' name='ledpwmval' value=$current_LED >
<input type='hidden' name='unit' value= $unit_id >
<input type='hidden' name='column' value= $column >
<input type= 'submit' name= 'change_but' style='text-align:center' value='Change LED Brightness'></form></td>";
    echo "<td>" . $row['Value'] . "</td>";
    
} //while

echo "</tr><tbody></table>";

?>