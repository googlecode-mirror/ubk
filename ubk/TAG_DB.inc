<?php
/**
 * Tag per la costruzione dei campi e restituzione dei valori provenienti dalla base dati
 * @author Ubik <emiliano.leporati@gmail.com>
 * @package tag
 */

/**
 * Genera le funzioni di controllo per l'obbligatorieta' dei dati, nonche' un campo nascosto con l'elenco delle checkbox, necessario a capire quali checkbox sono state de-spuntate (cfr. {@link prendi_riga}). 
 * @package tag
 */
class FORM extends CONTENITORE
{
	/**
	 * Dice se la modalita' del campo e' W o RW, e quindi e' necessario generare il javascript di controllo
	 * @param string $nome Il nome del campo o combo o picker di cui verificare la modalita' nel formato 'NOMETAG-NOMECAMPO'.
	 * 
	 */
	public function modalita_ok($nome, &$parametri) 
	{
		if (array_key_exists('MODALITA-FINALE', $parametri) && array_key_exists($nome, $parametri['MODALITA-FINALE'])) {
			return in_array($parametri['MODALITA-FINALE'][$nome], array("W", "RW"));
		} else {
			return FALSE;
		}
	}

	public function stampa_html(&$parametri)
	{
		static $numero_form = 0;

		$this->parametri =& $parametri;
		
		$parametri["FORM-N"] = $fn = $this->attributo('NUMERO', $numero_form);

		$nome_form  = $this->attributo("NAME", "ubkf_".$parametri["FORM-N"]);
		$campi_rich = explode("|", $this->attributo("CAMPI-RICHIESTI", ""));

		$this->attributi["NAME"] = $nome_form;
		$this->attributi["METHOD"] = "POST";
		$this->attributi["ENCTYPE"] = "multipart/form-data";

		// questi servono poi nei link x generare i vari javascript
		$parametri["FORM"]   = $nome_form;
//		$parametri["FORM-N"] = $parametri["FORM-N"];
		$parametri["FORM-CAMPI-RICHIESTI"] = $campi_rich;
		$parametri['MODALITA-FINALE'] = array();

		$this->parametri =& $parametri;

		$rs =& $parametri["GESTORE"];


		$atts = $this->attributi;
		unset($atts['CAMPI-RICHIESTI']);
		unset($atts['CAMPI-CONSIGLIATI']);
		unset($atts['NUMERO']);
		echo "<form".xml_att_crea($atts).">";
		unset($atts);
		// informazione sui campi CHECKBOX
		// devo sapere quali check ci sono sulla pagina, cosi' quando le posto sceccate (?) posso
		// sapere che c'erano e scrivere 0 sul db
		$checks   = array_merge($this->figli_profondita("CHECKBOX"), $this->figli_profondita("CHECKBOX_IMG"));
		$nomi_chk = array();
		foreach($checks as $check) {
			array_push($nomi_chk, $check->attributo_richiesto("NOME"));
		}
		echo "<input type=\"hidden\" name=\"checkboxes\" value=\"".implode("|", array_unique($nomi_chk))."\"/>";
		parent::stampa_html($parametri);
		$parametri["FORM-N"] = $fn;
		echo "</form>";
		
		// generazione javascript di controllo campi
		// voglio controllare: il formato di tutti i campi che ne hanno bisogno (data, float, int, etc.)
		// la presenza di tutti i dati necessari (not null sul db ?)
		$campi  = $this->figli_profondita("CAMPO");
		$combos = array_merge($this->figli_profondita("COMBO"), $this->figli_profondita("PICKER"), $this->figli_profondita("COD-DESC"));
		foreach($campi as $campo)
			$campo->parametri = $parametri;
		foreach($combos as $combo)
			$combo->parametri = $parametri;

		// campi consigliati
		$js = "function verifica_consigliati_{$parametri['FORM-N']}(id) {var msg = new Array(); var i = 0; var acapo = String.fromCharCode(10, 13);";
		$tmp = array();
		if ($this->attributo_esiste("CAMPI-CONSIGLIATI") && strlen($this->attributo("CAMPI-CONSIGLIATI"))) {
			// sintassi: (campo,desc)[|(campo,desc)*]
			$campi_cons = explode("|", $this->attributo("CAMPI-CONSIGLIATI"));

			foreach($campi_cons as $campo_cons) {
				list($nome, $desc) = explode(",", substr($campo_cons, 1, -1));
				$desc = str_replace("'", "\'", $desc);

				foreach($campi as $campo) {
					if ($this->modalita_ok($campo->nome.'-'.$nome, $parametri) && strtoupper($campo->attributo("TIPO","TEXT")) != "HIDDEN" ) {
						if (!in_array($nome, $tmp) && $nome == $campo->attributo_richiesto("NOME")) {
							array_push($tmp, $nome);
							$nome_campo_js = $campo->attributo_esiste("JS-ID") ? $campo->attributo('JS-ID') : $nome."[' + id + ']";
							$js .= "if (campo_vuoto('$nome_campo_js')) msg[i++] = '$desc';";
						}
					}
				}

				foreach($combos as $combo) {
					if ($this->modalita_ok($combo->nome.'-'.$nome, $parametri)) {
						if (!in_array($nome, $tmp) && $nome == $combo->nome()) {
							array_push($tmp, $nome);
							$nome_campo_js = $combo->attributo_esiste("JS-ID") ? $combo->attributo('JS-ID') : $nome."[' + id + ']";
							if ($combo->nome == "PICKER" || $combo->nome == "COD-DESC") {
								$js .= "if (campo_vuoto('$nome', id)) msg[i++] = '$desc';";
							} elseif (strtoupper($combo->attributo("TIPO","COMBO")) == "COMBO") {
								$js .= "if (combo_vuota('$nome_campo_js')) msg[i++] = '$desc';";
							} else {
								$js .= "if (radio_vuota('$nome_campo_js')) msg[i++] = '$desc';";
							}
						}
					}
				}
			}
			$js .= "if (msg.length == 0) { return true; } else { return confirm('Attenzione, si consiglia di riempire anche i seguenti campi: ' + acapo + acapo + msg.join(',' + acapo) + acapo + acapo + 'proseguire comunque ?') }";
		} else {
			$js .= "return true;";
		}
		$js .= "}";

		$campi_controllare = array_merge($rs->campi, $campi_rich);

		// necessarieta' campi
		$js .= "function verifica_obbligatori_{$parametri['FORM-N']}(id) { if (!verifica_consigliati_{$parametri['FORM-N']}(id)) return false; ";
		$tmp = array();
		foreach($campi as $campo) {
			$nome_campo = $campo->attributo_richiesto("NOME");
			$nome_esteso = $campo->nome.'-'.$nome_campo;
			if (in_array($nome_campo, $campi_controllare) && !in_array($nome_esteso, $tmp)) {
				array_push($tmp, $nome_esteso);
				if ($campo->attributo_esiste("VALIDAZIONE")) {
					$js .= $campo->attributo("VALIDAZIONE");
				} elseif ($this->modalita_ok($campo->nome.'-'.$nome_campo, $parametri) && strtoupper($campo->attributo("TIPO","TEXT")) != "HIDDEN" ) {
					if (!$campo->attributo_esiste("RICHIESTO") || ($campo->attributo_esiste("RICHIESTO") && cbool($campo->attributo_richiesto("RICHIESTO")))) {
						if (in_array($nome_campo, $campi_rich) || !$rs->nullable[$nome_campo] || cbool($campo->attributo("RICHIESTO","FALSE"))) {
							$nome_campo_js = $campo->attributo_esiste("JS-ID") ? $campo->attributo('JS-ID') : $nome_campo."[' + id + ']";
							$js .= "if (campo_obb_vuoto('$nome_campo_js')) return false; ";
						}
					}
				}
			}
		}

				
		// necessarieta' combo e picker
		$tmp = array();
		foreach($combos as $combo) {
			$nome_campo = $combo->nome();
			$nome_esteso = $combo->nome.'-'.$nome_campo;
			if (in_array($nome_campo, $campi_controllare) && !in_array($nome_esteso, $tmp)) {
				array_push($tmp, $nome_esteso);
				if ($this->modalita_ok($nome_esteso, $parametri)) {
					if ($combo->attributo_esiste("VALIDAZIONE")) {
						$js .= $combo->attributo("VALIDAZIONE");
					} elseif (!$combo->attributo_esiste("RICHIESTO") || ($combo->attributo_esiste("RICHIESTO") && cbool($combo->attributo_richiesto("RICHIESTO")))) {
						if (in_array($nome_campo, $campi_rich) || !$rs->nullable[$nome_campo] || cbool($combo->attributo("RICHIESTO","FALSE"))) {
							$nome_campo_js = $combo->attributo_esiste("JS-ID") ? $combo->attributo('JS-ID') : $nome_campo."[' + id + ']";
							if ($combo->nome == "PICKER" || $combo->nome == "COD-DESC") {
								$js .= "if (pick_obb_vuoto('$nome_campo_js')) return false; ";
							} elseif (strtoupper($combo->attributo("TIPO","COMBO")) == "COMBO") {
								$js .= "if (combo_obb_vuota('$nome_campo_js')) return false; ";
							} else {
								$js .= "if (radio_obb_vuoto('$nome_campo_js')) return false; ";
							}
						}
					} 
				}
			}
		}

		$js .= "return true; }";

		echo "<script language=\"Javascript\">$js</script>";

		$numero_form = max($numero_form + 1, $parametri['FORM-N']);

		unset($parametri["MODALITA-FINALE"]);
		unset($parametri["FORM"]);
		unset($parametri["FORM-N"]);
		unset($parametri["FORM-CAMPI-RICHIESTI"]);

		unset($this->attributi["NAME"]);
		unset($this->attributi["METHOD"]);
		unset($this->attributi["ENCTYPE"]);
	}
	
}


