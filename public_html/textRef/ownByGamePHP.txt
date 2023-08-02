<?php

include('connectionData.txt');

$conn = mysqli_connect($server, $user, $pass, $dbname, $port)
or die('Error connecting to MySQL server.');

?>

<html>
<head>
<style>
table, th, td {
border-collapse: collapse;
border-style:solid;
border-width:1px;
border-color:black;
padding: 5px;
}
</style>
  <title>jdenckla 415 Final Project</title>
  </head>
  
  <body bgcolor="white">
  
  
  <hr>
  
  
<?php
  
$state = $_POST['state'];

$state = mysqli_real_escape_string($conn, $state);
// this is a small attempt to avoid SQL injection
// better to use prepared statements

$query = "SELECT title, fname, lname, ownerID ";
$query = $query."FROM participants p ";
$query = $query."LEFT JOIN gameOwnership go on go.ownerID = p.ID ";
$query = $query."JOIN games on gameEID = eID ";
$query = $query."WHERE title = '".$state."' ";

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


echo "<table>";
echo "<tr><td>Game Title</td><td>Owner Name</td><td>Owner ID</td></tr>";
while ($row = mysqli_fetch_array($result, MYSQLI_ASSOC)) {
  $title   = $row['title'];
  $fname = $row['fname'];
  $lname = $row['lname'];
  $ownerID = $row['ownerID'];
  echo "<tr><td>".$title."</td><td>".$fname.' '.$lname."</td><td>".$ownerID."</td></tr>";
}
echo "</table>";

mysqli_free_result($result);

mysqli_close($conn);

?>


<br>

<hr>
<a href="ownership.html">Return to Ownership Lookup</a>


 
</body>
</html>
	  