<?php
/**
 * Definizione delle interfacce che ogni db-driver deve implementare
 * Classe di generazione istanze uniche (factory pattern)
 * @author Ubik <emiliano.leporati@gmail.com>
 *
 * @package db
 */


/**
 * Interfaccia per la lettura dei metadati
 * @author Ubik <emiliano.leporati@gmail.com>
 *
 * @package db
 * @abstract
 */
abstract class DB_META
{
	/**
	 * Imposta il db di lavoro
	 * @param array $db
	 * @returns string
	 */
	abstract public static function set_db($db = NULL);

	/**
	 * Elenco delle tabelle
	 * @returns array
	 */
	abstract public static function tables();
	/**
	 * Elenco dei campi della tabella
	 * @param string $table
	 * @returns array
	 */
	abstract public static function table_fields($table);
	/**
	 * Elenco dei campi della procedura / vista
	 * @param string $table
	 * @returns array
	 */
	abstract public static function procedure_fields($procedure);
	/**
	 * Elenco delle chiavi esterne della della tabella
	 * @param string $table
	 * @returns array
	 */
	abstract public static function table_foreign_keys($table);
	/**
	 * Elenco degli indici
	 * @param string $table
	 * @returns array
	 */
	abstract public static function table_indices($table);
	/**
	 * Elenco dei trigger della tabella
	 * @param string $table
	 * @returns array
	 */
	abstract public static function table_triggers($table);
	/**
	 * Corpo del trigger
	 * @param string $trigger
	 * @returns string
	 */
	abstract public static function table_trigger_body($trigger);


	
	/**
	 * Elenco delle viste del db
	 * @returns array
	 */
	abstract public static function views();
	/**
	 * Corpo della vista
	 * @param string $view
	 * @returns string
	 */
	abstract public static function view_body($view);



	/**
	 * Elenco delle procedure del db
	 * @returns array
	 */
	abstract public static function procedures();
	/**
	 * Elenco dei parametri della procedura
	 * @param string $procedure
	 * @returns array
	 */
	abstract public static function procedure_parameters($procedure);
	/**
	 * Corpo della procedura
	 * @param string $procedure
	 * @returns string
	 */
	abstract public static function procedure_body($procedure);

}

/**
 * Interfaccia per i driver - nascondono le funzioni base di interazione coi DBMS
 * @package db
 * @abstract
 */
abstract class DB_DRIVER
{
	/**
	 * Elenco delle ultime query eseguite
	 * @var array
	 */
	public static $SQL = array();

	/**
	 * Aggiunge una query all'elenco {@link $SQL}
	 * @param string $sql
	 */
	final public static function sql_push($sql)
	{
		array_unshift(self::$SQL, $sql);
		if (count(self::$SQL) > 20) array_pop(self::$SQL);
	}

	/**
	 * Nome della classe del driver - serve per le chiamate parametriche a metodi della stessa classe quando viene estesa
	 * @var string
	 */
	protected static $driver = 'DB_DRIVER';

	/**
	 * Imposta il nome della classe del driver
	 * @param string $driver
	 */
	final public static function set_driver($driver)
	{
		self::$driver = $driver;
	}

	/**
	 * Connessione
	 * @param array $db_connection Parametri di connessione
	 */
	abstract public static function connect($db_connection);
	/**
	 * Disconnessione
	 * @param resource &$conn_id La connection_id che verra' resa null a seguito dell'avvenuta disconnessione
	 */
	abstract public static function disconnect(&$conn_id);




	/**
	 * Ritorno errore (semplice)
	 * @return string
	 */
	abstract public static function get_last_message($conn_id = NULL);
	/**
	 * Ritorno errore (avanzato)
	 * @return string
	 */
	abstract public static function errmsg($sql, $conn_id = NULL);