/**
 * Questa classe non e' relazionata ad alcun tag; serve per mettere a disposizione dei suoi figli (i campi) un metodo comune di individuazione della modalita' corretta (e consentita in base ai diritti) di visualizzazione.
 * @package tag
 */
class CAMPO_ACCESSO extends TAG
{
	/** 
	 * Contatore che viene incrementato per garantire l'ordine dei campi
	 * @var integer $tabindex
	 */
	protected static $tabindex = 0;

	public static function get_static_tabindex($tabindex)
	{
		self::$tabindex = max(self::$tabindex + 1, $tabindex);
		return self::$tabindex;
	}

	public function get_tabindex()
	{
		self::$tabindex = max(self::$tabindex + 1, $this->attributo('TABINDEX', 0));
		return self::$tabindex;
	}

	public function valore_id($modalita, &$rs)
	{
		switch($modalita) {
		case "RW":
			if (cbool($this->attributo("USA-ID", "TRUE"))) {
				if ($this->attributo_esiste("VALORE-ID")) {
					return $this->attributo("VALORE-ID");
				} else {
					return $rs->valore($rs->campo_id);
				}
			} else {
				return "-1";
			}
			break;
		case "W":
			if (cbool($this->attributo("USA-ID", "FALSE"))) {
				if ($this->attributo_esiste("VALORE-ID")) {
					return $this->attributo("VALORE-ID");
				} else {
					return $rs->valore($rs->campo_id);
				}
			} else {
				return "-1";
			}
			break;
		default:
			return NULL;
		}
	}

	public function valore_id_esteso()
	{
		$campo    = $this->attributo_richiesto("NOME");
		$modalita = $this->modalita($campo, $this->parametri);
		$rs       =& $this->parametri["GESTORE"];

		return $this->valore_id($modalita, $rs);
	}

	/**
	 * Se non ci troviamo in alcuna sezione, o non vi sono diritti fra i parametri, la modalita' rimane immutata. Se invece ci sono entrambi, allora il risultato e' computato in questo modo: se e' richiesta la modalita' W, bisogna avere il diritto W. Se e' richiesta la modalita' R, bisogna avere il diritto R. Se e' richiesta la modalita' RW, o si ha il diritto RW, oppure la modalita' viene abbassata ad R se si ha il diritto R. Se nessuna di queste condizioni e' vera, viene restituito NONE, e il campo o il suo valore non verranno visualizzati per niente. Per i campi filtro, basta avere il diritto R.
	 * @param array $parametri I parametri della {@link stampa_html}
	 * @return string
	 */
	public function modalita($campo, &$parametri)
	{
		$modalita = $this->attributo("MODE", $parametri["MODE"]);

		// prendo la sezione fra i parametri (vedi tag SEZIONE)
		$sezione = array_get_default("SEZIONE", $parametri);

		// se non c'e' nessuna sezione specificata, allora ritorno la modalita' originale
		if (strlen($sezione) == 0) {
			$_mod = $modalita;
		// altrimenti guardo se ci sono dei diritti specificati nei parametri
		} else {
			$diritti = array_get("DIRITTI", $parametri, array());
			// se non ci sono, ritorno la modalita' originale
			if (count($diritti) == 0) {
				$_mod = $modalita;
			// altrimenti dipende dal diritto specificato
			} else {
				$diritto = $this->attributo("DIRITTO", $modalita);
				// se la classe e' uno dei filtri, mi basta avere il diritto R
				if (finisce_per(get_class($this), "FILTRO") || finisce_per(get_class($this), "FILTRO_TP")) {
					$_mod = ($diritti[$sezione]["R"] ? $modalita : "NONE");
				// se ho il diritto, la modalita' va bene
				} elseif ($diritti[$sezione][$diritto]) {
					$_mod = $modalita;
				// se non ho il diritto (RW), la modalita' e' RW e ho il diritto R, allora ritorno R
				} elseif ($modalita == "RW" && $diritti[$sezione]["R"]) {
					$_mod = "R";
				// altrimenti niente
				} else {
					$_mod = "NONE";
				}
			}
		}
			
		$nome_campo = $this->nome.'-'.$campo;
		if (!($_mod == 'R' && array_key_exists('MODALITA-FINALE', $parametri) && array_key_exists($nome_campo, $parametri['MODALITA-FINALE']))){
			$parametri['MODALITA-FINALE'][$nome_campo] = $_mod;
		}

		return $_mod;
	}

	/**
	 * Dato campo e valore id, ritorna il nome e l'id javascript che avra' il campo sulla maschera in base agli attributi MULTIPLO, JS-NAME e JS-ID
	 * @param string $campo_id Nome del campo sul db
	 * @param mixed $id Id della riga corrente
	 * @param string &$nome Valore che dovra' avere l'attributo NAME del tag, INPUT o SELECT che sia
	 * @param string &$js_id Valore che dovra' avere l'attributo ID del tag, INPUT o SELECT che sia
	 */
	public function componi_nome($campo, $id, &$nome, &$js_id)
	{
		if (cbool($this->attributo("MULTIPLO", "FALSE"))) {
			$nome = ($this->attributo("JS-NAME", $campo))."[]";
		} else {
			$nome = $this->attributo("JS-NAME", $campo."[".$id."]");
		}
		$js_id = $this->attributo("JS-ID", $campo."[".$id."]");
	}

	/**
	 * Ritorna il nome javascript del campo
	 * @return string
	 */
	public function nome_js()
	{
		return $this->attributo("JS-NAME", $this->nome());
	}

	/**
	 * Ritorna l'id javascript del campo
	 * @return string
	 */
	public function id_js()
	{
		$campo    = $this->attributo_richiesto("NOME");
		$modalita = $this->modalita($campo, $this->parametri);
		$rs       =& $this->parametri["GESTORE"];
		$id = $this->valore_id($modalita, $rs);

		return $this->attributo("JS-ID", $campo."[".$id."]");
	}

}


/**
 * Campo classico (INPUT). 
 * @package tag
 */
