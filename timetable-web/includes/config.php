<?php
	//setlocale(LC_TIME, 'it_IT.utf8');
	require(__DIR__ . "/variables.php");
	function calcola_orario($orario,$modifica){
		return date("H:i:s", strtotime($modifica, strtotime($orario)));
	}

	function check_uscite($uscita_delta){
		switch($uscita_delta){
			case ($uscita_delta===0): echo ""; return 0; break;
			case ($uscita_delta>=0 && $uscita_delta<1800): echo ""; return 0; break;
			case ($uscita_delta>=1800): echo "bold"; return 0; break;
			case ($uscita_delta<0): echo "red"; return 0; break;
		}
	}
	function check_entrata($entrata_delta){
		switch($entrata_delta){
			case ($entrata_delta===0): echo ""; return 0; break;
			case ($entrata_delta<=60 && $entrata_delta>-1800): echo ""; return 0; break;
			case ($entrata_delta>60 && $entrata_delta<=300): echo "red"; return 0; break;
			case ($entrata_delta>300):
				echo "red underline";
				$x = $entrata_delta % 3600;
				$giri = 0;
				if($x<=1800)
					$giri += 1;
				else
					$giri += 2;
				$giri += (($entrata_delta - $x)/3600)*2;

				return -($giri * 30);
				//dovrei ritornare il resto del delta di entrata.
				//devo controllare il resto: x=delta%3600
				//1) x<=1800 -> prima metà -> valore metà = +1
				//2) x>1800 && x<=3600 -> seconda metà -> valore metà = +2
				//per il ritardo le 1/2h da aggiungere sono uguali a:
				//{[orario in secondi timbratura - (orario in secondi timbratura % 3600)]/3600}*2 + valore metà
				break;
		}
	}
?>