	/**
	 * Esecuzione di una query (selezione / modifica)
	 * @param string $sql La query da eseguire
	 * @param resource $conn_id La connessione su cui eseguire la query
	 * @return integer
	 */
	abstract public static function query($sql, $conn_id);
	/**
	 * Caricamento della prossima riga in modalita' associativa (campo => valore)
	 * @param resource $rs Il result set di cui contare le righe coinvolte
	 * @return array
	 */
	abstract public static function & fetch_assoc($rs);
	/**
	 * Caricamento della prossima riga in modalita' numerale (numero riga => valore)
	 * @param resource $rs Il result set di cui contare le righe coinvolte
	 * @return array
	 */
	abstract public static function & fetch_row($rs);
	/**
	 * Liberazione risorse
	 * @param resource $rs Il result set da liberare
	 */
	abstract public static function free_result($rs);



	/**
	 * Quante righe sono coinvolte in una query di selezione
	 * @param resource $rs Il result set di cui contare le righe coinvolte
	 * @return integer
	 */
	abstract public static function num_rows_select($rs);
	/**
	 * Ritorna il numero di campi coinvolti da una query di selezione
	 * @param resource $rs Il result set di cui si vuole sapere il numero di campi
	 * @return integer 
	 */
	abstract public static function num_fields($rs);



	/**
	 * Ritorna le informazioni di un campo di una query di selezione
	 * @param resource $rs Il result set di cui si vuole sapere il numero di campi
	 * @param integer $i L'ordinale del campo di cui si vogliono conoscere le informazioni
	 * @return array 
	 */
	abstract public static function field_info_rs($rs, $i);
	/**
	 * Ritorna un array di array con le informazioni sui campi della tabella/vista richiesta
	 * @param string $table Il nome della tabella
	 * @param resource $conn_id connessione
	 * @return array 
	 */
	abstract public static function field_info_table($table, $conn_id);
	/**
	 * Ritorna un array di array con le informazioni sui parametri della store procedure richiesta
	 * @param string $table Il nome della store procedure
	 * @param resource $conn_id connessione
	 * @return array 
	 */
	abstract public static function field_info_procedure($procedure, $conn_id);
	/**
	 * Ritorna l'id corrente associato ad una tabella
	 * @param string $tabella Il nome della tabella di cui caricare l'identita' 
	 * @return integer
	 */
	abstract public static function identity($table, $conn_id);



	/**
	 * Prende un valore in formato database e lo trasforma nel formato utente (quello visualizzato a video). I formati conosciuti sono: d (data), h (ora), r (timestamp), b (booleano), i (intero), f (float), s (stringa uppercase), t (stringa), e (codifica reversibile), c (codifica hash irreversibile)
	 * @param string $colonna Nome del campo, mi serve per conoscerne il tipo
	 * @param mixed $valore Valore in formato db
	 * @return mixed
	 */
	abstract public static static function db_2_utente($colonna, $valore);
	/**
	 * Prende un valore in formato utente (quello visualizzato a video) lo trasforma nel formato database
	 * @param string $colonna Nome del campo, mi serve per conoscerne il tipo
	 * @param mixed $valore Valore in formato utente
	 * @return mixed
	 */
	abstract public static static function utente_2_db($colonna, $valore);

