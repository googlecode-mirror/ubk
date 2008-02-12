<?php
/**
 * Tag di creazione link - creano collegamenti ipertestuali con una serie di parametri sull'url interpretabili da {@link AZIONI_FORM}
 * @author Ubik <emiliano.leporati@gmail.com>
 * @package tag
 */

/**
 * Genera le funzioni di controllo per l'obbligatorieta' dei dati, nonche' un campo nascosto con l'elenco delle checkbox, necessario a capire quali checkbox sono state de-spuntate (cfr. {@link prendi_riga}). 
 * @package tag
 */
class AJAX_FORM extends CONTENITORE
{
	/**
	 * Dice se la modalita' del campo e' W o RW, e quindi e' necessario generare il javascript di controllo
	 * @param string $nome Il nome del campo o combo o picker di cui verificare la modalita' nel formato 'NOMETAG-NOMECAMPO'.
	 * 
	 */
	protected function modalita_ok($nome, &$parametri) 
	{
		if (array_key_exists('MODALITA-FINALE', $parametri) && array_key_exists($nome, $parametri['MODALITA-FINALE'])) {
			return in_array($parametri['MODALITA-FINALE'][$nome], array("W", "RW"));
		} else {
			return FALSE;
		}
	}

	protected function campi_consigliati(&$campi)
	{
		$tmp = array();
		$check = array();
		if ($this->attributo_esiste("CAMPI-CONSIGLIATI") && strlen($this->attributo("CAMPI-CONSIGLIATI"))) {
			// sintassi: (campo,desc)[|(campo,desc)*]
			$campi_cons = explode("|", $this->attributo("CAMPI-CONSIGLIATI"));

			foreach($campi_cons as $campo_cons) {
				list($nome_campo, $desc) = explode(",", substr($campo_cons, 1, -1));
				$desc = str_replace("'", "\'", $desc);

				foreach($campi as $campo) {
					$nome_esteso = $campo->nome.'-'.$nome_campo;
					if ($this->modalita_ok($nome_esteso, $this->parametri) 
//						&& strtoupper($campo->attributo("TIPO","TEXT")) != "HIDDEN" 
						) {
						if (!in_array($nome_campo, $tmp) && $nome_campo == $campo->nome()) {
							array_push($tmp, $nome_campo);
							$nome_campo_js = $campo->attributo_esiste("JS-ID") ? $campo->attributo('JS-ID') : $nome_campo."[#id]";
							$check[qt($nome_campo_js)] = qt($desc);
						}
					}
				}

			}
		}

		if (count($check)) {
// 			log_value('SUGGESTING: -------> '.implode(' , ',$check));
			return 'if (!Fld.Check.suggested(['.implode(',', array_keys($check)).'], ['.implode(',', array_values($check)).'])) return false;';
		} else {
			return '';
		}
	}

	protected function campi_richiesti(&$rs, &$campi)
	{
		$campi_controllare = array_merge($rs->campi, $this->campi_rich);

		$check = array();
		$tmp = array();
		foreach($campi as $campo) {
			$nome_campo = $campo->nome();
			$nome_esteso = $campo->nome.'-'.$nome_campo;
			// se non l'ho già controllato
			// è da controllare
			// è in modalità buona
			// allora controllo
			if (
				!in_array($nome_esteso, $tmp) 
				&& in_array($nome_campo, $campi_controllare)
				&& $this->modalita_ok($nome_esteso, $this->parametri) 
	//			&& strcasecmp($campo->attributo("TIPO","TEXT"), "HIDDEN") 
				) {
				$tmp[] = $nome_esteso;
				
				// se non dico che non è richiesto
				if (!$campo->attributo_esiste("RICHIESTO") 
						|| ($campo->attributo_esiste("RICHIESTO") && cbool($campo->attributo_richiesto("RICHIESTO")))) {
					// se è fra quelli che devo controllare
					// oppure non è nullable
					// oppure è richiesto
					// allora check
					if (in_array($nome_campo, $this->campi_rich) || !$rs->nullable[$nome_campo] || cbool($campo->attributo("RICHIESTO","FALSE"))) {
						$nome_campo_js = $campo->attributo_esiste("JS-ID") ? $campo->attributo('JS-ID') : $nome_campo."[#id]";
						$check[] = qt($nome_campo_js);
					}
				}
			}
		}

		if (count($check)) {
// 			log_value('CHECKING: -------> '.implode(' , ',$check));
			return 'if (!Fld.Check.mandatory(['.implode(',', array_values($check)).'])) return false;';
		} else {
			return '';
		}
	}

