<?php
/**
 * Funzioni di utilita' generale su stringhe, array, classi, file, etc.
 * @author Ubik <emiliano.leporati@gmail.com>
 * @package general
 */

/** 
 * Definizioni delle costanti
 */
require_once('defines.inc');
/** 
 * Funzioni data
 */
require_once('funzioni_data.inc');
/** 
 * Funzioni url
 */
require_once('funzioni_url.inc');
/** 
 * Crittatore
 */
require_once('c_crypter.inc');

/* *********************************************************************************************
 *
 * G E N E R I C H E
 *
 * *********************************************************************************************/

/**
 * Eccezione durante l'interazione con la base dati
 * @package general
 */
class DbException extends Exception
{
	public function __construct($function, $message)
	{
		parent::__construct($function.": ".$message);
	}
}

/**
 * Eccezione a livello di codice in fase di sviluppo
 * @package general
 */
class CodeException extends Exception {}

/**
 * Eccezione a livello applicativo
 * @package general
 */
class AppException extends Exception {}

/**
 * Eccezione fatale
 * @package general
 */
class FatalException extends Exception {}

/**
 * Ritorna TRUE se la costante e' definita e ha valore == TRUE, FALSE altrimenti
 * @param string $constant
 * @return boolean
 */
function constant_true($constant)
{
	if (defined($constant))
		return (bool)constant($constant);
	else
		return FALSE;
}

/**
 * Ritorna il valore della costante se e' definita, $valore-default altrimenti
 * @param string $constant
 * @param mixed $valore_default
 * @return mixed
 */
function constant_def($constant, $valore_default = NULL)
{
	if (defined($constant))
		return constant($constant);
	else
		return $valore_default;
}

/**
 * Negazione logica
 * @param boolean $valore
 * @return boolean
 */
function not($valore) 
{
	return !$valore;
}

/**
 * Ritorna TRUE se il valore passato e' "TRUE", FALSE altrimenti
 * @param string $valore
 * @return bool
 */
function cbool($valore) 
{
	return ($valore === "TRUE" || $valore === "true" ? TRUE : FALSE);
}

/**
 * Ritorna il tempo corrente in millisecondi
 * @return float
 */
function get_microtime()
{
    return array_sum(explode(' ', microtime()));
}

/**
 * Ritorna $else se $value == FALSE (stringa vuota, null, etc)
 * @param mixed $value
 * @param mixed $else
 * @return mixed
 */
function invl($value, $else)
{
	return $value ? $value : $else;
}

/* *********************************************************************************************
 *
 * S T R I N G H E
 *
 * *********************************************************************************************/

/**
 * Tronca la stringa alla dimensione specificata, aggiungendo eventuali ellissi
 * @param string $str
 * @param integer $len
 * @return string
 */
function trim_to($str, $len)
{
	if (strlen($str) <= $len + 3) {
		return $str;
	} else {
		return substr($str, 0, $len) . ELLIPSIS; //'...';
	}
}

/**
 * Ritorna il valore formattato in modo consono per essere inserito in una stringa javascript racchiusa fra apici singoli (apici singoli quotati e caratteri speciali trasformati in entita'
 * @param string $valore
 * @return string
 */
function stringa_js($valore) 
{
	return str_replace(array("'", "\n", "\r"), array("\'", ' ', ''), html_entity_decode($valore, ENT_QUOTES));
}

/**
 * Ritorna il valore formattato come una valuta (. come separatore migliaia, , come separatore decimale, due decimali di default
 * @param mixed $valore
 * @param integer $decimali
 * @return string
 */
function valuta($valore, $decimali = 2) 
{
	return number_format($valore, $decimali, ",", ".");
}

/**
 * Racchiude un valore fra apici singoli
 * @param mixed $valore
 * @return string
 */
function qt($valore)
{
	return "'$valore'";
}

/**
 * Racchiude un valore fra apici doppi
 * @param mixed $valore
 * @return string
 */
function dQt($valore)
{
	return '"'.$valore.'"';
}

/**
 * Racchiude un valore fra parentesi tonde
 * @param mixed $valore
 * @return string
 */
function par($valore)
{
	return '('.$valore.')';
}


/**
 * Dice se $stringa termina per $finale, senza tener conto delle maiuscole e minuscole (case - insensitive)
 * @param string $stringa La stringa in cui verificare il finale
 * @param string $finale La parte terminale da verificare
 * @return bool
 */
function finisce_per($stringa, $finale)
{
	return (strcasecmp(substr($stringa, -strlen($finale)) , $finale) == 0);
}

