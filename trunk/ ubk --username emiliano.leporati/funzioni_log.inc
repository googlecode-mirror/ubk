<?php
/**
 * Definisce funzioni per il supporto di logging ed error handling
 * @author Ubik <emiliano.leporati@gmail.com>
 * @package general
 */


define('BASE_LOG_DIR','system/log');
define('PERFORMANCE_LOG','system/timings.csv');
define('CSV_SEP',';');

date_default_timezone_set(defined('TIMEZONE') ? TIMEZONE : 'Europe/Rome');


/** 
 * Parser per gli errori
 */ 
require_once("c_parser.inc");

/**
 * Ritorna il nome del file di log, basato sul nome dell'host e sull'applicazione corrente
 */
function log_get_file_name()
{
	// disabilita la scrittura dei log se PHDIR non � settato
	if (!defined('LOG_FILE')){
		if (defined('PHDIR')){
			define('LOG_FILE','_default_log_file_'.PHDIR);
			define('LOG_DIR',path(PHDIR,BASE_LOG_DIR));
		} else {
			return false;
		}	
	}
	if (!isset($_SESSION[LOG_FILE])) {
		if (constant_true('SINGLE_LOG')){
			$log_file_name = 'log_file_'.date("Ymd").'.log';
		} elseif (isset($_SERVER['REMOTE_HOST'])) {
			$log_file_name = $_SERVER['REMOTE_HOST'];
		} else {
			// troppo tempo
			//$log_file_name = gethostbyaddr($_SERVER['REMOTE_ADDR']);
			$log_file_name = array_get_default('REMOTE_ADDR', $_SERVER, '127.0.0.1');
			$log_file_name = str_replace('.','_',$log_file_name);
		}

		if (defined('PHDIR')) {
			if (!file_exists(LOG_DIR)) {
				genera_path(LOG_DIR);
			}
			
			$_SESSION[LOG_FILE]= path(LOG_DIR,"$log_file_name.log");
		} else {// per ora non pi� usato in quanto in determinati casi genera errori
			$_SESSION[LOG_FILE] = "log/$log_file_name.log";
			//throw new CodeException("Definire PHDIR nei parametri prima di utilizzare le funzioni di log");
		}
	}
	//return false;
	return $_SESSION[LOG_FILE] ;
}

/**
 * Logga un valore dentro il file di log corrente.
 * Il valore pu� essere una stringa, un numero, un array o un oggetto; tipi di dato speciali
 * (array e oggetti) sono stampati usando print_r. Il file di log viene ottenuto dalla funzione {@link log_get_file_name}
 *
 * @param mixed $val Il valore da stampare sul file di log
 */
function log_value($val)
{
	$log_file_name = log_get_file_name();
	if ($log_file_name == false){
		return;	
	}
	// Extract info with print_r if is a special value
	if (is_array($val) || is_object($val)) {
		ob_start();
		print_r($val);
		$val = ob_get_contents();
		ob_end_clean();
	}
	
	$msg =  date("d/m/Y H:i:s"). ": $val\n";
	if (constant_true('SINGLE_LOG')){
		$msg =  $_SERVER['REMOTE_ADDR'] . " - $msg";
	} 
	if(!file_exists($log_file_name)){
		$fp = fopen($log_file_name,'w+');
		fclose($fp);
	}
	error_log($msg, 3, $log_file_name);
}

/**
 * Pulisce il file di log corrente (vedere {@link log_get_file_name}
 */
function log_clear()
{
	$log_file_name = log_get_file_name();
	if ($log_file_name == false){
		return;	
	}
	if ($handle = fopen ($log_file_name, "w")){
		fclose($handle);
	}
}

/**
 * Error handler di default
 * @param int $errtype Tipo errore (compile, parse, notice, etc.)
 * @param string $errstr Messaggio di errore
 * @param string $errfile Nome del file in cui si � verificato l'errore
 * @param integer $errline Linea di $errfile in cui si � verificato l'errore
 */

