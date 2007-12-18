<?php
/**
 * Tag per la costruzione dei campi di filtro. Estendono classi di {@link TAG_DB.inc} e si occupano di memorizzare il valore per chiamate successive sulla pagina.
 * @author Ubik <emiliano.leporati@gmail.com>
 * @package tag
 */

/**
 * Ritorna il valore filtro attualmente impostato per il campo nel file specificato - questi valori sono memorizzati automaticamente durante l'esecuzione di AZIONI_FORM::filtra
 * @param string $file Nome del file in cui risiede il campo di filtro
 * @param string $campo Nome del campo di filtro
 * @param string $op Operatore di filtro
 * @param string $default Valore di default che viene ritornato se nessun filtro per quel campo con quell'operatore e' impostato sulla pagina
 * @return mixed
 */
function filtro_def($file, $campo, $op, $default)
{
	$file = str_replace("/","_",$file);
	$filtro = "campi_filtro_".$file;
	if (isSet($_SESSION[$filtro][$op.$campo])) {
		return $_SESSION[$filtro][$op.$campo];
	} else {
		return $default;
	}
}

/**
 * Ritorna il valore filtro attualmente impostato nel file specificato - questi valori sono memorizzati automaticamente durante l'esecuzione di AZIONI_FORM::filtra
 * @param string $file Nome del file di cui conoscere le condizioni di filtro
 * @return string
 */
function filtro_pagina($file)
{
	$_file = str_replace("/","_", $file);

	if (isSet($_SESSION["filtro_$_file"]) && isSet($_SESSION["filtro_def_$_file"])) {
		return c_and($_SESSION["filtro_$_file"], $_SESSION["filtro_def_$_file"]);
	} else if (isSet($_SESSION["filtro_$_file"])) {
		return $_SESSION["filtro_$_file"];
	} else if (isSet($_SESSION["filtro_def_$_file"])) {
		return $_SESSION["filtro_def_$_file"];
	} else {
		return NULL;
	}
}

function ordine_pagina($file)
{
	$_file = str_replace("/","_", $file);
	return array_get_default("ordine_$_file", $_SESSION);
}

/**
 * Campo classico (INPUT). Sempre in modalita' W e con valore di default automatico (resettato in {@link db::AZIONI_FORM::mostra}). 
 * @package tag
 */
class CAMPO_FILTRO extends CAMPO
{
	protected static $op_desc = array(
		 "equ" => "uguale a"
		,"in_tp" => "uguale a"
		,"diff" => "diverso da"
		,"begins_like" => "inizia per"
		,"ends_like" => "finisce per"
		,"like" => "contiene"
		,"gt" => "maggiore di"
		,"gte" => "maggiore o uguale di"
		,"lt" => "minore di"
		,"lte" => "minore o uguale di"
	);

	function stampa_html(&$parametri)
	{
		$campo = $this->attributo("JS-NAME", $this->attributo_richiesto("NOME"));
		$op    = $this->attributo("OP","equ");
		if (!function_exists($op)) {
			throw new CodeException("Operatore non supportato: $op");
		}
		$this->attributi["MODE"] = "W";
		$this->attributi["JS-NAME"] = "filtro[$op|$campo]";
		$this->attributi["JS-ID"] = $this->attributo("JS-ID","filtro[$op|$campo]");
		$this->attributi["VALORE-DEFAULT"] = filtro_def(array_get("FILE", $parametri), $campo, $op, $this->attributo("VALORE-DEFAULT"));

		if (constant_true('ETICHETTA_FILTRI')) {
			echo '<table style="border: 0; margin: 0; padding: 0; border-collapse: collapse">';
			echo '<tr><td style="border: 0; margin: 0; padding: 0;">';
			parent::stampa_html($parametri);
			echo '</tr></td>';
			echo '<tr><td class="filter_label">', self::$op_desc[$op], '</td></tr>';
			echo '</table>';
		} else {
			parent::stampa_html($parametri);
		}
	}
	
}


/**
 * Checkbox. Sempre in modalita' W e con valore di default automatico (resettato in {@link db::AZIONI_FORM::mostra}). 
 * @package tag
 */
