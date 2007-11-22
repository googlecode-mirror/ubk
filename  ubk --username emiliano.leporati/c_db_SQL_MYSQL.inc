<?php

/**
 * Generazione SQL per MYSQL
 * @package db
 * @author Ubik <emiliano.leporati@gmail.com>
 */
class DB_SQL_MYSQL extends DB_SQL
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
		// Paginazione
		if (!is_null($pagina_iniziale) && !is_null($dim_pagina) && $dim_pagina != 0) 
			$_sql .= " LIMIT $dim_pagina OFFSET ".($pagina_iniziale * $dim_pagina);
		
		return $_sql;
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
//		return "F_YEAR($colonna) $operatore ".call_user_func(array(self::$driver, 'utente_2_db'), "i", $valore);
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
//		return "F_MONTH($colonna) $operatore ".call_user_func(array(self::$driver, 'utente_2_db'), "i", $valore);
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
//		return "F_DAY($colonna) $operatore ".call_user_func(array(self::$driver, 'utente_2_db'), "i", $valore);
	}


	/**
	 * Genera un confronto che dice se la data cade negli ultimi n giorni
	 * @param string $colonna Nome del campo
	 * @param integer $giorni Giorni da contare
	 * @return string
	 */ 
	public static function in_last_days($colonna, $giorni)
	{
//		return "$colonna >= current_date - $giorni";
	}

	/**
	 * Genera un confronto che dice se la data non cade negli ultimi n giorni
	 * @param string $colonna Nome del campo
	 * @param integer $giorni Giorni da contare
	 * @return string
	 */ 
	public static function not_in_last_days($colonna, $giorni)
	{
//		return "$colonna < current_date - $giorni";
	}

}


?>