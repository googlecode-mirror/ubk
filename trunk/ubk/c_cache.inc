<?php
/**
 * Gestisce la cache dai dati a db
 *  
 * @package general
 * @author Ubik <emiliano.leporati@gmail.com>
 */

class CACHE 
{
	// TIMINGS
	public static $time = array('CACHE' => 0);

	// IMMAGINI
	private $img_data = array();

	public function img_get($name)
	{
		if (!isset($this->img_data[$name])) {
			$this->img_data[$name] = xsl_transform("<gfx:img xmlns:gfx=\"".NS_GFX."\" nome=\"$name\"/>", true);
		} 
		return $this->img_data[$name];
	}


	// METADATI

	private $mtd_cache = FALSE;
	private $mtd_dir = '';
	private $mtd_data = array();
	private $sql = array();

	public function __construct()
	{
		$this->mtd_cache = constant_true('CACHE_METADATI');
		$this->mtd_dir = path(PHDIR, 'system/cache/metadata');

		$this->db_cache = constant_true('CACHE_DB');
		$this->db_dir = path(PHDIR, 'system/cache/query');

		$this->reg_file = path(PHDIR,'system/cache/registry.ser');
		$this->reg_lock = path(PHDIR,'system/cache/cache.lock');

		$this->xsl_cache = constant_true('CACHE_XSL');

		$this->page_cache = constant_true('CACHE_XML');
		$this->page_dir = path(PHDIR, 'cache/xml');
	}

	public function mtd_fname($signature)
	{
		return path($this->mtd_dir, $signature.'.obj');
	}

	public function mtd_signature($db_connection, $sql)
	{
		if (($p = stripos($sql,' where ')) !== FALSE) {
			$sql = substr($sql, 0, $p);
		}
		return md5($db_connection['DB_SERVER'].$db_connection['DB_NAME'].$sql);
	}

	public function mtd_write($db_connection, $sql, $info)
	{
		$sig = $this->mtd_signature($db_connection, $sql);
		
		$this->mtd_data[$sig] = $info;

		if ($this->mtd_cache) {
			scrivi_file($this->mtd_fname($sig), serialize($info));
		}
	}

	public function mtd_get($db_connection, $sql)
	{
		$sig = $this->mtd_signature($db_connection, $sql);

		if (isset($this->mtd_data[$sig])) {
			return $this->mtd_data[$sig];

		} elseif ($this->mtd_cache) {
			if (file_exists($this->mtd_fname($sig))) { 
				$this->mtd_data[$sig] = unserialize(file_get_contents($this->mtd_fname($sig)));
				return $this->mtd_data[$sig];
			}
		
		} else {
			return NULL;
		}
	}

	// PAGINE
	private $page_cache = FALSE;
	private $page_dir = '';

	public function page_name($filename)
	{
		return path($this->page_dir, str_replace('/','-',$filename).'.obj');
	}

	public function page_write($filename, &$DOM)
	{
		if ($this->page_cache) {
			scrivi_file($this->page_name($filename), serialize($this->pila[0]));
		}
	}

	public function page_get($filename)
	{
		if ($this->page_cache && file_exists($cache = $this->page_name($filename))) {
			return unserialize(file_get_contents($cache));
		} else {
			return NULL;
		}

	}

	// XSL
	private $xsl_cache = FALSE;

	public function xsl_get()
	{
		return array_get_default('CACHE_XSL', $_SESSION);
	}

	public function xsl_write($xsl)
	{
		if ($this->xsl_cache && !isset($_SESSION['CACHE_XSL']))
			$_SESSION['CACHE_XSL'] = $xsl;
	}

	// DB
	private $registry_update = array();
	private $registry_cache = array();
	private $reg_file = '';
	private $reg_lock = '';
	private $reg_shutdown = FALSE;

	
	private $db_cache = FALSE;
	private $db_data = array();
	private $db_dir = '';
	private $db_range = 30;

	/**
	 * Invalida la cache di una tabella, normalmente viene chiamato dalle funzioni
	 * di aggiornamento (update, add, delete).
	 * 
	 * @param string $nome_tabella nome della tabella modificata
	 */
	public function azzera($nome_tabella)
	{
		if (!$this->db_cache) return false;

		$this->reg_cache[$nome_tabella] = 0;
		$this->reg_update[$nome_tabella] = true;
		
		// aggiorna il registro a chiusura della pagina
		if (!$this->reg_shutdown) {
			$this->reg_shutdown = TRUE;
			register_shutdown_function(array($this, 'scrivi_registry'));
		}
	}