class CAMPO extends CAMPO_ACCESSO
{

	public function nome()
	{
		return $this->attributo_richiesto("NOME");
	}

	public function stampa_html(&$parametri)
	{
		if (($modalita = $this->modalita($this->attributo_richiesto("NOME"), $parametri)) == "NONE") return;

		$campo    = $this->attributo_richiesto("NOME");
		$rs       = & $parametri["GESTORE"];

		if (!in_array($campo, $rs->campi)) {
			throw new CodeException("Attenzione coder, il campo $campo non esiste nella tabella/vista {$rs->tabella}");
		}

		$tipo 	  = strtoupper($this->attributo("TIPO","TEXT"));
		$classe_css = $this->attributo("CLASS", array_get_default("CLASS", $parametri));

		// valore default
		switch($modalita) {
		case "RW":
			if ($campo[0] == 'b')
				$valore_default	= (int)$rs->valore($campo);
			else
				$valore_default	= $rs->valore($campo);
			break;
		case "W":
			$valore_default	= $this->attributo("VALORE-DEFAULT", $rs->valore_default($campo));
			break;
		}
		$id = $this->valore_id($modalita, $rs);

		// stampa
		switch($modalita) {
		case "R":
			switch($tipo) {
			case "HIDDEN":
			case "PASSWORD":
				return;
			default:
				$da_stampare = $rs->valore($campo);
				$da_stampare = ((!is_numeric($da_stampare)) && empty($da_stampare)) ? $this->attributo("SE-NULL", "-") : $da_stampare;
				if ($this->attributo_esiste("FORMATO") && strlen($this->attributo("FORMATO"))) {
					$fmt_fun = $this->attributo("FORMATO");
					if (!function_exists($fmt_fun)) {
						throw new CodeException("Funzione di formattazione inesistente '$fmt_fun'");
					}
					$da_stampare = $fmt_fun($da_stampare);
				} elseif ($tipo == "TEXTAREA") {
					$da_stampare = nl2br($da_stampare);
				}

				$trim_length = $this->attributo("TRIM", 0);
				if ($trim_length && strlen($da_stampare) > $trim_length){
					$da_stampare = substr($da_stampare,0,$trim_length) . '...';	
				}
				if (strlen($classe_css) > 0) {
					echo "<span class=\"$classe_css\">";
					echo $da_stampare;
					echo "</span>";
				} /* elseif ($tipo == "TEXTAREA" && cbool($this->attributo("SCROLLBARS", "TRUE"))) {
					$size = explode(",", $this->attributo("SIZE","2,50"));
					$height = $size[0]."em";
					$width = $size[1]."ex";
					echo "<div class=\"TEXTAREA_R\" style=\"overflow: auto; float: left; display: inline; height: $height; width: $width;\">", $da_stampare, "</div>";
				} */ else {
					echo $da_stampare;
				}
			}
			break;
		case "RW":
		case "W":
			if ($this->attributo_esiste("FORMATO") && strlen($this->attributo("FORMATO"))) {
				$fmt_fun = $this->attributo("FORMATO");
				if (!function_exists($fmt_fun)) {
					throw new CodeException("Funzione di formattazione inesistente '$fmt_fun'");
				}
				$valore_default = $fmt_fun($valore_default);
			}


			$maxsize  = $this->attributo("MAXLENGTH", $rs->lunghezza[$campo]);
			if ($tipo == "TEXTAREA") {
				$size = $this->attributo("SIZE","2,50");
			} else {
				switch($campo[0]) {
				case "d":
					$size = $this->attributo("SIZE", 12);
					break;
				case "h":
					$size = $this->attributo("SIZE", 5);
					break;
				case "i":
				case "f":
					$size = $this->attributo("SIZE", 6);
					break;
				default:
					$size = $this->attributo("SIZE");
					if (strtoupper($size) == "DB")
						$size = $rs->lunghezza[$campo];
				}
			}

			if (in_array($campo[0], array("d", "i", "f", "h"))) {
				$align = $this->attributo("ALIGN", "right");
			} else {
				$align = $this->attributo("ALIGN", "left");
			}

			$bloccato = cbool($this->attributo("BLOCCATO", "FALSE")) ? ' readonly="1"' : "";
			if ($bloccato)
				$classe_css .= " bloccato";

			// javascript
			$js_onblur = $this->attributo("JS-ONBLUR");
			$js_onchange = $this->attributo("JS-ONCHANGE");
			switch($campo[0]) {
			case "d":
				$century   = $this->attributo("SECOLO", 20);
				$js_onblur = "if (!Fld.Check.date(this, '$century')) { return false;}".$js_onblur;
				break;
			case "h":
				$js_onblur = "if (!Fld.Check.hour(this)) { return false;}".$js_onblur;
				break;
			case "f":
				$js_onblur = "if (!Fld.Check.real(this)) { return false;}".$js_onblur;
				break;
			case "i":
				$js_onblur = "if (!Fld.Check.integer(this)) { return false;}".$js_onblur;
				break;
			}

			$this->componi_nome($campo, $id, $nome, $js_id);

			$js_onblur = strtr(
							$js_onblur, 
							array(
								'@form' => array_get_default('FORM', $parametri, 'null'), 
								'@id' => $id
							));


			if (finisce_per(get_class($this), "FILTRO")  || finisce_per(get_class($this), "FILTRO_TP")) {
				$star = "";
			} else {
				$star = ((array_key_exists('FORM-CAMPI-RICHIESTI', $parametri) && in_array($campo, $parametri['FORM-CAMPI-RICHIESTI'])) || 
						(!$rs->nullable[$campo] && !$this->attributo_esiste("RICHIESTO") ) || 
						cbool($this->attributo("RICHIESTO", "FALSE"))) ? "*" : "";
				if (constant_true('STAR') && strlen($star))
					$star = '<span class="star">'.$star.'</span>';
			}

			switch($tipo) {
			case "HIDDEN":
				echo "<input type=\"$tipo\" id=\"$js_id\" name=\"$nome\" value=\"$valore_default\"/>";
				break;
			case "TEXT":
			case "PASSWORD":
				$tabindex = $this->get_tabindex();
				echo "<input tabindex=\"$tabindex\" class=\"$classe_css\" type=\"$tipo\" id=\"$js_id\" name=\"$nome\" size=\"$size\" value=\"$valore_default\" style=\"text-align: $align\" maxlength=\"$maxsize\" $bloccato onkeyup=\"$js_onchange\" onblur=\"$js_onblur\"/>".$star;
				if ($campo[0] == "d" && cbool($this->attributo("PICK", "FALSE"))) {
					$cal_gateway = $this->attributo("PICK-GATEWAY", "calendario_pop.php");
					$cal_file    = $this->attributo("PICK-FILE", "calendario_pop.xml");
					$location = "$cal_gateway?FILE=$cal_file";

					$location = url_codifica($location);					
					echo "<A href=\"#\" onclick=\"o = window.open('$location', 'calendario', 'top=50,left=50,width=320,height=230'); o.focus(); window.calendarTarget = '$js_id';\" title=\"Scegli da calendario\" class=\"$classe_css\">";
					echo CACHE_CLASS_FACTORY::get()->img_get('Search');
					echo "</A>";					
				}
				break;
			case "RADIO":
				$tabindex = $this->get_tabindex();
				// bloccato da rifare
				if($modalita=='RW'){
					$selected = $this->attributo("VALORE-DEFAULT") == $valore_default ? 'checked="1"' : '';
				}
				echo "<input tabindex=\"$tabindex\" class=\"$classe_css\" type=\"$tipo\" name=\"$nome\" id=\"$js_id\" value=\"$valore_default\" onclick=\"$js_onblur\" $selected/>".$star;
				break;
			case "TEXTAREA":
				$tabindex = $this->get_tabindex();
				$size = explode(",",$size);
				echo "<TEXTAREA tabindex=\"$tabindex\" onkeyup=\"$js_onchange\" onblur=\"if (this.value.length > $maxsize) { alert('Massimo $maxsize caratteri !'); this.focus(); return false; } $js_onblur\" class=\"$classe_css\" id=\"$js_id\" name=\"$nome\" rows=\"$size[0]\" cols=\"$size[1]\" style=\"text-align: $align\" $bloccato>$valore_default</TEXTAREA>".$star;
				break;
			}

		}
	}
	
}