	/**
	 * Questa serve nel caso in cui i dati non debbano arrivare a video, ma debbano subire manipolazioni intermedie; vengono restituite totalmente in chiaro. Vedere {@link GESTORE::valore_fwk}
	 * @param string $colonna Nome del campo, mi serve per conoscerne il tipo
	 * @param mixed $valore Valore in formato db
	 * @return mixed
	 */
	final public static function db_2_fwk($colonna, $valore)
	{
		switch($colonna[0]) {
		case 'd':
		case 'h':
		case 'r':
		case 'b':
		case 'i':
		case 'f':
		case 'c':
			return call_user_func(array(self::$driver, 'db_2_utente'), $colonna, $valore);
		case 't':
		case 's':
		case 'e':
			$_res = call_user_func(array(self::$driver, 'db_2_utente'), $colonna, $valore);
			return utf8_encode(html_entity_decode($_res, ENT_QUOTES));
		default:
			throw new CodeException("Formato colonna sconosciuto o non supportato: $colonna");
		}
	}
	/**
	 * Questa serve nel caso in cui i dati debbano arrivare a video ma non in HTML vengono restituite totalmente in chiaro. Vedere {@link GESTORE::valore_ascii}
	 * @param string $colonna Nome del campo, mi serve per conoscerne il tipo
	 * @param mixed $valore Valore in formato db
	 * @return mixed
	 */
	final public static function db_2_ascii($colonna, $valore)
	{
		switch($colonna[0]) {
		case 'd':
		case 'h':
		case 'r':
		case 'b':
		case 'i':
		case 'f':
		case 'c':
			return call_user_func(array(self::$driver, 'db_2_utente'), $colonna, $valore);
		case 't':
		case 's':
		case 'e':
			$_res = call_user_func(array(self::$driver, 'db_2_utente'), $colonna, $valore);
			return html_entity_decode($_res, ENT_QUOTES);
		default:
			throw new CodeException("Formato colonna sconosciuto o non supportato: $colonna");
		}
	}

	/**
	 * Esecuzione di una stored procedure
	 * @param string $procedure Il nome della stored procedure
	 * @param array $params parametri della stored procedure gia' in formato db
	 * @param resource $conn_id connessione su cui eseguire la stored
	 */
	abstract public static function exec_procedure($procedure, $params, $conn_id);

	/**
	 * Inizia una transazione
	 * @param resource $conn_id
	 */
	public static function tr_begin(&$conn_id)
	{ }
	/**
	 * Commit di una transazione
	 * @param resource $tr transazione da confermare (connessione)
	 */
	public static function tr_commit(&$tr)
	{
		self::disconnect($tr);
	}
	/**
	 * Rollback di una transazione
	 * @param resource $tr transazione da annullare (connessione)
	 */
	public static function tr_rollback(&$tr)
	{
		self::disconnect($tr);
	}


}

/**
 * Interfaccia per la trasformazione da dati-pagina a query e clausole per le condizioni where
 * @abstract
 * @package db
 */
abstract class DB_SQL
{
	/**
	 * Nome della classe del driver
	 * @var string
	 */
	protected static $driver = 'DB_DRIVER';

	/**
	 * Nome della classe dell'sql
	 * @var string
	 */
	protected static $sql = 'DB_SQL';

	/**
	 * Imposta il nome della classe del driver
	 * @param string $driver
	 */
	final public static function set_driver($driver)
	{
		self::$driver = $driver;
	}

	/**
	 * Genera una condizione AND unendo tutte le sotto-clausole passate come argomento (usa func_get_args)
	 * @return string
	 */ 
	final public static function c_and()
	{
		$clausole = func_get_args();
		return implode(' AND ', array_map('par', $clausole));
	}

	/**
	 * Genera una condizione OR unendo tutte le sotto-clausole passate come argomento (usa func_get_args)
	 * @return string
	 */ 
	final public static function c_or()
	{
		$clausole = func_get_args();
		return implode(' OR ', array_map('par', $clausole));
	}

	/**
	 * Nega la condizione passata
	 * @param string $cond
	 * @return string
	 */ 
	final public static function c_not($cond)
	{
		return 'NOT '.par($cond);
	}

