<?php
/**
 * Tag di integrazione con quelli xsl per la creazione di righe alternate, etc.
 * @author Ubik <emiliano.leporati@gmail.com>
 * @package tag
 */

/**
 * Cambia la classe alle celle in base al numero della riga - usata in combinazione con gli l'XSLT della xtable
 * @package tag
 * @deprecated
 */
class ALT_TR extends CONTENITORE
{
	function stampa_html(&$parametri)
	{

		if (!isSet($parametri["RIGA"])) $parametri["RIGA"] = 0;
		if (!cbool($this->attributo("ALTERNATE-SKIP", "FALSE")))
			$parametri["RIGA"] ++;

		$classe = $this->attributo_richiesto("CLASS");
		$force  = $this->attributo("TO-CLASS", NULL);
		if (is_null($force)) {
			if ($parametri["RIGA"] % 2 == 1) {
				$_search  = array("/\b".$classe."\b/", "/\b".$classe."_pari\b/");
				$_replace = array($classe."_dispari", $classe."_dispari");
			} else {
				$_search  = array("/\b".$classe."\b/", "/\b".$classe."_dispari\b/");
				$_replace = array($classe."_pari", $classe."_pari");
			}
		} else {
			$_search  = array("/\b".$classe."\b/", "/\b".$classe."_pari\b/", "/\b".$classe."_dispari\b/");
			$_replace = array($force, $force, $force);
		}
		
		for($i = 0; $i < count($this->figli); $i ++) {
			if (is_string($this->figli[$i])) {
				$this->figli[$i] = preg_replace($_search, $_replace, $this->figli[$i]);
			}
		}

		$_atts = "";
		foreach($this->attributi as $att => $val) {
			$_atts .= ' '.$att.'='.dQt($val);
		}

		echo "<TR$_atts>";
		parent::stampa_html($parametri);
		echo "</TR>";
	}
}
/*
class CALENDARIO extends CONTENITORE
{
	private $giorno_corrente;
	private $ultimo_giorno_del_mese;

	function stampa_html(&$parametri)
	{
		$nome_mese = array("Gennaio", "Febbraio", "Marzo", "Aprile", "Maggio", "Giugno", "Luglio", "Agosto", "Settembre", "Ottobre", "Novembre", "Dicembre");
		$valore_default = $this->attributo("VALORE-DEFAULT", date("d/m/Y"));
		$class     = $this->attributo("CLASS", "gas");
		$img_path  = $this->attributo("IMG-PATH", "img");

		if (!finisce_per($img_path, "/"))
			$img_path .= "/";

		$gateway = gateway($this->attributo(
								"GATEWAY", 
								array_get_default(
									"GATEWAY", 
									$parametri, 
									$_SERVER["PHP_SELF"]
								)
							)
					);
		$file    = $this->attributo("FILE", $parametri["FILE"]);

		list($giorno, $mese, $anno) = preg_split ("/[\/-]/", $valore_default);

		echo '<div style="text-align: center;">';
		echo '<input type="hidden" name="mese_id" id="mese_id"/>';
		echo '<input type="hidden" name="mese_nome" id="mese_nome"/>';
		// spostamento anno
		echo "<div style=\"z-index: 2;\">";
		echo "<table style=\"width: 200px; margin-left: auto; margin-right: auto;\">";
		if (cbool($this->attributo("CAMBIA-ANNO", "TRUE"))) {
			echo "<tr>";
			echo "<td style=\"width: 18; vertical-align: middle; text-align: right;\">".
					"<span style=\"cursor: pointer\" onclick=\"document.location = '$gateway?FILE=$file&amp;AZIONE=mostra_anno&amp;MESE=' + mese_corrente + '&amp;ANNO=".($anno - 1)."';\">".
					xsl_transform("<gfx:img xmlns:gfx=\"http://dev.mele.eu/ubk/TAG_XSL.xsd\" nome=\"Back\" path=\"$img_path\" alt=\"Vai all' anno precedente\"/>", true).
					"</span></td>";
			echo "<td style=\"vertical-align: middle; text-align: center; width: 155px;\">".
					'<INPUT style="width: 150px;" maxlength="4" class="'.$class.'_items" type="text" name="anno" value="'.$anno.'" onblur="if (!check_year(this)) {alert(\'Anno non valido !\'); this.focus(); return false;}; document.location = \''.$gateway.'?FILE='.$file.'&amp;AZIONE=mostra_anno&amp;MESE=\' + mese_corrente + \'&amp;ANNO=\' + this.value;"/>'.
					"</td>";
			echo "<td style=\"width: 18; vertical-align: middle; text-align: left;\">".
					"<span style=\"cursor: pointer\" onclick=\"document.location = '$gateway?FILE=$file&amp;AZIONE=mostra_anno&amp;MESE=' + mese_corrente + '&amp;ANNO=".($anno + 1)."';\">".
					xsl_transform("<gfx:img xmlns:gfx=\"http://dev.mele.eu/ubk/TAG_XSL.xsd\" nome=\"Fore\" path=\"$img_path\" alt=\"Vai all' anno successivo\"/>", true).
					"</span></td>";
			echo "</tr>";
		}
		if (cbool($this->attributo("CAMBIA-MESE", "TRUE"))) {
			// generazione combo mesi
			$gfx_combo_mese_xml = "<gfx:combo class=\"$class\" xmlns:gfx=\"http://dev.mele.eu/ubk/TAG_XSL.xsd\" extension-element-prefixes=\"gfx\"><gfx:label>".$nome_mese[$mese-1]."</gfx:label><gfx:items>";
		
			for($i=0;$i<count($nome_mese);$i++){
				$gfx_combo_mese_xml .= "<gfx:item onclick=\"mese_seleziona($i);\">$nome_mese[$i]</gfx:item>";
			}		
			$gfx_combo_mese_xml .= "</gfx:items></gfx:combo>";

			echo "<tr>";
			echo "<td style=\"vertical-align: middle; text-align: right;\">".
					"<span style=\"cursor: pointer\" onclick=\"mese_precedente('$gateway', '$file');\">".
					xsl_transform("<gfx:img xmlns:gfx=\"http://dev.mele.eu/ubk/TAG_XSL.xsd\" nome=\"Back\" path=\"$img_path\" alt=\"Vai al mese precedente\"/>", true).
					"</span></td>";
			echo "<td style=\"vertical-align: middle; text-align: center; \">".xsl_transform($gfx_combo_mese_xml, true)."</td>";
			echo "<td style=\"vertical-align: middle; text-align: left;\">".
					"<span style=\"cursor: pointer\" onclick=\"mese_successivo('$gateway', '$file');\">".
					xsl_transform("<gfx:img xmlns:gfx=\"http://dev.mele.eu/ubk/TAG_XSL.xsd\" nome=\"Fore\" path=\"$img_path\" alt=\"Vai al mese successivo\"/>", true).
					"</span></td>";
			echo "</tr>";
		}
		echo "</table>";
		echo "</div>";
		echo "<div style=\"z-index:1\">";			

		// stampa calendario
		$parametri["CALENDARIO-ANNO"] = $anno;
		for($m=0; $m < 12; $m++) {
			
			$this->ultimo_giorno_del_mese = $this->ultimo_giorno($m, $anno);
			$parametri["CALENDARIO-MESE"] = $m + 1;
			
			$str_mese = ($m === 0 ? '0' : (string)$m);

			echo "<div style=\"display:none;\" id=\"mese_{$str_mese}\">";			
	
			$this->prima_riga($m, $anno, $parametri);
			$this->righe($m, $anno, $parametri);
			$this->ultima_riga($m, $anno, $parametri);
			
			echo "</div>";
		}
		unSet($parametri["CALENDARIO-GIORNO"]);
		unSet($parametri["CALENDARIO-MESE"]);
		unSet($parametri["CALENDARIO-ANNO"]);
		unSet($parametri["CALENDARIO-FESTIVO"]);
		unSet($parametri["CLASS"]);

		$str_mese = ((-- $mese) === 0 ? '0' : (string)$mese);
		echo "</div>";
		echo "</div>";
		echo "<script language=\"javascript\">mese_seleziona({$str_mese}); anno_corrente = $anno;</script>";
	}

	function prima_riga($mese, $anno, &$parametri)
	{
		$class = $this->attributo("CLASS", "gas")."_calendar";
		
		//Stampo l'html della tabella
		$nome_giorno = array("Lun", "Mar", "Mer", "Gio", "Ven", "Sab", "Dom");
		echo "<table class=\"$class\" style=\"margin-left: auto; margin-right: auto\"><thead><tr>";
		for($i = 0; $i < 5; $i ++) {
			echo "<th class=\"$class\">".$nome_giorno[$i]."</th>";
		}
		for($i = 5; $i < 7; $i ++) {
			echo "<th class=\"$class holiday\">".$nome_giorno[$i]."</th>";
		}
		echo "</tr></thead><tbody>";

		// traslo l'anno
		$_anno = trasla_anno($anno);

		//Ricavo il numero di celle vuote da inserire nella prima riga della tabella
		if (date("w", mktime( 0, 0, 0, $mese + 1, 1, $_anno)) == 0) {
			$celle_vuote = 6;
		} else {
			$celle_vuote = (date("w", mktime( 0, 0, 0, $mese + 1, 1, $_anno)) + 7) % 8;
		}
		$this->giorno_corrente = 0;

		if($celle_vuote < 7) {
			echo "<TR>";
			for($i=0; $i < $celle_vuote; $i++){
				echo "<TD class=\"$class empty\">".NBSP."</TD>";
			}
			for($i=0; $i < 7 - $celle_vuote; $i++) {
				$class = $this->compose_class($i, $mese, $anno, 7 - $celle_vuote);
				echo "<TD class=\"$class\">";
				$parametri["CLASS"] = $class;
				$parametri["CALENDARIO-GIORNO"] = ++ $this->giorno_corrente;
				$parametri["CALENDARIO-FESTIVO"] = ($i >= 7 - $celle_vuote - 2);
				parent::stampa_html($parametri);
				echo "</TD>";
			}
			echo "</TR>";
		}

	}

	function righe($mese, $anno, &$parametri)
	{
		//Ricavo il numero dei giorni del mese
		$giorni_da_stampare     = $this->ultimo_giorno_del_mese - $this->giorno_corrente;
		$settimane_da_stampare  = $giorni_da_stampare / 7;
		
		for($i = 0; $i < $settimane_da_stampare - 1; $i ++ ) {
			echo "<TR>";
			for( $n = 0; $n < 7; $n++ ) {
				$class = $this->compose_class($n, $mese, $anno, 7);
				echo "<TD class=\"$class\">";
				$parametri["CLASS"] = $class;
				$parametri["CALENDARIO-GIORNO"] = ++ $this->giorno_corrente;
				$parametri["CALENDARIO-FESTIVO"] = ($n >= 5);
				parent::stampa_html($parametri);
				echo "</TD>";
			}
			echo "</TR>";
		}
	}

	function ultima_riga($mese, $anno, &$parametri)
	{
		$giorni_da_stampare = $this->ultimo_giorno_del_mese - $this->giorno_corrente;

		echo "<TR>";
		for ($i=0; $i < $giorni_da_stampare; $i++) {
			$class = $this->compose_class($i, $mese, $anno, 7);
			echo "<TD class=\"$class\">";
			$parametri["CLASS"] = $class;
			$parametri["CALENDARIO-GIORNO"] = ++ $this->giorno_corrente;
			$parametri["CALENDARIO-FESTIVO"] = ($i >= 5);
			parent::stampa_html($parametri);
			echo "</TD>";
		}

		$celle_vuote = 7 - $giorni_da_stampare;
		$class = $this->attributo("CLASS", "gas")."_calendar";

		for($i=0; $i < $celle_vuote; $i++){
			echo "<TD class=\"$class empty\">".NBSP."</TD>";
		}
		echo "</TR>";
		echo "</tbody></table>";
		$this->giorno_corrente = 0;
	}

	function compose_class($giorno, $mese, $anno, $limite_festa)
	{
		$mese ++;
		$class = $this->attributo("CLASS", "gas")."_calendar";
		if ($giorno >= $limite_festa - 2) $class .= " holiday";
		if ($anno.$mese.($this->giorno_corrente + 1) == date('Ynj')) $class .= " today";
		return $class;
	}


	function ultimo_giorno($mese, $anno)
	{
		$anno = trasla_anno($anno);
		return date("d", mktime( 0, 0, 0, $mese + 2, 0, $anno));
	}
	
}
*/
?>