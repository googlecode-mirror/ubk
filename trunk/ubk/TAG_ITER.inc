<?php
/**
 * Tag di iterazione (ciclo) - riproducono il loro contenuto sulle righe correnti o su righe calcolate secondo certe logiche.
 * @author Ubik <emiliano.leporati@gmail.com>
 * @package tag
 */

/**
 * Ripete il suo contenuto per il numero di volte specificato; imposta due variabili nei parametri (ITER-START, ITER-CURRENT)
 * @package tag
 */
class ITER extends CONTENITORE
{
	function stampa_html(&$parametri)
	{
		if ($this->attributo_esiste('INIZIO') && $this->attributo_esiste('FINE')) {

			$_start = $this->attributo_richiesto('INIZIO');
			$_step  = $this->attributo('STEP', 1);
			$parametri_new = $parametri;
			$parametri_new['ITER-START'] = $_start;
			// ricalcolando sempre la fine posso fare ciocchi brutti ....
			// mi serve per kimera su certe stampe
			for($i = $_start; $i < $this->attributo_richiesto('FINE'); $i += $_step) {
				$parametri_new['ITER-CURRENT'] = $i;
				parent::stampa_html($parametri_new);
			}

		} elseif ($this->attributo_esiste('ARRAY') || $this->attributo_esiste('DATI')) {
			if ($this->attributo_esiste('ARRAY')) {
				$_a = $this->attributo_richiesto('ARRAY');
				if (is_string($_a) && isset($GLOBALS[$_a])) {
					$_data = &$GLOBALS[$_a];
				} else {
					$_data = $_a;
				}

				if (!is_array($_data) || is_null($_data)) {
					throw new CodeException("ARRAY non valido");
				}

			} elseif ($this->attributo_esiste('DATI')) {
				$_data = eval('return array('.$this->attributo_richiesto('DATI').');');
			}

			if (count($_data) && is_array($_data)) {
				$parametri_new = $parametri;
				foreach($_data as $key => $value) {
					$parametri_new['ITER-CURRENT'] = &$value;
					$parametri_new['ITER-CURRENT-KEY'] = $key;
					parent::stampa_html($parametri_new);
				}
			}
		} else {
			throw new CodeException('Specificare ARRAY, DATI, INIZIO + FINE (+ STEP)');
		}


		unset($parametri_new);
	}
	
}


/**
 * Ripete il suo contenuto per ogni riga del gestore passato. 
 * @package tag
 */
class RIGHE extends CONTENITORE
{
	function stampa_html(&$parametri)
	{
		if ($this->attributo_esiste('PAGE-SIZE')) {
			$rs      =& $parametri['GESTORE'];

			if ($this->attributo_esiste('PAGE') && $this->attributo_esiste('START')) {
				throw new CodeException('Specificare o PAGE o START su RIGHE, non entrambi');
			} elseif ($this->attributo_esiste('START')) {
				$rs->slice($this->attributo('START'), $this->attributo('PAGE-SIZE'));
			} elseif ($this->attributo_esiste('PAGE')) {
				$rs->slice($this->attributo('PAGE') * $this->attributo('PAGE-SIZE'), $this->attributo('PAGE-SIZE'));
			}

			for($rs->move_first(); !$rs->eof; $rs->move_next()) {
				parent::stampa_html($parametri);
			}

			$rs->restore();

		} else {
		
			$rs =& $parametri['GESTORE'];
			$_pos = $rs->index;

			for($rs->move_first(); !$rs->eof; $rs->move_next()) {
				parent::stampa_html($parametri);
			}

			if ($rs->count >= 0 && $_pos >= 0) $rs->move($_pos);
		}
	}
	


}


/**
 * Ripete il suo contenuto per ogni riga correlata a quella corrente in una tabella di destinazione. 
 * @package tag
 */
class RIGHE_CORRELATE extends CONTENITORE
{

	function stampa_html(&$parametri)
	{

		$rs =& $parametri['GESTORE'];

		$tabella  =  $this->attributo_richiesto('TABELLA');
		$rs_link  = new GESTORE($tabella, $this->attributo('CAMPO-ID-TABELLA', NULL));
		
		$campo_id_link = $this->attributo('CAMPO-ID-LINK', $rs_link->campo_id);
		$campo_id = $this->attributo('CAMPO-ID', $campo_id_link);

		$dim_pagina = $this->attributo('PAGE-SIZE', NULL);
		if (!is_null($dim_pagina)) {
			if ($this->attributo_esiste('START')) {
				$start = $this->attributo('START', 0);
				$rs_link -> pagina = $pagina = floor($start / $dim_pagina);
				$rs_link -> dim_pagina = $dim_pagina;
				$start = $start % $dim_pagina;
			} else {
				$rs_link -> pagina = $pagina = $this->attributo('PAGE', 0);
				$rs_link -> dim_pagina = $dim_pagina;
				$start = 0;
			}
		} else {
			$pagina = 0;
			$start = 0;
		}

		$parametri['GESTORE'] =& $rs_link;

		// costruisco la where - di base e' il legame sulle righe correlate
		// se c'e' un filtro aggiuntivo, lo metto in and
		$where = equ($campo_id_link, $rs->valore($campo_id));
		if (array_key_exists('FILTRO', $this->attributi)) {
			$where = c_and($this->attributo('FILTRO'), $where);
		}

		
		// carico le righe
		// se nn ne ho trovate con la paginazione, vuol dire che ero oltre ! quindi nn stampo un cazzo
		$rs_link->carica($where, $this->attributo('ORDINE',NULL),$this->attributo('CAMPI',NULL),$this->attributo('DISTINCT',FALSE));
		
		if ($rs_link->pagina == $pagina) {
			// per orni riga stampo il contenuto
			if ($start) $rs_link->move_rel($start - 1);
			for(; !$rs_link->eof; $rs_link->move_next()) {
				parent::stampa_html($parametri);
			}
		} 
		$parametri['GESTORE'] =& $rs;
	
	}
	
}

