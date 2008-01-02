<?php
/**
 * Funzioni generiche di manipolazione dati per generare codice SQL in modo programmatico da PHP
 * @author Ubik <emiliano.leporati@gmail.com>
 * @package db
 */


/**
 * Verifica che l'array passato possa essere utilizzato come insieme di parametri di connessione a db
 * Se non e' valida, solleva eccezione
 * @param array $db_connection
 */
function check_db_connection($db_connection)
{
	$_needs = array("DB_SERVER", "DB_USERNAME", "DB_PASSWORD", "DB_NAME");
	foreach($_needs as $_key) {
		if (!array_key_exists($_key, $db_connection))
			throw new CodeException("Informazioni di connessione al db incomplete, manca ".$_key);
	}
}

/**
 * Restituisce la lunghezza di un campo in modo furbo in base al tipo concreto di dato
 * @param string $nome_campo nome del campo
 * @param string $tipo_campo tipo del campo (***)
 * @param string $lunghezza_db la lunghezza presente sul database
 * @return integer 
 */
function calcola_lunghezza_campo($nome_campo, $tipo_campo, $lunghezza_db)
{
	switch($tipo_campo) {
		case 'int':
		case 'float':
			$lunghezza = 11; 
			break;
		case 'date':
			$lunghezza = 10; 
			break;
		case 'text':
			$lunghezza = 65535 * 4;
			break;
		default:
			switch($nome_campo[0]) {
			case "d":
				$lunghezza = 10; 
				break;
			case "h":
				$lunghezza = 5; 
				break;
			default:
				$lunghezza = $lunghezza_db; 
				break;
			}
	}
	
	return $lunghezza;
}


/**
 * Ritorna il nome del campo id della tabella passata
 * @param string $tabella
 * @return string
 */
function campo_id_tabella($tabella)
{
	if (strtolower(substr($tabella, 0, 3)) == "t_e") {
		return "t_".(defined("TAB_STRIP") ? substr($tabella, TAB_STRIP) : $tabella);
	} else {
		return "i_".(defined("TAB_STRIP") ? substr($tabella, TAB_STRIP) : $tabella)."_id";
	}
}


/**
 * Dato un campo qualificato con il nome della tabella (tabella.campo), ritorna solo la parte (campo)
 * @param string $nome_campo_qualificato Nome del campo qualificato con il nome della tabella
 * @return string
 */ 
function isola_nome_campo($nome_campo_qualificato)
{
	return substr(strstr($nome_campo_qualificato, "."), 1);
}


/**
 * Ritorna il numero di righe nella tabella rispondenti ai criteri specificati
 * @param string $tabella
 * @param string $condizione
 * @param bool $distinct
 * @return integer
 */
function conta($tabella, $condizione = NULL, $distinct = FALSE)
{
	$_rs = new GESTORE($tabella);
	$_res = $_rs -> conta($condizione, $distinct);
	unSet($_rs);
	return $_res;
}

/**
 * Dice se esistono righe rispondenti ai criteri specificati nella tabella
 * @param string $tabella
 * @param string $condizione
 * @return bool
 */
function esiste($tabella, $condizione = NULL)
{
	$_rs = new GESTORE($tabella);
	$_res = $_rs -> esiste($condizione);
	unSet($_rs);
	return $_res;
}

/**
 * Ritorna il valore del $campo_out sulla $tabella in cui il valore di $campo_in e' uguale a $valore. 
 * Se $campo_in e' omesso, viene usato il campo chiave della tabella.
 * Se non viene trovato nulla e $ret_on_eof e' !== false, viene ritornato $ret_on_eof, altrimenti errore.
 * @param string $tabella Tabella in cui eseguire la decodifica
 * @param mixed $valore Valore da decodificare
 * @param string $campo_out Campo di cui ritornare il valore
 * @param string $campo_in Campo di cui eseguire la ricerca di {@link $valore}
 * @param mixed $ret_on_eof Se false solleva un errore nel caso la decode fallisca, altrimenti ritorna quel valore
 * @return mixed
 */
function decode($tabella, $valore, $campo_out, $campo_in = NULL, $ret_on_eof = FALSE)
{
	$_rs = new GESTORE($tabella, $campo_in);
	$_rs -> carica_righe_id($valore, NULL, $campo_out, FALSE, FALSE);
	
	if ($_rs -> eof) {
		unSet($_rs);

		if ($ret_on_eof !== FALSE) {
			return $ret_on_eof;
		} else {
			throw new CodeException("Valore $valore non trovato in $tabella.$campo_in");
		}
	}

	if (is_array($campo_out)) {
		$_out = array();
		foreach($campo_out as $_campo) {
			$_out[] = $_rs->valore($_campo);
			$_out[$_campo] = $_rs->valore($_campo);
		}
	} else {
		$_out = $_rs->valore($campo_out);
	}
	
	unSet($_rs);
	return $_out;
}

function decode_colonna($tabella, $valore, $campo_out, $campo_in = NULL)
{
	$_rs = new GESTORE($tabella, $campo_in);
	$_rs -> carica_righe_id($valore, NULL, $campo_out, FALSE, FALSE);
	
	if ($_rs -> eof) {
		unSet($_rs);
		return array();
	}

	if (is_array($campo_out)) {
		$_out = array();
		foreach($campo_out as $_campo)
			$_out[$_campo] = array();

		for(;!$_rs->eof; $_rs->move_next()) {
			foreach($campo_out as $_campo) {
				$_out[$_campo][] = $_rs->valore($_campo);
			}
		}
	} else {
		$_out = $_rs->valori_colonna($campo_out);
	}
	
	unSet($_rs);
	return $_out;
}

