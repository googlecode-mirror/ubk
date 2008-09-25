<?php
/**
 * Funzioni di utilita' generale per le date
 * @author Ubik <emiliano.leporati@gmail.com>
 * @package date
 */


/**
 * Classe base per la definizione di timestamp, date e tempi localizzati nel formato.
 * Per gestire date fuori dal range del sistema operativo, timestamp e date vengono rappresentate con un "timestamp esteso": timestamp di php + numero di anni di shift
 * @package date
 */
abstract class TIMEBASE
{
	
	/**
	 * Espressione regolare rappresentante l'oggetto su un database
	 * @var string
	 */
	protected $DB;
	/**
	 * Replace di $DB per trasformare l'oggetto su un database in un oggetto ISO (con gli elementi ordinati)
	 * @var string
	 */
	protected $DB_ISO;
	
	/**
	 * Stringa di formattazione per strftime che rappresenta il formato utente dell'oggetto
	 * @var string
	 */
	protected $USER;
	/**
	 * Espressione regolare in grado di estrarre l'anno da un oggetto in formato utente
	 * @var string
	 */
	protected $USER_SEARCH_Y;

	/**
	 * Stringa di formattazione per strftime che rappresenta il formato ISO (ordinato) dell'oggetto
	 * @var string
	 */
	protected $ISO;
	/**
	 * Espressione regolare in grado di estrarre l'anno da un oggetto in formato ISO
	 * @var string
	 */
	protected $ISO_SEARCH_Y;
	
	/**
	 * Stringa di formattazione per strftime che rappresenta il formato database dell'oggetto
	 * @var string
	 */
	protected $REV;
	/**
	 * Espressione regolare in grado di estrarre l'anno da un oggetto in formato database
	 * @var string
	 */
	protected $REV_SEARCH_Y;

	/**
	 * Funzione in grado di trasformare un "timestamp esteso" in qualunque altro formato
	 * @param mixed $ts Il timestamp (normale per TIME, esteso per gli altri) da trasformare
	 * @param string $fmt La stringa di formattazione strftime di destinazione
	 * @param string $search L'espressione regolare in grado di trovare l'anno nella stringa formattata
	 * @return string
	 */
	abstract protected function ts_2_any($ts, $fmt, $search);
	/**
	 * Funzione in grado di trasformare un oggetto da qualunque formato a "timestamp esteso"
	 * @param string $any La rappresentazione dell'oggetto
	 * @param string $fmt La stringa di formattazione strftime di origine
	 * @param string $search L'espressione regolare in grado di trovare l'anno nella stringa formattata
	 * @return string
	 */
	abstract protected function any_2_ts($any, $fmt, $search);
	/**
	 * Funzione che ritorna un timestamp semplice date le parti della data ricavate da strptime
	 * @param array $parts Le parti della data ricavate da strptime
	 * @return integer
	 */
	abstract protected function mktime($parts);
	
	public function ts_2_db($ts)
	{
		return $this->ts_2_any($ts, $this->REV, $this->REV_SEARCH_Y);
	}
	public function ts_2_iso($ts)
	{
		return $this->ts_2_any($ts, $this->ISO, $this->ISO_SEARCH_Y);
	}
	public function ts_2_user($ts)
	{
		return $this->ts_2_any($ts, $this->USER, $this->USER_SEARCH_Y);
	}
	
	public function db_2_ts($db)
	{
		$iso = $this->db_2_iso($db);
		return $this->iso_2_ts($iso);
	}
	public function db_2_iso($db)
	{
		// metto il dato db in formato iso
// 		log_value("ISO : ".$this->DB." ".$this->DB_ISO." ".$db." ".preg_replace($this->DB, $this->DB_ISO, $db));
		return preg_replace($this->DB, $this->DB_ISO, $db);
	}
	public function db_2_user($db)
	{
		if (empty($db)) return "";
		$ts  = $this->db_2_ts($db);
		return $this->ts_2_user($ts);
	}
	
	public function user_2_db($user)
	{
		if (empty($user)) return "";
		$ts = $this->any_2_ts($user, $this->USER, $this->USER_SEARCH_Y);
		return $this->ts_2_db($ts);
	}
	public function user_2_ts($user)
	{
		return $this->any_2_ts($user, $this->USER, $this->USER_SEARCH_Y);
	}
	public function user_2_iso($user)
	{
		$ts = $this->any_2_ts($user, $this->USER, $this->USER_SEARCH_Y);
		return $this->ts_2_iso($ts);
	}