	public function stampa_html(&$parametri)
	{
		static $numero_form = 0;

		$this->parametri =& $parametri;
		
		$nome_form  = $this->attributo("NAME", "ubkf_".$this->attributo('NUMERO', $numero_form));

		$this->attributi["NAME"] = $nome_form;
		$this->attributi["METHOD"] = "post";
		$this->attributi["ENCTYPE"] = "multipart/form-data";
		$this->attributi["ACTION"] = "?";

		// questi servono poi nei link x generare i vari javascript
		$parametri["FORM"]   = $nome_form;
		$parametri["FORM-N"] = $this->attributo('NUMERO', $numero_form);
		$parametri["FORM-CAMPI-RICHIESTI"] = $this->campi_rich = explode("|", $this->attributo("CAMPI-RICHIESTI", ""));
		$parametri['MODALITA-FINALE'] = array();

		$this->parametri =& $parametri;

		$rs =& $parametri["GESTORE"];

		$atts = $this->attributi;
		unset($atts['CAMPI-RICHIESTI']);
		unset($atts['CAMPI-CONSIGLIATI']);
		unset($atts['NUMERO']);
		unset($atts['CHECKBOXES']);
		// informazione sui campi CHECKBOX
		// devo sapere quali check ci sono sulla pagina, cosi' quando le posto sceccate (?) posso
		// sapere che c'erano e scrivere 0 sul db
		$checks   = array_merge($this->figli_profondita("CHECKBOX"), $this->figli_profondita("CHECKBOX_IMG"));
		if ($c = $this->attributo('CHECKBOXES')) {
			$nomi_chk = explode('|', $c);
		} else {
			$nomi_chk = array();
		}
		foreach($checks as $check) {
			array_push($nomi_chk, $check->attributo_richiesto("NOME"));
		}

		// pre-interpretazione del body - la modalità la so alla fine per i campi
		ob_start();
		parent::stampa_html($parametri);
		$form_content = ob_get_contents();
		ob_end_clean();

		// generazione javascript di controllo campi
		// voglio controllare: il formato di tutti i campi che ne hanno bisogno (data, float, int, etc.)
		// la presenza di tutti i dati necessari (not null sul db ?)
		$this->parametri = $parametri;
		$campi  = array_merge(
					$this->figli_profondita("COMBO")
					,$this->figli_profondita("PICKER")
					,$this->figli_profondita("COD-DESC")
					,$this->figli_profondita("CAMPO")
			);
		foreach($campi as $campo)
			$campo->parametri = $parametri;

		// campi consigliati
		$js = $this->campi_consigliati($campi);

		// campi necessari
		$js .= $this->campi_richiesti($rs, $campi);

		// ok finale
		$js .= "return true;";

		// stampa del forum
		echo "<form".xml_att_crea($atts)." onsubmit=\"$js\">";
		echo "<input type=\"hidden\" name=\"checkboxes\" value=\"".implode("|", array_unique($nomi_chk))."\"/>";
		echo $form_content;
		echo "</form>";
		unset($atts);

		$numero_form = max($numero_form + 1, $parametri['FORM-N']);

		// reset parametri
		unset($parametri["MODALITA-FINALE"]);
		unset($parametri["FORM"]);
		unset($parametri["FORM-N"]);
		unset($parametri["FORM-CAMPI-RICHIESTI"]);

		unset($this->attributi["NAME"]);
		unset($this->attributi["METHOD"]);
		unset($this->attributi["ENCTYPE"]);
		unset($this->attributi["ACTION"]);
	}
	
}



/**
 * Link configurabile in modo generico per le azioni non-standard. 
 * @package tag
 */
class LINK_AJAX_GENERICO extends TAG
{
	/**
	 * Ritorna il contenuto dell'attributo JS-ONCLICK con i parametri @form e @id sostituiti dai valori correnti del form in cui il link si trova e dell'id su cui opera
	 * @param array $parametri I parametri della {@link stampa_html}
	 * @return string
	 */
	protected function js_onclick(&$nodo, &$parametri)
	{
		$js_onclick = $nodo->attributo("JS-ONCLICK");
		if (strlen($js_onclick) > 0) {
			$usa_id    = cbool($nodo->attributo("USA-ID", "FALSE"));
			$post      = cbool($nodo->attributo_richiesto("POST"));
			if ($post) {
				$form =  array_get("FORM", $parametri);
			} else {
				$form =  array_get_default("FORM", $parametri, "null");
			}
			$js_onclick = str_replace("@form", $form, $js_onclick);

			if ($usa_id) {
				$campo_id  = $nodo->attributo("CAMPO-ID", $parametri["GESTORE"]->campo_id);
				if ($nodo->attributo_esiste("VALORE-ID")) {
					$valore_id = $nodo->attributo("VALORE-ID");
				} else {
					$valore_id = $parametri["GESTORE"]->valore($campo_id);
				}
				$js_onclick = str_replace("@id", $valore_id, $js_onclick);
			} else {
				$js_onclick = str_replace("@id", "-1", $js_onclick);
			}
			if (substr(trim($js_onclick ), -1) != ";") $js_onclick  .= ";";
		}

		return $js_onclick;
	}

	protected function js_id(&$parametri)
	{
		$nodo =& $this->figlio_richiesto("AZIONE");
		$nodo -> parametri = $parametri;

		$usa_id    = cbool($nodo->attributo("USA-ID", "FALSE"));
		if ($usa_id) {
			$campo_id  = $nodo->attributo("CAMPO-ID", $parametri["GESTORE"]->campo_id);
			if ($nodo->attributo_esiste("VALORE-ID")) {
				$valore_id = $nodo->attributo("VALORE-ID");
			} else {
				$valore_id = $parametri["GESTORE"]->valore($campo_id);
			}
		} else {
			$valore_id = '-1';
		}

		return $valore_id;
	}

