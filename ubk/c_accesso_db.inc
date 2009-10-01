<?php 
/**
 * Classi di accesso alla base dati. Le classi implementano metodi consoni per interagire con una tabella o una vista.
 * @author Ubik <emiliano.leporati@gmail.com>
 * @package db
 */

/** 
 * Inclusione delle funzioni generali
 */ 
require_once('funzioni_generali.inc'); 

/**
 * Inclusione delle funzioni di manipolazione dei dati
 */ 
require_once('funzioni_db.inc');
require_once('funzioni_bwc.inc');

/** 
 * Inclusione delle funzioni di accesso ai dati
 */
require_once('c_db.inc');

foreach($GLOBALS['DRIVERS'] as $driver) {
	require_once("c_db_META_{$driver}.inc");
	require_once("c_db_DRIVER_{$driver}.inc");
	require_once("c_db_SQL_{$driver}.inc");
}

/** 
 * Inclusione funzioni di gestione cache
 */
require_once('c_cache.inc');

/**
 * Gestisce la connessione e fornisce i metodi di base per caricamento ed esecuzione
 * @package db
 */
class DB_BASE 
{
	public static $time = array('SQL' => 0, 'NUM' => 0, 'ALL' => array());
	/**
	 * Classe di interazione col db
	 * @var DB_DRIVER
	 */
	protected $driver = NULL;
	/**
	 * Classe che fornisce query sui metadati
	 * @var DB_META
	 */
	protected $meta = NULL;
	/**
	 * Classe di generazione sql
	 * @var DB_SQL
	 */
	protected $sql = NULL;
	/**
	 * Classe per la gestione della cache
	 * @var CACHE
	 */
	protected $cache = NULL;

	/**
	 * Informazioni per la connessione al db
	 * @var array
	 */
	protected $db_connection = array();

	/**
	 * Recordset con i dati originali (formato db) dell'ultima select
	 * @var array
	 */
	protected $rs = array();

	/**
	 * Vettore con l'elenco delle colonne dell'ultima select
	 * @var array
	 */
	public $campi = array();

	/**
	 * Array associativo con le lunghezze dei campi
	 * @var array
	 */
	public $lunghezza = array();

	/**
	 * Array associativo con i tipi fisici dei campi
	 * @var array
	 */
	public $tipo = array();

	/**
	 * Array associativo con i valori di default dei campi - solo per tabelle
	 * @var array
	 */
	public $default = array();

	/**
	 * Array associativo di flag che mi dicono se il campo e' nullable (non obbligatorio) o meno - solo per tabelle
	 * @var array
	 */
	public $nullable = array();

	/**
	 * Numero di righe presenti in {@link $rs}
	 * @var integer
	 */
	public $count = 0;
	
	/**
	 * Numero di righe ritornate dalla query (per la paginazione)
	 * @var integer
	 */
	public $count_reale = 0;

	/**
	 * Richiama {@link set_db_connection} e predispone l'oggetto di gestione cache. se il parametro e' NULL, viene usata la connessione di default (DB_DEFAULT)
	 * @param array $db_connection
	 */
	public function __construct($db_connection = NULL)
	{
		$this->set_db_connection( is_null($db_connection) ? array_get('DB_DEFAULT', $GLOBALS) : $db_connection );

		$this->cache =& CACHE_CLASS_FACTORY::get();
	}

	/**
	 * Predispone le variabili membro {@link db_connection, driver, meta} per connettersi al db specificato
	 * @param array $db_connection
	 */
	public function set_db_connection($db_connection)
	{
		check_db_connection($db_connection);

		$this->db_connection = $db_connection;

		$this->driver =& DB_CLASS_FACTORY::get_driver($this->db_connection);
		$this->meta   =& DB_CLASS_FACTORY::get_meta($this->db_connection);
		$this->sql    =& DB_CLASS_FACTORY::get_sql($this->db_connection);
	
	}

	/**
	 * Inizia la transazione. 
	 */
	public function tr_begin()
	{
		$this->apri_connessione();
		$this->driver->tr_begin();
	}

	/**
	 * Fa il commit della transazione corrente
	 */
	public function tr_commit()
	{
		$this->driver->tr_commit();
	}

	/**
	 * Fa il rollback della transazione corrente. 
	 */
	public function tr_rollback()
	{
		$this->driver->tr_rollback();
	}

	/**
	 * Si connette al database usando i parametri di connessione specificati {@link __construct, set_db_connection}
	 */
	protected function apri_connessione()
	{
		$this->driver->connect($this->db_connection);
	}

	/**
	 * Chiude la connessione al database
	 */
	protected function chiudi_connessione()
	{
		$this->driver->disconnect();
	}

