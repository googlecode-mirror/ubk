<?php
/**
 * Funzioni per la compatibilita' con la vecchia versione per php5, e comunque di comodo
 * vengono rimandate sul driver del db di default
 * altre servono per incapsulare i nuovi oggetti di manipolazione XML e XSLT
 * @author Ubik <emiliano.leporati@gmail.com>
 * @package bwc
 */

/**
 * Definisce come funzioni senza namespace tutte quelle della classe DB_SQL per il db di default
 */
function define_sql_methods()
{
	$cls = 'DB_SQL_'.$GLOBALS['DB_DEFAULT']['DB_TYPE'];
	$mtds = get_class_methods($cls);

	foreach($mtds as $mtd) {
		$str = 'function '.$mtd.'() { $_p = func_get_args(); return call_user_func_array(array('.qt($cls).', '.qt($mtd).'), $_p); }';
		eval($str);
	}
}

/**
 * Crea un processore XSLT (backward compatibility)
 */
function & xslt_create()
{
	$o = new XsltProcessor();
	return $o;
}

/**
 * Esegue un processing XSLT (backward compatibility)
 */
function xslt_process($xsltproc, $xml_arg, $xsl_arg, $xslcontainer = null, $args = null, $params = null) 
{
	// Start with preparing the arguments
	$xml_arg = str_replace('arg:', '', $xml_arg);
	$xsl_arg = str_replace('arg:', '', $xsl_arg);
	// Create instances of the DomDocument class
	$xml = new DomDocument;
	$xsl = new DomDocument;
	// Load the xml document and the xsl template
	$xml->loadXML($args[$xml_arg]);
	$xsl->loadXML($args[$xsl_arg]);
	// Load the xsl template
	$xsltproc->importStyleSheet($xsl);
	// Set parameters when defined
	if ($params) {
		foreach ($params as $param => $value) {
			$xsltproc->setParameter("", $param, $value);
		}
	}
	// Start the transformation
	$processed = $xsltproc->transformToXML($xml);
	// Put the result in a file when specified
	if ($xslcontainer) {
		return @file_put_contents($xslcontainer, $processed);
	} else {
		return $processed;
	}
}

/**
 * Rilascia le risorse associate a un processore XSLT (backward compatibility)
 */
function xslt_free(&$xsltproc)
{
	unset($xsltproc);
}


?>