/**
 * Campo che in modalita' R genera un link di tipo mailto:. 
 * @package tag
 */
class CAMPO_MAIL extends CAMPO
{
	public function stampa_html(&$parametri)
	{
		$modalita = $this->modalita($this->attributo_richiesto("NOME"), $parametri);
		$tipo     = $this->attributo("TIPO", "TEXT");

		switch($modalita) {
		case "NONE":
			return;
		case "R":
			switch($tipo) {
			case "HIDDEN":
			case "PASSWORD":
				return;
			default:
				$rs    = & $parametri["GESTORE"];
				$campo = $this->attributo_richiesto("NOME");
				$classe_css = $this->attributo("CLASS", array_get_default("CLASS", $parametri));
				$mail  = $rs->valore($campo);
				echo "<A href=\"mailto:$mail\" title=\"Invia mail\" class=\"$classe_css\">$mail</A>";
				return;
			}
		default:
			if (!$this->attributo_esiste("JS-ONBLUR")) $this->attributi["JS-ONBLUR"] = "";
			$this->attributi["JS-ONBLUR"] = "if (!Fld.Check.mail(this)) { return false; }";
			parent::stampa_html($parametri);
		}
	}
}

/**
 * Checkbox. 
 * @package tag
 */
class CHECKBOX extends CAMPO_ACCESSO
{

	public function stampa_html(&$parametri)
	{
		if (($modalita = $this->modalita($this->attributo_richiesto("NOME"), $parametri)) == "NONE") return;

		$campo    = $this->attributo_richiesto("NOME");
		$rs       =& $parametri["GESTORE"];

		if (!in_array($campo, $rs->campi)) {
			throw new CodeException("Attenzione coder, il campo $campo non esiste nella tabella/vista {$rs->tabella}");
		}

		$bloccato   = cbool($this->attributo("BLOCCATO", "FALSE")) ? 'disabled="1"' : "";
		$classe_css = $this->attributo("CLASS", array_get_default("CLASS", $parametri));

		$id = $this->valore_id($modalita, $rs);
		$value = $this->attributo("VALORE", "1");

		$js_onclick = strtr(
						$this->attributo("JS-ONCLICK"), 
						array(
							'@form' => array_get_default('FORM', $parametri, 'null'), 
							'@id' => $id
						));

		switch ($modalita) {
		case "R":

			if ($this->attributo_esiste('ETICHETTE') && $this->attributo_esiste('IMG')) {
				
				throw new CodeException('Specificare o ETICHETTE o IMG su CHECKBOX, non entrambi.');
			
			} elseif ($this->attributo_esiste('ETICHETTE')) {
			
				$etichs = $this->attributo('ETICHETTE');
				if (strchr($etichs,'|') === false){
					throw new CodeException("E' necessario specificare i valori di entrambe le etichette per CHECKBOX/ETICHETTE");
				}
				list($valore_vero, $valore_falso) = explode('|', $etichs);

			} elseif ($this->attributo_esiste('IMG')) {

				$img = $this->attributo("IMG");
				if (!inizia_per($img, 'img/')) $img = 'img/'.$img;
				$estensione = constant_def('DEF_IMG_EXT', '.gif');
				$valore_vero = '<img src="'.$img.'SI'.$estensione.'"/>';
				$valore_falso = '<img src="'.$img.'NO'.$estensione.'"/>';

			} else {
				
				$valore_vero = 'Vero';
				$valore_falso = 'Falso';
			}
	
			echo $rs->valore($campo) ? $valore_vero : $valore_falso;
			break;

		case "W":
			$tabindex = $this->get_tabindex();
			$this->componi_nome($campo, $id, $nome, $js_id);
			$valore_default	= $this->attributo("VALORE-DEFAULT", $rs->valore_default($campo));
			$checked   	    = ($valore_default == "1" || cbool($valore_default)) ? 'checked="1"' : "";
			echo "<input class=\"$classe_css\" onclick=\"$js_onclick\" style=\"border: 0; background-color: transparent;\" tabindex=\"$tabindex\" type=\"checkbox\" id=\"$js_id\" name=\"$nome\" value=\"$value\" $checked $bloccato/>";
			break;

		case "RW":
			$tabindex = $this->get_tabindex();
			$this->componi_nome($campo, $id, $nome, $js_id);
			$checked = $rs->valore($campo) ? 'checked="1"' : "";
			echo "<input class=\"$classe_css\" onclick=\"$js_onclick\" style=\"border: 0; background-color: transparent;\" tabindex=\"$tabindex\" type=\"checkbox\" id=\"$js_id\" name=\"$nome\" value=\"$value\" $checked $bloccato/>";
			break;
		}
	}
	
}

class CHECKBOX_IMG extends CAMPO_ACCESSO
{

	public function stampa_html(&$parametri)
	{
		if (($modalita = $this->modalita($this->attributo_richiesto("NOME"), $parametri)) == "NONE") return;

		$campo    = $this->attributo_richiesto("NOME");
		$rs       =& $parametri["GESTORE"];

		if (!in_array($campo, $rs->campi)) {
			throw new CodeException("Attenzione coder, il campo $campo non esiste nella tabella/vista {$rs->tabella}");
		}

		$bloccato = cbool($this->attributo("BLOCCATO", "FALSE"));

		$id = $this->valore_id($modalita, $rs);
		$value = $this->attributo("VALORE", "1");

		$img = $this->attributo_richiesto("IMG"); if (!inizia_per($img, 'img/')) $img = 'img/'.$img;
		$img .= '/'.basename($img);
		$estensione = constant_def('DEF_IMG_EXT', '.gif');

		$js_onclick = trim(strtr(
						$this->attributo("JS-ONCLICK"), 
						array(
							'@form' => array_get_default('FORM', $parametri, 'null'), 
							'@id' => $id
						     )));
		if ($js_onclick && !finisce_per($js_onclick, ';')) $js_onclick .= ';';
		$js_onclick .= "Check.imageToggle(this, '$bloccato');";
		
		$alt = $this->attributo_richiesto('ALT');
		$classe_css = $this->attributo("CLASS", array_get_default("CLASS", $parametri));

		switch ($modalita) {
		case "R":

			$valore_vero  = "<img title=\"$alt\" class=\"check $classe_css\" src=\"{$img}SIR{$estensione}\"/>";
			$valore_falso = "<img title=\"$alt\" class=\"check $classe_css\" src=\"{$img}NOR{$estensione}\"/>";

			echo $rs->valore($campo) ? $valore_vero : $valore_falso;
			break;

		case "W":
			$this->componi_nome($campo, $id, $nome, $js_id);
			$valore_default	= $this->attributo("VALORE-DEFAULT", $rs->valore_default($campo));
			$checked   	    = ($valore_default == "1" || cbool($valore_default)) ? 'checked="1"' : "";

			$tabindex = $this->get_tabindex();
			echo "<input tabindex=\"$tabindex\" onclick=\"$js_onclick\" style=\"display: none;\" type=\"checkbox\" id=\"$js_id\" name=\"$nome\" value=\"$value\" $checked/>";
			
			$tabindex = $this->get_tabindex();
			$src = ($bloccato ? ($checked ? 'SIR' : 'NOR') : ($checked ? 'SI' : 'NO'));
			echo "<img title=\"$alt\" class=\"check $classe_css\" src=\"{$img}{$src}{$estensione}\" tabindex=\"$tabindex\" ".
					"onclick=\"Check.checkToggle(this, '$bloccato')\" onkeydown=\"Check.keyToggle(event, '$bloccato');\"/>";
			break;

		case "RW":
			$this->componi_nome($campo, $id, $nome, $js_id);
			$checked = $rs->valore($campo) ? 'checked="1"' : "";

			$tabindex = $this->get_tabindex();
			echo "<input tabindex=\"$tabindex\" onclick=\"$js_onclick\" style=\"display: none;\" type=\"checkbox\" id=\"$js_id\" name=\"$nome\" value=\"$value\" $checked/>";

			$tabindex = $this->get_tabindex();
			$src = ($bloccato ? ($checked ? 'SIR' : 'NOR') : ($checked ? 'SI' : 'NO'));
			echo "<img title=\"$alt\" class=\"check $classe_css\" src=\"{$img}{$src}{$estensione}\" tabindex=\"$tabindex\" ".
				"onclick=\"Check.checkToggle(this, '$bloccato');\" onkeydown=\"Check.keyToggle(event, '$bloccato');\"/>";
			break;
		}
	}
	
}

