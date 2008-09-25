<?php
/**
 * Classe base per l'esecuzione delle azioni comandate dai link delle pagine generate
 * @author Ubik <leporati@immaginazioneelavoro.it>
 * @package gateway
 */

/** 
 * Inclusione delle classi di gestione dati
 */ 
require_once('c_accesso_db.inc');
/** 
 * Inclusione del parser
 */ 
require_once('c_parser.inc');

/** 
 * Esegue le azioni comandate dalla pagina HTML chiamante via GET sui dati passati via POST
 * @package gateway
 */
class AZIONI_FORM
{
	/**
	 * Conteggi per le statistiche di esecuzione
	 * @var array
	 * @static
	 */
	public static $time = array('TRANS' => 0, 'TOTAL' => 0);

	/**
	 * Parametri di esecuzione: ID, AZIONE, TABELLA, CAMPO-ID, PAGE, PAGE-SIZE, FILE, MODE, ORDINE
	 * @var array
	 */
	protected $parametri = array();

	/**
	 * Indica se bisogna o meno invocare il metodo carica alla fine dell'esecuzione dell'azione. Puo' essere disabilitata via codice nei metodi riscritti
	 * @var bool
	 */
	protected $esegui_carica = TRUE;

	/**
	 * Costruttore - imposta i {@link $parametri} di default e quindi li sovrascrive con i valori attuali arrivati su $_GET
	 * @param array $defaults
	 */
	public function __construct($defaults = array())
	{
		$this->parametri = $defaults;

		foreach($_GET as $parametro => $valore) {
			if (inizia_per($parametro, "amp;"))
				$parametro = substr($parametro, 4);
			$this->parametri[$parametro] = $valore;
		}

		if (array_key_exists("ORDINE", $this->parametri) && $this->parametri['ORDINE'] == "") {
			unset($this->parametri['ORDINE']);
		}

		$this->parametri['GATEWAY'] = $_SERVER['PHP_SELF'];

		$this->esegui_carica = cbool($this->parametro('ESEGUI-CARICA', 'true'));

		$this->imposta_filtro_def();
	}

	/** 
	 * Lettura parametro opzionale
	 * @param string $nome nome del parametro
	 * @return mixed
	 */
	public function parametro($nome, $default = NULL)
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
	 * Impostazione parametro
	 * @param string $nome nome del parametro
	 * @param mixed $valore valore del parametro
	 */
	public function imposta_parametro($nome, $valore)
	{
		$this->parametri[$nome] = $valore;
	}

	/**
	 * Azione di visualizzazione (maschera di elenco) - rimuove i filtri e gli ordinamenti in sessione per la pagina corrente
	 * @param GESTORE &$rs
	 */
	public function mostra(&$rs)
	{
		$_file = str_replace("/","_", array_get("FILE", $this->parametri));
		unset($_SESSION["filtro_$_file"]);
		unset($_SESSION["campi_filtro_$_file"]);
		unset($_SESSION["ordine_$_file"]);
		unset($_SESSION["campi_ordine_$_file"]);
		$this -> imposta_filtro_def();
	}

	/**
	 * Azione di visualizzazione puntuale (una riga di dettaglio) - rimuove i filtri e gli ordinamenti in sessione per la pagina corrente
	 * @param GESTORE &$rs
	 */
	public function mostra_riga(&$rs)
	{
		$_file = str_replace("/","_", array_get("FILE", $this->parametri));
		unset($_SESSION["filtro_$_file"]);
		unset($_SESSION["campi_filtro_$_file"]);
		unset($_SESSION["ordine_$_file"]);
		unset($_SESSION["campi_ordine_$_file"]);
//		$this->parametri["PAGE"] = 0;
		$this -> imposta_filtro_def();
	}


	/**
	 * Carica direttamente l'ultimo record inserito nella tabella
	 * @param GESTORE &$rs
	 */
	public function mostra_ultimo_inserito(&$rs)
	{
		$this->parametri['ID'] = $rs -> ultimo_id();
		$this->mostra_riga($rs);
	}

	/**
	 * Azione di creazione riga usando i dati in $_POST
	 * @param GESTORE &$rs
	 */
	public function crea(&$rs)
	{
		$rs -> crea($_POST);
	}

	/**
	 * Azione di aggiornamento riga (quella identificata dal parametro ID) con i valori in $_POST
	 * @param GESTORE &$rs
	 */
	public function aggiorna(&$rs)
	{
		$id =  $this->parametro_richiesto('ID');
		$rs -> aggiorna($id, $_POST);
	}

