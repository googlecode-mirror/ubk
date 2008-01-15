<?php
/**
 * Tag di manipolazione di sotto-sezioni di pagina
 * @author Ubik <emiliano.leporati@gmail.com>
 * @package tag
 */

/**
 * Mostra una sotto-sezione o un'altra in base ad un test di controllo (espresso in php). 
 * @package tag
 */
class XML_IF extends TAG
{
	/**
	 * Ritorna il risultato del test, che puo' trovarsi sia come nodo che come attributo
	 * @param array $parametri
	 * @return bool
	 */
	private function test(&$parametri)
	{
		if ($this->attributo_esiste("TEST") && $this->figlio("TEST") != NULL) {
			throw new CodeException("Attento coder, hai sia l'attributo che il nodo TEST figlio di IF.");

		} elseif ($this->attributo_esiste("TEST")) {
		
			return eval(xml_2_php($this->attributo_originale("TEST")));
		
		} elseif ($this->figlio("TEST") != NULL) {

			$_n =& $this->figlio_richiesto("TEST");
			return eval(xml_2_php($_n->contenuto));

		} else {
			throw new CodeException("Attento coder, mancano sia l'attributo sia il nodo TEST figlio di IF.");
		}
	}

	public function stampa_html(&$parametri)
	{
		if ($this->test($parametri)) {
			$_mostra =& $this->figlio_richiesto("THEN");
			$_mostra->stampa_html($parametri);
		} else {
			$_mostra =& $this->figlio("ELSE");
			if (!is_null($_mostra)) {
				$_mostra->stampa_html($parametri);
			}
		}
	}
	
}

/**
 * Mostra una serie di sotto-sezioni in base al risultato dei test indicati. 
 * @package tag
 */
class WHEN extends TAG
{
	public function stampa_html(&$parametri)
	{
		$_tutti = cbool($this->attributo("TUTTE", "TRUE"));
		// su tutti i figli
		for($i = 0; $i < count($this->figli); $i ++) {
			$_figlio =& $this->figli[$i];
			// se ho un test
			if (!is_null($_figlio) && $_figlio->nome == "TEST") {
				// se e' vero, dopo deve esserci un THEN
				if (eval(xml_2_php($_figlio->contenuto))) {
					$_then =& $this->figli[++ $i];
					if ($_then->nome != "THEN") {
						throw new CodeException("Nodo TEST senza THEN in WHEN");
					} else {
						$_then->stampa_html($parametri);
						// se ne basta uno, esco
						if (!$_tutti) break;
					}
				// se e' falso, e il secondo figlio e' un ELSE, allora mostro
				} else {
					$_else =& $this->figli[$i + 2];
					if (!is_null($_else) && $_else->nome == "ELSE") {
						$_else->stampa_html($parametri);
						$i += 2;
					} else {
						$i ++;
					}
				}
			}

		}
	}
	
}

/**
 * Mostra una serie di sotto-sezioni in base al valore dell'espressione indicata. 
 * @package tag
 */
class XML_SWITCH extends TAG
{
	public function stampa_html(&$parametri)
	{
		$_test =& $this->figlio_richiesto("TEST");
		$_test =  eval(xml_2_php($_test->contenuto));
		$_trovato = FALSE;
		
		for($i = 0; $i < count($this->figli); $i ++) {
			$_figlio =& $this->figli[$i];
			// se ho un caso
			if ($_figlio->nome == "CASE") {
				// se il caso e' uguale al test
				if ($_test == eval(xml_2_php($_figlio->contenuto))) {
					// cerco il primo THEN
					for($j = $i + 1; !$_trovato && $j < count($this->figli); $j ++) {
						if ($this->figli[$j]->nome == "THEN") {
							$_trovato = TRUE;
						}
					}
					if ($_trovato) {
						$_then =& $this->figli[$j - 1];
						$_then -> stampa_html($parametri);
						break;
					} else {
						throw new CodeException("Nodo CASE senza THEN in SWITCH");
					}
				}
			}
		}
		// se non ho trovato nessuno, allora faccio l'else se esiste
		if (!$_trovato) {
			$_else = $this->figlio("ELSE");
			if (!is_null($_else)) {
				$_else->stampa_html($parametri);
			}
		}
	}
	
}

/**
 * Modifica una serie di parametri per la sezione che include. 
 * @package tag
 */