/**
 * Scende ricorsivamente in una tabella organizzata ad albero, in cui ogni riga ha un link alla riga padre. 
 * @package tag
 */
class RIGHE_RICORSIVE extends CONTENITORE
{
	/**
	 * tabella
	 * @var string
	 */
	protected $tabella;
	/**
	 * campo id figlio
	 * @var string
	 */
	protected $campo_id_figlio;
	/**
	 * campo id padre
	 * @var string
	 */
	protected $campo_id_padre;
	/**
	 * padding corrente
	 * @var int
	 */
	protected $padding;
	/**
	 * ordinamento durante la select
	 * @var string
	 */
	protected $ordine;
	/**
	 * tag usato per raccogliere i figli (div, ..)
	 * @var string
	 */
	protected $tag;
	/**
	 * metodo di padding (padding, margin, ..)
	 * @var string
	 */
	protected $pad_mtd;
	/**
	 * incremento del padding 
	 * @var int
	 */
	protected $pad_inc;
	/**
	 * unita' di misura del padding (pt, px, em, ..)
	 * @var string
	 */
	protected $pad_um;
	/**
	 * display dei figli (block, inline, ..)
	 * @var string
	 */
	protected $child_display;
	/**
	 * array associativo id-padre => numero figli
	 * @var array
	 */
	protected $rsf;

	/**
	 * Ritorna i vari attributi del singolo nodo: javascript, stile, classe
	 * @param mixed $id Identificativo del nodo
	 * @param int $padding Indentazione del nodo
	 * @return string
	 */
	protected function atts($id, $padding = NULL)
	{
		$over = $out = $click = $s = "";

		if ($o = $this->attributo('JS-OVER')) {
			$over = 'onmouseover='.dQt($o);
		}
		if ($o = $this->attributo('JS-OUT')) {
			$out = 'onmouseout='.dQt($o);
		}
		if ($o = $this->attributo('JS-CLICK')) {
			$click = 'onclick='.dQt($o);
		}

		if ($padding) 
			$s .= "{$this->pad_mtd}: {$padding};";
		if ($this->parametri['RIGHE-RICORSIVE-FIGLI'] && isset($this->nodi_aperti[$id]))
			$s .= "border-bottom: 0;";

		$ret = " $over $out $click";
		if ($s)
			$ret .= " style=\"$s\"";
		if ($class = $this->attributo('CLASS', NULL))
			$ret .= " class=\"{$class}\"";

		return $ret;
	}

	/**
	 * Stampa i figli del nodo specificato
	 * @param mixed $padre Identificativo del nodo
	 * @param GESTORE $rs Recordset con tutti i nodi da stampare
	 */
	protected function stampa_figli($padre, $rs)
	{
		$vecchio_gestore =& $this->parametri['GESTORE'];

		$this->parametri['GESTORE'] =& $rs;

		// arrivo alla riga giusta, quella col padre indicato
		for($rs->move_first(); !$rs->eof && $rs->valore($this->campo_id_padre) != $padre; $rs->move_next());
		// ciclo finche' il padre e' quello indicato
		for(; !$rs->eof && $rs->valore($this->campo_id_padre) == $padre; $rs->move_next()) {

			$id_nodo = $rs->valore($this->campo_id_figlio);

			$this->parametri['RIGHE-RICORSIVE-FIGLI'] = array_get_default($id_nodo, $this->rsf, 0);

			if ($this->padding == 0) {
				echo '<'.$this->tag.$this->atts($id_nodo).'>';
			} else {
				echo '<'.$this->tag.$this->atts($id_nodo , $this->padding.$this->pad_um).'>';
			}

			parent::stampa_html($this->parametri);
			$this->parametri['RIGHE-RICORSIVE-INDICE'] ++;
			if (is_null($this->nodi_aperti) || array_key_exists($id_nodo, $this->nodi_aperti)) {
				$this->parametri['RIGHE-RICORSIVE-LIVELLO'] ++;
				$pos = $rs->index;
				if ($this->pad_inc) {
					$this->padding += $this->pad_inc;
					echo '<'.$this->tag.' style="display: '.$this->child_display.'">';
					$this->stampa_figli($id_nodo, $rs);
					echo '</'.$this->tag.'>';
					$this->padding -= $this->pad_inc;
				} else {
					$this->stampa_figli($id_nodo, $rs);
				}
				$rs->move($pos);
				$this->parametri['RIGHE-RICORSIVE-LIVELLO'] --;
			}
			echo '</'.$this->tag.'>';
		}

		$this->parametri['GESTORE'] =& $vecchio_gestore;
	}