/**
 * Checkbox per visualizzare presenza dati su una tabella esterna.
 * @package tag
 */
class CHECKBOX_FK extends CAMPO_ACCESSO
{

	public function stampa_html(&$parametri)
	{
		if (($modalita = $this->modalita($this->attributo_richiesto("NOME"), $parametri)) == "NONE") return;

		$campo    = $this->attributo_richiesto("NOME");
		$rs       =& $parametri["GESTORE"];
		# boh, con calma aggiungere controlli sui campi

		$classe_css = $this->attributo("CLASS");
		$bloccato = cbool($this->attributo("BLOCCATO", "FALSE")) ? 'disabled="1"' : "";

		$id = $this->valore_id($modalita, $rs);

		// dati della tabella esterna
		$tabella_fk  = $this->attributo_richiesto("TABELLA");
		$campo_id_fk = $this->attributo("CAMPO-ID-LINK", $campo);
		if ($this->attributo_esiste("FILTRO")) {
			$filtro = c_and($this->attributo("FILTRO"), equ($campo_id_fk, $rs->valore($campo)));
		} else {
			$filtro = equ($campo_id_fk, $rs->valore($campo));
		}

		// verifico se esiste
		$esiste = conta($tabella_fk, $filtro);

		switch ($modalita) {
		case "R":
			if ($this->attributo_esiste("IMG")) {
				$img = $this->attributo("IMG");
				if (!inizia_per($img, 'img/')) $img = 'img/'.$img;
				echo '<img src="'.$img.($esiste ? "SI.gif" : "NO.gif").'"/>';
			} else {
				echo "<SPAN style=\"height: 10px; width: 10px; border: 1px solid black; font-size: 8px;\">";
				echo $esiste ? "X" : "";
				echo "</SPAN>";
			}
			break;

		case "RW":
			$tabindex = $this->get_tabindex();
			$this->componi_nome($campo, $id, $nome, $js_id);
			$checked = $esiste ? 'checked="1"' : "";
			echo "<input class=\"$classe_css\" style=\"border: 0; background-color: transparent;\" tabindex=\"$tabindex\" type=\"checkbox\" id=\"$js_id\" name=\"$nome\" value=\"1\" $checked $bloccato/>";
			break;
		}
	}
	
}

/**
 * Base per COMBO, PICKER, COD_DESC, implementa id_js
 * @package tag
 */
class CAMPO_TABELLA_ESTERNA extends CAMPO_ACCESSO
{
	public function id_js()
	{
		$tabella  = $this->attributo_richiesto("TABELLA");
		$campo_id_combo = $this -> attributo("CAMPO-ID-TABELLA", campo_id_tabella($tabella));

		$campo    = $this->attributo("NOME", $campo_id_combo);
		$modalita = $this->modalita($campo, $this->parametri);
		$rs       =& $this->parametri["GESTORE"];
		$id = $this->valore_id($modalita, $rs);

		return $this->attributo("JS-ID", $campo."[".$id."]");
	}

	public function valore_id_esteso()
	{
		$tabella  = $this->attributo_richiesto("TABELLA");
		$campo_id_combo = $this -> attributo("CAMPO-ID-TABELLA", campo_id_tabella($tabella));

		$campo    = $this->attributo("NOME", $campo_id_combo);
		$modalita = $this->modalita($campo, $this->parametri);
		$rs       =& $this->parametri["GESTORE"];

		$old_campo_id = $rs->campo_id;
		$rs->campo_id = $campo;
		$valore_id = $this->valore_id($modalita, $rs);
		$rs->campo_id = $old_campo_id;

		return $valore_id;
	}

}

/**
 * Combo / Radio group. 
 * @package tag
 */
class COMBO extends CAMPO_TABELLA_ESTERNA
{
	/**
	 * Ritorna il nome con cui sara' identificata la combo nel form - usata nella generazione dei controlli in {@link FORM::stampa_html}
	 * @return string
	 */
	public function nome()
	{
		$tabella  = $this->attributo_richiesto("TABELLA");
		$campo_id_combo = $this -> attributo("CAMPO-ID-TABELLA", campo_id_tabella($tabella));
		return $this->attributo("NOME", $campo_id_combo);
	}

