<?php
/**
 * Parser dei documenti XHTML. Ha lo scopo di trasformare un documento XML in un
 * albero di oggetti-tag su cui richiamare la trasformazione in HTML.
 * @author Ubik <emiliano.leporati@gmail.com>
 * @package tag
 */

/** 
 * Definisce quali tag necessitano di chiusura "inline" in XML
 * @global array $TAG_SINGOLI 
 */
$TAG_SINGOLI = array("BR", "LINK", "META", "BASE", "HR", "PARAM", "SPACER", "AREA", "FRAME", "IMG", "INPUT");

/** 
 * Definisce quali tag vengono trasformati senza il namespace UBK
 * @global array $TAG_HTML
 */
$TAG_HTML    = array('ACRONYM', 'TD', 'TH', 'TR', 'DIV', 'INPUT', 'IMG', 'SCRIPT', 'STYLE', 'SELECT', 'OPTION');

/** 
 * Definisce, per tutti quei TAG che hanno nome della classe di definizione differente dal nome del tag, o piu' classi associabili a seconda del contesto {@link CONTESTI_CLASSE}, i nomi della o delle classi che il tag puo' diventare
 * @global array $CLASSI_TAG 
 */
$CLASSI_TAG = array(
	"VAR"   => "XML_VAR"
	,"PHP"   => "XML_PHP"
	,"IF"    => "XML_IF"
	,"SWITCH"=> "XML_SWITCH"
	,"TEST"  => "TAG"
	,"CASE"  => "TAG"
	,"AZIONE" => "TAG"
	,"REAZIONE" => "TAG"
	,"EXTRA_GET" => "TAG"
	,"EXTRA_RE_GET" => "TAG"
	,"POP_UP" => "TAG"
	,"THEN"  => "CONTENITORE"
	,"ELSE"  => "CONTENITORE"
	,"CONTENUTO" => "CONTENITORE"
	,"PAGINA" => "TAG"
	,"HREF" => "XML_PHP"
	,"INTESTAZIONE" => "CONTENITORE"
	,"LINK" => "TAG"
	,"CONST" => "XML_CONST"
	,"INCLUDE" => "XML_INCLUDE"
);


/** 
 * Definisce, i contesti (ambiti di posizionamento entro il sorgente XHML) in cui certi tag o classi sono attivabili, ossia i contesti in cui il tag viene trasformato nella classe corrispondente o la classe viene istanziata
 * @global array $CONTESTI_CLASSE 
 */