	/**
	 * Azione di aggiornamento di tutte le righe presenti nella pagina. Si aspetta un array nel post contenente gli id delle righe da aggiornare, che ha come nome quello del campo ID presente nei parametri o quello del gestore in alternativa
	 * @param GESTORE &$rs
	 */
	public function aggiorna_tutto(&$rs)
	{
		$campo_id = $this->parametro('CAMPO-ID', $rs->campo_id);

		if (isset($_POST[$campo_id])) {
		
			$ids = $_POST[$campo_id];
			unset($_POST[$campo_id]);

			$_old_id = array_get_default('ID', $this->parametri);

			$rs->tr_begin();
				foreach($ids as $id) {
					$this->parametri['ID'] = $id;
					$this->aggiorna($rs);
				}
			$rs->tr_commit();

			$this->parametri['ID'] = $_old_id;
		}
	}


	/**
	 * Azione di aggiornamento di una tabella di legame (vedi checkbox-fk). Si aspetta delle checkbox.
	 * @param GESTORE &$rs
	 */
	public function aggiorna_fk(&$rs)
	{
		$campo_fk = array_get("CAMPO-FK", $this->parametri);
		$valore_fk = array_get("VALORE-FK", $this->parametri);

		$rs->tr_begin();
			$rs->elimina_dove(equ($campo_fk, $valore_fk));

			$campo_id = array_get_default("CAMPO-ID", $this->parametri, $rs->campo_id);
			if (isSet($_POST[$campo_id])) {
				foreach(array_keys($_POST[$campo_id]) as $id) {
					$rs->crea(emula_post('-1', array($campo_fk => $valore_fk, $campo_id => $id)));
				}
			}
		$rs->tr_commit();

	}


	/**
	 * Azione di eliminazione riga (quella identificata dal parametro ID)
	 * @param GESTORE &$rs
	 */
	public function elimina(&$rs)
	{
		$id =  $this->parametro_richiesto('ID');
		$rs -> elimina($id);
	}

	/**
	 * Azione di filtraggio (ricerca) - viene costruita la condizione di filtro usando i valori passati in $_POST["filtro"]
	 * @param GESTORE &$rs
	 */
	public function filtra(&$rs)
	{
		
		// prendo tutti i parametri di filtro
		if (isset($_POST["filtro"])) {
			$_file       = str_replace("/","_", array_get('FILE', $this->parametri));
			$_condizioni = array();
			// resetto il filtro sulla maschera
			$_SESSION["campi_filtro_$_file"] = array();
			// ciclo sui campi filtro
			foreach($_POST["filtro"] as $op_campo => $valore) {
				$op_campo = explode("|", $op_campo);
				$_op      = $op_campo[0];
				$_campo   = $op_campo[1];
				// se ho un confronto, lo metto nelle condizioni e salvo il valore di filtro
				// alrimenti resetto il valore di default per il campo
				if ((is_array($valore) && count($valore) > 0) || (!is_array($valore) && strlen($valore) > 0)) {
					if (in_array($_campo, $rs->campi)) {
						$_SESSION["campi_filtro_$_file"][$_op.$_campo] = $valore;
						switch ($_op) {
						case "in_tp":
						case "all_tp":
							array_push($_condizioni, $_op($op_campo[2], $op_campo[1], $op_campo[3], $valore));
							break;
						case 'op_tp':
							array_push($_condizioni, $_op($op_campo[4],$op_campo[2], $op_campo[1], $op_campo[3],$op_campo[1] ,$valore));
							break;
						default:
							array_push($_condizioni, $_op($_campo, $valore));
						}
					} elseif ($_op == "op_tp" && in_array($op_campo[5], $rs->campi)) {
						$_SESSION["campi_filtro_$_file"][$_op.$op_campo[5]] = $valore;
						array_push($_condizioni, $_op($op_campo[1], $op_campo[4], $op_campo[3], $op_campo[2], $op_campo[5], $valore));
					} else {
						log_value("AZIONI_FORM::filtra - $_campo non in ".$rs->tabella);
					}
				} else {
					unset($_SESSION["campi_filtro_$_file"][$_op.$_campo]);
				}
			}
			if (count($_condizioni) > 0){
				$_SESSION["filtro_$_file"] = implode(" AND ", $_condizioni);
			} else {
				unset($_SESSION["filtro_$_file"]);
			}
		}
	}