	/**
	 * Genera una condizione facente parte di una clausola WHERE, con un operatore binario qualunque. Se il valore da confrontare a' NULL, gli operatori = e != sono tradotti in IS e IS NOT, altrimenti viene sollevata un'eccezione
	 * @param string $colonna Nome del campo
	 * @param mixed $valore Valore in formato utente
	 * @param string $operatore Operatore binario di confronto
	 * @return string
	 */
	final protected static function cond($colonna, $valore, $operatore)
	{
		$valore_db = call_user_func(array(self::$driver, 'utente_2_db'), $colonna, $valore);

		if ($valore_db === 'NULL' && $operatore == '=') {
			$operatore = ' IS ';
		} elseif ($valore_db === 'NULL' && ($operatore == '!=' || $operatore == '<>')) {
			$operatore = ' IS NOT ';
		} elseif ($valore_db === 'NULL') {
			throw new DbException(__CLASS__.'::'.__METHOD__, "Confronto non valido con valore NULL ($operatore)");
		}
		return $colonna.' '.$operatore.' '.$valore_db;
	}

	/**
	 * Genera un confronto di uguaglianza (=)
	 * @param string $colonna Nome del campo
	 * @param mixed $valore Valore in formato utente
	 * @return string
	 */ 
	public static function equ($colonna, $valore)
	{
		return self::cond($colonna, $valore, '=');
	}

	/**
	 * Genera un confronto di uguaglianza (=) case-insensitive
	 * @param string $colonna Nome del campo
	 * @param mixed $valore Valore in formato utente
	 * @return string
	 */ 
	public static function iequ($colonna, $valore)
	{
		$cond = self::cond($colonna, $valore, '=');
		if (strpos($cond, '=') !== FALSE) {
			$pos = strlen($colonna) + 3;
			$cond = substr($cond, 0, $pos).strtoupper(substr($cond, $pos));
		}
		return strtr($cond, array($colonna => "UPPER($colonna)"));
	}

	/**
	 * Genera un confronto di differenza (!=)
	 * @param string $colonna Nome del campo
	 * @param mixed $valore Valore in formato utente
	 * @return string
	 */ 
	public static function diff($colonna, $valore)
	{	
		return self::cond($colonna, $valore, '!=');
	}

	/**
	 * Genera un confronto di maggioranza (>)
	 * @param string $colonna Nome del campo
	 * @param mixed $valore Valore in formato utente
	 * @return string
	 */ 
	final public static function gt($colonna, $valore)
	{
		return self::cond($colonna, $valore, '>');
	}

	/**
	 * Genera un confronto di minoranza (<)
	 * @param string $colonna Nome del campo
	 * @param mixed $valore Valore in formato utente
	 * @return string
	 */ 
	final public static function lt($colonna, $valore)
	{
		return self::cond($colonna, $valore, '<');
	}

	/**
	 * Genera un confronto di maggioranza o uguaglianza (>=)
	 * @param string $colonna Nome del campo
	 * @param mixed $valore Valore in formato utente
	 * @return string
	 */ 
	final public static function gte($colonna, $valore)
	{
		return self::cond($colonna, $valore, '>=');
	}