$CONTESTI_CLASSE = array(
	"THEN" => array("/IF/THEN","/WHEN/THEN", "/SWITCH/THEN")
	,"ELSE" => array("/IF/ELSE","/WHEN/ELSE", "/SWITCH/ELSE")
	,"TEST" => array("/IF/TEST","/WHEN/TEST", "/SWITCH/TEST")
	,"CASE" => "/SWITCH/CASE"
	,"HREF" => array("/LINK_MAILTO/HREF", "/LINK_HTML/HREF")
	,"AZIONE" => array("/LINK_GENERICO/AZIONE","/LINK_CREA/AZIONE","/LINK_AGGIORNA/AZIONE","/LINK_ELIMINA/AZIONE", "/LINK_FILTRA/AZIONE", "/LINK_ANNULLA_FILTRO/AZIONE", "/LINK_AGGIORNA_TUTTO/AZIONE",
					"/LINK_AJAX_GENERICO/AZIONE","/LINK_AJAX_CREA/AZIONE","/LINK_AJAX_AGGIORNA/AZIONE","/LINK_AJAX_ELIMINA/AZIONE", "/LINK_AJAX_FILTRA/AZIONE", "/LINK_AJAX_ANNULLA_FILTRO/AZIONE", "/LINK_AJAX_AGGIORNA_TUTTO/AZIONE")
	,"REAZIONE" => array("/LINK_GENERICO/REAZIONE","/LINK_CREA/REAZIONE","/LINK_AGGIORNA/REAZIONE","/LINK_ELIMINA/REAZIONE", "/LINK_FILTRA/REAZIONE", "/LINK_ANNULLA_FILTRO/REAZIONE", "/LINK_AGGIORNA_TUTTO/REAZIONE",
					"/LINK_AJAX_GENERICO/REAZIONE","/LINK_AJAX_CREA/REAZIONE","/LINK_AJAX_AGGIORNA/REAZIONE","/LINK_AJAX_ELIMINA/REAZIONE", "/LINK_AJAX_FILTRA/REAZIONE", "/LINK_AJAX_ANNULLA_FILTRO/REAZIONE", "/LINK_AJAX_AGGIORNA_TUTTO/REAZIONE")
	,"POP_UP" => array("/LINK_GENERICO/POP_UP", "/PICKER/LINK/POP_UP", "/PICKER_FILTRO/LINK/POP_UP", "/COD_DESC/LINK/POP_UP", "/COD_DESC_FILTRO/LINK/POP_UP",
					"/LINK_AJAX_GENERICO/POP_UP", "/PICKER/LINK/POP_UP", "/PICKER_FILTRO/LINK/POP_UP", "/COD_DESC/LINK/POP_UP", "/COD_DESC_FILTRO/LINK/POP_UP")
	,"EXTRA_GET" => array("/LINK_GENERICO/EXTRA_GET","/LINK_CREA/EXTRA_GET","/LINK_AGGIORNA/EXTRA_GET","/LINK_ELIMINA/EXTRA_GET", "/LINK_FILTRA/EXTRA_GET", "/LINK_ANNULLA_FILTRO/EXTRA_GET", "/LINK_AGGIORNA_TUTTO/EXTRA_GET", "/PICKER/LINK/EXTRA_GET",
					"/LINK_AJAX_GENERICO/EXTRA_GET","/LINK_AJAX_CREA/EXTRA_GET","/LINK_AJAX_AGGIORNA/EXTRA_GET","/LINK_AJAX_ELIMINA/EXTRA_GET", "/LINK_AJAX_FILTRA/EXTRA_GET", "/LINK_AJAX_ANNULLA_FILTRO/EXTRA_GET", "/LINK_AJAX_AGGIORNA_TUTTO/EXTRA_GET", "/PICKER/LINK/EXTRA_GET")
	,"EXTRA_RE_GET" => array("/LINK_GENERICO/EXTRA_RE_GET","/LINK_CREA/EXTRA_RE_GET","/LINK_AGGIORNA/EXTRA_RE_GET","/LINK_ELIMINA/EXTRA_RE_GET", "/LINK_FILTRA/EXTRA_RE_GET", "/LINK_ANNULLA_FILTRO/EXTRA_RE_GET", "/LINK_AGGIORNA_TUTTO/EXTRA_RE_GET", "/PICKER/LINK/EXTRA_RE_GET",
					"/LINK_AJAX_GENERICO/EXTRA_RE_GET","/LINK_AJAX_CREA/EXTRA_RE_GET","/LINK_AJAX_AGGIORNA/EXTRA_RE_GET","/LINK_AJAX_ELIMINA/EXTRA_RE_GET", "/LINK_AJAX_FILTRA/EXTRA_RE_GET", "/LINK_AJAX_ANNULLA_FILTRO/EXTRA_RE_GET", "/LINK_AJAX_AGGIORNA_TUTTO/EXTRA_RE_GET", "/PICKER/LINK/EXTRA_RE_GET")
	,"LINK" => array("/PICKER/LINK", "/PICKER_FILTRO/LINK", "/COD_DESC/LINK", "/COD_DESC_FILTRO/LINK")
);
// migliorare il linguaggio: XTABLE/TBODY/TR/!(XTABLE)/TD, XTABLE/TBODY/*/!(XTABLE)/TD, XTABLE/TBODY/*/!(XTABLE|TABLE)/TD, etc.

/** 
 * Inclusione delle funzioni di uso comune
 */ 
require_once("funzioni_generali.inc");
/** 
 * Inclusione delle classi in cui trasformare i tag
 */ 
require_once("TAG.inc");


/** 
 * Parser dei documenti XHTML. Ha lo scopo di trasformare un documento XML in un
 * albero di oggetti-tag su cui richiamare la trasformazione in HTML.
 * @package tag
 */
class PARSER 
{
	public static $time = array('PARSE' => 0, 'NUM' => 0);


	/**
	 * Stringa contenente la descrizione dell'ultimo errore di parsing, impostata solo se la funzione {@link} parsifica ritorna null
	 * @var array
	 */
	public $errore;

	/**
	 * Stack contenente gli oggetti-tag istanziati durante l'analisi del sorgente XML, impilati secondo la nidificazione sul sorgente stesso
	 * @var array
	 */
	private $pila;

	/**
	 * Indicazione del percorso (sequenza di tag nidificati) in cui il parser si trova al momento, guida l'istanziazione dei tag contestuali (vedi {@link $CONTESTI_CLASSE})
	 * @var string
	 */
	private $contesto;

