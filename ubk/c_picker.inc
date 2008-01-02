<?php
/**
 * Classe di azione da estendere per la gestione del pick dei dati ({@link PICKER})
 * @author Ubik <emiliano.leporati@gmail.com>
 * @package gateway
 */


/** 
 * Inclusione della classe base da estendere per le azioni
 */ 
require_once("c_azioni.inc");

/**
 * Classe di azione con implementata la pick
 * @package gateway
 * @deprecated
 */
class AZIONI_PICK extends AZIONI_FORM
{
	public function pick(&$rs)
	{
		$_file = str_replace("/", "_", $this->parametri['FILE']);

		$id = array_get("ID", $this->parametri);
		$campi = explode("|", $_SESSION["PICK-CAMPI-$_file"]);

		$rs_new = new GESTORE($_SESSION["PICK-CAMPO-TABELLA-$_file"],$_SESSION["PICK-CAMPO-ID-TABELLA-$_file"]);
		$rs_new->carica_righe_id($id);

		for($risultato = "", $i = 0; $i < count($campi) ; $i++){
			if ($i % 2 == 0) {
				$risultato .= $rs_new->valore_fwk($campi[$i]);
			} else {
				$risultato .= " ".$campi[$i]." ";			
			}
		} 

		echo "<HTML><HEAD><SCRIPT language=\"Javascript\">";
		echo "var o = window.opener.document.getElementById(".qt($_SESSION["PICK-CAMPO-TARGET-$_file"]).");";
		echo "o.value = ".qt($id).";";
		echo "o.nextSibling.value = ".qt(stringa_js($risultato)).";";
		echo "if (o.onchange != null) o.onchange.call(o);";
		echo "window.close();";
		echo "</SCRIPT></HEAD></HTML>";
		exit();
	}

	public function cod_desc_pick(&$rs)
	{
		$_file = str_replace("/", "_", $this->parametri['FILE']);

		$id = array_get("ID", $this->parametri);

		$campi = explode("|", $_SESSION["PICK-CAMPI-$_file"]);
		if (count($campi) != 2)
			throw new CodeException("Attenzione coder, l'azione cod_desc_pick richiede esattamente due campi.");


		$rs_new = new GESTORE($_SESSION["PICK-CAMPO-TABELLA-$_file"],$_SESSION["PICK-CAMPO-ID-TABELLA-$_file"]);
		$rs_new->carica_righe_id($id, NULL, $campi);

		echo "<HTML><HEAD><SCRIPT language=\"Javascript\">";
		echo "var o = window.opener.document.getElementById(".qt($_SESSION["PICK-CAMPO-TARGET-$_file"]).");";
		echo "o.value = ".qt($id).";";
		echo "o.nextSibling.value = ".qt(stringa_js($rs_new->valore_fwk($campi[0]))).";";
		echo "o.nextSibling.nextSibling.nextSibling.value = ".qt(stringa_js($rs_new->valore_fwk($campi[1]))).";";
		echo "if (o.onchange != null) o.onchange.call(o);";
		echo "window.close();";
		echo "</SCRIPT></HEAD></HTML>";
		exit();
	}

	public function cod_desc_bg_pick(&$rs)
	{
		$campo_target = array_get("CAMPO-TARGET", $this->parametri);
		$campi    = array_get("CAMPI", $this->parametri);
		$tabella  = array_get("TABELLA-PICK", $this->parametri);
		$campo_id_pick = array_get("CAMPO-ID-PICK", $this->parametri);

		$campi = explode("|", $campi);
		if (count($campi) != 2)
			throw new CodeException("Attenzione coder, l'azione cod_desc_bg_pick richiede esattamente due campi.");

		$id = array_get("ID", $this->parametri);
		$rs_new = new GESTORE($tabella, $campi[0]);
		$rs_new->carica_righe_id($id, NULL, array_merge($campi, array($campo_id_pick)));
		
		if ($rs_new->eof || $rs_new->count > 1) {
			echo "<HTML><HEAD><SCRIPT language=\"Javascript\">";
			echo "var o = parent.document.getElementById(".qt($campo_target).");";
			echo "o.value = '';";
			echo "o.nextSibling.value = '';";
			echo "o.nextSibling.nextSibling.nextSibling.value = '';";
			echo "if (o.onchange != null) o.onchange.call(o);";
			echo "</SCRIPT></HEAD></HTML>";
		} else {
			echo "<HTML><HEAD><SCRIPT language=\"Javascript\">";
			echo "var o = parent.document.getElementById(".qt($campo_target).");";
			echo "o.value = ".qt($rs_new->valore_fwk($campo_id_pick)).";";
			echo "o.nextSibling.value = ".qt(stringa_js($rs_new->valore_fwk($campi[0]))).";";
			echo "o.nextSibling.nextSibling.nextSibling.value = ".qt(stringa_js($rs_new->valore_fwk($campi[1]))).";";
			echo "if (o.onchange != null) o.onchange.call(o);";
			echo "</SCRIPT></HEAD></HTML>";
		}
		exit();
	}


	/**
	 * Memorizza in sessione il campo target (id) e i campi-pick (stringhe da visualizzare)
	 * @param GESTORE $rs
	 */
	public function mostra(&$rs)
	{
		$_file = str_replace("/", "_", $this->parametri['FILE']);

		$_SESSION["PICK-CAMPO-TARGET-$_file"] = array_get("CAMPO-TARGET", $this->parametri);
		$_SESSION["PICK-CAMPI-$_file"] = array_get("CAMPI", $this->parametri);
		$_SESSION["PICK-CAMPO-TABELLA-$_file"] = array_get("TABELLA-PICK", $this->parametri);
		$_SESSION["PICK-CAMPO-ID-TABELLA-$_file"] = array_get("CAMPO-ID-PICK", $this->parametri);
		parent::mostra($rs);
	}


}

?>