/**
 * Dice se $stringa inizia per $finale, senza tener conto delle maiuscole e minuscole (case - insensitive)
 * @param string $stringa La stringa in cui verificare l'inizio
 * @param string $inizio La parte iniziale da verificare
 * @return bool
 */
function inizia_per($stringa, $inizio)
{
	return (strcasecmp(substr($stringa, 0, strlen($inizio)) , $inizio) == 0);
}

/**
 * Dice se $stringa termina per $finale, tenendo conto delle maiuscole e minuscole (case - sensitive)
 * @param string $stringa La stringa in cui verificare il finale
 * @param string $finale La parte terminale da verificare
 * @return bool
 */
function case_finisce_per($stringa, $finale)
{
	return (strcmp(substr($stringa, -strlen($finale)) , $finale) == 0);
}

/**
 * Dice se $stringa inizia per $finale, tenendo conto delle maiuscole e minuscole (case - sensitive)
 * @param string $stringa La stringa in cui verificare l'inizio
 * @param string $inizio La parte iniziale da verificare
 * @return bool
 */
function case_inizia_per($stringa, $inizio)
{
	return (strcmp(substr($stringa, 0, strlen($inizio)) , $inizio) == 0);
}

/**
 * Inverso di bin2hex
 * @param string $stringa La stringa da trasformare
 * @return string
 */
function hex2bin($stringa) 
{
	return pack("H".strlen($stringa), $stringa);
}

/**
 * Aggiunge "return" all'inizio e ";" alla fine della stringa, se mancano
 * @param string $stringa La stringa da trasformare
 * @return string
 */
function xml_2_php($stringa) 
{
	$stringa = trim($stringa);
	if (strpos($stringa, "return") === FALSE) {
		$stringa = "return ".$stringa;
	}
	if (substr($stringa, -1) != ";") {
		$stringa .= ";";
	}
	return $stringa;
}

/**
 * Trasforma tutte le & della stringa passata in &amp;
 * @param string $stringa
 * @return string
 */
function xml_amp($stringa)
{
	return preg_replace("/&/", "&amp;", $stringa);
}

/**
 * Rimuove dalla stringa passata tutte le sezioni e i caratteri inutili per il parser XML (commenti, tabulazioni, etc.)
 * @param string $stringa
 * @return string
 */
function xml_carica($stringa)
{
	return preg_replace("/\n|\r|\t|(?s)<!--\.*-->?/","",$stringa);
//	return preg_replace("/(?s)<!--\.*-->?/","",$valore);
}

/**
 * Dato un array di coppie (attributo, valore), ritorna una stringa con spazio iniziale rappresentante tali coppie formattate in formato HTML (attributo="valore")
 * @param array $array_attributi
 * @return string
 */
function xml_att_crea($array_attributi, $xml = FALSE)
{
	$risultato = "";
	if ($xml)
		foreach($array_attributi as $nome => $valore) {
			$risultato .= " ".$nome."=".dQt($valore);
		}
	else
		foreach($array_attributi as $nome => $valore) {
			$risultato .= " ".strtolower($nome)."=".dQt($valore);
		}

	return $risultato;
}

/* *********************************************************************************************
 *
 * I N I
 *
 * *********************************************************************************************/

/**
 * Legge il file .ini specificato ed esegue delle define per ognuna delle chiavi presenti
 * @param string $file Il file .ini
 * @param bool $crittato Indica se i valori dei parametri sono crittati o meno
 */
function leggi_ini($file, $crittato = false)
{
	if (!file_exists($file))
		throw new CodeException("File ini $file non trovato");

	if ($crittato) {
		$cry = new CRYPTER("Chiave in chiaro furbizzima per crittare gli filez iniz");
	}

	$_all_defines = array();
	$_sect = parse_ini_file($file, TRUE);
	
	foreach($_sect as $_sect_name => $_keys) {
		// valori della sezione corrente
		$_vals = array();
		foreach($_keys as $_name => $_value) {
			if ($crittato) {
				$_vals[$_name] = $cry->decrypt($_value);
			} else {
				$_vals[$_name] = $_value;
			}
		}

		// gestisce i valori ini condizionali utilizzando la direttiva MATCH nel file ini su nomehost:porta
		if (array_key_exists('MATCH', $_vals)) {
			$_regexp = "/{$_vals['MATCH']}/";
			if (!isset($_URL)){
				if (isset($_SERVER['HTTP_HOST'])) { 
					$_URL = $_SERVER['HTTP_HOST'].':'.$_SERVER['SERVER_PORT'];
				} else {
					$_URL = false;	
				}
			}
			// se non matcha, non vado avanti
			// quindi non riempio $_all_defines,
			// e i valori di questa sezione non saranno definiti
			if (!preg_match($_regexp, $_URL)) {
				continue;
			}
		}


		// rendo globalmente disponibili i parametri di connessione ai vari DB
		if (inizia_per($_sect_name, "DB_")) {
			if ($crittato) {
				$GLOBALS[$_sect_name] = array();
				foreach($_keys as $_name => $_value) {
					$GLOBALS[$_sect_name][$_name] = $cry->decrypt($_value);
				}
			} else {
				$GLOBALS[$_sect_name] = $_vals;
			}
		} else {
			$_all_defines = array_merge($_all_defines, $_vals);
		}
	}
	
	foreach($_all_defines as $_name => $_value) {
		define($_name, $_value);
	}

	if ($crittato) {
		unset($cry);
	}
}