	/**
	 * Indica il livello di nidificazione, utile per l'identificazione univoca dei tag al momento della chiusura (vedi {@link _chiusura})
	 * @var string
	 */
	private $livello;


	/**
	 * dice come trattare tag e attributi (lowercase, as-is)
	 * @var boolean
	 */
	private $XML = FALSE;

	/**
	 * Dice se il contesto passato matcha con il contesto attuale, in base anche ai meta-caratteri (*, !, |, etc.)
	 * @access protected
	 * @param string $contesto contesto da verificare con {@link $contesto}
	 * @return bool
	 */
	private function _match($contesto)
	{
		$_contesto_attuale = array_reverse(explode("/", $this->contesto));
		$_contesto = array_slice(explode("/", $contesto), 1);

		// cerco dal fondo il primo elemento di $_contesto_attuale che sia uguale al primo di $contesto
		if (($_inizio = array_search($_contesto[0], $_contesto_attuale)) === FALSE) {
			return false;
		} else {
			$_contesto_attuale = array_reverse(array_slice($_contesto_attuale, 0, $_inizio + 1));
			for($i = 0; $i < count($_contesto); $i ++) {
				switch($_contesto[$i][0]) {
				case "*":
					// devo cercare il prossimo token a partire dal punto attuale
					if (($_inizio = array_search($_contesto[$i + 1], $_contesto_attuale)) === FALSE) {
						return false;
					} else {
						$_contesto_attuale = array_slice($_contesto_attuale, $_inizio);
					}
					break;
				case "!":
					if ($_contesto[$i] == $_contesto_attuale[0]) {
						return false;
					} else {
						$_contesto_attuale = array_slice($_contesto_attuale, 1);
					}
				default:
					if ($_contesto[$i] != $_contesto_attuale[0]) {
						return false;
					} else {
						$_contesto_attuale = array_slice($_contesto_attuale, 1);
					}
				}
			}
			return true;
		}
	}


	/**
	 * E' fondamentale capire se il tag e' istanziabile nel contesto attuale ({@link $contesto}). Se la classe non ha contesti di validitïa' particolari, allora e' istanziabile in qualunque contesto. Se e' attivabile in un solo contesto, il controllo e' puntuale e diretto. Se e' attivabile in piu' contesti, bisogna cercare fra tutti.
	 * @access protected
	 * @param string $classe classe di cui verificare l'istanziabilita'
	 * @return bool
	 */
	private function _contesto_corretto($classe)
	{
		global $CONTESTI_CLASSE;

		if (array_key_exists($classe, $CONTESTI_CLASSE)) {
			if (is_string($CONTESTI_CLASSE[$classe])) { 
				return $this->_match($CONTESTI_CLASSE[$classe]);
			} else {
				foreach($CONTESTI_CLASSE[$classe] as $_contesto) {
					if ( $this->_match($_contesto) ) {
						return true;
					}
				}
				log_value("Contesto non trovato per $classe");
				return false;
			}
		} else {
			return true;
		}
	}

	/**
	 * Dice se, nel contesto attuale, esiste una classe associata ad un certo tag, e se esiste, qual e'. Ad un tag corrisponde una classe se esiste una classe chiamata come il tag, oppure al tag e' associata una classe specifica, e se ci troviamo nel contesto corretto ({@link _contesto_corretto}). Ritorna false o il nome della classe.
	 * @access protected
	 * @param string $tag tag aperto dal parser
	 * @return mixed
	 */
	private function _esiste_classe($tag)
	{
		global $CLASSI_TAG;
		global $CONTESTI_CLASSE;
		
		// prendo tutte le classi che quel tag puo' avere:
		// quelle specificate, se esistono, o quella chiamata come il tag
		if (array_key_exists($tag, $CLASSI_TAG)) {
			$_classi = $CLASSI_TAG[$tag];
		} else {
			$_classi = $tag;
		}

		// se puo' avere solo una classe, controllo puntuale
		// altrimenti ciclo per ogni possibilita'

		// la classe associata al tag esiste se la classe PHP stessa esiste
		// e se sono nel contesto corretto
		if (is_string($_classi)) {
			// il contesto puo' essere definito indipendentemente sulla classe o sul tag, se l'associazione
			// e' univoca - questo non vale se l'associazione e' molteplice
			if (class_exists($_classi) && 
					(
						(array_key_exists($_classi, $CONTESTI_CLASSE) && $this->_contesto_corretto($_classi))
						|| 
						(array_key_exists($tag, $CONTESTI_CLASSE) && $this->_contesto_corretto($tag))
						|| 
						(!array_key_exists($tag, $CONTESTI_CLASSE) && !array_key_exists($_classi, $CONTESTI_CLASSE))
					)
				) {
				return $_classi;
			}
		} else {
			// qui la classe deve esistere PER FORZA, perche' la ho specificata io a mano,
			// quindi evito un controllo inutile
			// quindi esiste una classe associata al tag solo se mi trovo nel contesto corretto
			// come detto sopra, qui il contesto deve essere pfz definito sulla classe, non sul tag
			foreach($_classi as $_classe) {
				if ($this->_contesto_corretto($_classe)) {
					return $_classe;
				}
			}
			// se non ho trovato nulla, non esiste, in questo contesto,
			// nessuna classe associata al tag
			return false;
		}

	}

