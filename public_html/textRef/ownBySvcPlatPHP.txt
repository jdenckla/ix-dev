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

$query = "SELECT svcName, fname, lname, ownerID ";
$query = $query."FROM participants p ";
$query = $query."LEFT JOIN svcOwnership so on so.ownerID = p.ID ";
$query = $query."WHERE svcName = '".$state."' ";

$query1 = "SELECT pName, fname, lname, ownerID ";
$query1 = $query1."FROM participants p ";
$query1 = $query1."LEFT JOIN platformOwnership po on po.ownerID = p.ID ";
$query1 = $query1."WHERE pName = '".$state."' ";

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
$result1 = mysqli_query($conn, $query1)
or die(mysqli_error($conn));


echo "<table>";
echo "<tr><td>Service Name</td><td>Owner Name</td><td>Owner ID</td></tr>";
while ($row = mysqli_fetch_array($result, MYSQLI_ASSOC)) {
  $svcName   = $row['svcName'];
  $fname = $row['fname'];
  $lname = $row['lname'];
  $ownerID = $row['ownerID'];
  echo "<tr><td>".$svcName."</td><td>".$fname.' '.$lname."</td><td>".$ownerID."</td></tr>";
}
echo "</table>";
echo "<br>";
echo "<br>";
echo "<table>";
echo "<tr><td>Platform Name</td><td>Owner Name</td><td>Owner ID</td></tr>";
while ($row1 = mysqli_fetch_array($result1, MYSQLI_ASSOC)) {
  $pName   = $row1['pName'];
  $fname = $row1['fname'];
  $lname = $row1['lname'];
  $ownerID = $row1['ownerID'];
  echo "<tr><td>".$pName."</td><td>".$fname.' '.$lname."</td><td>".$ownerID."</td></tr>";
}
echo "</table>";

mysqli_free_result($result);
mysqli_free_result($result1);

mysqli_close($conn);

?>

<p>

<hr>
<a href="ownership.html">Return to Ownership Lookup</a>


 
</body>
</html>
	  