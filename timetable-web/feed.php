<?php
	if ($_SERVER["REQUEST_METHOD"] == "GET")
    {
		echo("Wrong request.\n");
    }
    else if ($_SERVER["REQUEST_METHOD"] == "POST")
    {
		header('Content-type: application/json');
		require(__DIR__ . "/includes/config.php");
		$con = mysqli_connect($url_db,$db_user,$db_pwd,$db_name);
		if (!$con)
			echo("Error");
		else{
			$json = file_get_contents('php://input');
			$data = json_decode($json,true);
			$col = "(";
			$valueDB = "(";
			while ($value = current($data)) {
				$col .= key($data).",";
				$valueDB .= "'$value',";
			    next($data);
			}
			$col = rtrim($col,",");
			$valueDB = rtrim($valueDB,",");
			$col .= ")";
			$valueDB .= ")";	
			$sql = "INSERT INTO $table $col VALUES $valueDB";
			if (!mysqli_query($con,$sql))
				echo("Error");
			else
				echo("OK");
		}
		mysqli_close($con);
	}
?>