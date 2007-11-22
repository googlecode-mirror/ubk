<?php

/**
 * Generazione SQL per SQL SERVER
 * @package db
 * @author Ubik <emiliano.leporati@gmail.com>
 */
class DB_SQL_MSSQL extends DB_SQL
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
		$sql = "SELECT ";

		if ($distinct) $sql .= "DISTINCT ";

		// Paginazione
		if (!is_null($pagina_iniziale) && !is_null($dim_pagina) && $dim_pagina != 0) {
			$sql .= "TOP ".(($pagina_iniziale + 1) * $dim_pagina)." ";
		}

		$sql .= $colonne;

		// inserisco il FROM
		$sql .= " FROM $nome_tabella";
		
		// Inserisco il WHERE se necessario
		if ($condizione) { 
			$sql .= " WHERE $condizione";
		}		
		// Inserisco l'ORDER BY, se necessario
		if ($ordine) {
			$sql .= " ORDER BY $ordine";
		}			
		
		$result = array(
			"SQL" => $sql,
			"TABELLA" => $nome_tabella,
			"CAMPI" => $colonne,
			"CONDIZIONE" => $condizione,
			"ORDINE" => $ordine,
			"DISTINCT" => $distinct,
			"PAGE" => $pagina_iniziale,
			"PAGE-SIZE" => $dim_pagina
			);

		return $result;
	}

	/**
	 * Genera un confronto di similitudine sulle varianti lessicali di ognuna delle parole (supposte separate da spazi) presenti nel valore specificato - solo per SQL Server
	 * @param string $colonna Nome del campo, attualmente non utilizzato - la ricerca viene fatta su tutte le colonne indicizzate per la ricerca full-text
	 * @param mixed $parole_str Elenco di parole separate da spazi, in formato utente
	 * @return string
	 */ 
	function text_like($colonna, $parole_str)
	{
		$parole = explode(" ", $parole_str);
		$clausole = array();

		foreach($parole as $parola)	{
		
			$parola = trim($parola);
			if(strlen($parola) > 0){
				array_push($clausole, dQt($parola));
			}
		}

		$star = (isSet($GLOBALS['FTS-TABLE']) ? $GLOBALS['FTS-TABLE']."." : "");

		return par("CONTAINS({$star}*, 'FORMSOF(INFLECTIONAL, ".implode(", ", $clausole).")')");
	}

	/**
	 * Genera un confronto sull'anno delle date, con operatore qualunque
	 * @param string $colonna Nome del campo
	 * @param mixed $valore Numero dell'anno, YYYY
	 * @param string $operatore Operatore di confronto, di default =
	 * @return string
	 */ 
	function year($colonna, $valore, $operatore = "=")
	{
		if (!is_numeric($valore)) {
			trigger_error("Confronto non valido con '$valore' per operatore 'year'.", E_USER_ERROR);
		}
		return "LEFT($colonna, 4) $operatore ".self::utente_2_db("t", $valore);
	}

	/**
	 * Genera un confronto sul mese delle date, con operatore qualunque
	 * @param string $colonna Nome del campo
	 * @param mixed $valore Numero del mese, MM
	 * @param string $operatore Operatore di confronto, di default =
	 * @return string
	 */ 
	function month($colonna, $valore, $operatore = "=")
	{
		if (!is_numeric($valore)) {
			trigger_error("Confronto non valido con '$valore' per operatore 'month'.", E_USER_ERROR);
		}
		return "SUBSTRING($colonna, 5, 2) $operatore ".self::utente_2_db("t", $valore);
	}

	/**
	 * Genera un confronto sul giorno delle date, con operatore qualunque
	 * @param string $colonna Nome del campo
	 * @param mixed $valore Numero del giorno, GG
	 * @param string $operatore Operatore di confronto, di default =
	 * @return string
	 */ 
	function day($colonna, $valore, $operatore = "=")
	{
		if (!is_numeric($valore)) {
			trigger_error("Confronto non valido con '$valore' per operatore 'day'.", E_USER_ERROR);
		}
		return "SUBSTRING($colonna, 7, 2) $operatore ".self::utente_2_db("t", $valore);
	}

	function all_tp($tabella_ricerca, $campo_ricerca, $campo_filtro, $valore)
	{
		if (is_array($valore)) {
			$conds = array();
			foreach($valore as $valore_i) {
				array_push($conds, self::equ($campo_filtro, $valore_i));
			}
			$or = implode(" OR ", array_map("par", $conds));
			return "$campo_ricerca IN (SELECT $campo_ricerca FROM $tabella_ricerca WHERE $or GROUP BY $campo_ricerca HAVING COUNT(*) = ".count($valore).")";
		} else {
			return "$campo_ricerca IN (SELECT $campo_ricerca FROM $tabella_ricerca WHERE ".self::equ($campo_filtro, $valore).")";
		}
	}

}


?>