	/**
	 * Carica i dati dalla cache di una tabella, se vecchia o non presente ricrea la
	 * cache. 
	 * 
	 * @param string $nome_tabella nome della tabella
	 * @param string $campo_id nome del campo su cui viene fatta la ricerca
	 * @param string $valore_id valore cercato
	 * 
	 * @return array l'insieme dei record cercati, o null.
	 */
	public function & carica_rs($nome_tabella, $campo_id, $valore_id)
	{
		if (constant_true('LOG_STATS')) $_start = get_microtime();
		
		if (!is_numeric($valore_id))
			$valore_id = substr($valore_id, 1, -1);

		$file_name = $this->nome_file($nome_tabella, $campo_id, $valore_id);

		// dati gia' caricati
		if (isset($this->db_data[$file_name])){
			$data =& $this->db_data[$file_name];

		// dati da caricare
		} else { 
			$file_path = path($this->db_dir,$file_name);
			$file_mtime = filemtime($file_path);
			// carico il file da disco	
			if ($file_mtime && isset($this->reg_cache[$nome_tabella]) && ($file_mtime >= $this->reg_cache[$nome_tabella])) {
				log_value("CACHE: tabella $nome_tabella ok (mtime $file_mtime ) <= ");
				$data = unserialize(file_get_contents($file_path));
			} else {
				if (!$file_mtime) log_value("CACHE: carico da db $file_name non esistente");
				elseif (!isset($this->reg_cache[$nome_tabella])) log_value("CACHE: carico da db $file_name non presente in cache");
				elseif (!($file_mtime >= $this->reg_cache[$nome_tabella])) 
					log_value("CACHE: carico da db $file_name percha' piu' aggiornato $file_mtime > " . $this->reg_cache[$nome_tabella] .
						' - ' . date("h:i:s d/m/Y",$file_mtime) . ' > ' . date("h:i:s d/m/Y",$this->reg_cache[$nome_tabella]));
			}
		}
		
		// file non presente o scaduto, devo crearlo
		if (!isset($data)) {
			if (is_numeric($valore_id)) {
				$min = $valore_id  - ($valore_id % $this->db_range); 
				$max = $min + $this->db_range;
				$cond = "where ".c_and(gte($campo_id, $min), lt($campo_id, $max));
			} else {
				$cond = "";
			}
			
			$sql = "select * from $nome_tabella $cond";
			$tmp_rs = new RECORDSET($sql);
			$data = $tmp_rs->getRS();
			unset($tmp_rs);
			
			// aggiorno il file su disco
			scrivi_file($file_path,serialize($data));
			$this->db_data[$file_name] = &$data;
			
			if (!isset($this->reg_cache[$nome_tabella])) {
				if (!isset($this->reg_update[$nome_tabella])) {
					$this->reg_update[$nome_tabella] = true;
					log_value("CACHE: update [$nome_tabella]");
				}
					
				if	(!$this->reg_shutdown){
					$this->reg_shutdown = TRUE;
					register_shutdown_function(array($this, 'scrivi_registry'));
				}
			}
		}
		
		
		$rs = array();
		if (isset($data[$valore_id])) {
			log_value("CACHE: id richiesto $valore_id trovato");
			$rs[] = $data[$valore_id];
		}

		if (constant_true('LOG_STATS')) {
			$_end = get_microtime();
			self::$time['CACHE'] += ($_end - $_start);
		}
		
		return $rs;
	}

	/**
	 * Carica il registro della cache se necessario (check sull'ora di modifica)
	 * 
	 * @param bool $ignore_session ignora i dati di cache in sessione
	 * 
	 * @return array il registro delle della cache ([nome_tabella] => time_stamp)
	 */ 
	public function & carica_registry($ignore_session = false)
	{
		$registry = null;
		if (!$this->db_cache)
			return $registry;

		if ($ignore_session){
			if (file_exists($this->reg_file)){
				$registry = unserialize(file_get_contents($this->reg_file));	
				return $registry;
			} else {
				return $registry;	
			}
		} else {
			//echo "caricamento registry\n";
			$cache_reg_mtime = @filemtime($this->reg_file);
			if (!$cache_reg_mtime){
				unset($_SESSION['$this->reg_file_TIME']);
				unset($this->reg_cache); 
				return $registry;
			}
			//echo "Registry del ".date("h:i:s d/m/Y",$cache_reg_mtime)."\n";
			if ($cache_reg_mtime && !empty($_SESSION['$this->reg_file_TIME'])){
				if ($cache_reg_mtime > $_SESSION['$this->reg_file_TIME']){
					// registro vecchio, ricaricare
					unset($this->reg_cache); 	
					log_value('CACHE: Registro cache da aggiornare '. $this->reg_file);
					log_value('CACHE: Registro session: '.date('d/m/Y h:i:s',$_SESSION['$this->reg_file_TIME']).
							' - ' . date('d/m/Y h:i:s',$cache_reg_mtime) .' mtime file');
				}
			}	
			
			if (!isset($this->reg_cache)){
				if ($cache_reg_mtime == null){
					$this->reg_cache = array();
					log_value('CACHE: Nessun registro cache presente '. $this->reg_file);
				} else {
					$registry = unserialize(file_get_contents($this->reg_file));
					if (!is_array($registry)){
						// errore nel caricamento del file
						$this->reg_cache = array();
						log_value('CACHE: Errore nel caricamento del registro cache '. $this->reg_file);
					} else {
						$this->reg_cache = &$registry;
						$_SESSION['$this->reg_file_TIME'] = $cache_reg_mtime;
						log_value('CACHE: Registro cache caricato '. $this->reg_file);
					}
				}
			}
			return $this->reg_cache;
		} 
	}


