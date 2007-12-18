<?php
/**
 * Radici ed elementi basilari della gerarchia di classi che compongono la nostra tag-library.
 * @author Ubik <emiliano.leporati@gmail.com>
 * @package tag
 */

/** 
 * Inclusione delle funzioni di uso comune
 */ 
require_once("funzioni_generali.inc");
require_once("funzioni_log.inc");


/**
 * Classe base della tag-library, fornisce tutte le utilitï¿½di accesso agli attributi e ai nodi figlio, la stampa della struttura e la conversione bufferizzata
 * @package tag
 */
class TAG 
{
	/**
	 * Array contenente i parametri originali passati al metodo {@link stampa_html} - messi qui sono raggiungibili da altri metodi (es. {@link attributo})
	 * @var array
	 */
	public $parametri = array();

	/**
	 * Array associativo (nome => valore) con gli attributi del nodo
	 * @var array
	 */
	public $attributi = array();

	/**
	 * Array contenente gli oggetti rappresentanti i tag di primo livello contenuti dal tag attuale nel sorgente
	 * @var array
	 */
	public $figli = array();

	/** 
	 * Contenuto testuale del tag (quando il tag non ï¿½un contenitore)
	 * @var string
	 */
	public $contenuto = "";

	/** 
	 * Oggetto padre nella nidificazione
	 * @var TAG
	 */
	public $padre = NULL;

	/** 
	 * Nome del tag, viene utilizzato nelle ricerche ({@link figlio})
	 * @var string
	 */
	public $nome;

	/** 
	 * Livello di nidificazione del tag nel sorgente XML, viene usato durante il parsing per effettuare le chiusure in modo corretto ({@link PARSER::_apertura, PARSER::_chiusura})
	 * @var int
	 */
	public $livello_in_sorgente;

	/** 
	 * Posizione del figlio rispetto al padre, -1 se {@link padre} ï¿½NULL
	 * @var int
	 */
	public $numero_figlio = -1;

	/** 
	 * Costruttore - imposta il nome del tag
	 * @param string $nome
	 */
	public function __construct($nome)
	{
		$this->nome = $nome;
	}

	/** 
	 * Dice se per l'attributo passato ï¿½stato specificato un valore
	 * @param string $nome nome dell'attributo
	 * @return bool
	 */
	public function attributo_esiste($nome)
	{
		return array_key_exists($nome, $this->attributi);
	}

	/** 
	 * Dice se l'attributo ï¿½calcolato o meno
	 * @param string $nome nome dell'attributo
	 * @return bool
	 */
	public function attributo_calcolato($nome)
	{
		if ($this->attributo_esiste($nome)) {
			return in_array(substr($this->attributo_originale($nome), 0, 1), array('$', '%'));
		} else {
			throw new CodeException("Manca attributo '$nome' in tag '$this->nome'");
		}
	}

	/** 
	 * Lettura attributo opzionale in modalitï¿½originale (il valore viene ritornato come ï¿½nel sorgente)
	 * @param string $nome nome dell'attributo
	 * @param mixed $se_manca valore di default, viene ritornato se l'attributo non ï¿½stato specificato nel sorgente
	 * @return mixed
	 */
	public function attributo_originale($nome, $se_manca = "")
	{
		return array_get_default($nome, $this->attributi, $se_manca);
	}
	

	/** 
	 * Lettura attributo richiesto in modalitï¿½originale (il valore viene ritornato come ï¿½nel sorgente); se l'attributo non ï¿½stato specificato nel sorgente, viene sollevato un errore
	 * @param string $nome nome dell'attributo
	 * @return mixed
	 */
	public function attributo_originale_richiesto($nome) 
	{
		if ($this->attributo_esiste($nome)) {
			return $this->attributi[$nome];
		} else {
			throw new CodeException("Manca attributo richiesto '$nome' in tag '$this->nome'");
		}
	}

	/** 
	 * Lettura attributo opzionale in modalitï¿½avanzata (se il valore comincia per '$', viene ritornato il contenuto della variabile specificata, anzichï¿½il suo nome; se il valore comincia per '%' viene considerata un'espressione e viene ritornato il risultato della sua valutazione)
	 * @param string $nome nome dell'attributo
	 * @param mixed $se_manca valore di default, viene ritornato se l'attributo non ï¿½stato specificato nel sorgente
	 * @return mixed
	 */
	public function attributo($nome, $se_manca = "")
	{
		$_valore = array_get_default($nome, $this->attributi, $se_manca);
		if (strlen($_valore) > 0 && $_valore[0] == "$") {
// 			log_value($this->parametro('FILE').$_valore);
			$_valore = eval(xml_2_php($_valore));
		} elseif (strlen($_valore) > 0 && $_valore[0] == "%") {
// 			log_value($this->parametro('FILE').$_valore);
			$_valore = eval(xml_2_php(substr($_valore, 1)));
		}	
		return $_valore;
	}