	/**
	 * Dice se il nodo in cima alla {@link $pila} e' un contenitore.
	 * @access protected
	 * @return bool
	 */
	private function _in_contenitore()
	{
		if (count($this->pila) > 0) {
			return $this->pila[0]->is_contenitore();
		} else {
			return false;
		}
	}

	/**
	 * Aggiunge una sezione testuale del sorgente nel contenitore in cima alla {@link $pila}. Da chiamare solo se siamo {@link _in_contenitore}.
	 * @access protected
	 * @param string $dati
	 */
	private function _aggiungi_a_contenitore($dati)
	{
		$this->pila[0]->aggiungi_dati($dati);
	}

	/**
	 * Routine di gestione dell'evento di apertura di un tag nel sorgente XML dal parser. Se {@link _esiste_classe} associata al tag nel contesto corrente, una sua istanza viene generata e aggiunta alla {@link $pila} e collegata all'oggetto padre; altrimenti, se siamo {@_link _in_contenitore} viene aggiunto il testo di apertura ai suoi dati.
	 * @param resource $parser
	 * @param string $nome il nome del tag che viene chiuso
	 * @param array $attributi coppie nome - valore degli attributi del tag
	 * @access protected
	 */
	private function _apertura($parser, $nome, $attributi)
	{
		global $TAG_SINGOLI;
		global $TAG_HTML;
		$_nome = strtoupper(str_replace("-", "_", $nome));
		if (inizia_per($nome, "ubk:")) $_nome = substr($_nome, 4);


		// incremento il livello, come prima cosa; decrementandolo
		// come ultima cosa, mantengo la simmetria
		$this -> livello ++;
		// sposto l'contesto
		$this -> contesto .= "/$_nome";

		// se la classe esiste, creo un nuovo nodo usando la classe giusta
		// altrimenti, se sono dentro una sezione "contenitore", aggiungo un pezzo

		if ((inizia_per($nome, "ubk:") || in_array(strtoupper($nome), $TAG_HTML)) && $_classe = $this->_esiste_classe($_nome)) {
		
			// creo il nodo e gli imposto gli attributi basilari
			$_nome_classe = strtoupper(inizia_per($nome, "ubk:") ? substr($nome, 4) : $nome);
			$_nodo   = new $_classe($_nome_classe);
			$_nodo -> attributi = array_change_key_case($attributi, CASE_UPPER);
			$_nodo -> livello_in_sorgente = $this->livello;
			
			// aggiungo il figlio al suo eventuale padre
			if (count($this->pila) > 0) {
				$_padre =& $this->pila[0];
				$_padre -> aggiungi_figlio($_nodo);
			}
			
			// metto il figlio sullo stack
			array_unshift($this->pila, $_nodo);

		} elseif ($this->_in_contenitore()) {

			if ($this->XML) {
				$_dati = "<".$nome.xml_att_crea($attributi, TRUE).">";
			} else {
				if (in_array(strtoupper($nome), $TAG_SINGOLI)) {
					$_dati = "<".strtolower($nome).xml_att_crea($attributi)."/>";
				} else {
					$_dati = "<".strtolower($nome).xml_att_crea($attributi).">";
				}
			}
			
			$this->_aggiungi_a_contenitore($_dati);
		}
	}