	/**
	 * Come filtra, ma ritorna il filtro in una stringa senza impostarlo in sessione
	 * @param GESTORE &$rs
	 * @return string
	 */
	public function condizione_filtro(&$rs)
	{
		$_condizioni = array();
		// prendo tutti i parametri di filtro
		if (isset($_POST["filtro"])) {
			// ciclo sui campi filtro
			foreach($_POST["filtro"] as $op_campo => $valore) {
				$op_campo = explode("|", $op_campo);
				$_op      = $op_campo[0];
				$_campo   = $op_campo[1];
				// se ho un confronto, lo metto nelle condizioni e salvo il valore di filtro
				// alrimenti resetto il valore di default per il campo
				if ((is_array($valore) && count($valore) > 0) || (!is_array($valore) && strlen($valore) > 0)) {
					if (in_array($_campo, $rs->campi)) {
						switch ($_op) {
						case "in_tp":
						case "all_tp":
							array_push($_condizioni, $_op($op_campo[2], $op_campo[1], $op_campo[3], $valore));
							break;
						case 'op_tp':
							array_push($_condizioni, $_op($op_campo[4],$op_campo[2], $op_campo[1], $op_campo[3],$op_campo[1] ,$valore));
							break;
						default:
							array_push($_condizioni, $_op($_campo, $valore));
						}
					} elseif ($_op == "op_tp" && in_array($op_campo[5], $rs->campi)) {
						array_push($_condizioni, $_op($op_campo[1], $op_campo[4], $op_campo[3], $op_campo[2], $op_campo[5], $valore));
					} else {
						log_value("AZIONI_FORM::condizione_filtro - $_campo non in ".$rs->tabella);
					}
				}
			}
		}

		if (count($_condizioni) > 0){
			return implode(" AND ", $_condizioni);
		} else {
			return NULL;
		}
	}

	/**
	 * Annulla il filtro corrente della pagina
	 * @param GESTORE &$rs
	 */
	public function annulla_filtro(&$rs)
	{
		$_file = str_replace("/","_", array_get('FILE', $this->parametri));
		unset($_SESSION["filtro_$_file"]);
		unset($_SESSION["campi_filtro_$_file"]);
		$this->parametri["PAGE"] = 0;
		$this->imposta_filtro_def();
	}

	/**
	 * Imposta il filtro della pagina
	 * @return string
	 */
	public function imposta_filtro($filtro, $file = NULL)
	{
		if (is_null($file)) {
			$_file = str_replace("/","_", array_get('FILE', $this->parametri));
		} else {
			$_file = str_replace("/","_", $file);
		}

		$_SESSION["filtro_$_file"] = $filtro;
		$_SESSION["campi_filtro_$_file"] = array();
	}

	/**
	 * Imposta il filtro di default per la pagina. Tale filtro viene cercato fra i parametri, chiave FILTRO-DEF
	 */
	public function imposta_filtro_def($filtro = NULL, $distinct = NULL, $colonne = NULL)
	{
		if (is_null($filtro))
			$filtro = array_get_default("FILTRO-DEF", $this->parametri);
		if (is_null($distinct))
			$distinct = array_get_default("DISTINCT", $this->parametri);
		if (is_null($colonne))
			$colonne = array_get_default("COLONNE", $this->parametri);

		if (array_key_exists("FILE", $this->parametri)) {
			$_file = str_replace("/","_", $this->parametri["FILE"]);
			if (!is_null($filtro))
				$_SESSION["filtro_def_$_file"] = $filtro;
			if (!is_null($distinct))
				$_SESSION["filtro_def_distinct_$_file"] = $distinct;
			if (!is_null($colonne))
				$_SESSION["filtro_def_colonne_$_file"] = $colonne;
		} elseif (!is_null($filtro) || !is_null($distinct) || !is_null($colonne)) {
			throw new CodeException("Nessun file su cui impostare il filtro di default.");
		}
	}

	/**
	 * Annulla il filtro di default corrente della pagina
	 */
	public function annulla_filtro_def()
	{
		if (array_key_exists("FILE", $this->parametri)) {
			$_file = str_replace("/","_",$this->parametri["FILE"]);
			unset($this->parametri["FILTRO-DEF"]);
			unset($this->parametri["COLONNE"]);
			unset($this->parametri["DISTINCT"]);
			unset($_SESSION["filtro_def_$_file"]);
			unset($_SESSION["filtro_def_distinct_$_file"]);
			unset($_SESSION["filtro_def_colonne_$_file"]);
		}
	}