/**
 * Crea una versione crittata (aggiunge al file il suffisso .crypt) di un file .ini
 * @param string $file Il file .ini
 */
function critta_ini($file)
{
	$cry = new CRYPTER("Chiave in chiaro furbizzima per crittare gli filez iniz");

	$_sect = parse_ini_file($file, TRUE);
	$_f    = fopen($file.".crypt", "w+");

	foreach($_sect as $_sect_name => $_keys) {
		fwrite($_f, "[".$_sect_name."]\n");
		foreach($_keys as $_name => $_value) {
			$_value = $cry->encrypt($_value);

			fwrite($_f, $_name."=".$_value."\n");
		}
	}
	fclose($_f);
	unset($cry);
}


/* *********************************************************************************************
 *
 * A R R A Y
 *
 * *********************************************************************************************/

/**
 * Ritorna $array[$chiave] se la chiave esiste, $se_manca altrimenti
 * @param mixed $chiave La chiave da cercare
 * @param array $array L'array in cui cercare
 * @param mixed $se_manca L'alternativa
 * @return mixed
 */
function array_get_default($chiave, $array, $se_manca = NULL)
{
	if (array_key_exists($chiave, $array)) {
		return $array[$chiave];
	} else {
		return $se_manca;
	}
}

/**
 * Ritorna $array[$chiave] se la chiave esiste, fallisce altrimenti
 * @param mixed $chiave La chiave da cercare
 * @param array $array L'array in cui cercare
 * @return mixed
 */
function array_get($chiave, $array)
{
	if (array_key_exists($chiave, $array)) {
		return $array[$chiave];
	} else {
		throw new CodeException("Chiave $chiave non trovata nell'array [".implode(", ".$array)."]");
	}
}

/**
 * Stampa (con delle echo) un array, scendendo ricorsivamente e indentando i sotto-array
 * @param array &$array L'array da stampare
 * @param int $livello Il livello di indentazione (da non impostare, serve per i sotto-array)
 */
function array_stampa(&$array, $livello = 0)
{
	foreach($array as $chiave => $valore) {
		echo str_repeat("&nbsp;", $livello * 5)."[$chiave] => ";
		if (is_array($valore)) {
			echo BR;
			array_stampa($valore, $livello + 2);
		} else {
			echo $valore.BR;
		}
	}

}

/**
 * Appiattisce un array contenente altri array ([a, [b, c], d] => [a, b, c, d])
 * @param array $a 
 * @return array
 */
function array_flatten($a) 
{
	$res = array();
	for($i = 0; $i < count($a); $i ++) {
		if (is_array($a[$i]))
			  $res = array_merge($res, array_flatten($a[$i]));
		  else
			  $res[] = $a[$i];
	}
	return $res;
}

/* *********************************************************************************************
 *
 * X S L T
 *
 * *********************************************************************************************/

/**
 * Esegue le trasformazioni XSLT indicate in PHDIR/xsl/transform.xsl (se questo file esiste) sul testo XML passato.
 * @param string $xml testo XML da trasformare
 * @param bool $strip dice se rimuovere o meno header xml e namespaces
 * @return string
 */
