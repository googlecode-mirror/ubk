<?php
/**
 * Tag di creazione link - creano collegamenti ipertestuali con una serie di parametri sull'url interpretabili da {@link AZIONI_FORM}
 * @author Ubik <emiliano.leporati@gmail.com>
 * @package tag
 */

/**
 * Link configurabile in modo generico per le azioni non-standard. 
 * @package tag
 */
class LINK_GENERICO extends TAG
{
	/**
	 * Ritorna il contenuto dell'attributo JS-ONCLICK con i parametri @form e @id sostituiti dai valori correnti del form in cui il link si trova e dell'id su cui opera
	 * @param array $parametri I parametri della {@link stampa_html}
	 * @return string
	 */
	function js_onclick(&$parametri)
	{
		$js_onclick = $this->attributo("JS-ONCLICK");
		if (strlen($js_onclick) > 0) {
			$nodo      =& $this->figlio_richiesto("AZIONE");
			$usa_id    = cbool($nodo->attributo("USA-ID", "FALSE"));
			$post      = cbool($this->attributo_richiesto("POST"));
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
		}

		return $js_onclick;
	}

	/**
	 * Genera la parte di url relativa all'azione
	 * @param array $parametri I parametri della {@link stampa_html}
	 * @return string
	 */
	function link_azione(&$parametri)
	{
		$rs   =& $parametri["GESTORE"];

		$nodo =& $this->figlio_richiesto("AZIONE");
		$nodo -> parametri = $parametri;

		// il gateway e': il mio, o quello dei parametri, o quello di partenza
		$gateway = gateway($nodo->attributo(
								"GATEWAY", 
								array_get_default(
									"GATEWAY", 
									$parametri, 
									$_SERVER["PHP_SELF"]
								)
							)
					);
		$azione  = $nodo->attributo_richiesto("NOME");
		$usa_id  = cbool($nodo->attributo("USA-ID", "FALSE"));
		$mode    = $nodo->attributo("MODE", $parametri["MODE"]);
		$file    = $nodo->attributo("FILE", $parametri["FILE"]);
		$page_size = $nodo->attributo("PAGE-SIZE", array_get_default("PAGE-SIZE", $parametri, 0));
		$page      = max($nodo->attributo("PAGE", array_get_default("PAGE", $parametri, 0)), 0);
		$tabella  = $nodo->attributo("TABELLA", $rs->tabella);
		$campo_id = $nodo->attributo("CAMPO-ID", $rs->campo_id);
		$order    = $nodo->attributo("ORDINE");

		if ($usa_id) {
			if ($nodo->attributo_esiste("VALORE-ID")) {
				$valore_id = $nodo->attributo("VALORE-ID");
			} else {
				$valore_id = $rs->valore($campo_id);
			}
			return "$gateway?AZIONE=$azione&amp;ORDINE=$order&amp;TABELLA=$tabella&amp;CAMPO-ID=$campo_id&amp;ID=$valore_id&amp;MODE=$mode&amp;FILE=$file&amp;PAGE=$page&amp;PAGE-SIZE=$page_size";
		} else {
			return "$gateway?AZIONE=$azione&amp;ORDINE=$order&amp;TABELLA=$tabella&amp;CAMPO-ID=$campo_id&amp;MODE=$mode&amp;FILE=$file&amp;PAGE=$page&amp;PAGE-SIZE=$page_size";
		}

		unSet($nodo);
	}

