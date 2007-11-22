<?php
/**
 * Funzioni per la compatibilita' con la vecchia versione per php5, e comunque di comodo
 * vengono rimandate sul driver del db di default
 * altre servono per incapsulare i nuovi oggetti di manipolazione XML e XSLT
 * @author Ubik <emiliano.leporati@gmail.com>
 * @package bwc
 */

function c_and()
{
	$_params = func_get_args();
	return call_user_func_array(
			array('DB_SQL_'.$GLOBALS['DB_DEFAULT']['DB_TYPE'], 'c_and'),
			$_params
		);
}

function c_or()
{
	$_params = func_get_args();
	return call_user_func_array(
			array('DB_SQL_'.$GLOBALS['DB_DEFAULT']['DB_TYPE'], 'c_or'),
			$_params
		);
}

function c_not()
{
	$_params = func_get_args();
	return call_user_func_array(
			array('DB_SQL_'.$GLOBALS['DB_DEFAULT']['DB_TYPE'], 'c_not'),
			$_params
		);
}

function equ()
{
	$_params = func_get_args();
	return call_user_func_array(
			array('DB_SQL_'.$GLOBALS['DB_DEFAULT']['DB_TYPE'], 'equ'),
			$_params
		);
}

function iequ()
{
	$_params = func_get_args();
	return call_user_func_array(
			array('DB_SQL_'.$GLOBALS['DB_DEFAULT']['DB_TYPE'], 'iequ'),
			$_params
		);
}

function diff()
{	
	$_params = func_get_args();
	return call_user_func_array(
			array('DB_SQL_'.$GLOBALS['DB_DEFAULT']['DB_TYPE'], 'diff'),
			$_params
		);
}

function gt()
{
	$_params = func_get_args();
	return call_user_func_array(
			array('DB_SQL_'.$GLOBALS['DB_DEFAULT']['DB_TYPE'], 'gt'),
			$_params
		);

}

function lt()
{
	$_params = func_get_args();
	return call_user_func_array(
			array('DB_SQL_'.$GLOBALS['DB_DEFAULT']['DB_TYPE'], 'lt'),
			$_params
		);
}

function gte()
{
	$_params = func_get_args();
	return call_user_func_array(
			array('DB_SQL_'.$GLOBALS['DB_DEFAULT']['DB_TYPE'], 'gte'),
			$_params
		);

}

function lte()
{
	$_params = func_get_args();
	return call_user_func_array(
			array('DB_SQL_'.$GLOBALS['DB_DEFAULT']['DB_TYPE'], 'lte'),
			$_params
		);

}

function like()
{
	$_params = func_get_args();
	return call_user_func_array(
			array('DB_SQL_'.$GLOBALS['DB_DEFAULT']['DB_TYPE'], 'like'),
			$_params
		);
}

function begins_like()
{
	$_params = func_get_args();
	return call_user_func_array(
			array('DB_SQL_'.$GLOBALS['DB_DEFAULT']['DB_TYPE'], 'begins_like'),
			$_params
		);
}

function ends_like()
{
	$_params = func_get_args();
	return call_user_func_array(
			array('DB_SQL_'.$GLOBALS['DB_DEFAULT']['DB_TYPE'], 'ends_like'),
			$_params
		);
}

function word_like()
{
	$_params = func_get_args();
	return call_user_func_array(
			array('DB_SQL_'.$GLOBALS['DB_DEFAULT']['DB_TYPE'], 'word_like'),
			$_params
		);
}

function in()
{
	$_params = func_get_args();
	return call_user_func_array(
			array('DB_SQL_'.$GLOBALS['DB_DEFAULT']['DB_TYPE'], 'in'),
			$_params
		);
}

function not_in()
{
	$_params = func_get_args();
	return call_user_func_array(
			array('DB_SQL_'.$GLOBALS['DB_DEFAULT']['DB_TYPE'], 'not_in'),
			$_params
		);
}

function between()
{
	$_params = func_get_args();
	return call_user_func_array(
			array('DB_SQL_'.$GLOBALS['DB_DEFAULT']['DB_TYPE'], 'between'),
			$_params
		);
}

function in_tp()
{
	$_params = func_get_args();
	return call_user_func_array(
			array('DB_SQL_'.$GLOBALS['DB_DEFAULT']['DB_TYPE'], 'in_tp'),
			$_params
		);
}

function op_tp()
{
	$_params = func_get_args();
	return call_user_func_array(
			array('DB_SQL_'.$GLOBALS['DB_DEFAULT']['DB_TYPE'], 'op_tp'),
			$_params
		);
}


function cond_tp()
{
	$_params = func_get_args();
	return call_user_func_array(
			array('DB_SQL_'.$GLOBALS['DB_DEFAULT']['DB_TYPE'], 'cond_tp'),
			$_params
		);
}

function not_cond_tp()
{
	$_params = func_get_args();
	return call_user_func_array(
			array('DB_SQL_'.$GLOBALS['DB_DEFAULT']['DB_TYPE'], 'not_cond_tp'),
			$_params
		);
}

function year($colonna, $valore, $operatore = "=")
{
	$_params = func_get_args();
	return call_user_func_array(
			array('DB_SQL_'.$GLOBALS['DB_DEFAULT']['DB_TYPE'], 'year'),
			$_params
		);
}

function month($colonna, $valore, $operatore = "=")
{
	$_params = func_get_args();
	return call_user_func_array(
			array('DB_SQL_'.$GLOBALS['DB_DEFAULT']['DB_TYPE'], 'month'),
			$_params
		);
}

function day($colonna, $valore, $operatore = "=")
{
	$_params = func_get_args();
	return call_user_func_array(
			array('DB_SQL_'.$GLOBALS['DB_DEFAULT']['DB_TYPE'], 'day'),
			$_params
		);
}

function text_like()
{
	$_params = func_get_args();
	return call_user_func_array(
			array('DB_SQL_MSSQL', 'text_like'),
			$_params
		);
}

function all_tp($tabella_ricerca, $campo_ricerca, $campo_filtro, $valore)
{
	$_params = func_get_args();
	return call_user_func_array(
			array('DB_SQL_MSSQL', 'all_tp'),
			$_params
		);
}

function in_last_days($colonna, $giorni)
{
	$_params = func_get_args();
	return call_user_func_array(
			array('DB_SQL_FBIRD', 'in_last_days'),
			$_params
		);
}

function not_in_last_days($colonna, $giorni)
{
	$_params = func_get_args();
	return call_user_func_array(
			array('DB_SQL_FBIRD', 'not_in_last_days'),
			$_params
		);
}

function tsEqu($colonna, $valore)
{
	$_params = func_get_args();
	return call_user_func_array(
			array('DB_SQL_FBIRD', 'tsEqu'),
			$_params
		);
}




function genera_insert()
{
	$_params = func_get_args();
	return call_user_func_array(
			array('DB_SQL_'.$GLOBALS['DB_DEFAULT']['DB_TYPE'], 'genera_insert'),
			$_params
		);
}

function genera_update()
{
	$_params = func_get_args();
	return call_user_func_array(
			array('DB_SQL_'.$GLOBALS['DB_DEFAULT']['DB_TYPE'], 'genera_update'),
			$_params
		);
}

function genera_delete()
{
	$_params = func_get_args();
	return call_user_func_array(
			array('DB_SQL_'.$GLOBALS['DB_DEFAULT']['DB_TYPE'], 'genera_delete'),
			$_params
		);
}



function & xslt_create()
{
	$o = new XsltProcessor();
	return $o;
}

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

function xslt_free(&$xsltproc)
{
	unset($xsltproc);
}


?>