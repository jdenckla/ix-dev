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

//basic info

$query = "SELECT id, fname, lname, timezone ";
$query = $query."FROM participants ";
$query = $query."WHERE CONCAT(fname, ' ', lname) IN ('".$state."') ";

//project ownership

$query2 = "SELECT projectID, projectName, description ";
$query2 = $query2."FROM participants pa ";
$query2 = $query2."LEFT JOIN projects pr on pa.ID = ownerID ";
$query2 = $query2."WHERE CONCAT(pa.fname, ' ', pa.lname) IN ('".$state."') ";
$query2 = $query2."ORDER BY projectID ";

//project participation but not owning

$query3 = "SELECT projectID, projectName, description, ownerID ";
$query3 = $query3."FROM participants pa ";
$query3 = $query3."LEFT JOIN projectParticipation pp on pa.ID = pp.participantID ";
$query3 = $query3."LEFT JOIN projects pr using(projectID) ";
$query3 = $query3."WHERE CONCAT(pa.fname, ' ', pa.lname) IN ('".$state."') and pa.id != ownerID ";
$query3 = $query3."ORDER BY projectID ";

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
$result2 = mysqli_query($conn, $query2)
or die(mysqli_error($conn));
$result3 = mysqli_query($conn, $query3)
or die(mysqli_error($conn));

echo "<b>Basic Information</b>";
echo "<table>";
echo "<tr><td>ID</td><td>Name</td><td>Timezone</td></tr>";
while ($row = mysqli_fetch_array($result, MYSQLI_ASSOC)) {
  $id   = $row['id'];
  $fname = $row['fname'];
  $lname = $row['lname'];
  $timezone = $row['timezone'];
  echo "<tr><td>".$id."</td><td>".$fname.' '.$lname."</td><td>".$timezone."</td></tr>";
}
echo "</table>";
echo "<br>";
echo "<br>";


echo "<b>Owned Projects</b>";
echo "<table>";
echo "<tr><td>Project ID</td><td>Project Name</td><td>Description</td></tr>";
while ($nrow = mysqli_fetch_array($result2, MYSQLI_ASSOC)) {
  $projectID   = $nrow['projectID'];
  $projectName = $nrow['projectName'];
  $description = $nrow['description'];
  echo "<tr><td>".$projectID."</td><td>".$projectName."</td><td>".$description."</td></tr>";
}
echo "</table>";
echo "<br>";
echo "<br>";

echo "<b>Other Associated Projects</b>";
echo "<table>";
echo "<tr><td>Project ID</td><td>Project Name</td><td>Description</td><td>Owner ID</td></tr>";
while ($lrow = mysqli_fetch_array($result3, MYSQLI_ASSOC)) {
  $projectID   = $lrow['projectID'];
  $projectName = $lrow['projectName'];
  $description = $lrow['description'];
  $ownerID = $lrow['ownerID'];
  echo "<tr><td>".$projectID."</td><td>".$projectName."</td><td>".$description."</td><td>".$ownerID."</td></tr>";
}
echo "</table>";



mysqli_free_result($result);
mysqli_free_result($result2);
mysqli_free_result($result3);

mysqli_close($conn);

?>


<br>

<hr>
<a href="participants.html">Return to Participant Lookup</a>


 
</body>
</html>
	  