	/**
	 * Genera la parte di url relativa alla reazione - ossia il redirect che viene effettuato dopo l'azione. Se il nodo manca e l'azione necessita di un post di dati, allora i parametri vengono interamente copiati dal nodo di azione.
	 * @param array $parametri I parametri della {@link stampa_html}
	 * @param bool $post Flag che dice se il link deve scatenare un post di dati (ed e' quindi inserito in un form) o meno
	 * @return string
	 */
	function parametri_reazione(&$parametri, $post)
	{
		$nodo_reazione =& $this->figlio("REAZIONE");
		$nodo_azione   =& $this->figlio_richiesto("AZIONE");
		$nodo_azione   -> parametri = $parametri;

		if ((is_null($nodo_reazione) && !$post)) {
		
			return "";

		} else {

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

			// il gateway e': il mio, o quello dei parametri, o quello del nodo azione, o quello di partenza
			$gateway     = gateway($nodo_reazione->attributo("GATEWAY", 
											array_get_default(
												"GATEWAY", 
												$parametri, 
												$nodo_azione->attributo("GATEWAY", $_SERVER["PHP_SELF"])
											)
									)
							);
			$tabella    = $nodo_reazione->attributo("TABELLA", $rs->tabella);
			$order      = $nodo_reazione->attributo("ORDINE", $nodo_azione->attributo("ORDINE"));
			$usa_id     = cbool($nodo_reazione->attributo("USA-ID", "FALSE"));

			$file       = $nodo_reazione->attributo("FILE", $nodo_azione->attributo("FILE",$parametri["FILE"]));
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
				return "&amp;R-AZIONE=$azione&amp;R-ORDINE=$order&amp;R-GATEWAY=$gateway&amp;R-TABELLA=$tabella&amp;R-CAMPO-ID=$campo_id&amp;R-ID=$valore_id&amp;R-FILE=$file&amp;R-MODE=$mode&amp;R-PAGE=$page&amp;R-PAGE-SIZE=$page_size";
			} else {
				$azione    = $nodo_reazione->attributo("NOME", "mostra");
				return "&amp;R-AZIONE=$azione&amp;R-ORDINE=$order&amp;R-GATEWAY=$gateway&amp;R-TABELLA=$tabella&amp;R-CAMPO-ID=$campo_id&amp;R-FILE=$file&amp;R-MODE=$mode&amp;R-PAGE=$page&amp;R-PAGE-SIZE=$page_size";
			}
		}

		unSet($nodo_reazione);
		unSet($nodo_azione);
	}

	/**
	 * Genera la parte di url personalizzabile
	 */
	function extra_get($figlio, $prefisso = "")
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


	
	function stampa_html(&$parametri)
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

			$target     = $this->attributo("TARGET", "_self");
			$post       = cbool($this->attributo_richiesto("POST"));
			if ($post && !array_key_exists("FORM", $parametri)) {
				throw new CodeException(__CLASS__." non inserito in un FORM");
			}

			if (cbool($this->attributo("REAZIONE", "TRUE"))) {
				$location   = $this->link_azione($parametri).$this->parametri_reazione($parametri, $post).$this->extra_get("EXTRA-GET").$this->extra_get("EXTRA-RE-GET", "R-");
			} else {
				$location   = $this->link_azione($parametri).$this->extra_get("EXTRA-GET");
			}

			$location = url_codifica($location);
			$classe_css = $this->attributo("CLASS", array_get_default("CLASS", $parametri));
			$js_id      = $this->attributo("JS-ID");
			$js_onclick = $this->js_onclick($parametri);
			$alt        = $this->attributo("ALT");
			if (substr(trim($js_onclick), -1) != ";") $js_onclick .= ";";

			$contenuto =& $this->figlio_richiesto("CONTENUTO");

			if ($post) {
				$form =  array_get("FORM", $parametri);
				$form = "document.forms['$form']";

				echo "<A href=\"#\" onclick=\"$js_onclick $form.target='$target';$form.action='$location';$form.submit();\" id=\"$js_id\" class=\"$classe_css\">";
				$contenuto->stampa_html($parametri);
				echo "</A>";
			} else {
				$pop_up =& $this->figlio("POP-UP");
				if (is_null($pop_up)) {
					echo "<A onclick=\"$js_onclick\" title=\"$alt\" target=\"$target\" href=\"$location\" id=\"$js_id\" class=\"$classe_css\">";
					$contenuto->stampa_html($parametri);
					echo "</A>";
				} else {
					$par_open = array("toolbar=no");
					if ($pop_up->attributo_esiste("TOP"))    array_push($par_open, "top=".$pop_up->attributo("TOP"));
					if ($pop_up->attributo_esiste("LEFT"))   array_push($par_open, "left=".$pop_up->attributo("LEFT"));
					if ($pop_up->attributo_esiste("HEIGHT")) array_push($par_open, "height=".$pop_up->attributo("HEIGHT"));
					if ($pop_up->attributo_esiste("WIDTH"))  array_push($par_open, "width=".$pop_up->attributo("WIDTH"));
					if ($pop_up->attributo_esiste("HEIGHT") && ($pop_up->attributo_esiste("WIDTH"))) {
						$resize = "o.resizeTo(".$pop_up->attributo("WIDTH").", ".($pop_up->attributo("HEIGHT") + 20).");";
					} else {
						$resize = "";
					}
					if ($pop_up->attributo_esiste("SCROLLBARS"))  
						array_push($par_open, "scrollbars=".(cbool($pop_up->attributo("SCROLLBARS")) ? "yes" : "no"));
					if ($pop_up->attributo_esiste("TOOLBAR"))  
						array_push($par_open, "toolbar=".(cbool($pop_up->attributo("TOOLBAR")) ? "yes" : "no"));
					if ($pop_up->attributo_esiste("STATUSBAR"))  
						array_push($par_open, "status=".(cbool($pop_up->attributo("STATUSBAR")) ? "yes" : "no"));

					echo "<A href=\"#\" title=\"$alt\" onclick=\"$js_onclick o=window.open('$location','$target', '".implode(",", $par_open)."'); o.focus(); $resize\" id=\"$js_id\" class=\"$classe_css\">";
					$contenuto->stampa_html($parametri);
					echo "</A>";
				}
			}
		}
	}
	
}

