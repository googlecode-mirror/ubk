<?php

require_once("funzioni_data.inc");

/**
 * Generazione SQL per POSTGRES
 * @package db
 * @author Ubik <emiliano.leporati@gmail.com>
 */
class DB_SQL_POSTGRES extends DB_SQL
{
	public static function db_2_utente($colonna, $valore)
	{
		switch($colonna[0]) {
		case "d":
			$obj = new DATE('POSTGRES');
			return $obj->db_2_user($valore);
		case "h":
			$obj = new TIME('POSTGRES');
			return $obj->db_2_user($valore);
		case "r":
			$obj = new TIMESTAMP('POSTGRES');
			return $obj->db_2_user($valore);
		case "b":
			return (is_null($valore) || $valore == 'f' || $valore == 'false') ? FALSE : TRUE;
		case "i":
			return (is_null($valore) ? $valore : (int)$valore);
		case "f":
			if (strpos($valore, '.') !== FALSE) {
				$valore = preg_replace('/[0]+$/', '', $valore);
				if (substr($valore, -1) == '.') $valore = substr($valore, 0, -1);
			}
			return (is_null($valore) ? $valore : (float)$valore);
		case "s":
			return rtrim(strtoupper($valore));
		case "t":
			return rtrim($valore);
		case "e":
			$cry = new CRYPTER("Chiave in chiaro da cambiare come si desidera");
			$value = $cry->decrypt($valore);
			unset($cry);
			
			return $value;
		case "c":
			return $valore;
		default:
			throw new CodeException("Formato colonna sconosciuto o non supportato: $colonna");
		}
	}


	public static function utente_2_db($colonna, $valore)
	{
		if ($valore === "" || is_null($valore)) return "NULL";

		switch($colonna[0]) {
		case "e":
			$cry = new CRYPTER("Chiave in chiaro da cambiare come si desidera");
			$value = $cry->encrypt($valore);
			unset($cry);

			return qt($value);
		case "c":
			return qt(CRYPTER::hash($valore));
		case "s":
			return qt(utf8_decode(htmlentities(strtoupper(stripslashes($valore)), ENT_QUOTES, 'UTF-8')));
		case "t":
			return qt(utf8_decode(htmlentities(stripslashes($valore), ENT_QUOTES, 'UTF-8')));
		case "i":
		case "f":
			return $valore;
		case "d":
			$obj = new DATE('POSTGRES');
			return qt($obj->user_2_db($valore));
		case "h":
			$obj = new TIME('POSTGRES');
			return qt($obj->user_2_db($valore));
		case "r":
			$obj = new TIMESTAMP('POSTGRES');
			return qt($obj->user_2_db($valore));
		case "b":
			return qt(empty($valore) ? 'false' : 'true');
		default:
			throw new CodeException("Formato colonna sconosciuto o non supportato: $colonna");
		}
	}


	/**
	 * Genera un confronto di similitudine (LIKE)
	 * @param string $colonna Nome del campo
	 * @param mixed $valore Valore in formato utente
	 * @return string
	 */ 
	public static function like($colonna, $valore)
	{
		switch($colonna[0]) {
		case 's':
		case 't':
			$valore = strtr($valore, array('?' => '_', '*' => '%'));
			return self::cond($colonna, "%$valore%", 'ILIKE');
		default:
			return self::equ($colonna, $valore);
		}
	}

	/**
	 * Genera un confronto di inizio simile (LIKE)
	 * @param string $colonna Nome del campo
	 * @param mixed $valore Valore in formato utente
	 * @return string
	 */ 
	public static function begins_like($colonna, $valore)
	{
		switch($colonna[0]) {
		case 's':
		case 't':
			$valore = strtr($valore, array('?' => '_', '*' => '%'));
			return self::cond($colonna, "$valore%", 'ILIKE');
		default:
			return self::equ($colonna, $valore);
		}
	}

	/**
	 * Genera un confronto di fine simile (LIKE)
	 * @param string $colonna Nome del campo
	 * @param mixed $valore Valore in formato utente
	 * @return string
	 */ 
	public static function ends_like($colonna, $valore)
	{
		switch($colonna[0]) {
		case 's':
		case 't':
			$valore = strtr($valore, array('?' => '_', '*' => '%'));
			return self::cond($colonna, "%$valore", 'ILIKE');
		default:
			return self::equ($colonna, $valore);
		}
	}