class USA extends CONTENITORE
{
	public function stampa_html(&$parametri)
	{

		// parametri che fanno env
		$params_name = array('MODE','GATEWAY','CLASS', 'FORM', 'FORM-N', 'PAGE', 'PAGE-SIZE', 'FILE', 'TARGET');
		
		$bkp_params = array();
		
		foreach ($params_name as $par_name){
			if ($this->attributo_esiste($par_name)) {
				if (array_key_exists($par_name, $parametri))
					$bkp_params[$par_name] = $parametri[$par_name];
				$parametri[$par_name] = $this->attributo($par_name);
			}
		}

		if ($this->attributo_esiste("TABELLA")) {
			$rs = new GESTORE($this->attributo("TABELLA"), $this->attributo("CAMPO-ID", NULL));

			if ($parametri["MODE"] != "W") {
				
				$ordine = $this->attributo("ORDINE", NULL);
				$distinct = $this->attributo("DISTINCT", FALSE);
				$rs->pagina = $this->attributo("PAGE", NULL);
				$rs->dim_pagina = $this->attributo("PAGE-SIZE", NULL);

				if ($this->attributo_esiste("VALORE-ID") && $this->attributi["VALORE-ID"] != "") {
					$rs->carica_righe_id($this->attributo("VALORE-ID"), $ordine, NULL, $distinct);
				} elseif ($this->attributo_esiste("FILTRO")) {
					$rs->carica($this->attributo("FILTRO"), $ordine, NULL, $distinct);
				} else {
					$rs->carica(NULL, $ordine, NULL, $distinct);
				}
				$bkp_params['ORDINE'] = array_get_default("ORDINE", $parametri);
				$parametri["ORDINE"] = $ordine;
			} 
			$bkp_params['GESTORE'] =& $parametri['GESTORE'];
			$parametri["GESTORE"] =& $rs;
		} else{
			if ($this->attributo_esiste("CAMPO-ID")) {
				$bkp_params['CAMPO-ID'] = $parametri["GESTORE"]->campo_id;
				$parametri["GESTORE"]->campo_id = $this->attributo("CAMPO-ID");
			}
			if ($this->attributo_esiste("ORDINE")) {
				$bkp_params['ORDINE'] = array_get_default("ORDINE", $parametri);
				$parametri["ORDINE"] = $this->attributo("ORDINE");
			}
		}

		parent::stampa_html($parametri);
		
		// ripristino parametri
		foreach ($params_name as $par_name){
			if (array_key_exists($par_name, $bkp_params)) {
				$parametri[$par_name] = $bkp_params[$par_name];
			} elseif ($this->attributo_esiste($par_name)) {
				unSet($parametri[$par_name]);
			}
		}
		
		if (array_key_exists('GESTORE', $bkp_params)) {
			$parametri['GESTORE'] =& $bkp_params['GESTORE'];
		} 
		if (array_key_exists('CAMPO-ID', $bkp_params)){
			$parametri["GESTORE"]->campo_id = $bkp_params["CAMPO-ID"];
		}
		if (array_key_exists('ORDINE', $bkp_params)){
			$parametri["ORDINE"] = $bkp_params["ORDINE"];
		}
		
	}
}


/**
 * Mostra il suo contenuto solo se i diritti passati nei parametri contemplano l'accesso alla sezione specificata. 
 * @package tag
 */
class SEZIONE extends CONTENITORE
{
	private function diritto($diritti)
	{
		$search = explode('|', strtoupper($this->attributo("DIRITTO", "R")));
		foreach ($search as $diritto) {
			if (array_key_exists($diritto, $diritti) && $diritti[$diritto])
				return $diritto;
		}
		foreach ($search as $diritto) {
			if (array_key_exists($diritto, $diritti))
				return $diritto;
		}
		return NULL;
	}