	/** 
	 * Lettura attributo richiesto in modalitï¿½avanzata (se il valore comincia per '$', viene ritornato il contenuto della variabile specificata, anzichï¿½il suo nome; se il valore comincia per '%' viene considerata un'espressione e viene ritornato il risultato della sua valutazione); se l'attributo non ï¿½stato specificato nel sorgente, viene sollevato un errore
	 * @param string $nome nome dell'attributo
	 * @return mixed
	 */
	public function attributo_richiesto($nome) 
	{
		if (array_key_exists($nome, $this->attributi)) {
			$_valore = $this->attributi[$nome];
			if (strlen($_valore) > 0 && $_valore[0] == "$") {
// 				log_value($this->parametro('FILE').$_valore);
				$_valore = eval(xml_2_php($_valore));
			} elseif (strlen($_valore) > 0 && $_valore[0] == "%") {
// 				log_value($this->parametro('FILE').$_valore);
				$_valore = eval(xml_2_php(substr($_valore, 1)));
			}
			
			return $_valore;
		} else {
			throw new CodeException("Manca attributo richiesto '$nome' in tag '$this->nome'");
		}
	}

	/** 
	 * Lettura parametro opzionale
	 * @param string $nome nome del parametro
	 * @return mixed
	 */
	public function parametro($nome, $default = null)
	{
		return array_get_default($nome, $this->parametri, $default);
	}

	/** 
	 * Lettura parametro richiesto 
	 * @param string $nome nome del parametro
	 * @return mixed
	 */
	public function parametro_richiesto($nome)
	{
		return array_get($nome, $this->parametri);
	}

	/** 
	 * Ritorna il primo oggetto figlio di primo livello con il nome specificato, NULL se non viene trovato
	 * @param string $nome nome del figlio da cercare
	 * @return TAG
	 */
	public function &figlio($nome) 
	{
		reset($this->figli);
		while(list($dummy ,$figlio) = each($this->figli)) {
			if (is_object($figlio) && $figlio->nome == $nome) {
				return $figlio;
			}
		}

		$figlio = NULL;
		return $figlio;
	}

	/** 
	 * Ritorna il primo oggetto figlio di primo livello con il nome specificato, solleva un'eccezione se non viene trovato
	 * @param string $nome nome del figlio da cercare
	 * @return TAG
	 */
	public function &figlio_richiesto($nome)
	{
		$figlio =& $this->figlio($nome);
		if (!is_null($figlio)) {
			return $figlio;
		} else {
			throw new CodeException("Manca sotto-tag richiesto '$nome' in tag '$this->nome'");
		}
	}

	/**
	 * Ritorna tutti gli oggetti figlio di primo livello con il nome specificato, in un array (vuoto se non ne vengono trovati)
	 * @param string $nome nome del figlio da cercare
	 * @return TAG array
	 */
	public function figli($nome) 
	{
		@reset($this->figli);
		$figli = array();
		while(list($dummy, $figlio) = each($this->figli)) {
			if (is_object($figlio) && $figlio->nome == $nome) {
				array_push($figli, $figlio);
			}
		}
		return $figli;
	}

	/**
	 * Ritorna tutti gli oggetti padri di qualsiasi livello con il nome specificato, in un array (vuoto se non ne vengono trovati)
	 * @param string $nome nome del padre da cercare
	 * @return TAG array
	 */
	public function padri($nome_limite = null) 
	{
		$padre =& $this->padre;
		if (is_null($padre)) {
			return array();
		} elseif ($padre->nome == $nome_limite) {
			return array($padre);
		} else {
			return array_merge(array($padre), $padre->padri($nome_limite));
		}
	}

	/**
	 * Ritorna tutti gli oggetti figlio di qualunque livello con il nome specificato, in un array (vuoto se non ne vengono trovati)
	 * @param string $nome nome del figlio da cercare
	 * @return array
	 */
	public function figli_profondita($nome) 
	{
		@reset($this->figli);
		$figli = array();
		while(list($dummy, $figlio) = each($this->figli)) {
			if (is_object($figlio)) {
				if ($figlio->nome == $nome) {
					array_push($figli, $figlio);
				} else {
					$figli = array_merge($figli, $figlio->figli_profondita($nome));
				}
			}
		}
		return $figli;
	}

	/**
	 * Aggiunge un figlio al nodo corrente, che viene impostato come suo padre
	 * @param TAG &$figlio
	 */
	public function aggiungi_figlio(&$figlio)
	{
		$figlio->padre =& $this;
		$figlio->numero_figlio = count($this->figli);
		array_push($this->figli, $figlio);
	}