	/**
	 * Genera la parte di url relativa all'azione
	 * @param array $parametri I parametri della {@link stampa_html}
	 * @return string
	 */
	protected function link_azione(&$parametri)
	{
		$rs   =& $parametri["GESTORE"];

		$nodo =& $this->figlio_richiesto("AZIONE");
		$nodo -> parametri = $parametri;


		$post       = cbool($nodo->attributo_richiesto("POST"));
		if ($post && !array_key_exists("FORM", $parametri)) {
			throw new CodeException(__CLASS__."::AZIONE non inserito in un FORM");
		}

		// il gateway e'  il mio, o quello dei parametri, o quello di partenza
		$gateway = gateway($nodo->attributo(
								"GATEWAY", 
								array_get_default(
									"GATEWAY", 
									$parametri, 
									$_SERVER["PHP_SELF"]
								)
							)
					);
		$show    = strtolower($nodo->attributo('REPLACE', 'TRUE'));
		if (cbool($show)) {
			$target  = $nodo->attributo('TARGET');
			if (!trim($target)) $target = array_get('TARGET', $parametri);
		} else {
			$target = 'null';
		}
		$async   = strtolower($nodo->attributo('ASYNC', 'TRUE'));
		$azione  = $nodo->attributo_richiesto("NOME");
		$usa_id  = cbool($nodo->attributo("USA-ID", "FALSE"));
		$mode    = $nodo->attributo("MODE", $parametri["MODE"]);
		$file    = $nodo->attributo("FILE", $parametri["FILE"]);
		$page_size = $nodo->attributo("PAGE-SIZE", array_get_default("PAGE-SIZE", $parametri, 0));
		$page      = max($nodo->attributo("PAGE", array_get_default("PAGE", $parametri, 0)), 0);
		$tabella  = $nodo->attributo("TABELLA", $rs->tabella);
		$campo_id = $nodo->attributo("CAMPO-ID", $rs->campo_id);
		$order    = $nodo->attributo("ORDINE");


		$pars = "AZIONE=$azione&amp;ORDINE=$order&amp;TABELLA=$tabella&amp;CAMPO-ID=$campo_id&amp;TARGET=$target&amp;MODE=$mode&amp;FILE=$file&amp;PAGE=$page&amp;PAGE-SIZE=$page_size&amp;REPLACE=$show";
		
		if ($usa_id) {
			if ($nodo->attributo_esiste("VALORE-ID")) {
				$valore_id = $nodo->attributo("VALORE-ID");
			} else {
				$valore_id = $rs->valore($campo_id);
			}
			$pars .= "&amp;ID=$valore_id";
		}

		$pars .= $this->extra_get("EXTRA-GET");

//		$pars = url_codifica($pars);

		if (cbool($this->attributo("REAZIONE", "TRUE"))) {
			$reazione = $this->parametri_reazione($parametri);
		}

		if (!cbool($async) && isset($reazione) && strlen($reazione)) {
			$onComplete = 'function() {'.$reazione.'}';
			unset($reazione);
		} else {
			$onComplete = 'null';
		}

		if ($target != 'null' && FALSE === strpos($target, "'")) $target = qt($target);
		if ($post) {
			$form =  array_get("FORM", $parametri);
			$form = qt($form);
			if (isset($reazione) && strlen($reazione)) {
				$ajax = "if (Ubk.post($form, '".$this->js_id($parametri)."', $target, '$gateway', '$pars', $show, $async, $onComplete)) $reazione ";
			} else {
				$ajax = "Ubk.silentPost($form, '".$this->js_id($parametri)."', $target, '$gateway', '$pars', $show, $async, $onComplete);"; // silent
			}
		} else {
			$ajax = "Ubk.follow($target, '$gateway', '$pars', $show, $async, $onComplete); ";
			if (isset($reazione))
				$ajax .= $reazione;
		}
		
		$ret = $this->js_onclick($nodo, $parametri);
		
		if (FALSE === strpos($ret, '@ajax')) {
			$ret .= $ajax;
		} else {
			$ret = str_replace('@ajax', $ajax, $ret);
		}

		
		unSet($nodo);
		return $ret;
	}