	/**
	 * Ritorna la query di selezione in base ai parametri specificati
	 * @param string $tabella Il nome della tabella di cui caricare l'identit�
	 * @param string $colonne La stringa con in nomi di colonna separati da virgole (come da SQL)
	 * @param string $condizione Il contenuto dell WHERE
	 * @param string $ordine Il contenuto della ORDER BY
	 * @param string $distinct Vero o falso
	 * @param string $pagina_iniziale Il numero di pagina da cui iniziare  a visualizzare
	 * @param string $dim_pagina Numero di record da visualizzare 
	 * @return string
	 */
	public static function genera_select($nome_tabella, $colonne, $condizione = NULL, $ordine = NULL, $distinct = false, $pagina_iniziale = NULL, $dim_pagina = NULL)
	{
		$_sql = "SELECT ";
		// DISTINCT se necessario
		if ($distinct) $_sql .= "DISTINCT ";
		// colonne
		$_sql .= $colonne;
		// FROM
		$_sql .= " FROM $nome_tabella";
		// WHERE se necessario
		if ($condizione) $_sql .= " WHERE $condizione";
		// ORDER BY se necessario
		if ($ordine) $_sql .= " ORDER BY $ordine";
		// paginazione
		if (!is_null($pagina_iniziale) && !is_null($dim_pagina) && $dim_pagina != 0)
			$_sql .= " LIMIT ".($dim_pagina)." OFFSET ".($dim_pagina * $pagina_iniziale);
		
		return $_sql;
	}

	/**
	 * Genera una query EXISTS su una tabella
	 * @param string $tabella Nome della tabella
	 * @param string $condizione Eventuale clausola WHERE
	 * @return string
	 */ 
	public static function genera_exists($tabella, $condizione = NULL)
	{	
		$_where = (is_null($condizione) ? '' : ' WHERE '.$condizione);

		return 'SELECT EXISTS(SELECT 1 FROM '.$tabella.$_where.') as b_exists';
	}

	/**
	 * Genera un confronto sull'anno delle date, con operatore qualunque
	 * @param string $colonna Nome del campo
	 * @param mixed $valore Numero dell'anno, YYYY
	 * @param string $operatore Operatore di confronto, di default =
	 * @return string
	 */ 
	public static function year($colonna, $valore, $operatore = "=")
	{
		if (!is_numeric($valore)) {
			throw new CodeException("Confronto non valido con '$valore' per operatore 'year'.");
		}
		return "extract(year from $colonna) $operatore ".call_user_func(array(self::$driver, 'utente_2_db'), "i", $valore);
	}

	/**
	 * Genera un confronto sul mese delle date, con operatore qualunque
	 * @param string $colonna Nome del campo
	 * @param mixed $valore Numero del mese, MM
	 * @param string $operatore Operatore di confronto, di default =
	 * @return string
	 */ 
	public static function month($colonna, $valore, $operatore = "=")
	{
		if (!is_numeric($valore)) {
			throw new CodeException("Confronto non valido con '$valore' per operatore 'month'.");
		}
		return "extract(month from $colonna) $operatore ".call_user_func(array(self::$driver, 'utente_2_db'), "i", $valore);
	}

	/**
	 * Genera un confronto sul giorno delle date, con operatore qualunque
	 * @param string $colonna Nome del campo
	 * @param mixed $valore Numero del giorno, GG
	 * @param string $operatore Operatore di confronto, di default =
	 * @return string
	 */ 
	public static function day($colonna, $valore, $operatore = "=")
	{
		if (!is_numeric($valore)) {
			throw new CodeException("Confronto non valido con '$valore' per operatore 'day'.");
		}
		return "extract(day from $colonna) $operatore ".call_user_func(array(self::$driver, 'utente_2_db'), "i", $valore);
	}


	/**
	 * Genera un confronto che dice se il timestamp cade negli ultimi n minuti
	 * @param string $colonna Nome del campo
	 * @param integer $minuti Minuti da contare
	 * @return string
	 */ 
	public static function in_last_minutes($colonna, $minuti)
	{
		return "current_timestamp - $colonna < $minuti * interval '1 minute'";
	}

	/**
	 * Genera un confronto che dice se la data cade negli ultimi n giorni
	 * @param string $colonna Nome del campo
	 * @param integer $giorni Giorni da contare
	 * @return string
	 */ 
	public static function in_last_days($colonna, $giorni)
	{
		return "current_timestamp - $colonna < $giorni * interval '1 day'";
	}

	/**
	 * Genera un confronto che dice se la data non cade negli ultimi n giorni
	 * @param string $colonna Nome del campo
	 * @param integer $giorni Giorni da contare
	 * @return string
	 */ 
	public static function not_in_last_days($colonna, $giorni)
	{
		return "current_timestamp - $colonna >= $giorni * interval '1 day'";
	}

}


?>