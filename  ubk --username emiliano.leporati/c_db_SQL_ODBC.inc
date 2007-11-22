<?php

/**
 * Generazione SQL per ODBC
 * @package db
 * @author Ubik <emiliano.leporati@gmail.com>
 */
class DB_SQL_ODBC extends DB_SQL
{

	/**
	 * Ritorna la query di selezione in base ai parametri specificati
	 * @param string $tabella Il nome della tabella di cui caricare l'identità
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