	/**
	 * Genera la parte di url relativa alla reazione - ossia il redirect che viene effettuato dopo l'azione. Se il nodo manca e l'azione necessita di un post di dati, allora i parametri vengono interamente copiati dal nodo di azione.
	 * @param array $parametri I parametri della {@link stampa_html}
	 * @param bool $post Flag che dice se il link deve scatenare un post di dati (ed e' quindi inserito in un form) o meno
	 * @return string
	 */
	protected function parametri_reazione(&$parametri)
	{
		$nodo_reazione =& $this->figlio("REAZIONE");
		$nodo_azione   =& $this->figlio_richiesto("AZIONE");
		$nodo_azione   -> parametri = $parametri;

		if ((is_null($nodo_reazione))) {
		
			return "";

		}

		$rs   =& $parametri["GESTORE"];
		if (is_null($nodo_reazione)) {
			$nodo_reazione =& $nodo_azione;
			$r_azione = $nodo_reazione->attributo_richiesto("NOME");
			// altrimenti mi ricarica una riga sola ...
			if (in_array($r_azione, array("aggiorna", "elimina"))) {
				$nodo_reazione->attributi["USA-ID"] = "false";
			} elseif ($r_azione == "crea") {
				$nodo_reazione->attributi["NOME"] = "mostra";
			}
		}
		$nodo_reazione -> parametri = $parametri;

		$post       = cbool($nodo_reazione->attributo("POST", "FALSE"));
		if ($post && !array_key_exists("FORM", $parametri)) {
			throw new CodeException(__CLASS__."::REAZIONE non inserito in un FORM");
		}

		// il gateway e' il mio, o quello dei parametri, o quello del nodo azione, o quello di partenza
		$gateway     = gateway($nodo_reazione->attributo("GATEWAY", 
										array_get_default(
											"GATEWAY", 
											$parametri, 
											$nodo_azione->attributo("GATEWAY", $_SERVER["PHP_SELF"])
										)
								)
						);
		$show       = strtolower($nodo_reazione->attributo('REPLACE', 'TRUE'));
		if (cbool($show)) {
			$target     = $nodo_reazione->attributo('TARGET', $nodo_azione->attributo('TARGET'));
			if (!trim($target)) $target = array_get('TARGET', $parametri);
		} else {
			$target = 'null';
		}
		$async      = strtolower($nodo_reazione->attributo('ASYNC', 'TRUE'));
		$tabella    = $nodo_reazione->attributo("TABELLA", $rs->tabella);
		$order      = $nodo_reazione->attributo("ORDINE", $nodo_azione->attributo("ORDINE"));
		$usa_id     = cbool($nodo_reazione->attributo("USA-ID", "FALSE"));

		$file       = $nodo_reazione->attributo("FILE", $parametri["FILE"]);
		$mode       = $nodo_reazione->attributo("MODE", $nodo_azione->attributo("MODE",$parametri["MODE"]));
		$page       = max($nodo_reazione->attributo("PAGE", array_get_default("PAGE", $parametri, 0)), 0);
		if ($nodo_reazione->attributo_esiste("TABELLA")) {
			$campo_id   = $nodo_reazione->attributo("CAMPO-ID", $nodo_azione->attributo("CAMPO_ID", campo_id_tabella($tabella)));
		} else {
			$campo_id   = $nodo_reazione->attributo("CAMPO-ID", $nodo_azione->attributo("CAMPO_ID", $rs->campo_id));
		}

		$page_size = $nodo_reazione->attributo("PAGE-SIZE", $nodo_azione->attributo("PAGE-SIZE", array_get_default("PAGE-SIZE", $parametri, 0)));

		if ($usa_id) {
			$azione    = $nodo_reazione->attributo("NOME", "mostra_riga");
			if (array_key_exists("VALORE-ID", $nodo_reazione->attributi)) {
				$valore_id = $nodo_reazione->attributo("VALORE-ID");
			} else {
				$valore_id = $rs->valore($campo_id);
			}
			$pars = "&amp;ID=$valore_id";
		} else {
			$azione = $nodo_reazione->attributo("NOME", "mostra");
			$pars = "";
		}
		$pars = "AZIONE=$azione&amp;ORDINE=$order&amp;GATEWAY=$gateway&amp;TABELLA=$tabella&amp;CAMPO-ID=$campo_id&amp;TARGET=$target&amp;FILE=$file&amp;MODE=$mode&amp;PAGE=$page&amp;PAGE-SIZE=$page_size&amp;REPLACE=$show".$pars;

		$pars .= $this->extra_get("EXTRA-RE-GET");

//		$pars = url_codifica($pars);

		$ret = $this->js_onclick($nodo_reazione, $parametri);

		if ($target != 'null' && FALSE === strpos($target, "'")) $target = qt($target);	
		if ($post) {
			$form =  array_get("FORM", $parametri);
			$form = qt($form);
			$ret .= "Ubk.silentPost($form, '".$this->js_id($parametri)."', $target, '$gateway', '$pars', $show, $async, null); ";	// silent
		} else {
			$ret .= "Ubk.follow($target, '$gateway', '$pars', $show, $async, null); ";
		}


		unSet($nodo_reazione);
		unSet($nodo_azione);

		return $ret;
	}

	/**
	 * Genera la parte di url personalizzabile
	 */
	protected function extra_get($figlio, $prefisso = "")
	{
		$link =& $this->figlio($figlio);

		if (is_null($link) || count($link->attributi) == 0) {

			return "";

		} else {
		
			$link->parametri = $this->parametri;
			$risultato = "";
			
			foreach(array_keys($link->attributi) as $parametro) {
				$risultato .= "&amp;".$prefisso.strtoupper($parametro)."=".rawurlencode($link->attributo($parametro));
			}
			
			return $risultato;
		}
	}


	
	public function stampa_html(&$parametri)
	{
		// prendo la sezione fra i miei attributi, se non c'e' fra i parametri (vedi tag SEZIONE)
//		$sezione = strtoupper($this->attributo("SEZIONE", array_get_default("SEZIONE", $parametri)));

		$sezioni = explode("|",strtoupper($this->attributo("SEZIONE", array_get_default("SEZIONE", $parametri))));

		// se non c'e' nessuna sezione specificata, allora visualizzo tutto
		if (strlen($sezioni[0]) == 0) {
			$passa = TRUE;
		// altrimenti guardo se ci sono dei diritti specificati nei parametri
		} else {
			$diritti = array_get("DIRITTI", $parametri, array());
			// se non ci sono, visualizzo tutto
			if (count($diritti) == 0) {
				$passa = TRUE;
			// altrimenti dipende dal diritto specificato
			} else {
				$diritto = $this->attributo("DIRITTO","R");
				$passa = FALSE;
				foreach($sezioni as $sezione){
					if (!array_key_exists($sezione, $parametri["DIRITTI"])) {
						throw new CodeException("Sezione non esistente '$sezione'");
					}
					if (!$passa){						
						$passa = $diritti[$sezione][$diritto];
					}
				}		
			}
		}

		if ($passa) {

			$rs   =& $parametri["GESTORE"];

			$ajax = $this->link_azione($parametri);

			$classe_css = $this->attributo("CLASS", array_get_default("CLASS", $parametri));
			$js_id      = $this->attributo("JS-ID");
			$alt        = $this->attributo("ALT");
			$datasrc    = $this->attributo("DATASRC");
			$tabindex   = CAMPO_ACCESSO::get_static_tabindex($this->attributo('TABINDEX'));

			$contenuto =& $this->figlio("CONTENUTO");


			echo "<a tabindex=\"$tabindex\" href=\"javascript:$ajax\" datasrc=\"$datasrc\" id=\"$js_id\" class=\"$classe_css\" title=\"$alt\">";
			if ($contenuto) $contenuto->stampa_html($parametri);
			echo "</a>";
		}
	}
	
}