function ubk_err_handler($errtype, $errstr, $errfile, $errline)
{
	switch($errtype) {
	case E_PARSE:
	case E_COMPILE_ERROR:
		die($errstr." file ".$errfile." linea ".$errline);

	case E_ERROR:
	case E_CORE_ERROR:
	case E_USER_ERROR:
		if (headers_sent()) {
			echo "</html>".$errstr." - file ".$errfile." linea ".$errline;
		} else {
			if (!file_exists($_file = path(PHDIR, "system/errore.xml"))) {
				$_file = "risorse/errore.xml";
			}
			$parametri = array('ERRORE' => $errstr);
			$p = new PARSER();
			$doc =& $p->parsifica($_file);
			$doc->stampa_html($parametri);
		}
		$source = file($errfile);
		$source = htmlspecialchars($source[$errline-1]);
		$file =  substr(strrchr(str_replace("\\",'/',$errfile),'/'),1);
		log_value("ERROR: [$file:$errline] $source ");
//		log_value(debug_backtrace());
		exit(1);

	case E_WARNING:
	case E_CORE_WARNING:
	case E_COMPILE_WARNING:
	case E_USER_WARNING:
		log_value("WARNING - file $errfile linea $errline: $errstr");
//		log_value(debug_backtrace());
		break;
	
	case E_NOTICE:
	case E_USER_NOTICE:
		log_value("NOTICE - file $errfile linea $errline: $errstr");
//		log_value(debug_backtrace());
		break;
	case E_STRICT:
//		log_value("STRICT - file $errfile $errline: $errstr");
		break;
	default:
		log_value("ERRORE TIPO NON GESTITO: $errtype, $errstr");
//		log_value(debug_backtrace());
		break;
	}
}

function ubk_exc_handler($exc)
{
	static $errfile;

	echo "</html>";

	if (!isset($errfile) && file_exists(path(PHDIR, "system/errore-exc.xml"))) {
		$errfile = path(PHDIR, "system/errore-exc.xml");
	} elseif (!isset($errfile)) {
		$errfile = path(UBK_ROOT, "risorse/errore-exc.xml");
	}

	$parametri = array('EXC' => $exc);
	$p = new PARSER();
	$doc =& $p->parsifica($errfile);
	$doc->stampa_html($parametri);
	

	log_value("ECCEZIONE: ".$exc->getMessage());
	log_value("           ".$exc->getTraceAsString());
	log_value(DB_DRIVER::$SQL);

	exit(1);
}

function log_timing()
{
	$times = array(
		'Totale' => AZIONI_FORM::$time['TOTAL']
		,'Stampa HTML' => AZIONI_FORM::$time['TRANS']
		,'Parsing' => PARSER::$time['PARSE']
		,'SQL' => DB_BASE::$time['SQL']
		,'Cache' => CACHE::$time['CACHE']
	);
	
	$txt = '';

	foreach($times as $desc => $value) {
		if ($txt) $txt .= ' - ';
		$txt .= "$desc: " . number_format($value, 4);
	}
	
	foreach(DB_BASE::$time['ALL'] as $sql => $time) {
		$txt .= "\n\t\t$time - $sql";
	}

	if ($txt) log_value($txt);
}

function log_performance(&$params)
{
	if (!file_exists(PERFORMANCE_LOG)) {
		$header = array('PHP','XML','AZIONE','MODE','TOTALE','SQL','STAMPA HTML','PARSING','COMPILAZIONE','CACHE','MEM','IP','TIMESTAMP');
		$header = implode(CSV_SEP, $header) . "\n";
		scrivi_file(PERFORMANCE_LOG, $header);
	}
	$data = array();
	$data[] = substr($_SERVER['PHP_SELF'],strlen(VDIR) + 2);
	$data[] = isset($params['FILE']) ? $params['FILE'] : ' ';
	$data[] = isset($params['AZIONE']) ? $params['AZIONE'] : ' ';
	$data[] = isset($params['MODE']) ? $params['MODE'] : ' ';
	$data[] = AZIONI_FORM::$time['TOTAL'];
	$data[] = AZIONI_FORM::$time['TRANS'];
	$data[] = PARSER::$time['PARSE'];
	$data[] = DB_BASE::$time['SQL'];
	$data[] = CACHE::$time['CACHE'];

	if (function_exists('memory_get_usage')){
		$data[] = memory_get_usage();
	} else {
		$data[] = 0;
	}
	$data[] = $_SERVER['REMOTE_ADDR'];
	$data[] = date("Ymdhis");
	
	error_log(implode(CSV_SEP,$data)."\n", 3, PERFORMANCE_LOG);
	
}

?>