	public function stampa_html(&$parametri)
	{
		if (($modalita = $this->modalita($this->nome(), $parametri)) == "NONE") return;

		$rs       =& $parametri["GESTORE"];

		$trim_length = $this->attributo("TRIM", 0);

		$tabella  = $this->attributo_richiesto("TABELLA");
		$campo_id_combo = $this -> attributo("CAMPO-ID-TABELLA", campo_id_tabella($tabella));

		$rs_combo = new GESTORE($tabella, $campo_id_combo);
		$campo_id = $this->attributo("NOME", $campo_id_combo);

		if (!in_array($campo_id, $rs->campi) && !$this->attributo_esiste('VALORE-ID')) {
			throw new CodeException("Attenzione coder, il campo $campo_id non esiste nella tabella/vista {$rs->tabella}");
		}

		if ($this->attributo_esiste("FORMATO") && strlen($this->attributo("FORMATO"))) {
			$fmt_fun = $this->attributo("FORMATO");
			if (!function_exists($fmt_fun)) {
				throw new CodeException("Funzione di formattazione inesistente '$fmt_fun'.");
			}
		}

		$tipo = strtoupper($this->attributo("TIPO", "COMBO"));

		$campi 		= explode("|", $this->attributo_richiesto("CAMPI"));

		$bloccato 	= cbool($this->attributo("BLOCCATO", "FALSE")) ? ' disabled="1"' : "";
		$classe_css = $this->attributo("CLASS", array_get_default("CLASS", $parametri));

		// computo le colonne da caricare
		$colonne = array($campo_id_combo);

		for($i = 0; $i < count($campi) ; $i+=2){
			array_push($colonne, $campi[$i]);
		}
		$colonne = array_unique($colonne);

		switch ($modalita) {
		case "R":

			if ($this->attributo_esiste("VALORE-ID")) {
				$valore_id = $this->attributo("VALORE-ID");
			} else {
				$valore_id = $rs->valore($campo_id);
			}
			if($this->attributo_esiste("FILTRO")){
				$rs_combo->carica(
						c_and(equ($campo_id_combo, $valore_id), $this->attributo("FILTRO", NULL)), 
						$this->attributo("ORDINE", NULL), 
						$colonne, 
						cbool($this->attributo("DISTINCT", "FALSE")), 
						FALSE);
			} else {
				$rs_combo->abilita_cache(true);
				$rs_combo->carica(equ($campo_id_combo, $valore_id), $this->attributo("ORDINE", NULL), $colonne, cbool($this->attributo("DISTINCT", "FALSE")),null,null,false);
			}


			if ($rs_combo->eof) {

				$risultato = $this->attributo("SE-NULL","-");

			} else {

				for($risultato = "", $i = 0; $i < count($campi) ; $i++){
					if ($i % 2 == 0) {
						if (isSet($fmt_fun)) {
							$risultato .= $fmt_fun($rs_combo -> valore($campi[$i]));
						} else {
							$risultato .= $rs_combo -> valore($campi[$i]);
						}
					} else {
						$risultato .= " ".$campi[$i]." ";
					}
				}
			}

			if ($trim_length && strlen($risultato) > $trim_length){
				$risultato = substr($risultato,0,$trim_length) . '...';	
			}

			if (strlen($classe_css) > 0) {
				echo "<SPAN class=\"$classe_css\">";
				echo $risultato;
				echo "</SPAN>";
			} else {
				echo $risultato;
			}
			break;

		case "W":
			if ($this->attributo_esiste("CAMPO-VALORE-DEFAULT") && 
					(!$this->attributo_esiste("VALORE-DEFAULT") || $this->attributo("VALORE-DEFAULT")=="")) {
				$campo_default = $this->attributo("CAMPO-VALORE-DEFAULT");
				array_push($colonne, $campo_default);
			}		
		case "RW":
			$valore_default	= ($modalita == "RW")
								? $rs->valore($campo_id)
								: $this->attributo("VALORE-DEFAULT", $rs_combo->valore_default($campo_id_combo));
			$id = $this->valore_id($modalita, $rs);

			$this->componi_nome($campo_id, $id, $nome, $js_id);

			$js_onclick = strtr(
							$this->attributo("JS-ONCLICK"), 
							array(
								'@form' => array_get_default('FORM', $parametri, 'null'), 
								'@id' => $id
							));

			if (finisce_per(get_class($this), "FILTRO")) {
				$star = "";
			} else {
				$star = ((array_key_exists('FORM-CAMPI-RICHIESTI', $parametri) && in_array($campo_id, $parametri['FORM-CAMPI-RICHIESTI'])) || 
						(!$rs->nullable[$campo_id] && !$this->attributo_esiste("RICHIESTO") ) || 
						cbool($this->attributo("RICHIESTO","FALSE"))) ? NBSP."*" : "";
				if (constant_true('STAR') && strlen($star))
					$star = '<span class="star">'.$star.'</span>';
			}

			$rs_combo->carica(
						$this->attributo("FILTRO", NULL), 
						$this->attributo("ORDINE", $campi[0]), 
						$colonne, 
						cbool($this->attributo("DISTINCT", "FALSE")),
						FALSE);

			if ($tipo == "COMBO") {

				$tabindex = $this->get_tabindex();
				echo "<select onchange=\"$js_onclick\" tabindex=\"$tabindex\" class=\"$classe_css\" id=\"$js_id\" name=\"$nome\" $bloccato>";
				if (cbool($this->attributo("RIGA-VUOTA", "TRUE"))) {
					echo "<option value=\"\"></option>";
				}

				for(; !$rs_combo->eof; $rs_combo->move_next()) {

					$valore_id = $rs_combo->valore($campo_id_combo);
					
					if (isset($campo_default)){
						if ($rs_combo->valore($campo_default)){
							$valore_default = $valore_id;
							unset($campo_default); // in questo modo prende solo il primo
						}	
					}
										
					$text = '';
					for($i = 0; $i < count($campi) ; $i++){
						if ($i % 2 == 0) {
							if (isSet($fmt_fun)) {
								$text .= $fmt_fun($rs_combo -> valore($campi[$i]));
							} else {
								$text .= $rs_combo -> valore($campi[$i]);
							}
						} else {
							$text .= " ".$campi[$i]." ";
						}
					}
					
					if ($trim_length && strlen($text) > $trim_length){
						$text = substr($text,0,$trim_length) . '...';	
					}

					if ( ($valore_id !== 0 && $valore_id == $valore_default) || ($valore_default === 0 && $valore_id === $valore_default) ) {
						echo "<option value=\"$valore_id\" selected=\"1\">$text</option>";
					} else {
						echo "<option value=\"$valore_id\">$text</option>";
					}
					
				}
				echo "</select>".$star;

			} else {
				for(; !$rs_combo->eof; $rs_combo->move_next()) {


					$tabindex = $this->get_tabindex();
					$valore_id = $rs_combo->valore($campo_id_combo);
					
					echo "<input tabindex=\"$tabindex\" type=\"radio\" class=\"$classe_css\" name=\"$nome\" value=\"$valore_id\" $bloccato";
					if ($valore_id == $valore_default) echo " checked=\"1\"";
					echo "/>";
					
					$text = '';
					
					for($i = 0; $i < count($campi) ; $i++){
						if ($i % 2 == 0) {
							if (isSet($fmt_fun)) {
								$text .= $fmt_fun($rs_combo -> valore($campi[$i]));
							} else {
								$text .= $rs_combo -> valore($campi[$i]);
							}
						} else {
							$text .= " ".$campi[$i]." ";
						}
					}
					
					if ($trim_length && strlen($text) > $trim_length){
						$text = substr($text,0,$trim_length) . '...';	
					}
					
					echo $text . BR;
				}
				echo $star;
			}
			unset($rs_combo);

			break;

		}
	}
	
}


/**
 * Campo picker - permette di scegliere un valore da una tabella aprendo una pop-up; utilizzabile al posto di combo molto pesanti. 
 * @package tag
 */
class PICKER extends CAMPO_TABELLA_ESTERNA
{
	/**
	 * Ritorna il nome con cui sara' identificata la combo nel form - usata nella generazione dei controlli in {@link FORM::stampa_html}
	 * @return string
	 */
	public function nome()
	{
		$tabella  = $this->attributo_richiesto("TABELLA");
		$campo_id_combo = $this -> attributo("CAMPO-ID-TABELLA", campo_id_tabella($tabella));
		return $this->attributo("NOME", $campo_id_combo);
	}