/**
 * Fornisce un metodo per la riscrittura del codice di js-onclick per le classi che aggiungono una conferma
 * @package tag
 */
abstract class LINK_AJAX_CONFERMA extends LINK_AJAX_GENERICO
{
	protected function aggiorna_js_onclick($js)
	{
		$nodo_azione =& $this->figlio_richiesto("AZIONE");

		if ($nodo_azione -> attributo_esiste("JS-ONCLICK")) {
			$jsOr = $nodo_azione -> attributo_originale("JS-ONCLICK");
			if (strpos($jsOr, '@ajax') === FALSE) {
				$nodo_azione -> attributi["JS-ONCLICK"] = str_replace('@ajax', $jsOr.'@ajax', $js);
			} else {
				$nodo_azione -> attributi["JS-ONCLICK"] = str_replace('@ajax', $jsOr, $js);
			}
		} else {
			$nodo_azione -> attributi["JS-ONCLICK"] = $js;
		}
	}
}

/**
 * Link che scatena l'eliminazione di una riga in una tabella. 
 * @package tag
 */
class LINK_AJAX_ELIMINA extends LINK_AJAX_CONFERMA
{
	private $setup_done = FALSE;

	public function stampa_html(&$parametri)
	{
		if (!$this->setup_done) {
			$nodo_azione =& $this->figlio("AZIONE");
			if (is_null($nodo_azione)) {
				$nodo_azione = new TAG("AZIONE");
			}
			
			if (!$this->attributo_esiste("DIRITTO"))
				$this -> attributi["DIRITTO"] = "D";

			if (!$nodo_azione -> attributo_esiste("NOME"))
				$nodo_azione -> attributi["NOME"] = "elimina";

			$nodo_azione -> attributi["USA-ID"] = "true";
			$nodo_azione -> attributi["POST"] = "false";

			$this->aggiorna_js_onclick("Ubk.confirm('Proseguire con l\'eliminazione ?',function(){@ajax});");

			array_unshift($this -> figli, $nodo_azione);

			$this->setup_done = TRUE;
		}

		parent::stampa_html($parametri);
	}
	
}


/**
 * Link che scatena l'aggiornamento di una riga in una tabella. 
 * @package tag
 */
class LINK_AJAX_AGGIORNA extends LINK_AJAX_CONFERMA
{
	private $setup_done = FALSE;

	public function stampa_html(&$parametri)
	{
		if (!$this->setup_done) {

			$rs =& $parametri["GESTORE"];

			$nodo_azione =& $this->figlio("AZIONE");
			if (is_null($nodo_azione)) {
				$nodo_azione = new TAG("AZIONE");
			}

			if (!$this->attributo_esiste("DIRITTO"))
				$this -> attributi["DIRITTO"] = "RW";
			if (!$nodo_azione -> attributo_esiste("NOME"))
				$nodo_azione -> attributi["NOME"] = "aggiorna";

			$nodo_azione -> attributi["USA-ID"] = "true";
			$nodo_azione -> attributi["POST"] = "true";

			$this->aggiorna_js_onclick("Ubk.confirm('Proseguire con l\'aggiornamento ?',function(){@ajax});");

			array_unshift($this -> figli, $nodo_azione);

			$this->setup_done = TRUE;
		}

		parent::stampa_html($parametri);
	}

}
 
/**
 * Link che scatena l'aggiornamento pagina, sulla quale devono essere presenti tutti gli id da aggiornare in un campo multiplo
 * @package tag
 */
class LINK_AJAX_AGGIORNA_TUTTO extends LINK_AJAX_CONFERMA
{
	private $setup_done = FALSE;

	public function stampa_html(&$parametri)
	{
		if (!$this->setup_done) {
			$rs =& $parametri["GESTORE"];

			$nodo_azione =& $this->figlio("AZIONE");
			if (is_null($nodo_azione)) {
				$nodo_azione = new TAG("AZIONE");
			}

			$nodo_azione -> attributi["POST"] = "true";
			if (!$this->attributo_esiste("DIRITTO"))
				$this -> attributi["DIRITTO"] = "RW";
			if (!$nodo_azione -> attributo_esiste("NOME"))
				$nodo_azione -> attributi["NOME"] = "aggiorna_tutto";

			$nodo_azione -> attributi["USA-ID"] = "false";

			$this->aggiorna_js_onclick("Ubk.confirm('Proseguire con l\'aggiornamento pagina ?',function(){@ajax});");

			array_unshift($this -> figli, $nodo_azione);

			$this->setup_done = TRUE;
		}

		parent::stampa_html($parametri);
	}
}
/**
 * Link che scatena l'aggiunta di una riga in una tabella. 
 * @package tag
 */
class LINK_AJAX_CREA extends LINK_AJAX_CONFERMA
{
	private $setup_done = FALSE;

	public function stampa_html(&$parametri)
	{
		if (!$this->setup_done) {
			$nodo_azione =& $this->figlio("AZIONE");
			if (is_null($nodo_azione)) {
				$nodo_azione = new TAG("AZIONE");
			}

			if (!$this->attributo_esiste("REAZIONE"))
				$this -> attributi["REAZIONE"] = "TRUE";
			if (!$this->attributo_esiste("DIRITTO"))
				$this -> attributi["DIRITTO"] = "W";
			if (!$nodo_azione -> attributo_esiste("NOME"))
				$nodo_azione -> attributi["NOME"] = "crea";

			$nodo_azione -> attributi["POST"] = "true";
			$nodo_azione -> attributi["USA-ID"] = "false";
			$this -> attributi["JS-ID"] = "link_crea_".($parametri['FORM-N']);

			$this->aggiorna_js_onclick("Ubk.confirm('Proseguire con la creazione ?',function(){@ajax});");

			array_unshift($this -> figli, $nodo_azione);

			$this->setup_done = TRUE;
		}
		
		parent::stampa_html($parametri);
	}
}