	/**
	 * Riempie gli array {@link $campi $lunghezza $tipo} con le informazioni della query passata; non tocca {@link $default $nullable}
	 * @param resource_id $res_rs risorsa risultato di una dbms_query
	 * @param string $sql sql della query (facoltativo usato per alimentare e ricercare nella cache dei metadati)
	 */
	protected function carica_info_colonne($res_rs, $sql = null)
	{
		
		// se e' presente l'sql definisco un nome per la cache e serializzare i dati		
		if (!is_null($sql)) {
			$_table_info = $this->cache->mtd_get($this->db_connection, $sql);
		} else {
			$_table_info = NULL;
		}
		
		if (is_null($_table_info) && !is_null($sql)) {
			$_table_info['default'] = array();
			$_table_info['null'] = array();
			$_table_info['fields'] = array();
			$_table_info['length'] = array();
			$_table_info['type']   = array();
	
			for ($i = 0; $i < $this->driver->num_fields($res_rs); $i++) {
	
				$_info = $this->driver->field_info_rs($res_rs, $i);
	
				$_table_info['fields'][$i]             = $_info['name'];
				$_table_info['default'][$_info['name']]   = NULL;
				$_table_info['null'][$_info['name']]   = TRUE;
				$_table_info['type'][$_info['name']]   = $_info['type'];
				$_table_info['length'][$_info['name']] = calcola_lunghezza_campo($_info['name'], $_info['type'], $_info['length']);
			}
			
			$this->cache->mtd_write($this->db_connection, $sql, $_table_info);
		}

		$this->default   = $_table_info['default'];
		$this->nullable  = $_table_info['null'];
		$this->campi     = $_table_info['fields'];
		$this->lunghezza = $_table_info['length'];
		$this->tipo      = $_table_info['type'];
	}

	/**
	 * Riempie gli array {@link $default $nullable} con le informazioni della tabella passata
	 * @param string $table tabella di cui caricare le informazioni
	 */
	protected function carica_info_tabella($table_name)
	{
		$table = ((strpos($table_name, "(") !== FALSE)
				? substr($table_name, 0, strpos($table_name, "("))
				: $table_name
			);

		
		$_table_info = $this->cache->mtd_get($this->db_connection, $table);

		// non ho il file serializzato, chiedo al db
		if (is_null($_table_info)) {
			
			$this->apri_connessione();
			$_table_info = $this->driver->field_info_table($table_name);
			$this->chiudi_connessione();
			
			// aggiorna le lunghezza dei campi secondo le regole del ubk
			foreach($_table_info['length'] as $_field => $_len){
				$_table_info['length'][$_field] = calcola_lunghezza_campo($_field, $_table_info['type'][$_field], $_len);	
			}

			$this->cache->mtd_write($this->db_connection, $table, $_table_info);
		}

		$this->default   = $_table_info['default'];
		$this->nullable  = $_table_info['null'];
		$this->campi     = $_table_info['fields'];
		$this->lunghezza = $_table_info['length'];
		$this->tipo      = $_table_info['type'];

	}
	
	/**
	 * Esegue la query di select passata come argomento, caricando le informazioni in {@link $rs}
	 * @param mixed $sql Query SELECT da caricare
	 * @param bool $carica_info Indica se caricare o meno le meta-informazioni sulla query da eseguire
	 * @param string $campo_id specifica il campo id (che deve essere presente
	 * nella query), che viene utilizzato per indicizzare l'array. !Attenzione!
	 * vengono persi gli eventuali record che hanno lo stesso campo_id!!!
	 */
	protected function select($sql, $carica_info, $campo_id = NULL)
	{	
		if (!$campo_id && $this->cache->carica_sql($sql, $carica_info, $this)) {
			return;
		}

		if (constant_true('LOG_STATS')) $_start = get_microtime();
		
		$this->apri_connessione();

		$_res_rs = $this->driver->query($sql);

		if ($carica_info) $this->carica_info_colonne($_res_rs, $sql);

		// metto tutto nell'array membro
		$this->rs = array();
		while ($_riga = $this->driver->fetch_assoc($_res_rs)) {
			if ($campo_id) {
				$this->rs[$_riga[$campo_id]] = $_riga;
			} else {
				array_push($this->rs, $_riga);
			}
		}

		$this->count_reale = $this->count = count($this->rs);


		if (constant_true('LOG_STATS')) {
			$_end = get_microtime();
			self::$time['SQL'] += ($_end - $_start);
			self::$time['NUM'] ++;
			self::$time['ALL'][$sql] = ($_end - $_start);
		}

		// libero le risorse
		$this->driver->free_result($_res_rs);
		$this->chiudi_connessione();
		
		if (!$campo_id) {
			$this->cache->salva_sql($sql, $this);
		}
	}

	/**
	 * Ritorna il valore di default per il campo passato, o stringa vuota se non e' memorizzato
	 * @param string $nome_campo Nome del campo di cui ottenere il default
	 * @return mixed
	 */
	public function valore_default($campo)
	{
		return array_get_default($campo, $this->default, "");
	}