	/**
	 * Routine di gestione dell'evento di chiusura di un tag nel sorgente XML dal parser. Se abbiamo chiuso il nodo in cima alla {@link $pila}, esso ne viene rimosso; altrimenti, se siamo {@_link _in_contenitore} viene aggiunto il testo di chiusura ai suoi dati.
	 * @param resource $parser
	 * @param string $nome il nome del tag che viene chiuso
	 * @access protected
	 */
	private function _chiusura($parser, $nome)
	{
		global $TAG_SINGOLI;

		// se ho chiuso il tag in cima alla pila, lo tolgo
		// altrimenti, aggiungo dell'html di chiusura al contenitore corrente,
		// se il tag non e' uno di quelli da chiudere necessariamente inline
		if ($this->pila[0]->livello_in_sorgente == $this->livello) {

			array_shift($this->pila);
		
		} elseif ($this->XML && $this->_in_contenitore()) {

			$this->_aggiungi_a_contenitore("</".$nome.">");

		} elseif (!in_array(strtoupper($nome), $TAG_SINGOLI) && $this->_in_contenitore()) {
		
			$this->_aggiungi_a_contenitore("</".strtolower($nome).">");
		}

		// infine, aggiorno il contesto e decremento il livello
		$this -> contesto = dirname($this->contesto);
		$this->livello --;
	}

	/**
	 * Routine di gestione delle sezioni dati del sorgente XML. Se siamo {@_link _in_contenitore} viene aggiunto il testo ai suoi dati; altrimenti, se c'e' un oggetto non-contenitore in cima alla pila, il testo va a costruire il suo contenuto.
	 * @access protected
	 */
	private function _dati($parser, $dati)
	{
		// molto semplicemente, se sono in un contenitore, li aggiungo
		// ad esso
		if ($this->_in_contenitore()) {
			$this->_aggiungi_a_contenitore($dati);
		// se invece c'e' un tag normale, costruisco il suo contenuto (serve per
		// quei tag come <PHP>, che hanno contenuto lungo e zero attributi)
		} elseif (count($this->pila) > 0) {
			$this->pila[0]->contenuto .= $dati;
		}
	}

	/**
	 * Esegue il parsing del testo XML passato. Crea un parser SAX, imposta le routine di gestione evento ({@link _apertura, _chiusura, _dati}) e lancia il processo. Prima di questo vengono eseguite delle trasformazioni XSLT sugli oggetti di tipo grafico. Incrementa {@link $PARSE_TIME $PARSE_NUM}. Ritorna null se il parsing non va a buon fine.
	 * @param string $xml testo XML da parsificare
	 * @return TAG
	 */
	public function &parsifica($filename, $xml = NULL, $xml_type = FALSE, $xsl = TRUE)
	{
		// scrittura cache
		$cache = CACHE_CLASS_FACTORY::get();
		$page = $cache->page_get($filename);
		if ($page) return $page;

		$this->XML = $xml_type;
		$null = null;


		if (is_null($xml) && !strlen($xml = file_get_contents($filename)))
			throw new CodeException("$filename vuoto o non trovato.");
		
		// trasformazione xsl
		if ($xsl) $xml = xsl_transform($xml);
//		$_curdir = getcwd();
//		chdir(PHDIR);
//		scrivi_file("system/debug.xml",$xml);
//		chdir($_curdir);

		// resetto tutte le variabili
		$this->pila   = array();
		$this->contesto = "";
		$this->livello == 0;

		// creo una radice fittizia che contiene tutto il file
		$_radice = new PAGINA("PAGINA");
		$_radice -> livello_in_sorgente = 0;
		array_push($this->pila, $_radice);
		
		// creo il parser e imposto gli handler di evento
		$_parser = xml_parser_create();
		xml_set_object($_parser, $this);
		xml_set_element_handler($_parser, "_apertura", "_chiusura");
		xml_set_character_data_handler($_parser, "_dati");
		xml_parser_set_option($_parser, XML_OPTION_SKIP_WHITE, 1);
		if ($this->XML) xml_parser_set_option($_parser, XML_OPTION_CASE_FOLDING, 0);


		// parsing php
		if (constant_true('LOG_STATS')) $_start = get_microtime();

		if (!xml_parse($_parser, xml_carica($xml), TRUE)) {
			$this->errore =		xml_error_string(xml_get_error_code($_parser)).BR.
								"linea ".xml_get_current_line_number($_parser).BR.
								"colonna ".xml_get_current_column_number($_parser).BR.
								"in file '$filename'".
								BR."[PARSER.parsifica]";
			return $null;
		}
			
		if (constant_true('LOG_STATS')) {
			$_end = get_microtime();
			self::$time['PARSE'] += ($_end - $_start);
			self::$time['NUM'] ++;
		}


		$cache->page_write($filename, $this->pila[0]);

		// ritorno la radice, che contiene tutto il documento
		return $this->pila[0];
	}
}

?>