	public function stampa_html(&$parametri)
	{
		$fail = cbool($this->attributo("FALLISCI", "TRUE"));

		$vecchi_diritti = array_get_default('DIRITTI', $parametri, array());
		if ($d = $this->attributo("DIRITTI")) {
			$parametri['DIRITTI'] = $d;
		}

		if ($this->attributo_esiste("NOME")) {
			$sezioni = explode("|", strtoupper($this->attributo_richiesto("NOME")));
		} elseif (array_key_exists("SEZIONE", $parametri)) {
			$sezioni = array($parametri["SEZIONE"]);
		} else {
			throw new CodeException("Nessuna sezione specificata in tag 'sezione'");
		}

		$strict  = cbool($this->attributo("RESTRINGI", "TRUE"));

		if (!array_key_exists("DIRITTI", $parametri)) {

			throw new CodeException("Informazioni di accesso non specificate in tag 'sezione'");

		} else {

			foreach($sezioni as $sezione) {
				// verifico che la sezione esista
				// $parametri["DIRITTI"] e' un array associativo (sezione -> permessi)
				// $parametri["DIRITTI"][$sezione] e' un array associativo (diritto -> permesso)
				// il diritto puo' essere R, W, RW, D, il permesso true o false
				if (!array_key_exists($sezione, $parametri["DIRITTI"])) {
					if ($fail)
						throw new CodeException("Sezione non esistente '$sezione'");
					else {
						log_value("Sezione non esistente '$sezione'");
					}
				
				// verifico che il diritto esista
				} elseif (!$diritto = $this->diritto($parametri["DIRITTI"][$sezione])) {

					throw new CodeException("Diritto non esistente '$diritto'");
				
				// se ho il diritto, allora stampo e metto la sezione corrente fra i parametri
				// ed esco dal ciclo
				} elseif ($parametri["DIRITTI"][$sezione][$diritto]) {

					if (!isSet($parametri["SEZIONI-VISITATE"])) {
						$parametri["SEZIONI-VISITATE"] = array($sezione);
					} else {
						array_push($parametri["SEZIONI-VISITATE"], $sezione);
					}

					$vecchia_sezione = array_get_default('SEZIONE', $parametri);
					$vecchio_mode    = array_get_default('MODE', $parametri);
					$parametri["SEZIONE"] = $sezione;
					if ($diritto == 'D') {
						$parametri["MODE"]    = 'RW';
					} else {
						$parametri["MODE"]    = $diritto;
					}

					parent::stampa_html($parametri);

					$parametri["SEZIONE"] = $vecchia_sezione;
					$parametri["MODE"]    = $vecchio_mode;
					$parametri['DIRITTI'] = $vecchi_diritti;
					return;

				} elseif (!$strict && $diritto == "RW" && !$parametri["DIRITTI"][$sezione]["RW"] && $parametri["DIRITTI"][$sezione]["R"]) {

					if (!isSet($parametri["SEZIONI-VISITATE"])) {
						$parametri["SEZIONI-VISITATE"] = array($sezione);
					} else {
						array_push($parametri["SEZIONI-VISITATE"], $sezione);
					}
					
					$vecchia_sezione = array_get_default('SEZIONE', $parametri);
					$vecchio_mode    = array_get_default('MODE', $parametri);
					$parametri["SEZIONE"] = $sezione;
					$parametri["MODE"]    = "R";

					parent::stampa_html($parametri);

					$parametri["SEZIONE"] = $vecchia_sezione;
					$parametri["MODE"]    = $vecchio_mode;
					$parametri['DIRITTI'] = $vecchi_diritti;
					return;
				}
			}

			// errore: impossibile accedere
			if ($fail) {
				throw new AppException("Diritti insufficienti per vedere la sezione '$sezione' in con diritto '$diritto'.");
			} else {
				$parametri['DIRITTI'] = $vecchi_diritti;
			}
		}
	}

}

/**
 * Include un file esterno impostando eventuali parametri d'ambiente
 * @package tag
 */
class XML_INCLUDE extends TAG
{
	/**
	 * Contiene una cache di tutti i file gia' caricati
	 * @var array
	 */
	private static $loadeds = array();

	/**
	 * Ritorna i nomi dei file caricati
	 * @return array
	 */
	public static function names()
	{
		return array_keys(self::$loadeds);
	}

	/**
	 * Rimuove dalla cache il file specificato
	 * @param string $name il nome del file da rimuovere
	 */
	public static function unload($name)
	{
		unset(self::$loadeds[$name]);
	}

	public function stampa_html(&$parametri)
	{
		static $parser = NULL;

		// backup parametri
		$bkp_params = array();
		
		foreach (array_keys($this->attributi) as $par_name){
			if (array_key_exists($par_name, $parametri))
				$bkp_params[$par_name] = $parametri[$par_name];
			$parametri[$par_name] = $this->attributo($par_name);
		}

		$file = $this->attributo_richiesto("FILE");
		// parsing
		if (!isset(self::$loadeds[$file])) {


			if (isset($parametri['PARSER'])) {
				$parser =& $parametri['PARSER'];
			} else {
				$parser = new PARSER();
			}

			self::$loadeds[$file] = $root = $parser->parsifica($file);
			
			if (is_null($root)) 
				throw new CodeException($parser->errore);

			$this -> aggiungi_figlio($root);

			$vecchio_parser =& $parametri["PARSER"];
			$parametri["PARSER"]  =& $parser;

			echo "<!-- $file -->";
			// non stampo "PAGINA"
			$root->stampa_contenuto($parametri);

			$parametri["PARSER"]  =& $vecchio_parser;

		} else {
			self::$loadeds[$file]->stampa_contenuto($parametri);
		}

		// ripristino parametri
		foreach (array_keys($this->attributi) as $par_name){
			if (array_key_exists($par_name, $bkp_params)) {
				$parametri[$par_name] = $bkp_params[$par_name];
			} elseif ($this->attributo_esiste($par_name)) {
				unSet($parametri[$par_name]);
			}
		}
	}
}

/**
 * Genera qualunque tag HTML
 * @package tag
 */