function xsl_transform($xml, $strip = false, $xsl = NULL, $params = NULL)
{
	if (!defined("PHDIR"))
		throw new CodeException("Definire il parametro PHDIR");

	$cache = CACHE_CLASS_FACTORY::get();

	// preparazione parametri

	$_fwk_root    = constant_def('UBK_ROOT', 'C:/php/includes/ubk');

	if (($_pos = strpos(PHDIR, ":")) !== FALSE) {
		$_phdir = str_replace(array(' ','\\'), array('%20','/'), 'file:///' . PHDIR);
	} else {
		$_phdir = PHDIR;
	}
	if (!finisce_per($_phdir, '/')) {
		$_phdir .= '/';
	}
	
	$_params = array(
		'phdir' => $_phdir
		,'def-img-ext' => constant_def('DEF_IMG_EXT', '.gif')
//		,'help-inline' => constant_def('HELP_INLINE', "'false'")
	);
	if (!is_null($params)) $_params = array_merge($params, $_params);

	// creazione processore
//	$xml = utf8_encode($xml);
	$_curdir = getcwd();
	chdir($_fwk_root);
	$_xslt_proc =& xslt_create();
	
	//xslt_set_log($_xslt_proc,true);
	//xslt_set_log($_xslt_proc,path(PHDIR,'system/xslt.log'));

	if ($xsl && file_exists($_xsl = pathl(PHDIR, $xsl))) {
		$_xsl = str_replace('OWN', $_phdir.'xsl', file_get_contents($_xsl));
	} else {
		/* pre processing eventuale*/
		if (file_exists($_pre = path(PHDIR, 'xsl/pre.xsl'))) {
			$_pre = str_replace('OWN', $_phdir.'xsl', file_get_contents($_pre));
		} elseif (file_exists($_pre = path(PHDIR, 'system/xsl/pre.xsl'))) {
			$_pre = str_replace('OWN', $_phdir.'system/xsl', file_get_contents($_pre));
		} else {
			unset($_pre);
		}

		if (isset($_pre)) {

			$_arguments = array(
				'/_xml' => $xml,
				'/_xsl' => $_pre
			);

			$result = xslt_process($_xslt_proc, 'arg:/_xml', 'arg:/_xsl', NULL, $_arguments, $_params);
			if (!$result) {
				throw new CodeException("Impossibile applicare le pre-trasformazioni XSLT");//.xslt_error($_xslt_proc)
			}

			$xml = $result;
		}

		/* processing vero */
		if (is_null($_xsl = $cache->xsl_get())) {
			if (file_exists($_xsl = path(PHDIR, 'xsl/transform.xsl'))) {
				$_xsl = str_replace('OWN', $_phdir.'xsl', file_get_contents($_xsl));
			} elseif (file_exists($_xsl = path(PHDIR, 'system/xsl/transform.xsl'))) {
				$_xsl = str_replace('OWN', $_phdir.'system/xsl', file_get_contents($_xsl));
			} else {
				$_xsl = NULL;
			}
		}
	}

	if (!is_null($_xsl)) {

		$cache->xsl_write($_xsl);

		$_arguments = array(
			'/_xml' => $xml,
			'/_xsl' => $_xsl
		);

		$_result = xslt_process($_xslt_proc, 'arg:/_xml', 'arg:/_xsl', NULL, $_arguments, $_params);
		
		// risultato
		if (!$_result) {
			throw new CodeException('Impossibile applicare le trasformazioni XSLT');//.xslt_error($_xslt_proc)
		} else {
			xslt_free($_xslt_proc);
			chdir($_curdir);
		}
	} else {
		xslt_free($_xslt_proc);
		chdir($_curdir);
		$_result = $xml;
	}

	if ($strip) {
		$_result = str_ireplace(
						array(
							'<?xml version="1.0"?>', 
							' xmlns:ubk='.dQt(NS_UBK), 
							' xmlns:gfx='.dQt(NS_GFX)
						)
						,array('', '', '')
						,$_result
					);
	}

	return $_result;

}


/** 
 * Ritorna il risultato della trasformazione xsl sul documento xml passato. Entrambi i parametri possono essere del codice xml / xsl o un nome di file.
 * @param string $xml Nome del documento da trasformare, o testo xml da trasformare
 * @param string $xsl Nome del documento di trasformazione, o codice xsl di trasformazione
 * @return string
 * @deprecated
 */
function xsl_transform_file($xml, $xsl, $params = NULL)
{
	if (file_exists($xml)) $xml = file_get_contents($xml);
	if (file_exists($xsl)) $xsl = file_get_contents($xsl);

	$_arguments = array('/_xml' => utf8_encode($xml), '/_xsl' => $xsl);
	$_xslt_proc =& xslt_create();
	$_result    = xslt_process($_xslt_proc, 'arg:/_xml', 'arg:/_xsl', NULL, $_arguments, $params);
	
	if (!$_result) {
		throw new CodeException('Impossibile applicare le trasformazioni XSLT ');
	} else {
		xslt_free($_xslt_proc);
		return $_result;
	}
}

