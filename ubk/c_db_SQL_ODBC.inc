<?php

/**
 * Generazione SQL per ODBC
 * @package db
 * @author Ubik <emiliano.leporati@gmail.com>
 */
class DB_SQL_ODBC extends DB_SQL
{

	public static function db_2_utente($colonna, $valore)
	{
		switch($colonna[0]) {
		case "d":
			$obj = new DATE('ODBC');
			return $obj->db_2_user($valore);
		case "h":
			$obj = new TIME('ODBC');
			return $obj->db_2_user($valore);
		case "r":
			$obj = new TIMESTAMP('ODBC');
			return $obj->db_2_user($valore);
		case "b":
			return ($valore == 1) ? true : false;
		case "i":
			return $valore;
		case "f":
			return (float)$valore;
		case "s":
			return strtoupper($valore);
		case "t":
			return $valore;
		default:
			throw new CodeException("Formato colonna sconosciuto o non supportato: $colonna");
		}
	}


	public static function utente_2_db($colonna, $valore)
	{
		if ($valore === "" || is_null($valore)) return "NULL";

		switch($colonna[0]) {
		case "s":
			return qt(htmlentities(utf8_decode(strtoupper(stripslashes($valore))), ENT_QUOTES));
		case "t":
			 return qt(htmlentities(utf8_decode(stripslashes($valore)), ENT_QUOTES));
		case "i":
		case "f":
			return $valore;
		case "d":
			$obj = new DATE('ODBC');
			return qt($obj->user_2_db($valore));
		case "h":
			$obj = new TIME('ODBC');
			return qt($obj->user_2_db($valore));
		case "r":
			$obj = new TIMESTAMP('ODBC');
			return qt($obj->user_2_db($valore));
		case "b":
			return qt(empty($valore) ? 0 : 1);
		default:
			throw new CodeException("Formato colonna sconosciuto o non supportato: $colonna");
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
		
		if ($distinct) 
			$_sql .= "DISTINCT ";

		$_sql .= $colonne;

		// inserisco il FROM
		$_sql .= " FROM $nome_tabella";
		
		// Inserisco il WHERE se necessario
		if ($condizione) {
			$_sql .= " WHERE $condizione";
		}		
		// Inserisco l'ORDER BY, se necessario
		if ($ordine) {
			$_sql .= " ORDER BY $ordine";
		}
		
		return $_sql;
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
		case "s":
		case "t":
			return self::cond("UCASE($colonna)", strtoupper("%{$valore}%"), "LIKE");
		default:
			return call_user_func(array(self::$sql, 'equ'), $colonna, $valore);
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
		case "s":
		case "t":
			return self::cond("UCASE($colonna)", strtoupper("{$valore}%"), "LIKE");
		default:
			return call_user_func(array(self::$sql, 'equ'), $colonna, $valore);
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
		case "s":
		case "t":
			return self::cond("UCASE($colonna)", strtoupper("%{$valore}"), "LIKE");
		default:
			return call_user_func(array(self::$sql, 'equ'), $colonna, $valore);
		}
	}


}


?>