	/**
	 * Conta tutte le righe della tabella che verificano la condizione passata
	 * @param string $table Nome della tabella o vista da caricare
	 * @param string $condizione Condizione WHERE
	 * @param mixed $distinct Indica se inserire la clausola DISTINCT nel conteggio; se FALSE o equivalente non la inserisce, else indica cosa contare in modo distinto (elenco colonne, *, etc.)
	 * @return integer
	 */
	protected function conta_righe($table, $condizione = NULL, $distinct = FALSE)
	{
		if (constant_true('LOG_STATS')) $_start = get_microtime();

		$this->apri_connessione();

		$_sql_cnt = $this->sql->genera_count($table, $condizione, $distinct);
		$_res_rs = $this->driver->query($_sql_cnt);
		$_riga   = $this->driver->fetch_assoc($_res_rs);
		$_righe  = $_riga["i_numero"];

		$this->driver->free_result($_res_rs);

		if (constant_true('LOG_STATS')) {
			$_end = get_microtime();
			self::$time['SQL'] += ($_end - $_start);
			self::$time['NUM'] ++;
			self::$time['ALL'][$_sql_cnt] = ($_end - $_start);
		}
		
		$this->chiudi_connessione();

		return $_righe;
	}

	/**
	 * Dice se esistono righe nella tabella che verificano la condizione passata
	 * @param string $table Nome della tabella o vista da caricare
	 * @param string $condizione Condizione WHERE 
	 * @return bool
	 */
	protected function esistono_righe($table, $condizione = NULL)
	{
		if (constant_true('LOG_STATS')) $_start = get_microtime();

		$this->apri_connessione();

		$_sql    = $this->sql->genera_exists($table, $condizione);
		$_res_rs = $this->driver->query($_sql);
		$_riga   = $this->driver->fetch_assoc($_res_rs);
		$_result = $_riga["b_exists"];

		$this->driver->free_result($_res_rs);

		if (constant_true('LOG_STATS')) {
			$_end = get_microtime();
			self::$time['SQL'] += ($_end - $_start);
			self::$time['NUM'] ++;
			self::$time['ALL'][$_sql] = ($_end - $_start);
		}
		
		$this->chiudi_connessione();

		return call_user_func(array($this->sql, "db_2_utente"), "b_exists", $_result);
	}

	/**
	 * Carica la variabile {@link $rs} con una query SELECT costruita in base ai parametri passati; ritorna il numero di pagine effettivamente caricate (se la pagina specificata e' maggiore del numero di pagine possibili, viene decrementato il suo valore)
	 * @param string $table Nome della tabella o vista da caricare
	 * @param mixed $colonne Opzionale, dice quali colonne devono essere caricate, tutte se omesso o NULL
	 * @param string $condizione Opzionale, condizioni della clausola WHERE
	 * @param string $ordine Opzionale, condizioni della clausola ORDER BY
	 * @param bool $distinct Indica se inserire la clausola DISTINCT nel caricamento
	 * @param integer $pagina Per caricamenti paginati, la pagina da caricare
	 * @param integer $dim_pagina Per caricamenti paginati, la dimensione delle pagine
	 * @param boolean $conta Dice se eseguire o meno in conteggio dei record
	 * @param mixed $usa_cache Dice se usare o meno la cache. Se FALSE non la usa, se !FALSE e' una stringa tabella.campo_id usata per capire se si puo' o meno
	 * @return integer
	 */
	protected function carica_righe($table, $colonne = NULL, $condizione = NULL, $ordine = NULL, $distinct = FALSE, 
							$pagina = NULL, $dim_pagina = NULL, 
							$conta = TRUE, $usa_cache = FALSE)
	{
		list($usa_cache, $valore_id) = $this->cache->validate($usa_cache, $table, NULL, $condizione);

		if ($usa_cache) {
			$this->carica_info_tabella($table);
			$this->rs = $this->cache->carica_rs($table, $campo_id, $valore_id);
			$this->count_reale = $this->count = count($this->rs);
			return 0;
		}

		// se nn specifico colonne, carico i metadati
		if (is_null($colonne)) {
			$this->carica_info_tabella($table);
			$_colonne = implode(", ", $this->campi);
		// se sono specificate e sono un array, allora metto le colonne specificate o * se l'array e' vuoto
		} elseif (is_array($colonne)) {
			$_colonne = (count($colonne) > 0 ? implode(", ", $colonne) : "*");
		// se mi arriva una stringa, uso quella		
		} else {
			$_colonne = $colonne;
		}

		if (is_array($ordine)) {
			$ordine = implode(', ', $ordine);
		}

			
		$_sql = $this->sql->genera_select($table, $_colonne, $condizione, $ordine, $distinct, $pagina, $dim_pagina);

		if ($distinct) {

			if (constant_true('LOG_STATS')) $_start = get_microtime();

			$this->apri_connessione();  

			$_sql_cnt = $this->sql->genera_select($table, $_colonne, $condizione, $ordine, $distinct);
			$_rs      = $this->driver->query($_sql_cnt);
			$_count   = $this->driver->num_rows_select();
			$this->driver->free_result($_rs);

			$this->chiudi_connessione();

			if (constant_true('LOG_STATS')) {
				$_end = get_microtime();
				self::$time['SQL'] += ($_end - $_start);
				self::$time['NUM'] ++;
				self::$time['ALL'][$_sql_cnt] = ($_end - $_start);
			}

		} elseif (is_null($pagina) || is_null($dim_pagina) || !$dim_pagina) {
			$_count = TRUE;
		} elseif($conta) {
			$_count = $this->conta_righe($table, $condizione, FALSE);
		} else {
			$_count = null;
		}

		$this->select($_sql, FALSE);

		if ($_count === TRUE) {
			$this->count_reale = count($this->rs);
		} else {
			$this->count_reale = $_count;
		}

		// potrei aver richiesto una pagina che sborda rispetto al set corrente di dati (es a seguito di una delete sull'ultimo record di una pagina)
		// quindi decremento il numero di pagina finche' non inglobo al pelo i dati caricati
		if (count($this->rs) == 0 && $pagina > 0) {
			return $this->carica_righe($table, $colonne, $condizione, $ordine, $distinct, $pagina - 1, $dim_pagina, $conta, $usa_cache);
		} else {
			return $pagina;
		}
	}
	
	

