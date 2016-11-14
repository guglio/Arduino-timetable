<!DOCTYPE html>
<html lang="">
<head>
  <meta charset="utf-8">
	<title>Attendees registration - with a touch</title>
	<meta name="description" content="" />
  	<meta name="keywords" content="" />
	<meta name="robots" content="" />
	<link rel="stylesheet" href="http://code.jquery.com/mobile/1.4.5/jquery.mobile-1.4.5.min.css" />
	<link rel="stylesheet" href="stylesheets/screen.css" />
	<script src="http://code.jquery.com/jquery-1.11.1.min.js"></script>
	<script src="http://code.jquery.com/mobile/1.4.5/jquery.mobile-1.4.5.min.js"></script>
</head>
<body>
<?php
	require(__DIR__ . "/includes/config.php");
	$table = "timbrature";
	$con = mysqli_connect($url_db,$db_user,$db_pwd,$db_name);
	if (!$con){
		echo ("Error connecting to database.\n");
	}
	else{
		$sql_all = "SELECT * FROM  $table ORDER BY ID DESC";
		$result = mysqli_query($con,$sql_all);
		?>
		<div data-role="header" data-position="fixed">
		    <h2>Attendees registration - with a touch</h2>
		</div>
<!-- 		<div class="table-container"> -->
			<table data-role="table" id="table-column-toggle" data-mode="reflow" class="ui-responsive table-stroke view_attendees" data-filter="true" data-filter-placeholder="Search...">
     <thead>
       <tr>
         <th>Date</th>
         <th>Time</th>
		 <th>Location</th>
         <th>Name</th>
         <th>Surname</th>
       </tr>
     </thead>
     <tbody>
	<?php
		while($row = mysqli_fetch_array($result)){
			$day = strftime('%m/%d/%Y %A',strtotime($row['Data']));
			echo "<tr>";
			if($today==NULL){
				$today = strftime('%d-%m-%Y %A',strtotime($row['Data']));
				echo "<th>".$day."</th>\n";
			}
			else if($day != $today){
				echo "<th>".$day."</th>\n";
				$today = $day;
			}
			else
				echo "\t<th>".$day."</th>\n";
				//echo "\t<th></th>\n";
			$time = date('h:i a',strtotime($row['Data']));
			echo "\t<td>".$time."</td>\n";
			echo "\t<td>".$row['location']."</td>\n";
			$sql_uid_search = "SELECT UID,Nome,Cognome,Orario FROM Utenti WHERE UID = '".$row['UID']."'";
			$utente = mysqli_fetch_array(mysqli_query($con,$sql_uid_search));

			echo "\t<td>".$utente['Nome']."</td>\n";
			echo "\t<td>".$utente['Cognome'][0].".</td>\n";
			echo "</tr>";		
		}
		echo "</tbody>\n</table>\n";
		mysqli_close($con);
	}
?>
<script>
  (function(i,s,o,g,r,a,m){i['GoogleAnalyticsObject']=r;i[r]=i[r]||function(){
  (i[r].q=i[r].q||[]).push(arguments)},i[r].l=1*new Date();a=s.createElement(o),
  m=s.getElementsByTagName(o)[0];a.async=1;a.src=g;m.parentNode.insertBefore(a,m)
  })(window,document,'script','//www.google-analytics.com/analytics.js','ga');

  ga('create', 'UA-71898492-1', 'auto');
  ga('send', 'pageview');
</script>
</body>
</html>