/**
 * Link che scatena la ricerca con le condizioni indicate dai campi filtro del form in cui si trova. 
 * @package tag
 */
class LINK_AJAX_FILTRA extends LINK_AJAX_GENERICO
{
	public function stampa_html(&$parametri)
	{
		// attributi
		$this -> attributi["DIRITTO"] = "R";
		$this -> attributi["JS-ID"] = "link_filtra_".($parametri['FORM-N']);
		
		// azione		
		$nodo_azione =& $this->figlio("AZIONE");
		if (is_null($nodo_azione)) {
			$nodo_azione = new TAG("AZIONE");
		}
		if (!$nodo_azione -> attributo_esiste("NOME")) {
			$nodo_azione -> attributi["NOME"] = "filtra";
		}
		$nodo_azione -> attributi["USA-ID"] = "false";
		$nodo_azione -> attributi["POST"] = "true";
		array_unshift($this -> figli, $nodo_azione);
		
		// reazione
		if (!$this->attributo_esiste("REAZIONE") || cbool($this->attributo("REAZIONE"))) {
			$this -> attributi["REAZIONE"] = "true";
			$nodo_reazione =& $this->figlio("REAZIONE");
			if (is_null($nodo_reazione)) {
				$nodo_reazione = new TAG("REAZIONE");
			}
			if (!$nodo_reazione -> attributo_esiste("NOME")) {
				$nodo_reazione -> attributi["NOME"] = "filtra";
			}
			if (!$nodo_reazione -> attributo_esiste("USA-ID")) {
				$nodo_reazione -> attributi["USA-ID"] = "false";
			}
			if (!$nodo_reazione -> attributo_esiste("TABELLA")) {
				$nodo_reazione -> attributi["TABELLA"] = $nodo_azione->attributo("TABELLA", array_get_default('TABELLA', $parametri));
			}
			if (!$nodo_reazione -> attributo_esiste("CAMPO-ID")) {
				$nodo_reazione -> attributi["CAMPO-ID"] = $nodo_azione->attributo("CAMPO-ID", array_get_default('CAMPO-ID', $parametri, campo_id_tabella($nodo_reazione->attributo('TABELLA'))));
			}
			array_unshift($this -> figli, $nodo_reazione);
		} else {
			$this -> attributi["REAZIONE"] = "false";
		}

		// pagina e stampa
		$old_page = array_get_default("PAGE", $parametri, 0);
		$parametri["PAGE"] = 0;
		parent::stampa_html($parametri);
		$parametri["PAGE"] = $old_page;
	}
	
}


/**
 * Link che scatena la ricerca con le condizioni indicate dai campi filtro del form in cui si trova. 
 * @package tag
 */
class LINK_AJAX_ANNULLA_FILTRO extends LINK_AJAX_GENERICO
{
	public function stampa_html(&$parametri)
	{
		$nodo_azione =& $this->figlio("AZIONE");
		if (is_null($nodo_azione)) {
			$nodo_azione = new TAG("AZIONE");
		}

		$this -> attributi["REAZIONE"] = "false";
		$this -> attributi["DIRITTO"] = "R";
		if (!$nodo_azione -> attributo_esiste("NOME")) {
			$nodo_azione -> attributi["NOME"] = "annulla_filtro";
		}
		$nodo_azione -> attributi["USA-ID"] = "false";
		$nodo_azione -> attributi["POST"] = "false";

		array_unshift($this -> figli, $nodo_azione);

		$old_page = array_get_default("PAGE", $parametri, 0);
		$parametri["PAGE"] = 0;
		parent::stampa_html($parametri);
		$parametri["PAGE"] = $old_page;
	}
}


/**
 * Link di spostamento alla pagina precedente. 
 * @package tag
 */
class LINK_AJAX_PRECEDENTE extends TAG
{

	public function stampa_html(&$parametri)
	{
		$pagina_corrente = $this->attributo('PAGE', array_get_default("PAGE", $parametri, 0));

		if ($pagina_corrente > 0) {

			$rs =& $parametri["GESTORE"];

			$gateway    = gateway($this->attributo(
								"GATEWAY", 
								array_get_default(
									"GATEWAY", 
									$parametri, 
									$_SERVER["PHP_SELF"]
									)
								));
			$file       = $this->attributo("FILE", $parametri["FILE"]);
			$mode       = $this->attributo("MODE", $parametri["MODE"]);
			$classe_css = $this->attributo("CLASS", array_get_default("CLASS", $parametri));
			$js_id      = $this->attributo("JS-ID", "link_precedente");
			$tabella    = $this->attributo("TABELLA", $rs->tabella);
			$campo_id   = $this->attributo("CAMPO-ID",$rs->campo_id);
			$dim_pagina = $this->attributo('PAGE-SIZE', array_get_default("PAGE-SIZE", $parametri, 0));
			$ordine     = array_get_default("ORDINE", $parametri, '');
			
			$async   = strtolower($this->attributo('ASYNC', 'TRUE'));
			$show    = strtolower($this->attributo('REPLACE', 'TRUE'));
			if (cbool($show)) {
				$target  = $this->attributo('TARGET');
				if (!trim($target)) $target = array_get('TARGET', $parametri);
			} else {
				$target = 'null';
			}

			$pars       = "FILE=$file&amp;AZIONE=filtra&amp;TABELLA=$tabella&amp;CAMPO-ID=$campo_id&amp;MODE=$mode&amp;ORDINE=$ordine";
			$pars      .= "&amp;PAGE=".($pagina_corrente - 1)."&amp;PAGE-SIZE=$dim_pagina&amp;TARGET=$target";

			$contenuto =& $this->figlio("CONTENUTO");

			if ($target != 'null' && FALSE === strpos($target, "'")) $target = qt($target);
			$click = "Ubk.follow($target, '$gateway', '$pars', $show, $async); ";

			echo "<a href=\"javascript:$click\" id=\"$js_id\" class=\"$classe_css\">";
			if ($contenuto) $contenuto->stampa_html($parametri);
			echo "</a>";
		}
	}

}