	/**
	 * Stampa il link che apre la finestra pop-up di ricerca
	 */
	public function stampa_link_pop_up(&$parametri, $js_id, $campi)
	{
		$link      =& $this->figlio_richiesto("LINK");
		$pop_up    =& $link->figlio_richiesto("POP-UP");
		$link -> parametri = $parametri;
		$pop_up->parametri = $parametri;
		if (is_null($contenuto = $link->figlio("CONTENUTO"))) {
			$contenuto = CACHE_CLASS_FACTORY::get()->img_get($this->attributo('IMG-CERCA', 'Search'));
		} else {
			$contenuto = $contenuto->stampa_html_in_stringa($parametri);
		}

		$classe_css = $link->attributo("CLASS", array_get_default("CLASS", $parametri));
		$target     = $link->attributo("TARGET", "pick_up");
		$title      = $link->attributo("ALT", "Scegli");

		$gateway     = gateway($link->attributo("GATEWAY",
										array_get_default(
											"GATEWAY",
											$parametri,
											$_SERVER["PHP_SELF"]
										)
								)
						);
		$tabella    = $link->attributo("TABELLA", $this->attributo_richiesto("TABELLA"));
		$tabella_pick    = $this->attributo_richiesto("TABELLA");
		$campo_id_pick   = $this->attributo("CAMPO-ID-TABELLA", campo_id_tabella($tabella_pick));
		$campo_id   = $link->attributo("CAMPO-ID", $this -> attributo("CAMPO-ID-TABELLA", campo_id_tabella($tabella)));
		$file       = $link->attributo_richiesto("FILE");
		$ordine     = $link->attributo('ORDINE', '');

		$azione     = $link->attributo("AZIONE", NULL);
		if (!is_null($azione)) {
			$azione = "AZIONE=$azione&amp;";
		}
		$usa_id  = cbool($this->attributo("USA-ID", "FALSE"));
		if ($usa_id) {
			if ($this->attributo_esiste('VALORE-ID')) {
				$valore_id = $this->attributo('VALORE-ID');
			} else {
				$rs = &$parametri['GESTORE'];
				$valore_id = $rs->valore($campo_id);
			}
			$valore_id = "&amp;ID=$valore_id";
		} elseif (cbool($link->attributo("USA-ID", "FALSE"))) {
			if ($link->attributo_esiste('VALORE-ID')) {
				$valore_id = $link->attributo('VALORE-ID');
			} else {
				$rs = &$parametri['GESTORE'];
				$valore_id = $rs->valore($campo_id);
			}
			$valore_id = "&amp;ID=$valore_id";
		} else {
			$valore_id ='';
		}

		$location  = $gateway."?{$azione}TABELLA-PICK=$tabella_pick&amp;CAMPO-ID-PICK=$campo_id_pick&amp;TABELLA=$tabella$valore_id&amp;ORDINE=$ordine&amp;CAMPO-ID=$campo_id&amp;FILE=$file&amp;CAMPO-TARGET=$js_id&amp;CAMPI=$campi";
		if ($link->attributo_esiste("PAGE-SIZE"))
			$location .= "&amp;PAGE-SIZE=".$link->attributo("PAGE-SIZE");

		$extra_get = $this->extra_get($link);
		
		if ($extra_get){
			$location .= $extra_get;	
		}

		$location = url_codifica($location);

		$par_open   = array("toolbar=no");
		if ($pop_up->attributo_esiste("TOP"))    array_push($par_open, "top=".$pop_up->attributo("TOP"));
		if ($pop_up->attributo_esiste("LEFT"))   array_push($par_open, "left=".$pop_up->attributo("LEFT"));
		if ($pop_up->attributo_esiste("HEIGHT")) array_push($par_open, "height=".$pop_up->attributo("HEIGHT"));
		if ($pop_up->attributo_esiste("WIDTH"))  array_push($par_open, "width=".$pop_up->attributo("WIDTH"));
		if ($pop_up->attributo_esiste("SCROLLBARS"))
			array_push($par_open, "scrollbars=".(cbool($pop_up->attributo("SCROLLBARS")) ? "yes" : "no"));

		if ($pop_up->attributo_esiste("HEIGHT") && ($pop_up->attributo_esiste("WIDTH"))) {
			$resize = "o.resizeTo(".$pop_up->attributo("WIDTH").", ".($pop_up->attributo("HEIGHT") + 20).");";
		} else {
			$resize = "";
		}

		$js_onclick = $link->attributo("JS-ONCLICK");

		echo "<a href=\"#\" title=\"$title\" onclick=\"$js_onclick; o=window.open('$location','$target', '".implode(",", $par_open)."'); o.focus(); $resize\" class=\"$classe_css\">";
		echo $contenuto;
		echo "</a>";
	}

	/**
	 * Stampa il link che svuota il campo testo
	 */
	public function stampa_link_annulla($js_id)
	{
		$img = $this->attributo('IMG-PULISCI', 'Del');
		echo "<a title=\"Svuota\" href=\"#\" onclick=\"var o = document.getElementById('$js_id'); o.value = ''; o.nextSibling.value = ''; if (o.onchange != null) o.onchange.call(o);\">";
		echo CACHE_CLASS_FACTORY::get()->img_get($img);
		echo "</a>";
	}

	/**
	 * Ritorna la stringa con cui riempire il campo testo
	 * @param GESTORE &$rs I dati correnti
	 * @param array $campi I campi e i separatori con cui comporre la stringa
	 * @param mixed $se_eof Il valore da visualizzare se non ci sono dati di origine
	 */
	public function decodifica(&$rs, $campi, $se_eof)
	{
		if ($rs->eof) {

			return $se_eof;

		} else {

			for($risultato = "", $i = 0; $i < count($campi) ; $i++){
				if ($i % 2 == 0) {
					$risultato .= $rs -> valore($campi[$i]);
				} else {
					$risultato .= " ".$campi[$i]." ";
				}
			}
			return $risultato;
		}
	}

	public function stampa_html(&$parametri)
	{
		if (($modalita = $this->modalita($this->nome(), $parametri)) == "NONE") return;

		$rs       =& $parametri["GESTORE"];

		$id = $this->valore_id($modalita, $rs);

		$tabella  = $this->attributo_richiesto("TABELLA");
		$campo_id_combo = $this -> attributo("CAMPO-ID-TABELLA", campo_id_tabella($tabella));

		$rs_combo = new GESTORE($tabella, $campo_id_combo);
		$campo_id = $this->attributo("NOME", $campo_id_combo);

		if (!in_array($campo_id, $rs->campi)) {
			throw new CodeException("Attenzione coder, il campo $campo_id non esiste nella tabella/vista {$rs->tabella}");
		}

		$campi 		= explode("|", $this->attributo_richiesto("CAMPI"));

		$classe_css = $this->attributo("CLASS", array_get_default("CLASS", $parametri));
		$size = $this->attributo("SIZE");

		$js_onchange = strtr(
						$this->attributo("JS-ONCHANGE"), 
						array(
							'@form' => array_get_default('FORM', $parametri, 'null'), 
							'@id' => $id
						));

		$colonne   = array();
		for($i = 0; $i < count($campi) ; $i+=2){
			array_push($colonne, $campi[$i]);
		}

		switch ($modalita) {
		case "R":

			$rs_combo->carica(equ($campo_id_combo, $rs->valore($campo_id)), NULL, $colonne,false,false);

			$risultato = $this->decodifica($rs_combo, $campi, $this->attributo("SE-NULL","-"));

			if (strlen($classe_css) > 0) {
				echo "<span class=\"$classe_css\">";
				echo $risultato;
				echo "</span>";
			} else {
				echo $risultato;
			}
			break;

		case "W":
		case "RW":
			$this->componi_nome($campo_id, $id, $nome, $js_id);
			$this->modalita($js_id, $parametri);
		
			$valore_default	= ($modalita == "RW"
								? $rs->valore($campo_id)
								: $this->attributo("VALORE-DEFAULT", $rs_combo->valore_default($campo_id_combo))
							  );

			if (finisce_per(get_class($this), "FILTRO")) {
				$star = "";
			} else {
				$star = ((array_key_exists('FORM-CAMPI-RICHIESTI', $parametri) && in_array($campo_id, $parametri['FORM-CAMPI-RICHIESTI'])) || 
						(!$rs->nullable[$campo_id] && !$this->attributo_esiste("RICHIESTO") ) || 
						cbool($this->attributo("RICHIESTO","FALSE"))) ? "*" : "";
				if (constant_true('STAR') && strlen($star))
					$star = '<span class="star">'.$star.'</span>';
			}

			// 02/03/2004 -- boh, non era mai capitato ... non me ne capacito un granche' 
			// prima esisteva solo il ramo else
			if (strlen($valore_default) == 0) {
				$risultato = "";
			} else {
				$rs_combo->carica(equ($campo_id_combo, $valore_default), NULL, $colonne);
				$risultato = $this->decodifica($rs_combo, $campi, "");
			}

			echo "<div style=\"border: 0; margin: 0; padding: 0; white-space: nowrap\">";
			echo "<input type=\"hidden\" onchange=\"$js_onchange\" name=\"$nome\" id=\"$js_id\" value=\"$valore_default\"/>";
			echo "<input type=\"text\" name=\"dummy[]\" value=\"$risultato\" readonly=\"1\" size=\"$size\"/>".$star;
			$this->stampa_link_pop_up($parametri, $js_id, $this->attributo_richiesto("CAMPI"));
			$this->stampa_link_annulla($js_id);
			echo "</div>";
			break;
		}


	}
	

	public function extra_get(&$tag_link)
	{
		$link_get =& $tag_link->figlio("EXTRA-GET");
		if (is_null($link_get) || count($link_get->attributi) == 0) {

			return "";

		} else {
		
			$link_get->parametri = $this->parametri;
			$risultato = '';
			
			foreach(array_keys($link_get->attributi) as $parametro) {
//				if ($parametro != 'CLASSE-TAG'){
					$risultato .= "&amp;".strtoupper($parametro)."=".rawurlencode($link_get->attributo($parametro));
//				}
			}
			
			return $risultato;
		}
	}
}