	/**
	 * Aggiorna il registro della cache su disco
	 */ 
	public function scrivi_registry()
	{
		log_value("CACHE: scrivendo il registry");
		
		
		if ( !($fp = fopen($this->reg_lock, 'w')) ) {
			log_value("CACHE: Impossibile aprire il lock file ".$this->reg_lock);
			throw new FatalException("Impossibile aprire il lock file ".$this->reg_lock);
		}
		if( !(flock($fp, LOCK_EX)) ) {
			log_value("CACHE: Impossibile eseguire il lock sulla cache ".$this->reg_lock);
			throw new FatalException("Impossibile eseguire il lock sulla cache");
		}
		
		$registry = $this->carica_registry(true);
		
		// da verificare l'aggiornamento della cache
		
		if (!$registry){
			$registry = array();	
		}

		foreach($this->reg_update as $tabella => $value){
			$registry[$tabella] = time();
			log_value('CACHE: Aggiornata tabella: ' . $tabella . ' ts: ($registry[$tabella]) ' . date("h:i:s d/m/Y",$registry[$tabella]));
		}
		scrivi_file($this->reg_file,serialize($registry));
		
		if( !(flock($fp, LOCK_UN)) ){
			throw new FatalException("Impossibile rilasciare il lock sulla cache");
		}
		fclose($fp);
		
	}


	/**
	 * Restituisce il nome del file che deve contenere i dati. 
	 * 
	 * @param string $nome_tabella nome della tabella
	 * @param string $campo_id nome del campo su cui viene fatta la ricerca
	 * @param string $valore_id valore cercato
	 * 
	 * @return string il nome del file
	 */ 
	public function nome_file($nome_tabella, $campo_id, $valore_id)
	{
		return "{$nome_tabella}_{$campo_id}_" . (floor($valore_id / $this->db_range) + 1) . '.db';
	}


	public function carica_sql($sql, $carica_info, &$db_object)
	{
		if (!$this->db_cache) return false;

		if (is_array($sql)){
			$sql = $sql['SQL'] . $sql['PAGE'] . $sql['PAGE-SIZE'];
		}	
		if (!isset($this->sql[$sql])){
			return false;
		}
		
		$cache_rs = & $this->sql[$sql];
		
		$db_object->setRS($cache_rs->getRS());
		
		if ($carica_info){
			$db_object->campi = $cache_rs->campi;
			$db_object->lunghezza = $cache_rs->lunghezza;
			$db_object->tipo = $cache_rs->tipo;
		}

		return true;
	}


	public function validate($usa_cache, $table, $campo_id, $condizione)
	{
		if (!$this->db_cache) return array(FALSE, NULL);

		if ($this->db_cache && $usa_cache && $table[0] == 't'){
			if (is_null($campo_id)) $campo_id = preg_replace('/^(\w+)\.(\w+)$/','\2', $usa_cache);
			if ($condizione && !(bool)preg_match('/^\s*'.$campo_id.'\s*=\s*(\d+)\s*$/', $condizione, $_info_condizione)) {
				return array(FALSE, NULL);
			} 
			return array($usa_cache, $_info_condizione[1]);
		} 
	}


	public function salva_sql($sql, &$db_object)
	{
		if (!$this->db_cache) return false;

		if (is_array($sql)){
			$sql = $sql['SQL'] . $sql['PAGE'] . $sql['PAGE-SIZE'];
		}	

		$this->sql[$sql] =& $db_object;

		return true;
	}

}


class CACHE_CLASS_FACTORY
{
	private static $instance = NULL;

	final public static function & get()
	{
		if (is_null(self::$instance)) {

			self::$instance = new CACHE();
			self::$instance->carica_registry();

		}

		return self::$instance;
	}
}

?>