<?php
/**
 * Funzioni di utilita' generale per le date
 * @author Ubik <emiliano.leporati@gmail.com>
 * @package date
 */

/**
 * Trasforma l'anno in uno di struttura equivalente sui giorni
 * @param integer $anno L'anno da traslare
 * @return integer
 */
function trasla_anno($anno)
{
	if ($anno % 100 == 0 && $anno % 400 != 0) {
		$anno -= 6;
	}
	$secoli_attraversati = 0;

	while($anno <= 1970) {
		for($i = 0; $i < 28; $i ++) {
			if ($anno % 100 == 0 && $anno % 400 != 0) {
				$secoli_attraversati ++;
			}
			$anno ++;
		}
	}
	while($anno >= 2038) {
		for($i = 0; $i < 28; $i ++) {
			if ($anno % 100 == 0 && $anno % 400 != 0) {
				$secoli_attraversati --;
			}
			$anno --;
		}
	}

	$anno += 12 * $secoli_attraversati;

	if ($anno <= 1970 || $anno >= 2038)
		return trasla_anno($anno);
	else
		return $anno;
}

/**
 * Aggiunge il numero di giorni alla data passata
 * @param string $data Data in formato gg/mm/yyyy
 * @param integer $numero Numero di giorni da sommare / sottrarre
 * @return string
 */
function somma_giorni($data, $numero)
{
	$data = explode("/", $data);
	$anno = trasla_anno($data[2]);
	$ts   = mktime(0,0,0, $data[1], $data[0] + $numero, $anno);
	return date("d/m", $ts)."/".($data[2] + (date('Y', $ts) - $anno));
}

/**
 * Ritorna il numero di giorni fra le due date, con segno. La prima e' quella di riferimento, quindi delta_giorni('01/01/2005', '02/01/2005') ritorna +1, delta_giorni('02/01/2005', '01/01/2005') ritorna -1
 * @param string $data1 Data in formato gg/mm/yyyy
 * @param string $data2 Data in formato gg/mm/yyyy
 * @return integer
 */
function delta_giorni($data1, $data2)
{
	// data1 e' il riferimento
	$data1 = explode("/", $data1);
	$data2 = explode("/", $data2);
	$i_data1 = implode("", array_reverse($data1));
	$i_data2 = implode("", array_reverse($data2));

	
	if ($i_data1 > $i_data2) {
		$segno = -1;
		$tmp = $data1;
		$data1 = $data2;
		$data2 = $tmp;
		$i_data1 = implode("", array_reverse($data1));
		$i_data2 = implode("", array_reverse($data2));
	} else {
		$segno = 1;
	}

	if ($i_data1 == $i_data2) return 0;

	for($delta = 1; $i_data1 != $i_data2; $delta ++) {
		$i_data1 = date("Ymd", mktime(0,0,0, $data1[1], $data1[0] + $delta, $data1[2]));
	}
	return $segno * ($delta - 1);
}

/**
 * Ritorna il numero del giorno della settimana (1 - 7)
 * @param string $data Data in formato gg/mm/yyyy
 * @return integer
 */
function giorno_settimana($data)
{
	$data = explode("/", $data);
	$anno = trasla_anno($data[2]);

	$wday = date("w", mktime(0,0,0, $data[1], $data[0], $anno));
	
	if ($wday == 0) $wday = 7;		

	return $wday;
}

/**
 * Ritorna un numero < 0 se data1 < data2; 0 se sono uguali; > 0 se data2 > data1
 * @param string $data1 Data in formato gg/mm/yyyy
 * @param string $data2 Data in formato gg/mm/yyyy
 * @return integer
 */
function datecmp($data1, $data2)
{
	$i_data1 = implode("", array_reverse(explode("/", $data1)));
	$i_data2 = implode("", array_reverse(explode("/", $data2)));

	return strcmp($i_data1, $i_data2);
}

/**
 * Trasforma una data da formato gg/mm/yyyy nel formato yyyymmgg
 * @param string $data Data in formato gg/mm/yyyy
 * @return string
 */
function dateISO($data)
{
	return implode("", array_reverse(explode("/", $data)));
}

/**
 * Ritorna il numero di minuti che avanzano dal conteggio di ore intere da una quantita' di minuti complessiva. 
 * @param int $minuti numero di minuti
 * @return integer numero di minuti nell'ora non finita 
 */
function calcola_minuti($minuti){
	return ($minuti % 60);
}

/**
 * Ritorna il numero di ore intere presenti in una quantita' di minuti
 * @param int $minuti numero di minuti
 * @return integer numero di ore 
 */
function calcola_ore($minuti){
	return floor($minuti / 60);	
}


?>