	/**
	 * Genera un confronto di minoranza o uguaglianza (<=)
	 * @param string $colonna Nome del campo
	 * @param mixed $valore Valore in formato utente
	 * @return string
	 */ 
	final public static function lte($colonna, $valore)
	{
		return self::cond($colonna, $valore, '<=');
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
			$cond = self::cond($colonna, "%$valore%", 'LIKE');
			$pos = strlen($colonna) + 6;
			$cond = substr($cond, 0, $pos).strtoupper(substr($cond, $pos));
			return strtr($cond, array('?' => '_', '*' => '%', $colonna => "UPPER($colonna)"));
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
			$cond = self::cond($colonna, "$valore%", 'LIKE');
			$pos = strlen($colonna) + 6;
			$cond = substr($cond, 0, $pos).strtoupper(substr($cond, $pos));
			return strtr($cond, array('?' => '_', '*' => '%', $colonna => "UPPER($colonna)"));
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
			$cond = self::cond($colonna, "%$valore", 'LIKE');
			$pos = strlen($colonna) + 6;
			$cond = substr($cond, 0, $pos).strtoupper(substr($cond, $pos));
			return strtr($cond, array('?' => '_', '*' => '%', $colonna => "UPPER($colonna)"));
		default:
			return self::equ($colonna, $valore);
		}
	}


	/**
	 * Genera un confronto di similitudine (LIKE) su ognuna delle parole (supposte separate da spazi) presenti nel valore specificato
	 * @param string $colonna Nome del campo
	 * @param mixed $parole_str Elenco di parole separate da spazi, in formato utente
	 * @return string
	 */ 
	final public static function word_like($colonna, $parole_str)
	{
		$parole = explode(' ', $parole_str);
		$clausole = array();
		foreach($parole as $parola)
		{
			$parola = trim($parola);
			if(strlen($parola) > 0){
				array_push(
						$clausole, 
						self::like($colonna, $parola)
					);
			}
		}
		return par(implode(' OR ', array_map('par', $clausole)));
	}

	/**
	 * Genera una condizione IN. Se l'array di valori passato a' vuoto, genera una clausola "dummy" sempre falsa
	 * @param string $colonna Nome del campo
	 * @param array $valori Valori di confronto
	 * @return string
	 */ 
	final public static function in($colonna, $valori)
	{
		$valori_db = array();
		foreach($valori as $valore) {
			array_push($valori_db, call_user_func(array(self::$driver, 'utente_2_db'), $colonna, $valore));
		}
		switch(count($valori_db)) {
		case 0:
			return '(1 = 0)';
		case 1:
			return "$colonna = {$valori_db[0]}";
		default:
			return "$colonna IN ".par(implode(', ', $valori_db));
		}
	}

	/**
	 * Genera una condizione NOT IN. Se l'array di valori passato a' vuoto, genera una clausola "dummy" sempre vera
	 * @param string $colonna Nome del campo
	 * @param array $valori Valori di confronto
	 * @return string
	 */ 
	final public static function not_in($colonna, $valori)
	{
		$valori_db = array();
		foreach($valori as $valore) {
			array_push($valori_db, call_user_func(array(self::$driver, 'utente_2_db'), $colonna, $valore));
		}
		if (count($valori_db) == 0) {
			return '(1 = 1)';
		} else {
			return "$colonna NOT IN ".par(implode(', ', $valori_db));
		}
	}

	/**
	 * Genera una condizione BETWEEN
	 * @param string $colonna Nome del campo
	 * @param mixed $valore_inizio Valore minimo
	 * @param mixed $valore_fine Valore massimo
	 * @return string
	 */ 
	final public static function between($colonna, $valore_inizio, $valore_fine)
	{
		$valore_inizio = call_user_func(array(self::$driver, 'utente_2_db'), $colonna, $valore_inizio);
		$valore_fine   = call_user_func(array(self::$driver, 'utente_2_db'), $colonna, $valore_fine);
		if ($valore_inizio == 'NULL' || $valore_fine == 'NULL') {
			throw new DbException(__CLASS__.'::'.__METHOD__, 'Confronto non valido con valore NULL (BETWEEN)');
		}
		return "$colonna BETWEEN $valore_inizio AND $valore_fine";
	}

	/**
	 * Genera una clausola IN basata su una subquery. La subquery viene generata come SELECT $campo_ricerca FROM $tabella_ricerca WHERE cond($campo_filtro, $valore), $campo_ricerca deve essere presente anche nella tabella corrente. Es. i_acc_utente_id IN (SELECT i_acc_utente_id FROM t_p_acc_utente_gruppo WHERE i_acc_gruppo_id = 4)
	 * @param string $tabella_ricerca Tabella in cui la ricerca viene effettuata
	 * @param string $campo_ricerca Nome del campo su cui viene effettuata la ricerca (il campo della tabella corrente viene confrontato con i valori della query su $tabella_ricerca.$campo_ricerca)
	 * @param string $campo_filtro Campo su cui viene fatto un filtro nella tabella di ricerca
	 * @param mixed $valore Valore ( o array di valori ) su cui viene fatto il filtro
	 * @return string
	 */
	final public static function in_tp($tabella_ricerca, $campo_ricerca, $campo_filtro, $valore)
	{
		if (is_array($valore)) {
			$_cond = self::in($campo_filtro, $valore);
		} else {
			$_cond = self::equ($campo_filtro, $valore);
		}
		return "$campo_ricerca IN (SELECT $campo_ricerca FROM $tabella_ricerca WHERE $_cond)";
	}

	/**
	 * Genera una clausola IN basata su una subquery. E' una generalizzazione di {@link in_tp}, in cui la ricerca nella subquery e' generalizzata e i campi nella tabella corrente e  in $tabella_ricerca non devono essere necessariamente omonimi.
	 * @param string $operatore Operatore di confronto fra $campo_filtro e $valore
	 * @param string $tabella_ricerca Tabella in cui la ricerca viene effettuata
	 * @param string $campo_ricerca Nome del campo su cui viene effettuata la ricerca (il campo della tabella corrente viene confrontato con i valori della query su $tabella_ricerca.$campo_ricerca)
	 * @param string $campo_filtro Campo su cui viene fatto un filtro nella tabella di ricerca
	 * @param string $campo Campo della tabella corrente, operando di sinistra della clausola IN
	 * @param mixed $valore Valore ( o array di valori ) su cui viene fatto il filtro
	 * @return string
	 */
	final public static function op_tp($operatore, $tabella_ricerca, $campo_ricerca, $campo_filtro, $campo, $valore)
	{
		$_cond = self::$operatore($campo_filtro, $valore);
		return "$campo IN (SELECT $campo_ricerca FROM $tabella_ricerca WHERE $_cond)";
	}


	/**
	 * Versione contratta della {@link op_tp}, in cui la condizione viene passata gia' calcolata, senza usare la terna $operatore($campo_filtro, $valore)
	 * @param string $tabella_ricerca Tabella in cui la ricerca viene effettuata
	 * @param string $campo_ricerca Nome del campo su cui viene effettuata la ricerca (il campo della tabella corrente viene confrontato con i valori della query su $tabella_ricerca.$campo_ricerca)
	 * @param string $campo Campo della tabella corrente, operando di sinistra della clausola IN
	 * @param string $cond Condizione di filtro su $tabella_ricerca
	 * @return string
	 */
	final public static function cond_tp($tabella_ricerca, $campo_ricerca, $campo, $cond)
	{
		return "$campo IN (SELECT $campo_ricerca FROM $tabella_ricerca WHERE $cond)";
	}

	/**
	 * Come la {@link cond_tp}, ma genera una clausola del tipo NOT IN
	 * @param string $tabella_ricerca Tabella in cui la ricerca viene effettuata
	 * @param string $campo_ricerca Nome del campo su cui viene effettuata la ricerca (il campo della tabella corrente viene confrontato con i valori della query su $tabella_ricerca.$campo_ricerca)
	 * @param string $campo Campo della tabella corrente, operando di sinistra della clausola IN
	 * @param string $cond Condizione di filtro su $tabella_ricerca
	 * @return string
	 */
	final public static function not_cond_tp($tabella_ricerca, $campo_ricerca, $campo, $cond)
	{
		return "$campo NOT IN (SELECT $campo_ricerca FROM $tabella_ricerca WHERE $cond)";
	}

	/**
	 * Ritorna la query di selezione in base ai parametri specificati
	 * @param string $tabella Il nome della tabella di cui caricare l'identita' 	 * @param string $colonne La stringa con in nomi di colonna separati da virgole (come da SQL)
	 * @param string $condizione Il contenuto dell WHERE
	 * @param string $ordine Il contenuto della ORDER BY
	 * @param string $distinct Vero o falso
	 * @param string $pagina_iniziale Il numero di pagina da cui iniziare  a visualizzare
	 * @param string $dim_pagina Numero di record da visualizzare 
	 * @return string
	 */
	abstract public static function genera_select($nome_tabella, $colonne, $condizione = NULL, $ordine = NULL, $distinct = false, $pagina_iniziale = NULL, $dim_pagina = NULL);

	/**
	 * Genera una clausola INSERT su una tabella dati i valori in un array associativo e le eventuali colonne di filtro
	 * @param string $tabella Nome della tabella
	 * @param array $valori Array associativo (campo => valore)
	 * @param array $colonne_filtro Se impostato, indica le sole colonne da inserire
	 * @return string
	 */ 
	public static function genera_insert($tabella, $valori, $colonne_filtro = NULL)
	{
		$_colonne  = array();
		$_valori = array();
		if (is_null($colonne_filtro)) {
			$_colonne  = array_keys($valori);
			$_valori   = array_map(array(self::$driver, 'utente_2_db'), $_colonne, array_values($valori));
		} else {
			foreach($valori as $colonna => $valore) {
				if (in_array($colonna, $colonne_filtro)) {
					array_push($_colonne, $colonna);
					array_push($_valori, call_user_func(array(self::$driver, 'utente_2_db'), $colonna, $valore));
				}
			}
		}
		return "INSERT INTO $tabella".par(implode(', ', $_colonne)).' VALUES '.par(implode(', ', $_valori));
	}

	/**
	 * Genera una clausola UPDATE su una tabella dati i valori in un array associativo e le eventuali colonne di filtro
	 * @param string $tabella Nome della tabella
	 * @param array $valori Array associativo (campo => valore)
	 * @param array $colonne_filtro Se impostato, indica le sole colonne da inserire
	 * @param string $condizione Eventuale clausola WHERE
	 * @return string
	 */ 
	public static function genera_update($tabella, $valori, $colonne_filtro = NULL, $condizione = NULL )
	{
		$_colonne = array();
		$_valori  = array();
		$_campi_valori = array();
		
		$_res = "UPDATE $tabella SET ";
		
		if (is_null($colonne_filtro)) {
			foreach($valori as $colonna => $valore) {
				array_push($_campi_valori, $colonna.' = '.call_user_func(array(self::$driver, 'utente_2_db'), $colonna, $valore)); 
			}
		} else {
			foreach($valori as $colonna => $valore) {
				if (in_array($colonna, $colonne_filtro)) {
					array_push($_campi_valori, $colonna.' = '.call_user_func(array(self::$driver, 'utente_2_db'), $colonna, $valore)); 
				}
			}
		}	
		$_res .= implode(', ', $_campi_valori);
		
		if (!is_null($condizione)){
			$_res .= ' WHERE '.par($condizione);
		}
		return $_res;
	}


	/**
	 * Genera una clausola DELETE su una tabella data un'eventuale condizione
	 * @param string $tabella Nome della tabella
	 * @param string $condizione Eventuale clausola WHERE
	 * @return string
	 */ 
	public static function genera_delete($tabella, $condizione = NULL)
	{
		$_res = "DELETE FROM $tabella";

		if (!is_null($condizione)) {
			$_res .= ' WHERE '.par($condizione);
		}
		return $_res;
	}

	/**
	 * Genera una query COUNT su una tabella
	 * @param string $tabella Nome della tabella
	 * @param string $condizione Eventuale clausola WHERE
	 * @param mixed $distinct se FALSE a' un count *, se diverso a' il campo di cui fare la distinct
	 * @return string
	 */ 
	public static function genera_count($tabella, $condizione = NULL, $distinct = FALSE)
	{
		$_res = 'SELECT COUNT('.
				($distinct ? "DISTINCT $distinct" : "*").
				') AS i_numero '.
				"FROM $tabella".
				(is_null($condizione) ? '' : ' WHERE '.$condizione);

		return $_res;
	}

	/**
	 * Genera una query EXISTS su una tabella
	 * @param string $tabella Nome della tabella
	 * @param string $condizione Eventuale clausola WHERE
	 * @return string
	 */ 
	public static function genera_exists($tabella, $condizione = NULL)
	{	
		// di base facciamo una count
		// poi se il db ha la clausola, fara' override di questa funzione
		$_where = (is_null($condizione) ? '' : 'WHERE '.$condizione);
		$_res = "SELECT COUNT(*) AS b_exists FROM $tabella $_where";

		return $_res;
	}

}