	/**
	 * Esegue una query generica che non necessita di ritorno di righe (INSERT, UPDATE, DELETE, etc.)
	 * @param string $sql La query (INSERT, UPDATE, DELETE) da eseguire
	 */
	public function esegui($sql)
	{
		$this->apri_connessione();

		$_res_rs = $this->driver->query($sql);
		$this->driver->free_result($_res_rs);

		$this->chiudi_connessione();
	}

	/**
	 * Ritorna il valore dell'ultimo ID ad incremento automatico generato nella tabella specificata
	 * @param string $tabella
	 * @return long
	 */
	protected function ultimo_id_tabella($tabella)
	{
		$this->apri_connessione();

		$_res_rs = $this->driver->identity($tabella);
		$_riga = $this->driver->fetch_assoc($_res_rs);
		$this->driver->free_result($_res_rs);

		$this->chiudi_connessione();

		return $_riga["i_numero"];
	}
	
		
	/**
	 * Esegue una stored procedure sul database
	 * @param string $procedure nome della stored procedure da chiamare
	 * @param mixed $parametro (quanti ne servono)
	 */
	public function esegui_stored_procedure()
	{
		$_params = func_get_args();
		$_fmtd_values = call_user_func_array(array($this, 'parametri_stored_procedure'), $_params);

		if (constant_true('LOG_STATS')) $_start = get_microtime();

		$this->apri_connessione();
		$this->driver->exec_procedure( $_params[0], $_fmtd_values);
		$this->chiudi_connessione();

		if (constant_true('LOG_STATS')) {
			$_end = get_microtime();
			self::$time['SQL'] += ($_end - $_start);
			self::$time['NUM']  ++;
			self::$time['ALL'][$_params[0].par(implode(', ', $_fmtd_values))] = ($_end - $_start);
		}
	}

	/**
	 * Ritorna i parametri formattati in modo consono (es. per caricamenti)
	 * @param string $procedure nome della stored procedure
	 * @param mixed $parametro (quanti ne servono)
	 * @return array
	 */
	public function parametri_stored_procedure()
	{
		$_params = func_get_args();
		$_procedure = $_params[0];
		$_params = array_slice($_params, 1);

		$_procedure_info = $this->cache->mtd_get($this->db_connection, $_procedure);


		if (is_null($_procedure_info)) {

			$this->apri_connessione();  
			$_procedure_info = $this->driver->field_info_procedure($_procedure);
			$this->chiudi_connessione();  

			$this->cache->mtd_write($this->db_connection, $_procedure, $_procedure_info);
		}

		$_fmtd_values = array();

		if (count($_params) != count($_procedure_info['params']))
				throw new CodeException("Numero di parametri errati per la stored '$_procedure'");

		for($i = 0; $i < count($_params); $i ++) {

			array_push($_fmtd_values, $this->sql->utente_2_db($_procedure_info['params'][$i], $_params[$i]));

		}

		return $_fmtd_values;
	}

	public function getRS($fmt = NULL)
	{
		if (is_null($fmt)) {
			return $this->rs;
		} else {
			$data = $this->rs;
			for($i = 0; $i < count($data); $i ++) {
				foreach($this->campi as $campo) {
					$data[$i][$campo] = call_user_func(array($this->driver, $fmt), $campo, $data[$i][$campo]);
				}
			}
			return $data;
		}

	}

	public function setRS(&$rs)
	{
		$this->rs = $rs;
	}
}


/** 
 * Fornisce le funzionalita' di spostamento fra righe con impostazione di EOF e BOF, nonche' la restituzione dei valori in formato utente
 * @package db
 */