	/**
	 * Imposta o rimuove le informazioni di ordinamento in base ai parametri passati via GET - l'ordinamento viene mantenuto su un solo campo, a differenza dell'azione {@link ordina} che lo mantiene su pi campi
	 * @param GESTORE &$rs
	 */
	public function ordina_singolo(&$rs)
	{
		$_campo = array_get("CAMPO", $_GET);
		$_direz = strtoupper(array_get("DIREZIONE", $_GET));
		$_file  = str_replace("/","_", array_get('FILE', $this->parametri));
		
		if ($_direz == "NONE") {
			unset($_SESSION["campi_ordine_$_file"]);
			unset($_SESSION["ordine_$_file"]);
		} else {
			$_SESSION["campi_ordine_$_file"] = array();
			$_SESSION["campi_ordine_$_file"][$_campo] = $_direz;
			$_SESSION["ordine_$_file"] = $_campo." ".$_direz;
		}
	}

	/**
	 * Imposta o rimuove le informazioni di ordinamento in base ai parametri passati via GET
	 * @param GESTORE &$rs
	 */
	public function ordina(&$rs)
	{
		$_campo = array_get("CAMPO", $_GET);
		$_direz = strtoupper(array_get("DIREZIONE", $_GET));
		$_file  = str_replace("/","_", array_get('FILE', $this->parametri));
		if (!isset($_SESSION["campi_ordine_$_file"])) {
			$_SESSION["campi_ordine_$_file"] = array();
		}
		if ($_direz == "NONE") {
			unset($_SESSION["campi_ordine_$_file"][$_campo]);
		} else {
			$_SESSION["campi_ordine_$_file"][$_campo] = $_direz;
		}
		if (count($_SESSION["campi_ordine_$_file"]) == 0) {
			unset($_SESSION["ordine_$_file"]);
		} else {
			$_ord = array();
			foreach($_SESSION["campi_ordine_$_file"] as $campo => $direz) {
				array_push($_ord, "$campo $direz");
			}
			$_SESSION["ordine_$_file"] = implode(", ", $_ord);
		}
	}

	/**
	 * Da chiamare come reazione sulle pop-up quando si vuole chiuderle
	 * @param GESTORE &$rs
	 */
	public function chiudi_pop_up(&$rs)
	{
		echo	 "<html>"
				,"<head>"
				,'<script language="Javascript">'
				,'if (window.opener != null) {'
				,'	window.opener.focus();'
				,'}'
				,'window.close();'
				,'</script>'
				,'</head>'
				,'</html>';
		exit();
	}

	/**
	 * Da chiamare come reazione sulle pop-up quando si vuole chiuderle e aggiornare la pagina che ha aperto la pop-up
	 * @param GESTORE &$rs
	 */
	public function chiudi_pop_up_refresh(&$rs)
	{
		echo	 "<html>"
				,"<head>"
				,'<script language="Javascript">'
				,'if (window.opener != null) {'
				,'	window.opener.location.reload();'
				,'	window.opener.focus();'
				,'}'
				,'window.close();'
				,'</script>'
				,'</head>'
				,'</html>';
		exit();
	}

	/**
	 * Esegue la stored NOME-STORED con i parametri PARAMETRO-0, PARAMETRO-1, ..., PARAMETRO-N
	 * @param GESTORE &$rs
	 */
	public function esegui_stored_procedure(&$rs)
	{
		$params = array($this->parametro_richiesto('NOME-STORED'));
		foreach(array_keys($this->parametri) as $param) {
			if (inizia_per($param, 'PARAMETRO')) {
				$params[(int)substr($param, 10) + 1] = $this->parametro($param);
			}
		}

		call_user_func_array(array($rs, 'esegui_stored_procedure'), $params);
	}

	/**
	 * Ritorna il filtro in sessione per il file presente nei parametri, risultato della combinazione fra il filtro di default e il filtro della maschera, NULL se non e' presente alcun filtro
	 * @return string
	 */
	public function filtro_pagina($_file = NULL)
	{
		if (is_null($_file))
			$_file = $this->parametro_richiesto('FILE');
		$_file = str_replace("/","_", $_file);

		if (isset($_SESSION["filtro_$_file"]) && isset($_SESSION["filtro_def_$_file"])) {
			return c_and($_SESSION["filtro_$_file"], $_SESSION["filtro_def_$_file"]);
		} else if (isset($_SESSION["filtro_$_file"])) {
			return $_SESSION["filtro_$_file"];
		} else if (isset($_SESSION["filtro_def_$_file"])) {
			return $_SESSION["filtro_def_$_file"];
		} else {
			return NULL;
		}
	}