	public function stampa_html(&$parametri)
	{
		$this->nodi_aperti = $this->parametro('NODI-APERTI', NULL);


		$this->tabella = $this->attributo_richiesto('TABELLA');
		$this->padding = $this->attributo_richiesto('INDENTAZIONE');
		$this->pad_inc = $this->attributo('INCREMENTO-IND', 0);
		$this->pad_um  = $this->attributo('UM-IND', 'px');
		$this->ordine  = $this->attributo('ORDINE', NULL);
		$this->tag     = $this->attributo('TAG', 'div');
		$this->pad_mtd = $this->attributo('METODO-IND', 'padding-left');
		$this->child_display = $this->attributo('CHILD-DISPLAY', 'block');
		if ($si = $this->attributo('START-IND', 0))
			$this->padding += $si;

		$rs = new GESTORE($this->tabella, $this->attributo('CAMPO-ID-FIGLIO', NULL));

		$this->campo_id_figlio = $rs->campo_id;
		$this->campo_id_padre  = $this->attributo_richiesto('CAMPO-ID-PADRE');
		$rsf = new RECORDSET("SELECT {$this->campo_id_padre}, count(*) as i_figli FROM {$this->tabella} GROUP BY {$this->campo_id_padre}");
		$v = $rsf->valori($this->campo_id_padre, 'i_figli');
		unset($rsf);
		$this->rsf = array_combine($v[$this->campo_id_padre], $v['i_figli']);

		// devo SEMPRE avere questa condizione perchè carico i dati una volta sola
		// e devo averceli ordinati, dato che faccio un loop sugli id padre
		if (is_null($this->ordine)) {
			$this->ordine = $this->campo_id_padre.' ASC';
		} else {
			$this->ordine = $this->campo_id_padre.' ASC, '.$this->ordine;
		}

		$where = array();

		// filtro su radice e filtro generale: in or
		if ($this->attributo_esiste('FILTRO') && $this->attributo_esiste('FILTRO-GENERALE') ) {
			$where[] = 	c_or(
							c_and(
								equ($this->campo_id_padre, NULL)
								,$this->attributo('FILTRO')
							),
							c_and(
								diff($this->campo_id_padre, NULL)
								,$this->attributo('FILTRO-GENERALE')
							)
						);
		// filtro su radice
		} elseif ($this->attributo_esiste('FILTRO')) {
			$where[] = 	c_and(
							equ($this->campo_id_padre, NULL)
							,$this->attributo('FILTRO')
						);
		// filtro generico
		} elseif ($this->attributo_esiste('FILTRO-GENERALE')) {
			$where[] = $this->attributo('FILTRO-GENERALE');
		}
		
		// ho dei nodi aperti
		if (!is_null($this->nodi_aperti)) {
			// se ho un filtro sulla radice
			// filtro finale: filtro iniziale OR filtro nodi aperti
			if ($this->attributo_esiste('FILTRO')) {
				$where = array(c_or(
								par(implode(') AND (', $where))
								, in($this->campo_id_padre, array_keys($this->nodi_aperti))
							));
			// se no, potrei avere un filtro generale
			} else {
				// filtro finale: filtro generale AND (filtro nodi aperti OR nodi radice)
				if ($this->attributo_esiste('FILTRO-GENERALE')) {
					$where = array(c_and(
									par(implode(') AND (', $where))
									,c_or(
										 in($this->campo_id_padre, array_keys($this->nodi_aperti))
										, equ($this->campo_id_padre, NULL)
									)
								));
				// filtro finale: filtro nodi aperti OR nodi radice
				} else {
					$where[] = c_or(
									 in($this->campo_id_padre, array_keys($this->nodi_aperti))
									, equ($this->campo_id_padre, NULL)
								);
				}
			}
		}

		if (count($where)) {
			$rs->carica(par(implode(') AND (', $where)), $this->ordine);
		} else {
			$rs->carica(NULL, $this->ordine);
		}

		$this->parametri['RIGHE-RICORSIVE-LIVELLO'] = 0;
		$this->parametri['RIGHE-RICORSIVE-INDICE'] = 0;
		$this->stampa_figli($this->attributo('VALORE-PADRE', NULL), $rs);
		unSet($this->parametri['RIGHE-RICORSIVE-FIGLI']);
		unSet($this->parametri['RIGHE-RICORSIVE-LIVELLO']);
		unSet($this->parametri['RIGHE-RICORSIVE-INDICE']);
	}
	
}

?>