class RECORDSET extends DB_BASE
{
	/**
	 * Fine del recordset
	 * @var bool
	 */
	public $eof 	= TRUE;
	/**
	 * Inizio del recordset
	 * @var bool
	 */
	public $bof 	= TRUE;
	/**
	 * Riga corrente nel recordset
	 * @var integer
	 */
	public $index 	= -1;
	/**
	 * Stack di storico delle "epurazioni" di record fatte tramite il metodo slice. Viene usato per ripristinare i recordset con chiamate a restore.
	 * @var integer
	 */
	private $slice_stack = array();
	/**
	 * Query identificativa del recordset
	 * @var string
	 */
	private $recordset_query = NULL;
	
	/**
	 * Esegue e carica direttamente la query SELECT passata
	 * @param string $sql Query SELECT da caricare
	 * @param array $db_connection Connessione su cui operare
	 */
	public function __construct($sql, $db_connection = NULL)
	{
		DB_BASE::__construct($db_connection);
		DB_BASE::select($this->recordset_query = $sql, TRUE);
		$this->move_first();
	}

	/**
	 * Esegue e carica direttamente la query SELECT passata
	 * @param string $sql Query SELECT da caricare
	 * @param array $db_connection Connessione su cui operare
	 */
	public function RECORDSET($sql, $db_connection = NULL)
	{
		DB_BASE::set_db_connection( is_null($db_connection) ? array_get('DB_DEFAULT', $GLOBALS) : $db_connection );
		DB_BASE::select($sql, TRUE);
		$this->move_first();
	}

	/**
	 * Ri-esegue la query SELECT con cui e' stato creato
	 */
	public function carica()
	{
		DB_BASE::select($this->recordset_query, TRUE);
		$this->move_first();
	}

	/**
	 * Si sposta sul primo record
	 */
	public function move_first()
	{
		if ($this->count != 0) {
			$this->eof   = FALSE;
			$this->bof   = FALSE;
			$this->index = 0;
		} else {
			$this->eof   = TRUE;
			$this->bof   = TRUE;
			$this->index = -1;	
		}
	}
	
	/**
	 * Si sposta sul record successivo
	 */
	public function move_next()
	{
		if (!$this->eof) {
			$this->index ++;
			$this->bof = FALSE;
			if ($this->index > ($this->count - 1)) {
				$this->eof = TRUE;
			}
		}
	}
	
	/**
	 * Si sposta sull'ultimo record
	 */
	public function move_last()
	{
		if ($this->count) {
			$this->bof   = FALSE;
			$this->eof   = FALSE;
			$this->index = $this->count - 1;
		} else {
			$this->eof   = TRUE;
			$this->bof   = TRUE;
			$this->index = 0;
		}
	}
	
	/**
	 * Si sposta sul record precedente
	 */
	public function move_previous()
	{
		if (!$this->bof) {
			$this->index --;
			$this->eof = FALSE;
			if ($this->index < 0) {
				$this->bof = TRUE;
			}
		}
	}
	
	/**
	 * Si sposta sulla riga indicata
	 * @param integer $riga riga in cui posizionarsi
	 */
	public function move($riga)
	{
		if ($riga >= 0 and $riga < $this->count) {
			if ($riga == 0) {
				$this->move_first();
			} elseif ($riga == ($this->count - 1)) {
				$this->move_last();
			} else {
				$this->eof   = FALSE;
				$this->bof   = FALSE;
				$this->index = $riga;				
			}
		} else {
			throw new CodeException("Indice $riga oltre i limiti [0, ".($this->count - 1)."]");
		}
	}

	public function move_to_value($field, $value)
	{
		$values = $this->valori_colonna($field);
		$pos = array_search($value, $values);

		if ($pos === FALSE) {
			$this->move_last();
			$this->move_next();
		} else {
			$this->move($pos);
		}
	}

	/**
	 * Si sposta sulla riga indicata via offset rispetto a quella corrente
	 * @param integer $offset spostamento rispetto alla riga corrente
	 */
	public function move_rel($offset)
	{
		$this->move($this->index + $offset);
	}

	private function valore_generico($campo, $formato)
	{
		if ($this->eof || $this->bof) {
			throw new DbException(__CLASS__.'::'.$formato, "RECORDSET oltre i limiti o non caricato (EOF o BOF).");
		} elseif (!array_key_exists($campo, $this->rs[$this->index])) {
			throw new CodeException("Il campo '$campo' non esiste nella tabella ".$this->tabella);
		} elseif (is_null($formato)) {
			return $this->rs[$this->index][$campo];
		} else {
			return call_user_func(array($this->sql, $formato), $campo, $this->rs[$this->index][$campo]);
		}
	}

	/**
	 * Come valore in sintassi $o->campo
	 * @param string $campo Nome del campo di cui ritornare il valore
	 * @return mixed
	 */
	public function __get($campo)
	{
		return $this->valore_generico($campo, "db_2_utente");
	}

	/**
	 * Ritorna il valore del campo specificato in formato utente
	 * @param string $campo Nome del campo di cui ritornare il valore
	 * @return mixed
	 */
	public function valore($campo)
	{
		return $this->valore_generico($campo, "db_2_utente");
	}