/**
 * Ritorna il valore del $campo_out sulla $tabella filtrata in base alla $condizione.
 * Se non viene trovato nulla e $ret_on_eof e' !== false, viene ritornato $ret_on_eof, altrimenti errore.
 * @param string $tabella Tabella in cui eseguire la ricerca
 * @param string $condizione Condizione di filtro
 * @param string $campo_out Campo di cui ritornare il valore
 * @param mixed $ret_on_eof Se false solleva un errore nel caso la decode fallisca, altrimenti ritorna quel valore
 * @return mixed
 */
function lookup($tabella, $condizione, $campo_out, $ret_on_eof = FALSE)
{
	$_rs = new GESTORE($tabella);
	$_rs -> carica($condizione, NULL, $campo_out, FALSE, FALSE);
	
	if ($_rs -> eof) {
		unSet($_rs);

		if ($ret_on_eof !== FALSE) {
			return $ret_on_eof;
		} else {
			throw new CodeException("Nessuna riga rispondente ai criteri '$condizione' in $tabella");
		}
	}

	if (is_array($campo_out)) {
		$_out = array();
		foreach($campo_out as $_campo) {
			$_out[] = $_rs->valore($_campo);
			$_out[$_campo] = $_rs->valore($_campo);
		}
	} else {
		$_out = $_rs->valore($campo_out);
	}
	unSet($_rs);
	return $_out;
}


function lookup_colonna($tabella, $condizione, $campo_out, $ret_on_eof = FALSE)
{
	$_rs = new GESTORE($tabella);
	$_rs -> carica($condizione, NULL, $campo_out, FALSE, FALSE);
	
	if ($_rs -> eof) {
		unSet($_rs);

		if ($ret_on_eof !== FALSE) {
			return $ret_on_eof;
		} else {
			throw new CodeException("Nessuna riga rispondente ai criteri '$condizione' in $tabella");
		}
	}

	if (is_array($campo_out)) {
		$_out = array();
		foreach($campo_out as $_campo) {
			$_out[] = $_rs->valori_colonna($_campo);
			$_out[$_campo] = $_rs->valori_colonna($_campo);
		}
	} else {
		$_out = $_rs->valori_colonna($campo_out);
	}


	unSet($_rs);
	return $_out;
}


/**
 * Sulla pagina HTML i dati sono memorizzati in una matrice (array di array) della forma array(campo => array(id_riga => valore)); le funzioni {@link genera_update} e {@link genera_insert} prevedono parametri del tipo array(campo => valore), quindi dobbiamo fare una funzione di "trasduzione"
 * @param mixed $id_riga Id della riga da cercare
 * @param array $valori Matrice dei valori (proveniente di solito dalla pagina HTML)
 * @return array
 */ 
function prendi_riga($id_riga, $valori)
{
	$risultato = array();
	$id_riga = (string)$id_riga;
	foreach($valori as $campo => $valori_campo) {
		if (is_array($valori_campo) && array_key_exists($id_riga, $valori_campo)) {
			$risultato[$campo] = $valori_campo[$id_riga];
		}
	}

	if (array_key_exists("checkboxes", $valori)) {
		$checks = explode("|", $valori["checkboxes"]);
		foreach($checks as $check) {
			if (!array_key_exists($check, $risultato)) {
				$risultato[$check] = "0";
			}
		}
	}

	return $risultato;
}

/**
 * Sulla pagina HTML i dati sono memorizzati in una matrice (array di array) della forma array(campo => array(id_riga => valore)); quando dobbiamo invocare metodi di un {@link GESTORE} con dati che non arrivano da una pagina, questa funzione crea l'array giusto partendo da dei bind del tipo array(campo => valore)
 * @param mixed $id_riga Id della riga da cercare
 * @param array $valori Matrice dei valori (proveniente di solito dalla pagina HTML)
 * @return array
 */ 
function emula_post($id_riga, $valori)
{
	$trans = array_flip(get_html_translation_table(HTML_ENTITIES));
	$risultato = array();

	foreach($valori as $campo => $valore) {
		$risultato[$campo][$id_riga] = ($valore === FALSE ? FALSE : strtr($valore, $trans));
	}
	return $risultato;
}

/**
 * Sulla pagina HTML i dati sono memorizzati in una matrice (array di array) della forma array(campo => array(id_riga => valore)); quando dobbiamo invocare metodi di un {@link GESTORE} con dati che non arrivano da una pagina, questa funzione crea l'array giusto partendo da dei bind del tipo array(campo => valore), codificando in formato utf-8, da usare al posto dell'altra nel caso ci siano problemi di codifica.
 * @param mixed $id_riga Id della riga da cercare
 * @param array $valori Matrice dei valori (proveniente di solito dalla pagina HTML)
 * @return array
 */ 
function emula_post_utf8($id_riga, $valori)
{
	$trans = array_flip(get_html_translation_table(HTML_ENTITIES));
	$risultato = array();

	foreach($valori as $campo => $valore) {
		$risultato[$campo][$id_riga] = ($valore === FALSE ? FALSE : strtr(utf8_encode($valore), $trans));
	}
	return $risultato;
}





?>