class CHECKBOX_FILTRO extends CHECKBOX
{
	function stampa_html(&$parametri)
	{
		$campo = $this->attributo_richiesto("NOME");
		$op    = $this->attributo("OP","equ");
		if (!function_exists($op)) {
			throw new CodeException("Operatore non supportato: $op");
		}
		$this->attributi["MODE"] = "W";
		$this->attributi["JS-NAME"] = "filtro[$op|$campo]";

		$this->attributi["JS-ID"] = $this->attributo("JS-ID","filtro[$op|$campo]");
		$this->attributi["VALORE-DEFAULT"] = filtro_def(array_get("FILE", $parametri), $campo, $op, $this->attributo("VALORE-DEFAULT"));

		parent::stampa_html($parametri);
	}
	
}

/**
 * Combo / Radio group. Sempre in modalita' W e con valore di default automatico (resettato in {@link db::AZIONI_FORM::mostra}). 
 * @package tag
 */
class COMBO_FILTRO extends COMBO
{
	function stampa_html(&$parametri)
	{
		$campo = $this->nome_js();

		$op    = $this->attributo("OP","equ");
		if (!function_exists($op)) {
			throw new CodeException("Operatore non supportato: $op");
		}
		$this->attributi["MODE"] = "W";
		$this->attributi["JS-NAME"] = "filtro[$op|$campo]";
		$this->attributi["JS-ID"] = $this->attributo("JS-ID","filtro[$op|$campo]");
		$this->attributi["VALORE-DEFAULT"] = filtro_def(array_get("FILE", $parametri), $campo, $op, $this->attributo("VALORE-DEFAULT"));

		parent::stampa_html($parametri);
	}
	
}


/**
 * Picker. Sempre in modalita' W e con valore di default automatico (resettato in {@link db::AZIONI_FORM::mostra}). 
 * @package tag
 */
class PICKER_FILTRO extends PICKER
{
	function stampa_html(&$parametri)
	{
		$campo = $this->nome_js();

		$op    = $this->attributo("OP","equ");
		if (!function_exists($op)) {
			throw new CodeException("Operatore non supportato: $op");
		}
		$this->attributi["MODE"] = "W";
		$this->attributi["JS-NAME"] = "filtro[$op|$campo]";
		$this->attributi["JS-ID"] = $this->attributo("JS-ID","filtro[$op|$campo]");
		$this->attributi["VALORE-DEFAULT"] = filtro_def(array_get("FILE", $parametri), $campo, $op, $this->attributo("VALORE-DEFAULT"));

		parent::stampa_html($parametri);
	}
	
}

/**
 * Cod-desc. Sempre in modalita' W e con valore di default automatico (resettato in {@link db::AZIONI_FORM::mostra}). 
 * @package tag
 */
class COD_DESC_FILTRO extends COD_DESC
{
	function stampa_html(&$parametri)
	{
		$campo = $this->nome_js();

		$op    = $this->attributo("OP","equ");
		if (!function_exists($op)) {
			throw new CodeException("Operatore non supportato: $op");
		}
		$this->attributi["MODE"] = "W";
		$this->attributi["JS-NAME"] = "filtro[$op|$campo]";
		$this->attributi["JS-ID"] = $this->attributo("JS-ID","filtro[$op|$campo]");
		$this->attributi["VALORE-DEFAULT"] = filtro_def(array_get("FILE", $parametri), $campo, $op, $this->attributo("VALORE-DEFAULT"));

		parent::stampa_html($parametri);
	}
	
}

/**
 * Genera un filtro utilizzando l'operatore virtuale {@link op_tp}: il valore immesso nella casella di testo viene usato per filtrare una tabella esterna. Esempio tipico: una ricerca per comune. Il comune e' su tabella esterna. Devo selezionare tutti i fornitori che stanno in un comune che comincia per TO. In casi simili, usare questo campo (o la combo), che genera una clausola IN basata su una subquery: SELECT * FROM tabella WHERE ...... AND i_comune_id IN (SELECT i_comune_id FROM coumne WHERE t_descrizione LIKE 'TO%')
 * @package tag
 */