	/**
	 * Ritorna il valore del campo specificato in formato fwk (vedere {@link db_2_fwk})
	 * @param string $campo Nome del campo di cui ritornare il valore
	 * @return mixed
	 */
	public function valore_fwk($campo)
	{
		return $this->valore_generico($campo, "db_2_fwk");
	}
	
	/**
	 * Ritorna il valore del campo specificato in formato ASCII (vedere {@link db_2_ascii})
	 * @param string $campo Nome del campo di cui ritornare il valore
	 * @return mixed
	 */
	public function valore_ascii($campo)
	{
		return $this->valore_generico($campo, "db_2_ascii");
	}

	/**
	 * Ritorna il valore del campo specificato come appare sul db
	 * @param string $campo Nome del campo di cui ritornare il valore
	 * @return mixed
	 */
	public function valore_db($campo)
	{
		return $this->valore_generico($campo, NULL);
	}

	/**
	 * Ritorna tutti i valori attualmente caricati della / delle colonne specificate in un array associativo (colonna, array valori)
	 * @param mixed $colonne Puo' essere un array o una stringa singola
	 * @return array
	 */
	public function valori()
	{
		$colonne = func_get_args();

		if (!count($colonne))
			$colonne = $this->campi;
		elseif (is_array($colonne[0]))
			$colonne = $colonne[0];
		else
			$colonne = array_intersect($colonne, $this->campi);

		$_result = array();
		$_pos    = $this->index;

		// inizializzo gli array risultanti
		for ($i = 0; $i < count($colonne); $i++ ) {
			$_result[$colonne[$i]] = array();
		}
		// per ogni riga, per ogni colonna accodo il valore
		for($this->move_first(); !$this->eof; $this->move_next()) {
			for ($i = 0; $i < count($colonne); $i++ ) {
				array_push($_result[$colonne[$i]], $this->valore($colonne[$i]));
			}
		}
		

		if ($this->count >= 0 && $_pos >= 0 && $_pos < $this->count) $this->move($_pos);

		return $_result;
	}

	/**
	 * Ritorna tutti i valori attualmente caricati della colonna specificata in un array
	 * @param string $colonna 
	 * @return array
	 */
	public function valori_colonna($colonna)
	{
		$risultato = $this->valori($colonna);
		return $risultato[$colonna];
	}

	/**
	 * Ritorna tutti i valori attualmente caricati nella riga indicata (quella corrente se omessa)
	 * @param integer $riga La riga da ritornare
	 * @return array
	 */
	public function valori_riga($riga = NULL)
	{
		if ($this->eof || $this->bof) {
			throw new DbException(__CLASS__.'::'.__METHOD__, "RECORDSET oltre i limiti o non caricato (EOF o BOF).");
		}
	
		if (is_null($riga)) $riga = $this->index;
		if ($riga < 0 || $riga >= $this->count) {
			throw new CodeException("Indice $riga oltre i limiti [0, ".($this->count)."]");
		}

		return $this->rs[$riga];
	}

	/**
	 * Ritorna la somma di tutti valori presenti nella colonna indicata, 0 se la colonna non e' numerica (f o i)
	 * @param string $colonna 
	 * @return float
	 */
	public function somma_colonna($colonna)
	{
		switch ($colonna[0]) {
		case "f":
		case "i":
			for($_riga = 0, $_result = 0; $_riga < $this->count; $_riga ++) {
				$_result += $this->rs[$_riga][$colonna];
			}
			return $_result;
			break;
		default:
			return 0;
		}
	}

	/**
	 * Imposta il valore del campo nella riga corrente
	 * @param string $campo Nome del campo di cui ritornare il valore
	 * @param mixed $valore Valore da assegnare
	 */
	public function imposta_valore($campo, $valore)
	{
		if ($this->eof || $this->bof) {
			throw new DbException(__CLASS__.'::'.__METHOD__, "RECORDSET oltre i limiti o non caricato (EOF o BOF).");
		}

		$this->rs[$this->index][$campo] = $valore;
	}

	/**
	 * Modifica il recordset sostituendolo con una sua parte, da $start a $end. 
	 * Viene tenuto uno stack in modo da poter ritornare indietro (restore)
	 * @param integer $start Inizio del taglio
	 * @param integer $end Fine del taglio
	 */
	public function slice($start, $end = NULL)
	{
		array_push($this->slice_stack, $this->rs);

		if (!is_null($end)) {
			$this->rs = array_slice($this->rs, $start, $end);
		} else {
			$this->rs = array_slice($this->rs, $start);
		}
		$this->count = count($this->rs);

		if ($this->index >= $this->count - 1)
			$this->move_last();
	}

	/**
	 * Ritorna al recordset immediatamente precedente all'ultimo taglio
	 */
	public function restore()
	{
		if (count($this->slice_stack))
			$this->rs = array_pop($this->slice_stack);
	}

}



