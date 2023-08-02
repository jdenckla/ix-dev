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

$query = "SELECT * FROM projects ORDER BY projectID ";

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
echo "<tr><td>PID</td><td>Project Name</td><td>Description</td><td>Owner ID</td></tr>";
while ($row = mysqli_fetch_array($result, MYSQLI_ASSOC)) {
  $projectID   = $row['projectID'];
  $projectName = $row['projectName'];
  $description = $row['description'];
  $ownerID = $row['ownerID'];
  echo "<tr><td>".$projectID."</td><td>".$projectName."</td><td>".$description."</td><td>".$ownerID."</td></tr>";
}
echo "</table>";
echo "<br>";





mysqli_free_result($result);

mysqli_close($conn);

?>

<p>

<hr>
<a href="projects.html">Return to Project Lookup</a>


 
</body>
</html>
	  