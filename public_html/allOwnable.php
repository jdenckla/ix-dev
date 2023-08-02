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

$query = "SELECT * FROM games ORDER BY title ";

$query2 = "SELECT * FROM platforms ORDER BY platformName ";

$query3 = "SELECT * FROM services ORDER BY svcName ";

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

echo "<b>Games</b>";
echo "<table>";
echo "<tr><td>Title</td><td>Description</td><td>Genre</td><td>Player Limit</td><td>Platform</td></tr>";
while ($row = mysqli_fetch_array($result, MYSQLI_ASSOC)) {
  $title   = $row['title'];
  $description = $row['description'];
  $genre = $row['genre'];
  $playerLimit = $row['playerLimit'];
  $platform = $row['platform'];
  echo "<tr><td>".$title."</td><td>".$description."</td><td>".$genre."</td><td>".$playerLimit."</td><td>".$platform."</td></tr>";
}
echo "</table>";
echo "<br>";
echo "<br>";


echo "<b>Platforms</b>";
echo "<table>";
echo "<tr><td>Platform Name</td></tr>";
while ($nrow = mysqli_fetch_array($result2, MYSQLI_ASSOC)) {
  $platformName   = $nrow['platformName'];
  echo "<tr><td>".$platformName."</td></tr>";
}
echo "</table>";
echo "<br>";
echo "<br>";

echo "<b>Services</b>";
echo "<table>";
echo "<tr><td>Service Name</td><td>Cost (mo.)</td></tr>";
while ($lrow = mysqli_fetch_array($result3, MYSQLI_ASSOC)) {
  $svcName   = $lrow['svcName'];
  $cost = $lrow['cost'];
  echo "<tr><td>".$svcName."</td><td>".$cost."</td></tr>";
}
echo "</table>";



mysqli_free_result($result);
mysqli_free_result($result2);
mysqli_free_result($result3);

mysqli_close($conn);

?>

<p>

<hr>
<a href="ownership.html">Return to Ownership Lookup</a>


 
</body>
</html>
	  