	public function iso_2_db($iso)
	{
		$ts = $this->any_2_ts($iso, $this->ISO, $this->ISO_SEARCH_Y);
		return $this->ts_2_db($ts);
	}
	public function iso_2_ts($iso)
	{
		return $this->any_2_ts($iso, $this->ISO, $this->ISO_SEARCH_Y);
	}
	public function iso_2_user($iso)
	{
		$ts = $this->any_2_ts($iso, $this->ISO, $this->ISO_SEARCH_Y);
		return $this->ts_2_user($ts);
	}
}

/**
 * Classe che rappresenta una data
 * @package date
 */
class DATE extends TIMEBASE
{
	/**
	 * Costruisce una data per interagire con un certo dbms
	 * @param string $db FBIRD, MYSQL, MSSQL, ODBC, ...
	 */
	public function __construct($db)
	{
		$this->DB     = trim(lang_database($db, 'DT-DB'));
		$this->DB_ISO = trim(lang_database($db, 'DT-DB2ISO'));
		$this->USER          = trim(lang_database($db, 'DT-USER'));
		$this->USER_SEARCH_Y = trim(lang_database($db, 'DT-USER-SEARCH-YEAR'));
		$this->ISO          = trim(lang_database($db, 'DT-ISO'));
		$this->ISO_SEARCH_Y = trim(lang_database($db, 'DT-ISO-SEARCH-YEAR'));
		$this->REV          = trim(lang_database($db, 'DT-REV'));
		$this->REV_SEARCH_Y = trim(lang_database($db, 'DT-REV-SEARCH-YEAR'));
	}

	protected function mktime($parts)
	{
		return mktime(0, 0, 0, 1 + $parts['tm_mon'], $parts['tm_mday'], 1900 + $parts['tm_year']);
	}

	// con queste due, entro ed esco dalla coppia ts / shift
	protected function any_2_ts($any, $fmt, $search_y)
	{
		setlocale(LC_TIME, trim(lang('LC_TIME')));

		$shift = 0;
		// cerco l'anno
		if (preg_match($search_y, $any, $m)) {
	
			$year  = $this->shift_year($m[0], $shift);
			// sostituisco ritraslando
			$valid = preg_replace($search_y, $year, $any);
			// prendo le parti
			$parts = strptime($valid, $fmt);
			if ($parts === FALSE) {
				trigger_error(lang('ERR-TIME-FMT', $valid, $fmt), E_USER_WARNING);
			}
			// faccio l'array
			return array(
				'ts' => $this->mktime($parts)
				,'shift' => $shift
			);
		} else {
			trigger_error(lang('ERR-TIME-FMT', $any, $fmt), E_USER_WARNING);
			return NULL;
		}
		
	}
	
	protected function ts_2_any($ts, $fmt, $search_y)
	{
		setlocale(LC_TIME, trim(lang('LC_TIME')));

		// formatto il valido
		$valid = strftime($fmt, $ts['ts']);
		// cerco l'anno
		preg_match($search_y, $valid, $m);
		// sostituisco ritraslando
// 		log_value("TO ".preg_replace($search_y, $m[0] - $ts['shift'], $valid));
		return preg_replace($search_y, $m[0] - $ts['shift'], $valid);
	}

	/**
	 * Dato un anno, ne ritorna un altro con struttura corrispondente, nel range manipolabile dal sistema
	 * @param integer $year Anno da traslare
	 * @param integer $shift Numero di anni di cui è stato traslato
	 * @return integer
	 */
	public function shift_year($year, &$shift)
	{
		if ($year % 100 == 0 && $year % 400 != 0) {
			$year  -= 6; $shift -= 6;
		}
		$centuries = 0;

		while($year <= 1970) {
			for($i = 0; $i < 28; $i ++) {
				if ($year % 100 == 0 && $year % 400 != 0) {
					$centuries ++;
				}
				$year ++; $shift ++;
			}
		}
		while($year >= 2037) {
			for($i = 0; $i < 28; $i ++) {
				if ($year % 100 == 0 && $year % 400 != 0) {
					$centuries --;
				}
				$year --; $shift --;
			}
		}

		$year  += 12 * $centuries;
		$shift += 12 * $centuries;

		if ($year <= 1970 || $year >= 2037)
			return trasla_anno($year, $shift);
		else
			return $year;
	}	

	/**
	 * Aggiunge una parte a una data ISO, ritornando un'altra data ISO
	 * @param string $iso Data in formato ISO
	 * @param string $part Parte da aggiungere (Y m d H i)
	 * @param integer $amount Quantita' da aggiungere
	 * @param integer $gap Massima quantita' da aggiungere per iterazione
	 * @return string
	 */
	public function iso_add($iso, $part, $amount, $gap = 1)
	{
		$sgn    = ($amount > 0 ? 1 : -1);
		$amount = abs($amount);

		$ts    = $this->iso_2_ts($iso);
		$shift = $ts['shift'];
		$ts    = $ts['ts'];
		$parts = array('Y','m','d','H','i');
		foreach($parts as $p)
			$$p = date($p, $ts);
		
		while($amount > 0) {
	
			$delta = min($gap, $amount);
			
			$$part += $sgn * $delta;
			
			$ts = mktime($H, $i, 0, $m, $d, $Y);

			foreach($parts as $p)
				$$p = date($p, $ts);
	
			$Y = $this->shift_year($Y, $shift);

			$amount -= $delta;
		}
	
		return $this->ts_2_iso(array('ts' => $ts, 'shift' => $shift));
	}
	