class CAMPO_FILTRO_TP extends CAMPO_FILTRO
{
	function stampa_html(&$parametri)
	{
		$campo = $this->attributo_richiesto("NOME");

		$op = $this->attributo_richiesto("OP");
		$campo_ricerca   = $this->attributo_richiesto("NOME-RICERCA");
		$tabella_ricerca = $this->attributo_richiesto("TABELLA-RICERCA");
		$campo_risultati = $this->attributo("CAMPO-ID-TABELLA", campo_id_tabella($tabella_ricerca));


		if (!function_exists($op)) {
			throw new CodeException("Operatore non supportato: $op");
		}
		$this->attributi["MODE"] = "W";
		$this->attributi['NOME'] = $campo_ricerca;
		$this->attributi["RICHIESTO"] = "false";
		$this->attributi["JS-NAME"] = "filtro[op_tp|$op|$campo_ricerca|$campo_risultati|$tabella_ricerca|$campo]";
		$this->attributi["JS-ID"] = $this->attributo("JS-ID","filtro[$op|$campo]");
		$this->attributi["VALORE-DEFAULT"] = filtro_def(array_get("FILE", $parametri), $campo, "op_tp", $this->attributo("VALORE-DEFAULT"));

		$vecchio_gestore = $parametri['GESTORE'];
		$parametri['GESTORE'] = new GESTORE($tabella_ricerca);

		if (constant_true('ETICHETTA_FILTRI')) {
			echo '<table style="border: 0; margin: 0; padding: 0; border-collapse: collapse">';
			echo '<tr><td style="border: 0; margin: 0; padding: 0;">';
			parent::stampa_html($parametri);
			echo '</tr></td>';
			echo '<tr><td class="filter_label">', self::$op_desc[$op], '</td></tr>';
			echo '</table>';
		} else {
			parent::stampa_html($parametri);
		}

		$parametri['GESTORE'] =& $vecchio_gestore;
	}
}

/**
 * Restrizione del {@link CAMPO_FILTRO_TP} sulla ricerca per uguaglianza (nella subquery).
 * @package tag
 */
class COMBO_FILTRO_TP extends COMBO
{
	function stampa_html(&$parametri)
	{
		$rs       =& $parametri["GESTORE"];

		$tabella  = $this->attributo_richiesto("TABELLA");
		$campo_id_combo = $this->attributo_richiesto("CAMPO-ID-TABELLA");
		$rs_combo = new GESTORE($tabella, $campo_id_combo);

		$campo_ricerca   = $this->attributo_richiesto("NOME");
		$tabella_ricerca = $this->attributo_richiesto("TABELLA-RICERCA");

		if (!in_array($campo_ricerca, $rs->campi)) {
			throw new CodeException("Attenzione coder, il campo $campo_ricerca non esiste nella tabella/vista {$rs->tabella}");
		}

		$campi 		= explode("|", $this->attributo_richiesto("CAMPI"));

		$classe_css = $this->attributo("CLASS", array_get_default("CLASS", $parametri));

		$js_onclick = $this->attributo("JS-ONCLICK");

		// computo le colonne da caricare
		$colonne = array($campo_id_combo);

		for($i = 0; $i < count($campi) ; $i+=2){
			array_push($colonne, $campi[$i]);
		}

		$valore_default = filtro_def(array_get("FILE", $parametri), $campo_ricerca, "in_tp", NULL);

		$op = $this->attributo('OP', NULL);
		if ($op) {
			$nome = "filtro[op_tp|$campo_ricerca|$tabella_ricerca|$campo_id_combo|$op]";
		} else { 
			$nome = "filtro[in_tp|$campo_ricerca|$tabella_ricerca|$campo_id_combo]";
		}
		$js_id    = $this->attributo("JS-ID", $nome);
		$bloccato = cbool($this->attributo("BLOCCATO", "FALSE")) ? ' disabled="1"' : "";

		$rs_combo->carica($this->attributo("FILTRO", NULL), $this->attributo("ORDINE",NULL), $colonne, cbool($this->attributo("DISTINCT", "FALSE")));

		$tabindex = $this->get_tabindex();
		echo "<select onchange=\"$js_onclick\" tabindex=\"$tabindex\" class=\"$classe_css\" id=\"$js_id\" name=\"$nome\" $bloccato>";
		if (cbool($this->attributo("RIGA-VUOTA", "TRUE"))) {
			echo "<option value=\"\"></option>";
		}

		for(; !$rs_combo->eof; $rs_combo->move_next()) {

			$valore_id = $rs_combo->valore($campo_id_combo);
			if ( ($valore_id !== 0 && $valore_id == $valore_default) || ($valore_default === 0 && $valore_id === $valore_default) ) {
				echo "<option value=\"$valore_id\" selected=\"1\">";
			} else {
				echo "<option value=\"$valore_id\">";
			}
			for($i = 0; $i < count($campi) ; $i++){
				if ($i % 2 == 0) {
					echo $rs_combo->valore($campi[$i]);
				} else {
					echo " ".$campi[$i]." ";
				}
			}
			echo "</option>";
		}
		echo "</select>";

	}

}
?>