/**
 * Link di spostamento alla prima pagina. 
 * @package tag
 */
class LINK_AJAX_PRIMO extends LINK_AJAX_PRECEDENTE
{
	public function stampa_html(&$parametri)
	{
		$pagina_corrente = array_get_default("PAGE", $parametri, 0);

		if ($pagina_corrente > 0) {
			$parametri["PAGE"] = 1;
			$this->attributi["JS-ID"] = $this->attributo("JS-ID", "link_primo");
			parent::stampa_html($parametri);
			$parametri["PAGE"] = $pagina_corrente;
		}
	}

}

/**
 * Link di spostamento alla pagina successiva quella corrente
 * @package tag
 */
class LINK_AJAX_SUCCESSIVO extends TAG
{

	public function stampa_html(&$parametri)
	{
		$rs =& $parametri["GESTORE"];
		$pagina_corrente = $this->attributo('PAGE', array_get_default("PAGE", $parametri, 0));
		$dim_pagina      = $this->attributo('PAGE-SIZE', array_get_default("PAGE-SIZE", $parametri, 0));
		$last_page       = ($dim_pagina == 0) ? 0 : ceil($rs->count_reale / $dim_pagina) - 1;

		if ($rs->count_reale > 0 && $pagina_corrente < $last_page) {

			$gateway    = gateway($this->attributo("GATEWAY", array_get_default("GATEWAY", $parametri, $_SERVER["PHP_SELF"])));
			$file       = $this->attributo("FILE", $parametri["FILE"]);
			$mode       = $this->attributo("MODE", $parametri["MODE"]);
			$classe_css = $this->attributo("CLASS", array_get_default("CLASS", $parametri));
			$js_id      = $this->attributo("JS-ID", "link_successivo");
			$ordine     = array_get_default("ORDINE", $parametri, '');

			$tabella    = $this->attributo("TABELLA", $rs->tabella);
			$campo_id   = $this->attributo("CAMPO-ID", $rs->campo_id);

			$async   = strtolower($this->attributo('ASYNC', 'TRUE'));
			$show    = strtolower($this->attributo('REPLACE', 'TRUE'));
			if (cbool($show)) {
				$target  = $this->attributo('TARGET');
				if (!trim($target)) $target = array_get('TARGET', $parametri);
			} else {
				$target = 'null';
			}

			$pars       = "FILE=$file&amp;AZIONE=filtra&amp;TABELLA=$tabella&amp;CAMPO-ID=$campo_id&amp;MODE=$mode&amp;ORDINE=$ordine";
			$pars      .= "&amp;PAGE=".($pagina_corrente + 1)."&amp;PAGE-SIZE=$dim_pagina&amp;TARGET=$target";

//			$location = url_codifica($location);

			$contenuto =& $this->figlio("CONTENUTO");

			if ($target != 'null' && FALSE === strpos($target, "'")) $target = qt($target);
			$click = "Ubk.follow($target, '$gateway', '$pars', $show, $async); ";

			echo "<a href=\"javascript:$click\" id=\"$js_id\" class=\"$classe_css\">";
			if ($contenuto) $contenuto->stampa_html($parametri);
			echo "</a>";
		}
	}
	
}

/**
 * Link di spostamento all'ultima pagina. 
 * @package tag
 */
class LINK_AJAX_ULTIMO extends LINK_AJAX_SUCCESSIVO
{
	public function stampa_html(&$parametri)
	{
		$rs =& $parametri["GESTORE"];
		$pagina_corrente = array_get_default("PAGE", $parametri, 0);
		$dim_pagina      = array_get_default("PAGE-SIZE", $parametri, 0);
		$last_page       = ($dim_pagina == 0) ? 0 : ceil($rs->count_reale / $dim_pagina) - 1;

		if ($rs->count_reale > 0 && $pagina_corrente < $last_page) {
			$parametri["PAGE"] = $last_page - 1;
			$this->attributi["JS-ID"] = $this->attributo("JS-ID", "link_ultimo");
			parent::stampa_html($parametri);
			$parametri["PAGE"] = $pagina_corrente;
		}
	}

}

/**
 * Inserisce una barra di navigazione che permette di spostarsi su una pagina specifica
 * @package tag
 */