	/**
	 * Stampa la struttura dell'oggetto e dei suoi figli
	 * @param int $livello livello di indentazione
	 */
	public function stampa($livello = 0)
	{
		$_nest = str_repeat("   ", $livello);
		echo $_nest."&lt;$this->nome(".get_class($this).")".xml_att_crea($this->attributi)."&gt;\n";
		foreach($this->figli as $figlio) {
			$figlio->stampa($livello + 1);
		}
		echo $_nest."&lt;/".$this->nome."&gt;\n";
	}

	/**
	 * Esegue la trasformazione del nodo corrente in HTML - qui il metodo ï¿½ancora vuoto, da implementare nelle classi figlio
	 * @param array &$parametri parametri di "ambiente" impostati da altri nodi o dai gateway
	 */
	public function stampa_html(&$parametri)
	{
		// da implementare nelle classi figlio
	}

	/**
	 * Versione bufferizzata di {@link stampa_html}
	 * @param array &$parametri parametri di "ambiente" impostati da altri nodi o dai gateway
	 * @return string
	 */
	public function stampa_html_in_stringa(&$parametri)
	{
		ob_start();
		$this->stampa_html($parametri);
		$_risultato = ob_get_contents();
		ob_end_clean();

		return $_risultato;
	}

	/**
	 * Mi dice se il nodo attuale discende o meno dalla classe {@link CONTENITORE}
	 * @return bool
	 */
	public function is_contenitore()
	{
		return ($this instanceof CONTENITORE);
	}

	/**
	 * Ritorna il valore del campo nel GESTORE corrente dei parametri
	 * @param string $campo Il nome del campo di cui si vuole il valore
	 * @return mixed
	 */
	public function valore($campo)
	{
		if (!isSet($this->parametri['GESTORE']))
			throw new CodeException("Nessun gestore presente per metodo 'valore'");
		return $this->parametri['GESTORE']->valore($campo);
	}

	/**
	 * Ritorna il valore del campo id nel GESTORE corrente dei parametri
	 * @return mixed
	 */
	public function valore_campo_id()
	{
		if (!isSet($this->parametri['GESTORE']))
			throw new CodeException("Nessun gestore presente per metodo 'valore_id'");
		$rs =& $this->parametri['GESTORE'];
		return $rs->valore($rs->campo_id);
	}

	/**
	 * Come valore in sintassi $o->campo
	 * @param string $campo Nome del campo di cui ritornare il valore
	 * @return mixed
	 */
	public function __get($campo)
	{
		return $this->valore($campo);
	}

}

/**
 * Classe base per i tag contenitore, che includono sia sezioni HTML (testo) che altri nodi, tutti inseriti nell'array {@link TAG::$figli}
 * @package tag
 */
class CONTENITORE extends TAG 
{
	public function stampa($livello = 0)
	{
		if ($livello == 0) echo "<PRE>";

		$_nest = str_repeat("   ", $livello);
		echo $_nest."&lt;".$this->nome."(".get_class($this).")".xml_att_crea($this->attributi)."&gt;\n";
		foreach($this->figli as $figlio) {
			if (is_object($figlio)) {
				$figlio->stampa($livello + 1);
			} else {
				echo $_nest."   ".htmlspecialchars(trim($figlio))."\n";
			}
		}
		echo $_nest."&lt;/".$this->nome."&gt;\n";
		
		if ($livello == 0) echo "</PRE>";
	}

	/**
	 * I figli, o sono stringhe, o sono oggetti. Se sono stringhe vengono stampati, se sono oggetti ne viene invocato il metodo {@link TAG::stampa_html}
	 * @param array $parametri parametri di "ambiente" impostati da altri nodi o dai gateway
	 */
	public function stampa_html(&$parametri)
	{
		reset($this->figli);
		while(list($dummy ,$figlio) = each($this->figli)) {
			if (is_object($figlio)) {
				$figlio->parametri =& $parametri;
				$figlio->stampa_html($parametri);
			} else {
				echo xml_amp($figlio);
			}
		}
	}

	/**
	 * Aggiunge del testo semplice tra i figli del nodo, in alternativa ad aggiungerne un figlio , o sono stringhe, o sono oggetti. Se sono stringhe vengono stampati, se sono oggetti ne viene invocato il metodo {@link TAG::stampa_html}
	 * @param array $parametri parametri di "ambiente" impostati da altri nodi o dai gateway
	 */
	public function aggiungi_dati($dati)
	{
		$_last = count($this->figli) - 1;
		if ($_last >= 0 && is_string($this->figli[$_last])) {
			$this->figli[$_last] .= $dati;
		} else {
			array_push($this->figli, $dati);
		}
	}

}