/** 
 * Gestisce la manipolazione dati su una tabella specifica, facendo da interfaccia tra la pagina HTML e il database
 * @package db
 */
class GESTORE extends RECORDSET
{
	/**
	 * Tabella gestita
	 * @var string
	 */
	public $tabella 	= "";
	/**
	 * Campo id della tabella (di default i_nome_tabella_id)
	 * @var string
	 */
	public $campo_id 	= "";
	/**
	 * Pagina caricata / da caricare
	 * @var string
	 */
	public $pagina     = NULL;
	/**
	 * Dimensione delle pagine
	 * @var string
	 */
	public $dim_pagina = NULL;
	
	/**
	 * Utilizzo cache
	 * @var string
	 */
	private $usa_cache = FALSE;
	
	/**
	 * Carica le informazioni sulla tabella specificata
	 * @param string $tabella
	 * @param string $campo_id Opzionale
	 * @param bool $usa_cache Opzionale, attiva la cache per i risultati del gestore
	 * @param array $db_connection Connessione su cui operare
	 */
	public function __construct($tabella, $campo_id = NULL, $usa_cache = FALSE, $db_connection = NULL)
	{
		DB_BASE::__construct($db_connection);

		$this->tabella  = $tabella;
		$this->campo_id = is_null($campo_id) ? campo_id_tabella($tabella) : $campo_id;
		$this->carica_info_tabella($tabella);
		$this->usa_cache = ($usa_cache == TRUE ) ? $tabella.'.'.$this->campo_id : FALSE;
	}


	/**
	 * Backward compatibility
	 * @param string $tabella
	 * @param string $campo_id Opzionale
	 * @param bool $usa_cache Opzionale, attiva la cache per i risultati del gestore
	 * @param array $db_connection Connessione su cui operare
	 */
	public function GESTORE($tabella, $campo_id = NULL, $usa_cache = FALSE, $db_connection = NULL)
	{
		DB_BASE::set_db_connection( is_null($db_connection) ? array_get('DB_DEFAULT', $GLOBALS) : $db_connection );

		$this->tabella  = $tabella;
		$this->campo_id = is_null($campo_id) ? campo_id_tabella($tabella) : $campo_id;
		$this->carica_info_tabella($tabella);
		$this->usa_cache = ($usa_cache == TRUE ) ? $tabella.'.'.$this->campo_id : FALSE;
	}

	/**
	 * Abilita la cache per il gestore
	 * @param bool $usa_cache Opzionale, attiva la cache per i risultati del
	 * gestore
	 */
	public function abilita_cache($usa_cache = TRUE)
	{
		$this->usa_cache = ($usa_cache == TRUE ) ? $this->tabella.'.'.$this->campo_id : FALSE;
	}



	/**
	 * Esegue un caricamento
	 * @param string $condizione Condizione WHERE
	 * @param string $ordine Opzionale, campi da ordinare (argomento della ORDER BY)
	 * @param array $colonne Opzionale, colonne da reperire - se omesso, vengono prese tutte le colonne della tabella gestita
	 * @param bool $distinct Indica se inserire la clausola DISTINCT nel caricamento
	 * @param bool $conta Indica se eseguire la conta prima di fare il
	 * caricamento
	 */
	public function carica($condizione = NULL, $ordine = NULL, $colonne = NULL, $distinct = FALSE, $conta = TRUE)
	{
		// impostando sempre le colonne, evito che la carica faccia il fetch delle informazioni
		if (is_null($colonne)) $colonne = $this->campi;
		$this->pagina = DB_BASE::carica_righe($this->tabella, $colonne, $condizione, $ordine, $distinct, $this->pagina, $this->dim_pagina, $conta, $this->usa_cache);
		$this->move_first();
	}

	/**
	 * Esegue un caricamento delle righe il cui valore di {@link $campo_id} corrisponde al valore passato
	 * @param mixed $valore_id
	 * @param string $ordine Opzionale, campi da ordinare (argomento della ORDER BY)
	 * @param array $colonne Opzionale, colonne da reperire - se omesso, vengono prese tutte le colonne della tabella gestita
	 * @param bool $distinct Indica se inserire la clausola DISTINCT nel caricamento
	 */
	public function carica_righe_id($valore_id, $ordine = NULL, $colonne = NULL, $distinct = FALSE, $conta = TRUE)
	{
		$this->carica($this->sql->equ($this->campo_id, $valore_id), $ordine, $colonne, $distinct, $conta);
	}

	/**
	 * Esegue un aggiornamento della riga il cui valore di {@link $campo_id} corrisponde al valore passato, con i valori specificati
	 * @param mixed $id_riga
	 * @param array $valori Matrice del tipo (id_riga, nome_colonna) => valore contenente dati, tipicamente provenienti dalla pagina ($_POST)
	 */
	public function aggiorna($id_riga, $valori)
	{
		DB_BASE::esegui(
				$this->sql->genera_update(
					$this->tabella, 
					prendi_riga($id_riga, $valori), 
					$this->campi, 
					$this->sql->equ($this->campo_id, $id_riga)
				)
		);
		$this->cache->azzera($this->tabella);
	}