	/**
	 * Questo metodo viene sempre richiamato terminata l'esecuzione dell'azione comandata via GET - esegue il caricamento ordinato ed eventualmente filtrato sul gestore passato
	 * @param GESTORE &$rs
	 */
	protected function carica(&$rs)
	{
		$_file = str_replace("/","_", array_get('FILE', $this->parametri));

		if (isset($this->parametri['ORDINE']) && !in_array($this->parametri['AZIONE'], array("ordina_singolo", "ordina"))) {
			$ords = array_map("trim", explode(",", $this->parametri['ORDINE']));
			$_SESSION["ordine_$_file"] = $this->parametri['ORDINE'];
			foreach($ords as $ord) {
				$aord = @explode(" ", $ord);
				$campo = trim(array_get(0, $aord));
				$direz = trim(strtoupper(array_get_default(1, $aord, 'ASC')));
				$_SESSION["campi_ordine_$_file"][$campo] = $direz;
			}
		}
		
		
		$_ordine = array_get_default("ordine_$_file", $_SESSION,
						array_get_default("ORDINE", $this->parametri, NULL)
					);
		$_distinct = array_get_default("DISTINCT", $this->parametri, FALSE);
		$_colonne = array_get_default("COLONNE", $this->parametri, NULL);


		switch($this->parametri["AZIONE"]) {
		case "mostra_riga":
		case "mostra_ultimo_inserito":
			$id = array_get("ID", $this->parametri);
			$rs->carica_righe_id($id, $_ordine, $_colonne, $_distinct);
			break;
		default:
			$_filtro = $this->filtro_pagina();
			if (!is_null($_filtro)) {
				$_distinct = array_get_default("filtro_def_distinct_$_file", $_SESSION, false);
				$_colonne  = array_get_default("filtro_def_colonne_$_file", $_SESSION, $_colonne);
				$rs->carica($_filtro, $_ordine, $_colonne, $_distinct);
			} else {
				$rs->carica(NULL, $_ordine, $_colonne, $_distinct);
			}
		}
	}

	/**
	 * Esecuzione dell'azione presente in $parametri["AZIONE"] - viene richiamato il metodo con il nome corrispondente, parsificato il file $parametri["FILE"] e trasformato con gli stessi parametri
	 * @param GESTORE &$rs
	 * @param bool $check_url Se FALSE, disabilita il check degli URL
	 */
	public function esegui(&$rs, $check_url = TRUE)
	{
		if ($check_url && !url_valida())
			throw new FatalException("Checksum errato.");

		if (constant_true('LOG_STATS')) $_total_start = get_microtime();

		$_azione      = $this->parametro_richiesto('AZIONE');

		if (method_exists($this, $_azione)) {
			// eseguo l'azione
			call_user_func(array($this, $_azione), $rs);
			// se ho una reazione comandata, faccio un redirect
			if (isset($this->parametri["R-GATEWAY"])) {
				$_params = array("R-AZIONE", "R-TABELLA", "R-FILE", "R-MODE", "R-PAGE", "R-PAGE-SIZE", "R-CAMPO-ID", "R-ORDINE", "R-ID");

				$_head = "";
				foreach($_params as $_param) {
					if (array_key_exists($_param, $this->parametri))
						$_head .= "&".substr($_param, 2)."=".$this->parametri[$_param];
				}

				foreach(array_keys($_GET) as $_param_reazione) {
					if (!in_array($_param_reazione, $_params) && substr($_param_reazione, 0, 2) == 'R-')
						$_head .= "&".substr($_param_reazione, 2)."=".$this->parametri[$_param_reazione];
				}

				if (strlen($_head)) {
					$_url = $this->parametri["R-GATEWAY"]."?".substr($_head, 1);
				} else {
					$_url = $this->parametri["R-GATEWAY"];
				}

				// aggiungo un parametro per indicare che e' una reazione cosi' la funzione di decodifica
				// non va a male
				$_url .= "&IS-REAZIONE=1";

				$_url = url_codifica($_url);
				$_url = str_replace("&amp;","&",$_url);
				header("Location: ".$_url);

				exit;
			// altrimenti e' una cosa tipo mostra
			} else {
				$_file = $this->parametro_richiesto('FILE');
				
				$rs -> pagina     = $this->parametro("PAGE", 0);
				$rs -> dim_pagina = $this->parametro("PAGE-SIZE", 0);

				if ($this->esegui_carica) $this->carica($rs);

				$this->parametri["GESTORE"] =& $rs;
				$this->parametri["PAGE"]    =  $rs->pagina;
				$this->parametri["FILE"]    =  $_file;

				$_parser = new PARSER();
				$_root =& $_parser->parsifica($_file);
				if (is_null($_root)) 
					throw new CodeException($_parser->errore);

				$this->parametri["PARSER"] =& $_parser;

				echo "<!-- $_file by ".$this->parametri['GATEWAY']." -->";
				if (constant_true('LOG_STATS')) $_start = get_microtime();

				$_root->stampa_html($this->parametri);
				
				if (constant_true('LOG_STATS')) {
					$_end   = get_microtime();
					self::$time['TRANS'] += ($_end - $_start);
					$_total_end = get_microtime();
					self::$time['TOTAL'] = ($_total_end - $_total_start);

					log_timing();
					log_performance($this->parametri);
				}
			}

		} else {
			throw new CodeException("Azione non implementata: $_azione in ".$_SERVER['PHP_SELF']);
		}

	}

}