/**
 * Esegue il codice php specificato come contenuto del tag nel sorgente XML. Tag associato: PHP. 
 * @package tag
 */
class XML_PHP extends TAG
{
	public function stampa_html(&$parametri)
	{
#		log_value($this->contenuto);
		eval($this->contenuto);
	}
}

/**
 * Produce il contenuto della variabile indicata come contenuto del tag nel sorgente XML. Tag associato: VAR. 
 * @package tag
 */
class XML_VAR extends TAG
{
	public function stampa_html(&$parametri)
	{
		$_nome_var = trim($this->contenuto);

		$_res = eval(xml_2_php($_nome_var));
		if (is_null($_res))
			echo $this->contenuto;
		else if (is_array($_res))
			array_stampa($_res);
		else
			echo $_res;
	}

}

/**
 * Produce il contenuto delle costanti indicate come contenuto del tag nel sorgente XML. Tag associato: CONST. 
 * @package tag
 */
class XML_CONST extends TAG
{
	public function stampa_html(&$parametri)
	{
		$_nomi_const = explode("|", $this->attributo_richiesto("NOME"));
		foreach($_nomi_const as $_nome_const) {
			if (defined($_nome_const)) {
				echo constant($_nome_const);
			} else {
				throw new CodeException("Costante non definita: $_nome_const");
			}
		}
	}
	
}

/**
 * Contenitore di tutta la pagina analizzata dal {@link PARSER} - si occupa di spedire gli header relativi al controllo della cache e della vita della pagina
 * @package tag
 */
class PAGINA extends CONTENITORE
{
	public function output_handler($buffer)
	{
		$buffer = str_ireplace(
			array(
				'<?xml version="1.0"?>', 
				' xmlns:ubk='.dQt(NS_UBK),
				' xmlns:gfx='.dQt(NS_GFX)
			)
			,array('', '', '')
			,$buffer
		);
		// strip degli attributi vuoti
		$buffer = preg_replace('#\s+\w+="\s*"#U', '', $buffer);
#		$buffer = preg_replace("#('\w+)(&amp;\w+=&amp;)(\w+')#U", '\1&amp;\3', $buffer);
#		$buffer = preg_replace("#('\w+)(&\w+=&)(\w+')#U", '\1&\3', $buffer);
#		$buffer = preg_replace("#('\w+)&amp;\w+='#", '\1', $buffer);
#		$buffer = preg_replace("#('\w+)&\w+='#", '\1', $buffer);
#		$buffer = preg_replace('/(".*)(&[^amp;])(.*")/U', '\1&amp;\3', $buffer);
		return $buffer;
	}

	public function stampa_html(&$parametri)
	{
		if (!headers_sent()) {
			header("Expires: Mon, 28 Jul 1997 00:00:00 GMT");    // Date in the past
			header("Last-Modified: " . gmdate("D, d M Y H:i:s") . " GMT"); // always modified
			header("Cache-Control: no-store, no-cache, must-revalidate");  // HTTP/1.1
			header("Cache-Control: post-check=0, pre-check=0", false);
			header("Cache-control: private");
			header("Content-type: text/xhtml; charset=UTF-8");
			header("Pragma: no-cache"); // HTTP/1.0
		}
		ob_start(array($this, 'output_handler'));
		echo '<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">';
		parent::stampa_html($parametri);
		ob_end_flush();
	}

	public function stampa_contenuto(&$parametri)
	{
		ob_start(array($this, 'output_handler'));
		parent::stampa_html($parametri);
		ob_end_flush();
	}
}


/** 
 * Inclusione di tutti gli altri tag della libreria (ITERATORI)
 */ 
require_once("TAG_ITER.inc");
/** 
 * Inclusione di tutti gli altri tag della libreria (DB)
 */ 
require_once("TAG_DB.inc");
/** 
 * Inclusione di tutti gli altri tag della libreria (FILTRO)
 */ 
require_once("TAG_FILTRO.inc");
/** 
 * Inclusione di tutti gli altri tag della libreria (LINK)
 */ 
require_once("TAG_LINK.inc");
/** 
 * Inclusione di tutti gli altri tag della libreria (SEZIONI, CONDIZIONALI)
 */ 
require_once("TAG_SEZ.inc");
/** 
 * Inclusione di tutti gli altri tag della libreria (TABELLE)
 */ 
#require_once("TAG_TABELLE.inc");
/** 
 * Inclusione di tutti gli altri tag della libreria (AJAX)
 */ 
require_once("TAG_LINK_AJAX.inc");

?>