	/**
	 * Aggiunge varie parti a una data ISO, ritornando un'altra data ISO
	 * @param string $iso Data in formato ISO
	 * @param array $part Parti da aggiungere (Y m d H i) con relativa quantita': array('Y' => 12, 'm' => 4)
	 * @return string
	 */
	public function iso_add_parts($iso, $parts)
	{
		$gaps = array('d' => 365, 'm' => 12, 'Y' => 1, 'H' => 24 * 365, 'i' => 60 * 24 * 365);
		
		foreach($parts as $part => $amount) {
			$iso = $this->iso_add($iso, $part, $amount, array_get_default($part, $gaps, 1));
		}
		
		return $iso;
	}
}



/**
 * Classe che rappresenta un timestamp (data e ora)
 * @package date
 */
class TIMESTAMP extends DATE
{
	/**
	 * Replace di $DB per trasformare l'oggetto su un database in una data ISO
	 * @var string
	 */
	protected $DB_DATE_ISO;
	/**
	 * Replace di $DB per trasformare l'oggetto su un database in un orario ISO
	 * @var string
	 */
	protected $DB_TIME_ISO;

	/**
	 * Costruisce un timestamp per interagire con un certo dbms
	 * @param string $db FBIRD, MYSQL, MSSQL, ODBC, ...
	 */
	public function __construct($db)
	{
		$this->DB     = trim(lang_database($db, 'TS-DB'));
		$this->DB_ISO = trim(lang_database($db, 'TS-DB2ISO-TS'));
		$this->DB_DATE_ISO = trim(lang_database($db, 'TS-DB2ISO-DATE'));
		$this->DB_TIME_ISO = trim(lang_database($db, 'TS-DB2ISO-TIME'));
		$this->USER          = trim(lang_database($db, 'TS-USER'));
		$this->USER_SEARCH_Y = trim(lang_database($db, 'TS-USER-SEARCH-YEAR'));
		$this->ISO          = trim(lang_database($db, 'TS-ISO'));
		$this->ISO_SEARCH_Y = trim(lang_database($db, 'TS-ISO-SEARCH-YEAR'));
		$this->REV          = trim(lang_database($db, 'TS-REV'));
		$this->REV_SEARCH_Y = trim(lang_database($db, 'TS-REV-SEARCH-YEAR'));
	}

	protected function mktime($parts)
	{
		return mktime($parts['tm_hour'], $parts['tm_min'], 0, 1 + $parts['tm_mon'], $parts['tm_mday'], 1900 + $parts['tm_year']);
	}

	/**
	 * Ritorna la parte di data in formato ISO di un timestamp in formato database
	 * @param string $timestamp Timestamp in formato database
	 * @return string
	 */
	public function db_2_iso_date($timestamp)
	{
		return preg_replace($this->DB, $this->DB_DATE_ISO, $timestamp);
	}
	/**
	 * Ritorna la parte di orario in formato ISO di un timestamp in formato database
	 * @param string $timestamp Timestamp in formato database
	 * @return string
	 */
	public function db_2_iso_time($timestamp)
	{
		return trim(preg_replace($this->DB, $this->DB_TIME_ISO, $timestamp));
	}
}

/**
 * Classe che rappresenta un orario (ore e minuti)
 * @package date
 */
class TIME extends TIMEBASE
{
	/**
	 * Costruisce un orario per interagire con un certo dbms
	 * @param string $db FBIRD, MYSQL, MSSQL, ODBC, ...
	 */
	public function __construct($db)
	{
		$this->DB     = trim(lang_database($db, 'TM-DB'));
		$this->DB_ISO = trim(lang_database($db, 'TM-DB2ISO'));
		$this->USER   = trim(lang_database($db, 'TM-USER'));
		$this->ISO    = trim(lang_database($db, 'TM-ISO'));
		$this->REV    = trim(lang_database($db, 'TM-REV'));

		$this->ISO_SEARCH_Y  = '';
		$this->USER_SEARCH_Y = '';
		$this->REV_SEARCH_Y  = '';
	}

