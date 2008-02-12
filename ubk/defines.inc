<?php
/**
 * Definizione di entita' di uso comune
 *  
 * @package tag
 * @author Ubik <emiliano.leporati@gmail.com>
 */

/**
 * Array associativo [entita'] => [codice hex], utile per applicare delle strtr e generare codice xml valido
 * @var array
 */
$ENT_HEX = array();

/**
 * Il BR di XHTML
 */
define("BR","<br />");

/**
 * Le entita' piu' particolari
 */
define('LT',      '<');
define('GT',      '>');
define('AMP',     '&');

/**
 * Entita' UTF-8
 */
define('euro',    '&#x20AC;');
define('ellipsis','&#x2026;');
define('trade' ,  '&#x2122;');

/**
 * Definisce tutte le entita' standard come [nome entita'] => [codice hex] e imposta l'array globale $ENT_HEX
 */
function define_entities() 
{
	global $ENT_HEX;

	$table = get_html_translation_table(HTML_ENTITIES);
	unset($table['&']);
	unset($table['<']);
	unset($table['>']);
	unset($table[' ']);

	foreach($table as $char => $entity) {
		$ent = substr($entity, 1, -1);
		$hex = '&#x'.base_convert(ord($char), 10, 16).';';

		$ENT_HEX[$entity] = $hex;
		define($ent, $hex);
	}
}

define_entities();

?>