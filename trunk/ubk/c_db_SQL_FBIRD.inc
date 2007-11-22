<?php

/**
 * Generazione SQL per FIREBIRD / INTERBASE
 * @package db
 * @author Ubik <emiliano.leporati@gmail.com>
 */
class DB_SQL_FBIRD extends DB_SQL
{
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
#		// Paginazione
#		if (!is_null($pagina_iniziale) && !is_null($dim_pagina) && $dim_pagina != 0) 
#			$_sql .= "FIRST $dim_pagina SKIP ".($pagina_iniziale * $dim_pagina)." ";
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
			$_sql .= " ROWS ".($dim_pagina * $pagina_iniziale + 1)." TO ".($dim_pagina * ($pagina_iniziale + 1));
		
		return $_sql;
	}

	public static function tsEqu($colonna, $valore)
	{
		$valore_db = call_user_func(array(self::$driver, 'utente_2_db'), $colonna, $valore);

		return $colonna.' - CAST('.$valore_db.' as TIMESTAMP) < 1';
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
	 * Genera un confronto che dice se la data cade negli ultimi n giorni
	 * @param string $colonna Nome del campo
	 * @param integer $giorni Giorni da contare
	 * @return string
	 */ 
	public static function in_last_days($colonna, $giorni)
	{
		return "$colonna >= current_date - $giorni";
	}

	/**
	 * Genera un confronto che dice se la data non cade negli ultimi n giorni
	 * @param string $colonna Nome del campo
	 * @param integer $giorni Giorni da contare
	 * @return string
	 */ 
	public static function not_in_last_days($colonna, $giorni)
	{
		return "$colonna < current_date - $giorni";
	}

}


?>