/* *********************************************************************************************
 *
 * F I L E
 *
 * *********************************************************************************************/

/**
 * Esegue una copia del file o della directory specificata
 * @param string $old_name Il file o directory sorgente
 * @param string $new_name Il file o directory destinazione
 * @param bool $copy_perms Dice se copiare o meno le permission presenti sul file di origine
 */
function copy_file($old_name, $new_name, $copy_perms = TRUE)
{
	// se e' un file, lo copio pari pari
	if (is_file($old_name)) {

		copy($old_name, $new_name);
		if ($copy_perms) {
			$permissions = fileperms($old_name);
			chmod($new_name, $permissions);
		} else {
			chmod($new_name, 0777);
		}

	// se e' una dir, la copio con la sua routine
	} elseif (is_dir($old_name)) {

		copy_dir($old_name, $new_name, $copy_perms);
	
	// altrimenti errore
	} else {

		throw new FatalExeption("Impossibile copiare $old_name, non &egrave; n&egrave; un file n&egrave; una directory.");
	} 
}
    
/**
 * Esegue una copia della directory specificata
 * @param string $old_name La directory sorgente
 * @param string $new_name La directory destinazione
 * @param bool $copy_perms Dice se copiare o meno le permission presenti sui file di origine contenuti nella directory
 */
function copy_dir($old_name, $new_name, $copy_perms = TRUE)
{
	// se la destinazione !esiste, la creo
	if (!is_dir($new_name)) {
		@mkdir($new_name);
	}

	// copio ogni file della dir sorgente nella dir destinazione
	$dir = opendir($old_name);
	
	while (($file = readdir($dir)) !== FALSE) {
	
		if($file != "." && $file != "..") {
			copy_file("$old_name/$file", "$new_name/$file", $copy_perms);
		}
	}

	closedir($dir);
}


/**
 * Crea il percorso indicato
 * @param string $nome Il percorso completo del file da creare
 */
function genera_path($nome)
{
	$_curdir = getcwd();
	$_path   = preg_split('/\\\|\//', $nome);

	for($i = 0; $i < count($_path); $i ++) {
		if ($_path[$i] == "") {
			chdir("/");
		} else {
			if (!is_dir(getcwd() . "/" . $_path[$i]) && !preg_match('/\w:/', $_path[$i])) {
				@mkdir($_path[$i])
					or log_value("MKDIR FAILED: ".implode(", ", $_path)." - pos $i");
			}
			chdir($_path[$i]);
		}
	}

	chdir($_curdir);
}

/**
 * Genera il file specificato con il contenuto indicato. Se il percorso specificato nel nome del file non esiste, lo crea
 * @param string $nome Il percorso completo del file da creare
 * @param string $contenuto Il contenuto da scrivere nel file
 */
function scrivi_file($nome, $contenuto)
{
	$_curdir = getcwd();
	$_path   = preg_split('/\\\|\//', $nome);

	for($i = 0; $i < count($_path) - 1; $i ++) {
		if ($_path[$i] == "") {
			chdir("/");
		} else {
			if (!is_dir(getcwd() . "/" . $_path[$i]) && !preg_match('/\w:/', $_path[$i])) {
				@mkdir($_path[$i])
					or log_value("MKDIR FAILED: ".implode(", ", $_path)." - pos $i");
			}
			chdir($_path[$i]);
		}
	}

	$f = fopen($_path[$i], "w+");
	fwrite($f, $contenuto);
	fflush($f);
	fclose($f);

	chdir($_curdir);
}


/**
 * Ritorna un array con il contenuto della directory, che ha per chiavi i nomi dei file o delle directory, e per valori una stringa con il nome dei file per i file, o un array con il contenuto della directory per le 
directory
 * @param string $nome Il percorso completo del file da creare
 * @return array
 */
function dir_2_array($nome, $filtro = "*")
{
	$_curdir = getcwd();
	$_result = array();

	chdir($nome);
	$files = glob($filtro);
	if (is_array($files))
		foreach($files as $file) {
			if ($file != "." && $file != "..") {
				if (is_dir($file)) {
					$_result[$file] = dir_2_array($file);
				} else {
					$_result[$file] = $file;
				}
			}
		}
	chdir($_curdir);

	return $_result;
}


/**
 * Elimina una directory e tutto il suo contenuto
 * @param string $dir Il percorso da eliminare
 */
function rmdirr($dir)
{
    if($objs = glob($dir."/*")){
          foreach($objs as $obj) {
                is_dir($obj) ? rmdirr($obj) : unlink($obj);
          }
    }
    rmdir($dir);
}

?>