	protected function any_2_ts($any, $fmt, $search)
	{
		setlocale(LC_TIME, trim(lang('LC_TIME')));

		// prendo le parti
		$parts = strptime($any, $fmt);
		if ($parts === FALSE)
			trigger_error(lang('ERR-TIME-FMT', $any, $fmt), E_USER_WARNING);
		// faccio l'array
		return $this->mktime($parts);
	}
	
	protected function ts_2_any($ts, $fmt, $search)
	{
		return strftime($fmt, $ts);
	}
	
	protected function mktime($parts)
	{
		return mktime($parts['tm_hour'], $parts['tm_min']);
	}

	/**
	 * Ritorna un timestamp di sistema partendo da un numero di minuti anche superiore a 60
	 * @param integer $minutes
	 * @return integer
	 */
	public function minutes_2_ts($minutes)
	{
		$h = floor($minutes / 60);
		$m = $minutes % 60;
		
		return mktime($h, $m);
	}
}

/**
 * Trasforma l'anno in uno di struttura equivalente sui giorni
 * @param integer $anno L'anno da traslare
 * @return integer
 * @deprecated
 */
function trasla_anno($anno, &$shift)
{
	if ($anno % 100 == 0 && $anno % 400 != 0) {
		$anno  -= 6; $shift -= 6;
	}
	$secoli_attraversati = 0;

	while($anno <= 1970) {
		for($i = 0; $i < 28; $i ++) {
			if ($anno % 100 == 0 && $anno % 400 != 0) {
				$secoli_attraversati ++;
			}
			$anno ++; $shift ++;
		}
	}
	while($anno >= 2037) {
		for($i = 0; $i < 28; $i ++) {
			if ($anno % 100 == 0 && $anno % 400 != 0) {
				$secoli_attraversati --;
			}
			$anno --; $shift --;
		}
	}

	$anno  += 12 * $secoli_attraversati;
	$shift += 12 * $secoli_attraversati;

	if ($anno <= 1970 || $anno >= 2037)
		return trasla_anno($anno, $shift);
	else
		return $anno;
}

/**
 * Aggiunge il numero di giorni alla data passata
 * @param string $data Data in formato gg/mm/yyyy
 * @param integer $numero Numero di giorni da sommare / sottrarre
 * @return string
 * @deprecated
 */
function somma_giorni($data, $giorni)
{
	$shift  = 0;
	$sgn    = ($giorni > 0 ? 1 : -1);
	$giorni = abs($giorni);

	$data   = explode("/", $data);
	$anno   = trasla_anno($data[2], $shift);
	$mese   = $data[1];
	$giorno = $data[0];
	$ts = 0;
	while ($giorni > 0) {

		$delta = min(365, $giorni);
		
		$ts = mktime(0, 0, 0, $mese, $giorno + $sgn * $delta, $anno);

		$anno   = trasla_anno(date('Y', $ts), $shift);
		$mese   = date('m', $ts);
		$giorno = date('d', $ts);
		
		$giorni -= $delta;
	}

	return date("d/m", $ts)."/".($anno - $shift);
}

/**
 * Ritorna il numero di giorni fra le due date, con segno. La prima e' quella di riferimento, quindi delta_giorni('01/01/2005', '02/01/2005') ritorna +1, delta_giorni('02/01/2005', '01/01/2005') ritorna -1
 * @param string $data1 Data in formato gg/mm/yyyy
 * @param string $data2 Data in formato gg/mm/yyyy
 * @return integer
 * @deprecated
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
 * @deprecated
 */
function giorno_settimana($data)
{
	$shift = 0;
	$data = explode("/", $data);
	$anno = trasla_anno($data[2], $shift);

	$wday = date("w", mktime(0,0,0, $data[1], $data[0], $anno));
	
	if ($wday == 0) $wday = 7;

	return $wday;
}

/**
 * Ritorna un numero < 0 se data1 < data2; 0 se sono uguali; > 0 se data2 > data1
 * @param string $data1 Data in formato gg/mm/yyyy
 * @param string $data2 Data in formato gg/mm/yyyy
 * @return integer
 * @deprecated
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
 * @deprecated
 */
function dateISO($data)
{
	return implode("", array_reverse(explode("/", $data)));
}

/**
 * Ritorna il numero di minuti che avanzano dal conteggio di ore intere da una quantita' di minuti complessiva. 
 * @param int $minuti numero di minuti
 * @return integer numero di minuti nell'ora non finita 
 * @deprecated
 */
function calcola_minuti($minuti)
{
	return ($minuti % 60);
}

/**
 * Ritorna il numero di ore intere presenti in una quantita' di minuti
 * @param int $minuti numero di minuti
 * @return integer numero di ore 
 * @deprecated
 */
function calcola_ore($minuti)
{
	return floor($minuti / 60);	
}


?>