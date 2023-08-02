<?php

include('connectionData.txt');

$conn = mysqli_connect($server, $user, $pass, $dbname, $port)
or die('Error connecting to MySQL server.');

?>

<html>
<head>
  <title>jdenckla 415 Final Project</title>
  </head>
  
  <body bgcolor="white">
  
  
  <hr>
  
  
<?php
  
$state = $_POST['state'];

$state = mysqli_real_escape_string($conn, $state);
// this is a small attempt to avoid SQL injection
// better to use prepared statements

$query = "SELECT title, COUNT(title) as numPrefered ";
$query = $query."FROM desiredEntertainment e  JOIN participants p on p.id = e.participantID ";
$query = $query."JOIN ( SELECT e.eID, COALESCE(g.title, s.title, m.title) as title, ";
$query = $query."COALESCE (g.description, s.description, m.description) as description, ";
$query = $query."COALESCE(g.genre, s.genre, m.genre) as genre, g.playerLimit, g.platform, ";
$query = $query."COALESCE(s.service, m.service) as service FROM entertainment e ";
$query = $query."LEFT JOIN games g using(eID) LEFT JOIN shows s on s.eID = e.eID ";
$query = $query."LEFT JOIN movies m on m.eID = e.eID)  as result using (eID) ";
$query = $query."WHERE CONCAT(p.fname, ' ', p.lname) IN (".$state.") ";
$query = $query."GROUP BY (title) ";
$query = $query."ORDER BY numPrefered desc ";

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
    print "$row[title]  $row[numPrefered]";
  }
print "</pre>";

mysqli_free_result($result);

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
	  