class PAGINATORE_AJAX extends TAG
{
	public function stampa_html(&$parametri)
	{
		
		$rs =& $parametri["GESTORE"];
		$pagina_corrente = array_get_default('PAGE', $parametri, 0);
		$dim_pagina      = array_get_default('PAGE-SIZE', $parametri, 0);
		$extra           = $this->attributo('EXTRA-GET', '');
		$last_page       = ($dim_pagina == 0) ? 0 : ceil($rs->count_reale / $dim_pagina) - 1;

		// non deve apparire  null se c'e' solo un'unica pagina
		if ($last_page == 0) {
			return;	
		}

		// size
		$size = $this->attributo('SIZE', 10);
		// now, end
		$blocco_attuale = floor($pagina_corrente / $size) * $size; 
		$blocco_fine = floor($last_page / $size) * $size;

		$gateway    = gateway($this->attributo("GATEWAY", array_get_default("GATEWAY", $parametri, $_SERVER["PHP_SELF"])));
		$file       = $this->attributo("FILE", $parametri["FILE"]);
		$mode       = $this->attributo("MODE", $parametri["MODE"]);
		$classe_css = $this->attributo("CLASS", array_get_default("CLASS", $parametri));
		$js_id      = $this->attributo("JS-ID");
		$azione = $this->attributo('AZIONE', 'filtra');
		
		$show    = strtolower($this->attributo('REPLACE', 'TRUE'));
		if (cbool($show)) {
			$target  = $this->attributo('TARGET');
			if (!trim($target)) $target = array_get('TARGET', $parametri);
		} else {
			$target = 'null';
		}

		$async   = strtolower($this->attributo('ASYNC', 'TRUE'));
		$tabella    = $this->attributo("TABELLA", $rs->tabella);
		$campo_id   = $this->attributo("CAMPO-ID", $rs->campo_id);
		$ordine     = array_get_default("ORDINE", $parametri, '');
		$no_blocks = cbool($this->parametro('HIDE-BLOCKS', 'FALSE'));
		$load = strtoupper($this->parametro('ESEGUI-CARICA', 'TRUE'));
		
		// forza una mostra riga e stampa anche il link alla pagina corrente
		$force = cbool($this->parametro('FORCE-GO', 'FALSE'));
		if ($force) $azione = 'mostra_riga';
		if ($rs->eof) {
			$id = -1;
		} else {
			$id = $this->parametro('VALORE-ID', $rs->valore($campo_id));
		}

		$base_pars = "TARGET=$target&amp;FILE=$file&amp;AZIONE=$azione&amp;TABELLA=$tabella&amp;CAMPO-ID=$campo_id&amp;ID=$id&amp;ORDINE=$ordine&amp;MODE=$mode&amp;PAGE-SIZE=$dim_pagina&amp;ESEGUI-CARICA=$load&amp;PAGE=";
		if ($extra) $base_pars = $extra.'&amp;'.$base_pars;
		if ($target != 'null' && FALSE === strpos($target, "'")) $target = qt($target);

		$pags = array();
		// salto indietro
		if ($pagina_corrente >= $size){
			// se non sono al primo blocco, vado a 1
			if ($blocco_attuale - $size > 0) {
				$pars  = $base_pars . 0 ;
				$etich = 1; 
				$click = "Ubk.follow($target, '$gateway', '$pars', $show, $async);";
				array_push($pags, "<a href=\"javascript:$click\" id=\"$js_id\" class=\"$classe_css first\">", $etich, '</a>');
			}
			if (!$no_blocks) {
				$pars  = $base_pars . (max($blocco_attuale, 1) - 1) ;
				$etich = ($blocco_attuale - ($size - 1)) .'-'. $blocco_attuale; 
				$click = "Ubk.follow($target, '$gateway', '$pars', $show, $async);";
					
				array_push($pags, "<a href=\"javascript:$click\" id=\"$js_id\" class=\"$classe_css jump\">", $etich, '</a>');
			}
		}
		
		// centrali
		$limit = ($blocco_attuale + $size < $last_page + 1) ?  $blocco_attuale + $size : $last_page + 1;
		for ($pag = $blocco_attuale + 1; $pag <= $limit ; $pag ++ ){
			if ($force || $pag != ($pagina_corrente + 1)){
				$pars  = $base_pars .($pag-1);
				$click = "Ubk.follow($target, '$gateway', '$pars', $show, $async);";
				array_push($pags, "<a href=\"javascript:$click\" id=\"$js_id\" class=\"$classe_css\">$pag</a>");
			} else {
				array_push($pags,"<b>$pag</b>");
			}
		}

		// salto avanti
		if ($blocco_attuale < $blocco_fine){
			if (!$no_blocks) {
				// se sono uguali, c'e' solo l'ultima pagina
				if (($blocco_attuale + ($size + 1)) == $last_page + 1) {
					$etich = $last_page + 1;
					$pars  = $base_pars . $last_page;
				} else {
					$etich = ($blocco_attuale + ($size + 1)) .'-'. (min($blocco_attuale + (2 * $size), $last_page + 1)); 
					$pars  = $base_pars . (min($blocco_attuale + ($size + 1), $last_page + 1) - 1);
					// altrimenti devo poi mandare l'ultima pagina
				}
				$click = "Ubk.follow($target, '$gateway', '$pars', $show, $async);";
				array_push($pags, "<a href=\"javascript:$click\" id=\"$js_id\" class=\"$classe_css jump\">", $etich, '</a>');
			}
			// qua
			if ($no_blocks || $blocco_attuale + (2 * $size) < $last_page + 1) {
				$etich = $last_page + 1;
				$pars  = $base_pars . $last_page;
				$click = "Ubk.follow($target, '$gateway', '$pars', $show, $async);";
				array_push($pags, "<a href=\"javascript:$click\" id=\"$js_id\" class=\"$classe_css last\">", $etich, '</a>');
			}
		}

		if (count($pags)) {
			echo implode('', $pags);
		}

	}
	
}

?>