/**
 * Link che scatena l'eliminazione di una riga in una tabella. 
 * @package tag
 */
class LINK_ELIMINA extends LINK_GENERICO
{
	private $setup_done = FALSE;

	function stampa_html(&$parametri)
	{
		if (!$this->setup_done) {
			$nodo_azione =& $this->figlio("AZIONE");
			if (is_null($nodo_azione)) {
				$nodo_azione = new TAG("AZIONE");
			}
			
			$js = " if (!confirm('Proseguire con l\'eliminazione ?')) return false;";
			if (isSet($this -> attributi["JS-ONCLICK"])) {
				$this -> attributi["JS-ONCLICK"] .= $js;
			} else {
				$this -> attributi["JS-ONCLICK"] = $js;
			}
			$this -> attributi["POST"] = "false";
			if (!$this->attributo_esiste("DIRITTO"))
				$this -> attributi["DIRITTO"] = "D";

			if (!$nodo_azione -> attributo_esiste("NOME")) {
				$nodo_azione -> attributi["NOME"] = "elimina";
			}
			$nodo_azione -> attributi["USA-ID"] = "true";

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
class LINK_AGGIORNA extends LINK_GENERICO
{
	private $setup_done = FALSE;

	function stampa_html(&$parametri)
	{
		if (!$this->setup_done) {

			$rs =& $parametri["GESTORE"];

			$nodo_azione =& $this->figlio("AZIONE");
			if (is_null($nodo_azione)) {
				$nodo_azione = new TAG("AZIONE");
			}

			$this -> attributi["POST"] = "true";
			if (!$this->attributo_esiste("DIRITTO"))
				$this -> attributi["DIRITTO"] = "RW";
			if (!$nodo_azione -> attributo_esiste("NOME")) {
				$nodo_azione -> attributi["NOME"] = "aggiorna";
			}
			$nodo_azione -> attributi["USA-ID"] = "true";

			array_unshift($this -> figli, $nodo_azione);

			$js = " if (!confirm('Proseguire con l\'aggiornamento ?')) return false; if (!verifica_obbligatori_".$parametri["FORM-N"]."('@id')) return false;";
			if (isSet($this -> attributi["JS-ONCLICK"])) {
				$this -> attributi["JS-ONCLICK"] .= $js;
			} else {
				$this -> attributi["JS-ONCLICK"] = $js;
			}
			$this->setup_done = TRUE;
		}

		parent::stampa_html($parametri);
	}

}
 
/**
 * Link che scatena l'aggiornamento pagina, sulla quale devono essere presenti tutti gli id da aggiornare in un campo multiplo
 * @package tag
 */
class LINK_AGGIORNA_TUTTO extends LINK_GENERICO
{
	private $setup_done = FALSE;


	function stampa_html(&$parametri)
	{
		if (!$this->setup_done) {
			$rs =& $parametri["GESTORE"];

			$nodo_azione =& $this->figlio("AZIONE");
			if (is_null($nodo_azione)) {
				$nodo_azione = new TAG("AZIONE");
			}

			$this -> attributi["POST"] = "true";
			if (!$this->attributo_esiste("DIRITTO"))
				$this -> attributi["DIRITTO"] = "RW";
			if (!$nodo_azione -> attributo_esiste("NOME")) {
				$nodo_azione -> attributi["NOME"] = "aggiorna_tutto";
			}
			$nodo_azione -> attributi["USA-ID"] = "false";

			array_unshift($this -> figli, $nodo_azione);

			$js = " if (!confirm('Proseguire con l\'aggiornamento pagina ?')) return false;";
			if (isSet($this -> attributi["JS-ONCLICK"])) {
				$this -> attributi["JS-ONCLICK"] .= $js;
			} else {
				$this -> attributi["JS-ONCLICK"] = $js;
			}
			$this->setup_done = TRUE;
		}

		parent::stampa_html($parametri);
	}
}
/**
 * Link che scatena l'aggiunta di una riga in una tabella. 
 * @package tag
 */
class LINK_CREA extends LINK_GENERICO
{
	private $setup_done = FALSE;

	function stampa_html(&$parametri)
	{
		if (!$this->setup_done) {
			$nodo_azione =& $this->figlio("AZIONE");
			if (is_null($nodo_azione)) {
				$nodo_azione = new TAG("AZIONE");
			}

			if (!$this->attributo_esiste("REAZIONE")){
				$this -> attributi["REAZIONE"] = "TRUE";
			}

			$js = " if (!confirm('Proseguire con la creazione ?')) return false; if (!verifica_obbligatori_".$parametri["FORM-N"]."('-1')) return false;";
			if (isSet($this -> attributi["JS-ONCLICK"])) {
				$this -> attributi["JS-ONCLICK"] .= $js;
			} else {
				$this -> attributi["JS-ONCLICK"] = $js;
			}
			$this -> attributi["POST"] = "true";
			if (!$this->attributo_esiste("DIRITTO"))
				$this -> attributi["DIRITTO"] = "W";
			if (!$nodo_azione -> attributo_esiste("NOME")) {
				$nodo_azione -> attributi["NOME"] = "crea";
			}
			$nodo_azione -> attributi["USA-ID"] = "false";

			$this -> attributi["JS-ID"] = "link_crea_".($parametri['FORM-N']);


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
class LINK_FILTRA extends LINK_GENERICO
{
	function stampa_html(&$parametri)
	{
		// attributi
		$this -> attributi["POST"] = "true";
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
class LINK_ANNULLA_FILTRO extends LINK_GENERICO
{
	function stampa_html(&$parametri)
	{
		$nodo_azione =& $this->figlio("AZIONE");
		if (is_null($nodo_azione)) {
			$nodo_azione = new TAG("AZIONE");
		}

		$this -> attributi["POST"] = "false";
		$this -> attributi["REAZIONE"] = "false";
		$this -> attributi["DIRITTO"] = "R";
		if (!$nodo_azione -> attributo_esiste("NOME")) {
			$nodo_azione -> attributi["NOME"] = "annulla_filtro";
		}
		$nodo_azione -> attributi["USA-ID"] = "false";

		array_unshift($this -> figli, $nodo_azione);

		$old_page = array_get_default("PAGE", $parametri, 0);
		$parametri["PAGE"] = 0;
		parent::stampa_html($parametri);
		$parametri["PAGE"] = $old_page;
	}
	
	
}

/**
 * Link di tipo generico con href variabile. 
 * @package tag
 */
class LINK_HTML extends TAG
{
	function stampa_html(&$parametri)
	{
		$_proto = $this->attributo("PROTO");
		$_target = $this->attributo("TARGET");
		$_href = $this->figlio_richiesto("HREF");
		$_cont = $this->figlio_richiesto("CONTENUTO");
		$_class = $this->attributo("CLASS", array_get_default("CLASS", $parametri));
		$_alt  = $this->attributo("ALT");
		echo "<a class=\"$_class\" title=\"$_alt\" target=\"$_target\" href=\"$_proto".eval(xml_2_php($_href->contenuto))."\">";
		$_cont->stampa_html($parametri);
		echo "</a>";
	}
	
}

/**
 * Link di tipo mailto con href variabile. 
 * @package tag
 */
class LINK_MAILTO extends LINK_HTML
{
	function stampa_html(&$parametri)
	{
		$this->attributi["PROTO"] = "mailto:";
		parent::stampa_html($parametri);
	}
	
}


/**
 * Genera un link che comanda o annulla l'ordinamento della tabella corrente rispetto ad un campo. Gli ordinamenti sono cumulabili.
 * @package tag
 */
class LINK_ORDINA extends TAG
{
	function stampa_html(&$parametri)
	{
		$rs =& $parametri["GESTORE"];
		// il gateway e': il mio, o quello dei parametri, o quello di partenza
		$gateway    = gateway($this->attributo("GATEWAY", 
										array_get_default(
											"GATEWAY", 
											$parametri, 
											$_SERVER["PHP_SELF"]
										)
								)
						);
		
		$campo_id   = $this->attributo("CAMPO-ID", $rs->campo_id);

		$tabella    = $this->attributo("TABELLA", $rs->tabella);

		$file       = $this->attributo("FILE", $parametri["FILE"]);
		$mode       = $parametri["MODE"];
		$campo      = $this->attributo_richiesto("CAMPO");
		$direzione  = strtoupper($this->attributo_richiesto("DIREZIONE"));
		$azione     = $this->attributo("AZIONE", "ordina");
		$contenuto  =& $this->figlio_richiesto("CONTENUTO");
		$classe_css = $this->attributo("CLASS", array_get_default("CLASS", $parametri));
		$page      = max(array_get_default("PAGE", $parametri, 0), 0);
		$page_size = $this->attributo("PAGE-SIZE", array_get_default("PAGE-SIZE", $parametri, 0));
		
		if (!inizia_per($azione, "ordina")) {
			throw new CodeException("Azione non valida: $azione in LINK-ORDINA");
		}

		$location = "$gateway?AZIONE=$azione&amp;CAMPO=$campo&amp;DIREZIONE=$direzione&amp;TABELLA=$tabella&amp;CAMPO-ID=$campo_id&amp;MODE=$mode&amp;FILE=$file&amp;PAGE=$page&amp;PAGE-SIZE=$page_size";
		$location = url_codifica($location);
		
		echo "<A class=\"$classe_css\" title=\"Ordina\" href=\"$location\">";

		$contenuto->stampa_html($parametri);
		echo "</A>";
	}
	
}

/**
 * Classe base dei link di spostamento.
 * @package tag
 */
class LINK_SPOSTAMENTO extends TAG
{
	/**
	 * Imposta il contenuto del sotto-tag CONTENUTO, se questo non e' gia' impostato
	 * @param mixed $default
	 */
	function imposta_contenuto($default)
	{
		$contenuto =& $this->figlio("CONTENUTO");
		if (is_null($contenuto)) {
			$contenuto = new CONTENITORE("CONTENUTO");
			$contenuto -> aggiungi_dati($default);
			$this -> aggiungi_figlio($contenuto);
		}
	}
}

/**
 * Link di spostamento alla pagina precedente. 
 * @package tag
 */
class LINK_PRECEDENTE extends LINK_SPOSTAMENTO
{

	function stampa_html(&$parametri)
	{
		$pagina_corrente = array_get_default("PAGE", $parametri, 0);

		if ($pagina_corrente > 0) {

			$rs =& $parametri["GESTORE"];

			$target     = $this->attributo("TARGET", "_self");
			$gateway    = gateway($this->attributo(
								"GATEWAY", 
								array_get_default(
									"GATEWAY", 
									$parametri, 
									$_SERVER["PHP_SELF"]
									)
								)
							);
			$file       = $this->attributo("FILE", $parametri["FILE"]);
			$mode       = $this->attributo("MODE", $parametri["MODE"]);
			$classe_css = $this->attributo("CLASS", array_get_default("CLASS", $parametri));
			$js_id      = $this->attributo("JS-ID", "link_precedente");
			$tabella    = $this->attributo("TABELLA", $rs->tabella);
			$campo_id   = $this->attributo("CAMPO-ID",$rs->campo_id);
			$dim_pagina = array_get_default("PAGE-SIZE", $parametri, 0);

			$location   = "$gateway?FILE=$file&amp;AZIONE=filtra&amp;TABELLA=$tabella&amp;CAMPO-ID=$campo_id&amp;MODE=$mode";
			$location  .= "&amp;PAGE=".($pagina_corrente - 1)."&amp;PAGE-SIZE=$dim_pagina";

			$location = url_codifica($location);
			$this->imposta_contenuto("Indietro");

			$contenuto =& $this->figlio("CONTENUTO");

			echo "<A target=\"$target\" href=\"$location\" id=\"$js_id\" class=\"$classe_css\">";
			$contenuto->stampa_html($parametri);
			echo "</A>";
		}
	}

}

/**
 * Link di spostamento alla prima pagina. 
 * @package tag
 */
class LINK_PRIMO extends LINK_PRECEDENTE
{
	function stampa_html(&$parametri)
	{
		$pagina_corrente = array_get_default("PAGE", $parametri, 0);

		if ($pagina_corrente > 0) {
			$parametri["PAGE"] = 1;
			$this->attributi["JS-ID"] = $this->attributo("JS-ID", "link_primo");
			$this->imposta_contenuto("Primo");
			parent::stampa_html($parametri);
			$parametri["PAGE"] = $pagina_corrente;
		}
	}

}

/**
 * Link di spostamento alla pagina successiva. 
 * @package tag
 */
class LINK_SUCCESSIVO extends LINK_SPOSTAMENTO
{

	function stampa_html(&$parametri)
	{
		$rs =& $parametri["GESTORE"];
		$pagina_corrente = array_get_default("PAGE", $parametri, 0);
		$dim_pagina      = array_get_default("PAGE-SIZE", $parametri, 0);
		$last_page       = ($dim_pagina == 0) ? 0 : ceil($rs->count_reale / $dim_pagina) - 1;

		if ($rs->count_reale > 0 && $pagina_corrente < $last_page) {

			$target     = $this->attributo("TARGET", "_self");
			$gateway    = gateway($this->attributo("GATEWAY", array_get_default("GATEWAY", $parametri, $_SERVER["PHP_SELF"])));
			$file       = $this->attributo("FILE", $parametri["FILE"]);
			$mode       = $this->attributo("MODE", $parametri["MODE"]);
			$classe_css = $this->attributo("CLASS", array_get_default("CLASS", $parametri));
			$js_id      = $this->attributo("JS-ID", "link_successivo");

			$tabella    = $this->attributo("TABELLA", $rs->tabella);
			$campo_id   = $this->attributo("CAMPO-ID", $rs->campo_id);

			$location   = "$gateway?FILE=$file&amp;AZIONE=filtra&amp;TABELLA=$tabella&amp;CAMPO-ID=$campo_id&amp;MODE=$mode";
			$location  .= "&amp;PAGE=".($pagina_corrente + 1)."&amp;PAGE-SIZE=$dim_pagina";
			$location = url_codifica($location);

			$this->imposta_contenuto("Avanti");

			$contenuto =& $this->figlio("CONTENUTO");

			echo "<A target=\"$target\" href=\"$location\" id=\"$js_id\" class=\"$classe_css\">";
			$contenuto->stampa_html($parametri);
			echo "</A>";
		}
	}
	
}

/**
 * Link di spostamento all'ultima pagina. 
 * @package tag
 */
class LINK_ULTIMO extends LINK_SUCCESSIVO
{
	function stampa_html(&$parametri)
	{
		$rs =& $parametri["GESTORE"];
		$pagina_corrente = array_get_default("PAGE", $parametri, 0);
		$dim_pagina      = array_get_default("PAGE-SIZE", $parametri, 0);
		$last_page       = ($dim_pagina == 0) ? 0 : ceil($rs->count_reale / $dim_pagina) - 1;

		if ($rs->count_reale > 0 && $pagina_corrente < $last_page) {
			$parametri["PAGE"] = $last_page - 1;
			$this->attributi["JS-ID"] = $this->attributo("JS-ID", "link_ultimo");
			$this->imposta_contenuto("Ultimo");
			parent::stampa_html($parametri);
			$parametri["PAGE"] = $pagina_corrente;
		}
	}

}

/**
 * Inserisce una barra di navigazione che permette di spostarsi su una pagina specifica
 * @package tag
 */
class PAGINATORE extends TAG
{
	function stampa_html(&$parametri)
	{
		
		$rs =& $parametri["GESTORE"];
		$pagina_corrente = array_get_default("PAGE", $parametri, 0);
		$dim_pagina      = array_get_default("PAGE-SIZE", $parametri, 0);
		$last_page       = ($dim_pagina == 0) ? 0 : ceil($rs->count_reale / $dim_pagina) - 1;

		// non deve apparire  null se c'e' solo un'unica pagina
		if ($last_page == 0) {
			return;	
		}

		$decina_attuale = floor($pagina_corrente /10) * 10; 
		$decina_fine = floor($last_page /10) * 10;

		$target     = $this->attributo("TARGET", "_self");
		$gateway    = gateway($this->attributo("GATEWAY", array_get_default("GATEWAY", $parametri, $_SERVER["PHP_SELF"])));
		$file       = $this->attributo("FILE", $parametri["FILE"]);
		$mode       = $this->attributo("MODE", $parametri["MODE"]);
		$classe_css = $this->attributo("CLASS", array_get_default("CLASS", $parametri));
		$js_id      = $this->attributo("JS-ID");

		//$azione    = $this->attributo("AZIONE", $parametri["AZIONE"]);
		$tabella    = $this->attributo("TABELLA", $rs->tabella);
		$campo_id   = $this->attributo("CAMPO-ID", $rs->campo_id);
		$ordine     = array_get_default("ORDINE", $parametri, '');

		$base_location   = "$gateway?FILE=$file&amp;AZIONE=filtra&amp;TABELLA=$tabella&amp;CAMPO-ID=$campo_id&amp;ORDINE=$ordine&amp;MODE=$mode&amp;PAGE-SIZE=$dim_pagina&amp;PAGE=";
			

		if ($pagina_corrente >= 10){
			$location  = $base_location . (max($pagina_corrente - 9, 1) - 1) ;
			$location = url_codifica($location);
			$etich = ($decina_attuale - 9) .'-'. $decina_attuale; 
			echo "<a target=\"$target\" href=\"$location\" id=\"$js_id\" class=\"$classe_css\"> $etich";
			//$contenuto->stampa_html($parametri);
			echo "</a>&nbsp;&nbsp;";
		}
		
		$limit = ($decina_attuale + 10 < $last_page + 1) ?  $decina_attuale + 10 : $last_page + 1;
		$pags = array();
		for ($pag = $decina_attuale + 1; $pag <= $limit ; $pag ++ ){
			if ($pag != ($pagina_corrente + 1)){
				$location  = $base_location .($pag-1);
				$location = url_codifica($location);
				array_push($pags,"<a target=\"$target\" href=\"$location\" id=\"$js_id\" class=\"$classe_css\">$pag</a>");
			} else {
				array_push($pags,"<b>$pag</b>");
			}
		}
		if (count($pags)) {
			echo "[ ";
			echo implode(' - ',$pags);
			echo " ] ";
		}
		
		if ($decina_attuale < $decina_fine){
			$location  = $base_location . (min($pagina_corrente + 11, $last_page + 1) - 1);
			$location = url_codifica($location);
			if (($decina_attuale + 11) == $last_page + 1) {
				$etich = $last_page + 1;
			} else {
				$etich = ($decina_attuale + 11) .'-'. (min($decina_attuale + 20, $last_page + 1)); 
			}
			echo "&nbsp;&nbsp;<a target=\"$target\" href=\"$location\" id=\"$js_id\" class=\"$classe_css\">$etich</a>";
			//$contenuto->stampa_html($parametri);
			//echo "</a> ";
		}
	}
	
}


?>