	/**
	 * Esegue un aggiornamento delle riga rispondenti al criterio passato con i valori specificati
	 * @param array $associazioni Matrice del tipo nome_colonna => valore contenente i dati da aggiornare
	 * @param string $condizione Condizione WHERE 
	 */
	public function aggiorna_dove($associazioni, $condizione)
	{
		DB_BASE::esegui(
				$this->sql->genera_update(
					$this->tabella, 
					$associazioni, 
					$this->campi,
					$condizione
				)
			);
		
		$this->cache->azzera($this->tabella);
	}

	/**
	 * Esegue un inserimento con i valori specificati (riga -1)
	 * @param array $valori Matrice del tipo (id_riga, nome_colonna) => valore contenente dati, tipicamente provenienti dalla pagina ($_POST)
	 */
	public function crea($valori, $id = '-1')
	{
		DB_BASE::esegui(
				$this->sql->genera_insert(
					$this->tabella, 
					prendi_riga($id, $valori), 
					$this->campi
				)
			);
		
		$this->cache->azzera($this->tabella);
	}

	/**
	 * Elimina le righe il cui valore di {@link $campo_id} corrisponde al valore passato, con i valori specificati
	 * @param mixed $id_riga
	 */
	public function elimina($id_riga)
	{
		DB_BASE::esegui(
				$this->sql->genera_delete(
					$this->tabella,
					$this->sql->equ($this->campo_id, $id_riga)
				)
			);
		
		$this->cache->azzera($this->tabella);
	}

	/**
	 * Elimina le righe rispondenti al criterio passato
	 * @param string $condizione Condizione WHERE 
	 */
	public function elimina_dove($condizione)
	{
		
		DB_BASE::esegui(
				$this->sql->genera_delete(
					$this->tabella,
					$condizione
				)
			);

		$this->cache->azzera($this->tabella);
	}

	/**
	 * Conta tutte le righe della tabella che verificano la condizione passata
	 * @param string $condizione Condizione WHERE 
	 * @param mixed $distinct Indica se inserire la clausola DISTINCT nel conteggio; se FALSE o equivalente non la inserisce, else indica cosa contare in modo distinto (elenco colonne, *, etc.)
	 * @return integer
	 */
	public function conta($condizione = NULL, $distinct = FALSE)
	{
		return DB_BASE::conta_righe($this->tabella, $condizione, $distinct);
	}

	/**
	 * Dice se esistono righe della tabella che verificano la condizione passata
	 * @param string $condizione Condizione WHERE 
	 * @return bool
	 */
	public function esiste($condizione = NULL)
	{
		return DB_BASE::esistono_righe($this->tabella, $condizione);
	}

	/**
	 * "Decodifica" un valore in un altro nella stessa tabella: ritorna il valore di {@link $campo_out} legato al {@link $valore} di {@link $campo_in} nella tabella
	 * @param mixed $valore Valore da cercare
	 * @param string $campo_out Campo di cui ritornare il valore
	 * @param string $campo_in Opzionale - campo di cui cercare {@link $valore}, se omesso viene usato {@link $campo_id}
	 * @param mixed $ret_on_eof Se false solleva un errore nel caso la decode fallisca, altrimenti ritorna quel valore
	 * @return mixed
	 */
	public function decode($valore, $campo_out, $campo_in = NULL, $ret_on_eof = FALSE)
	{
		if (is_null($campo_in)) $campo_in = $this->campo_id;
		$_rs = new GESTORE($this->tabella, $this->campo_id);
		$_rs -> carica($this->sql->equ($campo_in, $valore), NULL, array($campo_out), FALSE, FALSE);
		if ($_rs->eof) {
			unset($_rs);
			if ($ret_on_eof !== FALSE) {
				return $ret_on_eof;
			} else {
				throw new CodeException("Valore $valore non trovato in {$this->tabella}.$campo_in");
			}
		}
		
		$_out = $_rs->valore($campo_out);
		unSet($_rs);
		return $_out;
	}

	/**
	 * Ritorna i valori dei campi specificati come provenissero dalla pagina ($_POST) per eseguire una query (crea o aggiorna) sulla riga specificata
	 * @param array $campi Campi per cui ritornare i valori
	 * @param string $id Riga su cui bisognera' operare
	 * @return array
	 */
	public function valori_per_query($campi, $id = "-1")
	{
		$valori = array();
		foreach($campi as $campo) {
			$valori[$campo] = array($id => $this->valore_fwk($campo));
		}
		return $valori;
	}

	/**
	 * Ritorna il valore dell'ultimo ID ad incremento automatico generato nella tabella gestita
	 * @return integer
	 */
	public function ultimo_id()
	{
		return DB_BASE::ultimo_id_tabella($this->tabella);
	}

}

?>