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


$query = "SELECT title, description, genre, service, numPrefered ";
$query = $query."FROM shows JOIN ";
$query = $query."(SELECT title, COUNT(title) as numPrefered  ";
$query = $query."FROM desiredEntertainment e  JOIN participants p on p.id = e.participantID  ";
$query = $query."JOIN ( SELECT e.eID, title ";
$query = $query."FROM entertainment e  ";
$query = $query."JOIN shows s using(eID))  as result using (eID)  ";
$query = $query."WHERE p.id IN (".$state.")  ";
$query = $query."GROUP BY (title) ) num using(title) ";
$query = $query."ORDER BY numPrefered desc ";

$query2 = "SELECT title, description, genre, service, numPrefered ";
$query2 = $query2."FROM movies JOIN ";
$query2 = $query2."(SELECT title, COUNT(title) as numPrefered  ";
$query2 = $query2."FROM desiredEntertainment e  JOIN participants p on p.id = e.participantID  ";
$query2 = $query2."JOIN ( SELECT e.eID, title ";
$query2 = $query2."FROM entertainment e  ";
$query2 = $query2."JOIN movies m using(eID))  as result using (eID)  ";
$query2 = $query2."WHERE p.id IN (".$state.")  ";
$query2 = $query2."GROUP BY (title) ) num using(title) ";
$query2 = $query2."ORDER BY numPrefered desc ";

$query3 = "SELECT title, description, genre, playerLimit, platform, numPrefered ";
$query3 = $query3."FROM games JOIN ";
$query3 = $query3."(SELECT title, COUNT(title) as numPrefered  ";
$query3 = $query3."FROM desiredEntertainment e  JOIN participants p on p.id = e.participantID  ";
$query3 = $query3."JOIN ( SELECT e.eID, title ";
$query3 = $query3."FROM entertainment e  ";
$query3 = $query3."JOIN games g using(eID))  as result using (eID)  ";
$query3 = $query3."WHERE p.id IN (".$state.")  ";
$query3 = $query3."GROUP BY (title) ) num using(title) ";
$query3 = $query3."ORDER BY numPrefered desc ";

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

echo "<b>Shows</b>";
echo "<table>";
echo "<tr><td>Title</td><td>Description</td><td>Genre</td><td>Service</td><td>Number Who Prefer</td></tr>";
while ($row = mysqli_fetch_array($result, MYSQLI_ASSOC)) {
  $title   = $row['title'];
  $description = $row['description'];
  $genre = $row['genre'];
  $service = $row['service'];
  $numPrefered = $row['numPrefered'];
  echo "<tr><td>".$title."</td><td>".$description."</td><td>".$genre."</td><td>".$service."</td><td>".$numPrefered."</td></tr>";
}
echo "</table>";
echo "<br>";
echo "<br>";
echo "<b>Movies</b>";
echo "<table>";
echo "<tr><td>Title</td><td>Description</td><td>Genre</td><td>Service</td><td>Number Who Prefer</td></tr>";
while ($row2 = mysqli_fetch_array($result2, MYSQLI_ASSOC)) {
  $title   = $row2['title'];
  $description = $row2['description'];
  $genre = $row2['genre'];
  $service = $row2['service'];
  $numPrefered = $row2['numPrefered'];
  echo "<tr><td>".$title."</td><td>".$description."</td><td>".$genre."</td><td>".$service."</td><td>".$numPrefered."</td></tr>";
}
echo "</table>";
echo "<br>";
echo "<br>";
echo "<b>Games</b>";
echo "<table>";
echo "<tr><td>Title</td><td>Description</td><td>Genre</td><td>Player Limit</td><td>Platform</td><td>Number Who Prefer</td></tr>";
while ($row3 = mysqli_fetch_array($result3, MYSQLI_ASSOC)) {
  $title   = $row3['title'];
  $description = $row3['description'];
  $genre = $row3['genre'];
  $playerLimit = $row3['playerLimit'];
  $platform = $row3['platform'];
  $numPrefered = $row3['numPrefered'];
  echo "<tr><td>".$title."</td><td>".$description."</td><td>".$genre."</td><td>".$playerLimit."</td><td>".$platform."</td><td>".$numPrefered."</td></tr>";
}
echo "</table>";


mysqli_free_result($result);
mysqli_free_result($result2);
mysqli_free_result($result3);

mysqli_close($conn);

?>

<p>
<hr>

<p>
<br>

<hr>
<a href="preference.html">Return Home</a>


 
</body>
</html>
	  