class COD_DESC extends PICKER
{
	public function link_bg_load(&$parametri, $js_id, $campi)
	{
		$link      =& $this->figlio_richiesto("LINK");

		$gateway     = gateway($link->attributo("GATEWAY",
										array_get_default(
											"GATEWAY",
											$parametri,
											$_SERVER["PHP_SELF"]
										)
								)
						);

		$tabella    = $link->attributo("TABELLA", $this->attributo_richiesto("TABELLA"));
		$tabella_pick    = $this->attributo_richiesto("TABELLA");
		$campo_id_pick   = $this->attributo("CAMPO-ID-TABELLA", campo_id_tabella($tabella_pick));
		$campo_id   = $link->attributo("CAMPO-ID", $this -> attributo("CAMPO-ID-TABELLA", campo_id_tabella($tabella)));
		$file       = $link->attributo_richiesto("FILE");
		$ordine     = $link->attributo('ORDINE', '');

		$location  = $gateway."?AZIONE=cod_desc_bg_pick&amp;TABELLA-PICK=$tabella_pick&amp;CAMPO-ID-PICK=$campo_id_pick&amp;TABELLA=$tabella&amp;ORDINE=$ordine&amp;CAMPO-ID=$campo_id&amp;FILE=$file&amp;CAMPO-TARGET=$js_id&amp;CAMPI=$campi";
		if ($link->attributo_esiste("PAGE-SIZE"))
			$location .= "&amp;PAGE-SIZE=".$link->attributo("PAGE-SIZE");

		$extra_get = $this->extra_get($link);
		
		if ($extra_get){
			$location .= $extra_get;	
		}

		$location .= "&amp;ID=";

		return $location;
	}


	public function stampa_html(&$parametri)
	{
		if (($modalita = $this->modalita($this->nome(), $parametri)) == "NONE") return;

		$rs       =& $parametri["GESTORE"];

		$tabella  = $this->attributo_richiesto("TABELLA");
		$campo_id_combo = $this -> attributo("CAMPO-ID-TABELLA", campo_id_tabella($tabella));

		$rs_combo = new GESTORE($tabella, $campo_id_combo);
		$campo_id = $this->attributo("NOME", $campo_id_combo);

		if (!in_array($campo_id, $rs->campi)) {
			throw new CodeException("Attenzione coder, il campo $campo_id non esiste nella tabella/vista {$rs->tabella}");
		}

		$campi = explode("|", $this->attributo_richiesto("CAMPI"));
		if (count($campi) != 2)
			throw new CodeException("Attenzione coder, COD-DESC richiede esattamente due campi senza alcun separatore.");

		$classe_css = $this->attributo("CLASS", array_get_default("CLASS", $parametri));
		$size = explode(",", $this->attributo("SIZE"));
		if (count($size) != 2)
			throw new CodeException("Attenzione coder, COD-DESC richiede esattamente due dimensioni separate da virgola.");

		switch ($modalita) {
		case "R":

			$rs_combo->carica(equ($campo_id_combo, $rs->valore($campo_id)), NULL, $campi ,false,false);

			$risultato = $this->decodifica($rs_combo, array($campi[0], ' - ', $campi[1]), $this->attributo("SE-NULL","-"));

			if (strlen($classe_css) > 0) {
				echo "<SPAN class=\"$classe_css\">";
				echo $risultato;
				echo "</SPAN>";
			} else {
				echo $risultato;
			}
			break;

		case "W":
		case "RW":
			$id = $this->valore_id($modalita, $rs);
			$this->componi_nome($campo_id, $id, $nome, $js_id);
			$this->modalita($js_id, $parametri);

			$js_onchange = strtr(
							$this->attributo("JS-ONCHANGE"), 
							array(
								'@form' => array_get_default('FORM', $parametri, 'null'), 
								'@id' => $id
							));

			$valore_default	= ($modalita == "RW"
								? $rs->valore($campo_id)
								: $this->attributo("VALORE-DEFAULT", $rs_combo->valore_default($campo_id_combo))
							  );

			if (finisce_per(get_class($this), "FILTRO")) {
				$star = "";
			} else {
				$star = ((array_key_exists('FORM-CAMPI-RICHIESTI', $parametri) && in_array($campo_id, $parametri['FORM-CAMPI-RICHIESTI'])) || 
						(!$rs->nullable[$campo_id] && !$this->attributo_esiste("RICHIESTO") ) || 
						cbool($this->attributo("RICHIESTO","FALSE"))) ? NBSP."*" : "";
				if (constant_true('STAR') && strlen($star))
					$star = '<span class="star">'.$star.'</span>';
			}

			$bg_load = $this->link_bg_load($parametri, $js_id, implode('|', $campi));

			echo "<div style=\"border: 0; margin: 0; padding: 0; white-space: nowrap\">";
			echo "<iframe name=\"dummy[]\" src=\"about:blank\" style=\"height: 0; width: 0; display: inline;\" frameborder=\"0\"></iframe>";
			echo "<input type=\"hidden\" onchange=\"$js_onchange\" name=\"$nome\" id=\"$js_id\" value=\"$valore_default\"/>";

			$bloccato = cbool($this->attributo("BLOCCATO", "FALSE")) ? 'readonly="1"' : "";

			$tabindex = $this->get_tabindex();
			if (strlen($valore_default) == 0) {
				if ($bloccato) {
					echo "<input type=\"text\" tabindex=\"$tabindex\" name=\"dummy[]\" value=\"\" size=\"{$size[0]}\" $bloccato/>";
				} else {
					echo "<input type=\"text\" tabindex=\"$tabindex\" name=\"dummy[]\" value=\"\" size=\"{$size[0]}\" onblur=\"this.previousSibling.previousSibling.src = '$bg_load' + this.value;\"/>";
				}
				echo "<span style=\"border: 0; margin: 0; padding: 0\"> - </span>";
				echo "<input type=\"text\" name=\"dummy[]\" value=\"\" readonly=\"1\" size=\"{$size[1]}\"/>";
			} else {
				$rs_combo->carica(equ($campo_id_combo, $valore_default), NULL, $campi);
				$valori = array($rs_combo->valore($campi[0]), $rs_combo->valore($campi[1]));
				if ($bloccato) {
					echo "<input type=\"text\" tabindex=\"$tabindex\" name=\"dummy[]\" value=\"{$valori[0]}\" size=\"{$size[0]}\" $bloccato/>";
				} else {
					echo "<input type=\"text\" tabindex=\"$tabindex\" name=\"dummy[]\" value=\"{$valori[0]}\" size=\"{$size[0]}\" onblur=\"this.previousSibling.previousSibling.src = '$bg_load' + this.value;\"/>";
				}
				echo "<span style=\"border: 0; margin: 0; padding: 0\"> - </span>";
				echo "<input type=\"text\" name=\"dummy[]\" value=\"{$valori[1]}\" readonly=\"1\" size=\"{$size[1]}\"/>";
			}
			
			echo $star;
			if (cbool($this->attributo('PICK', 'TRUE'))) {
				$this->stampa_link_pop_up($parametri, $js_id, implode('|', $campi));
				$this->stampa_link_annulla($js_id);
			}
			echo "</div>";
			break;
		}
	}

	/**
	 * Stampa il link che svuota il campo testo
	 */
	public function stampa_link_annulla($js_id)
	{
		$img = $this->attributo('IMG-PULISCI', 'Del');
		echo "<a title=\"Svuota\" href=\"#\" onclick=\"var o = document.getElementById('$js_id'); o.value = ''; o.nextSibling.value = ''; o.nextSibling.nextSibling.nextSibling.value = ''; if (o.onchange != null) o.onchange.call(o); o.nextSibling.focus();\">";
		echo CACHE_CLASS_FACTORY::get()->img_get($img);
		echo "</a>";
	}

}

?>