/**
 * Si occupa di mantenere un'unica istanza di driver, meta ed sql per ogni tipo di driver quando richiesto
 * @package db
 */
class DB_CLASS_FACTORY
{
	/**
	 * Istanze dei driver per tipo di dbms
	 * @var array
	 */ 
	private static $drivers = array();
	/**
	 * Istanze dei provider di meta-dati per tipo di dbms
	 * @var array
	 */ 
	private static $metas   = array();
	/**
	 * Istanze dei generatori sql per tipo di dbms
	 * @var array
	 */ 
	private static $sqls    = array();

	/**
	 * Ritorna il driver necessario per interagire con il dbms specificato nei parametri di connessione
	 * @param array $db_connection Parametri di connessione
	 * @return DB_DRIVER
	 */ 
	final public static function & get_driver($db_connection)
	{
		if (is_array($db_connection)) {
			check_db_connection($db_connection);
			$_db_type = $db_connection['DB_TYPE'];
		} else {
			$_db_type = $db_connection;
		}
		
		if (!array_key_exists($_db_type, self::$drivers)) {
			if (class_exists($_class = 'DB_DRIVER_'.$_db_type)) {
				self::$drivers[$_db_type] = new $_class;
				self::$drivers[$_db_type] -> set_driver('DB_DRIVER_'.$_db_type);
			} else {
				throw new FatalException("Driver per il dbms '$_db_type' non implementato.");
			}
		}

		return self::$drivers[$_db_type];
	}