/** 
 * Esegue le azioni comandate dalla pagina HTML chiamante via GET sui dati passati via POST utilizzando i link di tipo AJAX
 * @package gateway
 */
class AZIONI_FORM_AJAX extends AZIONI_FORM
{

	public function __construct($defaults = array())
	{
		correct_get_post();
		parent::__construct($defaults);
	}

	public function crea(&$rs)
	{
		parent::crea($rs);
		die('+OK');
	}

	public function aggiorna(&$rs)
	{
		parent::aggiorna($rs);
		die('+OK');
	}

	public function elimina(&$rs)
	{
		parent::elimina($rs);
		die('+OK');
	}

	public function aggiorna_tutto(&$rs)
	{
		parent::aggiorna_tutto($rs);
		die('+OK');
	}

	public function aggiorna_fk(&$rs)
	{
		parent::aggiorna_fk($rs);
		die('+OK');
	}

	public function esegui_stored_procedure(&$rs)
	{
		parent::esegui_stored_procedure($rs);
		die('+OK');
	}

	/**
	 * Rispetto alla versione di AZIONI_FORM, non tiene in considerazione alcun tipo di reazione, poiche' queste vengono comandate dal framework javascript
	 * @param GESTORE &$rs
	 * @param bool $check_url Se FALSE, disabilita il check degli URL
	 */
	public function esegui(&$rs, $check_url = TRUE)
	{
		if ($check_url && !url_valida())
			throw new FatalException("Checksum errato.");

		$_azione = $this->parametro_richiesto('AZIONE');

		if (!method_exists($this, $_azione))
			throw new CodeException("Azione non implementata: $_azione in ".$_SERVER['PHP_SELF']);

		if (constant_true('LOG_STATS')) $_total_start = get_microtime();

		// eseguo l'azione
		call_user_func(array($this, $_azione), $rs);
		// mostro la pagina se devo (e se sono arrivato qua)
		if (!cbool($this->parametro('REPLACE', 'TRUE'))) return;

		$_file = $this->parametro_richiesto('FILE');
		
		$rs -> pagina     = $this->parametro("PAGE", 0);
		$rs -> dim_pagina = $this->parametro("PAGE-SIZE", 0);

		if ($this->esegui_carica) $this->carica($rs);

		$this->parametri["GESTORE"] =& $rs;
		$this->parametri["PAGE"]    =  $rs->pagina;
		$this->parametri["FILE"]    =  $_file;

		$_parser = new PARSER();
		$_root =& $_parser->parsifica($_file);
		if (is_null($_root)) 
			throw new CodeException($_parser->errore);

		$this->parametri["PARSER"] =& $_parser;

		echo "<!-- $_file by ".$this->parametri['GATEWAY']." -->";
		if (constant_true('LOG_STATS')) $_start = get_microtime();

		$_root->stampa_html($this->parametri);
		
		if (constant_true('LOG_STATS')) {
			$_end   = get_microtime();
			self::$time['TRANS'] += ($_end - $_start);
			$_total_end = get_microtime();
			self::$time['TOTAL'] = ($_total_end - $_total_start);

			log_timing();
			log_performance($this->parametri);
		}

	}
}

?>