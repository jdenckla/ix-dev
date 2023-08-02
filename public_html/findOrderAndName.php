<?php

include('connectionData.txt');

$conn = mysqli_connect($server, $user, $pass, $dbname, $port)
or die('Error connecting to MySQL server.');

?>

<html>
<head>
  <title>Another Simple PHP-MySQL Program</title>
  </head>
  
  <body bgcolor="white">
  
  
  <hr>
  
  
<?php
  
$state = $_POST['state'];

$state = mysqli_real_escape_string($conn, $state);
// this is a small attempt to avoid SQL injection
// better to use prepared statements

$query = "SELECT fname, lname, description FROM orders o ";
$query = $query."JOIN customer USING(customer_num) JOIN items i using(order_num) ";
$query = $query."JOIN manufact m using(manu_code) JOIN stock using(manu_code, stock_num) ";
$query = $query."WHERE m.manu_name = '$state' ORDER BY 2;";

?>

<p>
The query:
<p>
<?php
print $query;
?>

<hr>
<p>
Result of query:
<p>

<?php
$result = mysqli_query($conn, $query)
or die(mysqli_error($conn));

print "<pre>";
while($row = mysqli_fetch_array($result, MYSQLI_BOTH))
  {
    print "\n";
    print "$row[fname]  $row[lname]  $row[description]";
  }
print "</pre>";

mysqli_free_result($result);

mysqli_close($conn);

?>

<p>
<hr>

<p>
 
</body>
</html>
	  