	/**
	 * Ritorna il provider di meta-dati necessario per interagire con il dbms specificato nei parametri di connessione
	 * @param array $db_connection Parametri di connessione
	 * @return DB_META
	 */ 
	final public static function & get_meta($db_connection)
	{
		if (is_array($db_connection)) {
			check_db_connection($db_connection);
			$_db_type = $db_connection['DB_TYPE'];
		} else {
			$_db_type = $db_connection;
		}

		if (!array_key_exists($_db_type, self::$metas)) {
			if (class_exists($_class = 'DB_META_'.$_db_type)) {
				self::$metas[$_db_type] = new $_class;
			} else {
				throw new FatalException("Driver metadati per il dbms '$_db_type' non implementato.");
			}
		}

		if (is_array($db_connection) && array_key_exists('DB_NAME', $db_connection))
			self::$metas[$_db_type]->set_db($db_connection['DB_NAME']);

		return self::$metas[$_db_type];
	}

	/**
	 * Ritorna il generatore di sql necessario per interagire con il dbms specificato nei parametri di connessione
	 * @param array $db_connection Parametri di connessione
	 * @return DB_SQL
	 */ 
	final public static function & get_sql($db_connection)
	{
		if (is_array($db_connection)) {
			check_db_connection($db_connection);
			$_db_type = $db_connection['DB_TYPE'];
		} else {
			$_db_type = $db_connection;
		}

		if (!array_key_exists($_db_type, self::$sqls)) {
			if (class_exists($_class = 'DB_SQL_'.$_db_type)) {
				self::$sqls[$_db_type] = new $_class;
				self::$sqls[$_db_type] -> set_driver('DB_DRIVER_'.$_db_type);
				#self::$sqls[$_db_type] -> set_sql('DB_SQL_'.$_db_type);
			} else {
				throw new FatalException("Driver sql per il dbms '$_db_type' non implementato.");
			}
		}

		return self::$sqls[$_db_type];
	}
}

?>