<?php

require_once("params.inc");
require_once("ubk/c_azioni.inc");

class AZIONI_GENERICHE extends AZIONI_FORM
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
		die('+OKEseguito');
	}

	public function elimina(&$rs)
	{
		parent::elimina($rs);
		die('+OK');
	}
}


$a =& new AZIONI_GENERICHE(
			array(
				"MODE" => "RW"
				,"PAGE" => 0
				,"PAGE-SIZE" => 0
				,"AZIONE" => "mostra"
				,"FILE" => 'esempio.xml'
			));

$rs =& new GESTORE($a->parametro('TABELLA', 'esempio'), $a->parametro('CAMPO-ID'));

$a->esegui($rs);

?>
