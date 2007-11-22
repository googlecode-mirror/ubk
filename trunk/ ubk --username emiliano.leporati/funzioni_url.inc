<?php
/**
 * Funzioni di utilit�per la codifica degli url ai fini di protezione delle operazioni
 * @author Ubik <emiliano.leporati@gmail.com>
 * @package general
 */

/**
 * implica che $location contiene una url con & e viene restituita com &amp;
 */
define("URL_AMP_DEFAULT", 0);
/**
 * la url viene passata con &amp; e restituita con &amp;
 */
define("URL_AMP_XML", 1);
/**
 * la url viene passata con &amp; e restituita con &
 */
define("URL_AMP_PHP", 2);


define("PROTO", isset($_SERVER['https']) ? 'https://' : 'http://');

define("AMP_SEARCH", '/&(?!(?U)[\w-&]{0,6};)/');
define("AMP_REPLACE", '&amp;');

/**
 * Collega due stringhe nella formazione di un percorso corretto (es: path('c:\pippo','bravo.xml') = 'c:/pippo/bravo.xml')
 * @param string $inizio
 * @param string $fine
 * @return string
 */
function path($inizio, $fine = '')
{
	if (substr($inizio, -1) == "/") {
		$inizio = substr($inizio, 0, -1);
	}
	if (strlen($fine) > 0 && $fine[0] == "/") {
		$fine = substr($fine, 1);
	}
	return str_replace("\\","/",$inizio."/".$fine);
}

function pathl()
{
	$args = func_get_args();
	$args = array_flatten($args);
	$res = "";
	$res = $last = array_shift($args);
	
	while(count($args)) {
		$res = path($res, array_shift($args));
	}

	return $res;
}

/**
 * Costruisce l'url che punta al gateway (file php) passato
 * @param string $file
 * @return string
 */
function gateway($file)
{
	if (strcasecmp(PROTO, 'https://') == 0) {
		$host = PROTO.$_SERVER['HTTP_HOST'];

		if (substr($host, -1) == "/") {
			$host = substr($host, 0, -1);
		}
		if ($file[0] == "/") {
			$file = substr($file, 1);
		}
		if (defined("VDIR")) {
			if (substr(VDIR, -1) == "/") {
				$vdir = substr(VDIR, 0, -1);
			} else {
				$vdir = VDIR;
			}
			if (!inizia_per($file, $vdir)) {
				$file = path($vdir, $file);
			}
		}
	
		return path($host, $file);
	} else {
		return basename($file);
	}

}

/**
 * genera una URL con checksum per impedire la modifica manuale della URL
 * @param string $location la stringa contenente la url da codificare
 * @param integer $replace_amp indica come trasformare le & (vedere {@link URL_AMP_DEFAULT}, {@link URL_AMP_XML}, {@link URL_AMP_PHP}
 * @return string
 */
function url_codifica($location, $replace_amp = URL_AMP_DEFAULT)
{
	// se arriva una stringa vuota o non devo fare nulla, non faccio nulla
	if (strlen($location) == 0 || !constant_true('CHECKSUM_LINK')) {
		return $location;
	}
	// se arriva una stringa senza ? viene accodato
	if (strpos($location, '?') === FALSE) {
		$location = $location.'?';
	}

	$location = preg_replace(AMP_SEARCH, AMP_REPLACE, $location);
	$location = str_replace(' ', '%20', $location);
	
	if ( !inizia_per($location, PROTO) ) {
		if (defined('VDIR') && strlen(VDIR))
			$location = path(PROTO.$_SERVER['HTTP_HOST'], path(VDIR, $location));
		else
			$location = path(PROTO.$_SERVER['HTTP_HOST'], $location);
	}

	$location .= "&amp;CHK=".bin2hex(mhash(MHASH_MD5, $location, CHECKSUM_PASSWORD));

	if ($replace_amp == URL_AMP_PHP)
		$location = str_replace('&amp;', '&', $location);

	return $location;
}

/**
 * Controlla se la url passata �coerente con il checksum previsto
 * @param string $location la stringa contenente la url da verificare (se null, viene presa l'ultima pagina richiesta)
 * @return boolean
 */
function url_valida($location = NULL)
{
	// se non devo fare nulla, va bene dibbase
	if (!constant_true('CHECKSUM_LINK'))
		return TRUE;

	// se location �null, faccio la validazione sulla pagina corrente
	if (strlen($location) == 0)
		$location = PROTO.$_SERVER['HTTP_HOST'].$_SERVER['REQUEST_URI'];

	// rimetto a posto le &
	$location = preg_replace(AMP_SEARCH, AMP_REPLACE, $location);

	if (strpos($location, '&amp;CHK') === FALSE)
		return TRUE;

	// tolgo la checksum e ricalcolo l'hash
	$location_attesa = explode("&amp;CHK",$location);
	$location_attesa = url_codifica($location_attesa[0]);

	if ($location == $location_attesa) {
		return TRUE;
	} else {
		log_value("attesa  = ".$location_attesa);
		log_value("passata = ".$location);
		return FALSE;
	}
}

?>