class HTML extends CONTENITORE
{
	public function stampa_html(&$parametri)
	{
		$tag = strtolower($this->attributo_richiesto("TAG"));

		echo LT.$tag;
		foreach($this->attributi as $name => $value) {
			if (strcasecmp($name, 'TAG') != 0 && $valore = $this->attributo($name))
				echo ' '.strtolower($name).'='.dQt($valore);
		}

		if (in_array(strtoupper($tag), PARSER::$TAG_SINGOLI)) {
			if ($this->contenuto)
				throw new CodeException("Uso invalido del tag $tag");
			echo '/'.GT;
		} else {
			echo GT;
			parent::stampa_html($parametri);
			echo LT.'/'.$tag.GT;
		}
	}
	
}

/**
 * Classe base per la generazione di qualunque tag html, basandosi sul nome classe
 * @package tag
 */
abstract class HTML_TAG extends HTML
{
	public function stampa_html(&$parametri)
	{
		$this->attributi['TAG'] = $this->nome;
		if (in_array($this->nome, array('INPUT', 'A', 'TEXTAREA', 'SELECT', 'IMG')))
			$this->attributi['TABINDEX'] = CAMPO_ACCESSO::get_static_tabindex($this->attributo('TABINDEX'));
		parent::stampa_html($parametri);
	}
}

/**
 * Tag SCRIPT
 * @package tag
 */
class SCRIPT extends HTML
{
	public function stampa_html(&$parametri)
	{
		if ($this->attributo_esiste('SRC')) {
			$this->attributi['TAG'] = 'script';
			$this->attributi['TYPE'] = 'text/javascript';
			parent::stampa_html($parametri);
		} else {
			echo '<script type="text/javascript">';
			echo "/*<![CDATA[*/";
			CONTENITORE::stampa_html($parametri);
			echo "/*]]>*/";
			echo '</script>';
		}
	}
}

/**
 * Tag STYLE
 * @package tag
 */
class STYLE extends HTML
{
	public function stampa_html(&$parametri)
	{
		echo '<style type="text/css" media="screen">';
		echo "/*<![CDATA[*/";
		CONTENITORE::stampa_html($parametri);
		echo "/*]]>*/";
		echo '</style>';
	}
}

/**
 * Definisce e richiama snippets di codice XHTML, con parametri
 * @package tag
 */
class SNIPPET extends CONTENITORE
{
	/**
	 * Contiene una cache di tutti gli snippet gia' caricati
	 * @var array
	 */
	private static $loadeds = array();

	/**
	 * Ritorna i nomi degli snippet caricati
	 * @return array
	 */
	public static function names()
	{
		return array_keys(self::$loadeds);
	}

	/**
	 * Rimuove dalla cache lo snippet specificato
	 * @param string $name il nome dello snippet da rimuovere
	 */
	public static function unload($name)
	{
		unset(self::$loadeds[$name]);
	}

	/**
	 * Routine effettiva di stampa
	 * @param array $parametri Ambiente di valutazione
	 */
	public function stampa_contenuto(&$parametri)
	{
		parent::stampa_html($parametri);
	}

	public function stampa_html(&$parametri)
	{
		if ($n = $this->attributo('NOME')) {

			self::$loadeds[$n] = $this;

		} elseif ($n = $this->attributo('CALL')) {
			$ns = explode('|', $n);

			foreach($ns as $nn) {

				if (isset(self::$loadeds[$nn])) {

					// backup parametri
					$pars = array();
					$bkp_params = array();
					foreach(array_keys($this->attributi) as $par_name)
						if (strcasecmp($par_name, 'CALL') != 0) {
							$pars[] = $par_name;
							if (array_key_exists($par_name, $parametri))
								$bkp_params[$par_name] = $parametri[$par_name];
							$parametri[$par_name] = $this->attributo($par_name);
						}

					// stampa
					self::$loadeds[$nn]->stampa_contenuto($parametri);

					// ripristino parametri
					foreach ($pars as $par_name){
						if (array_key_exists($par_name, $bkp_params)) {
							$parametri[$par_name] = $bkp_params[$par_name];
						} elseif ($this->attributo_esiste($par_name)) {
							unSet($parametri[$par_name]);
						}
					}
					// fine
					return;
				}
			}

			throw new CodeException("Snippet $n non definito.");

		} else {
			throw new CodeException("Uso invalido.");
		}
	}

	/**
	 * Permette ai FORM di trovare i campi attraverso le chiamate a snippet
	 * @param $nome Il nome dei tag da trovare
	 */
	public function figli_profondita($nome) 
	{
		if ($n = $this->attributo('CALL')) {
			$ns = explode('|', $n);
			foreach($ns as $nn) {
				if (isset(self::$loadeds[$nn])) {
					return self::$loadeds[$nn]->figli_profondita($nome);
				}
			}
			return array();
		} else {
			return CONTENITORE::figli_profondita($nome);
		}
	}

}
?>