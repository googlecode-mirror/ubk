<?php
/**
 * Classi di azione da estendere per la gestione di utenti, gruppi, diritti e legami di accesso
 * @author Ubik <emiliano.leporati@gmail.com>
 * @package gateway
 */


/** 
 * Inclusione della classe base da estendere per le azioni
 */ 
require_once("c_azioni.inc");


/** 
 * Azioni personalizzate di gestione dei gruppi di accesso
 * @deprecated
 * @package gateway
 */ 
class AZIONI_GRUPPO extends AZIONI_FORM
{
	/** 
	 * Memorizza in sessione l'id del gruppo da gestire e imposta un filtro def su di esso
	 * @param GESTORE $rs
	 */
	public function mostra_riga(&$rs)
	{
		$_SESSION["i_acc_gruppo_id"] = $this->parametri["ID"];
		$this->parametri['FILTRO-DEF'] = equ("i_acc_gruppo_id", $_SESSION["i_acc_gruppo_id"]);
		$this->imposta_filtro_def();

		parent::mostra_riga($rs);
	}

	/** 
	 * Rimuove dalla sessione l'id del gruppo correntemente gestito e annulla il filtro def
	 * @param GESTORE $rs
	 */
	public function mostra(&$rs)
	{
		unSet($_SESSION["i_acc_gruppo_id"]);
		$this->annulla_filtro_def();

		parent::mostra($rs);
	}

	/** 
	 * Aggiorna tutti i diritti del gruppo in una volta sola
	 * @param GESTORE $rs
	 */
	public function aggiorna_diritti(&$rs)
	{
		// faccio un update sulla tabella ponte dei diritti per ogni riga che viene postata
		$rs_diritti = new GESTORE("t_p_acc_gruppo_sezione");
		
		$chiavi = array_get("i_acc_gruppo_sezione_id", $_POST);
		unset($_POST["i_acc_gruppo_sezione_id"]);

		foreach($chiavi as $id) {
			$rs_diritti->aggiorna($id, $_POST);
		}
	}
}

/**
 * Ridefinisce l'azione di elimina con un controllo parametrico preventivo di esistenza della chiave primaria in tabelle esterne
 * @deprecated
 * @package gateway
 */
class AZIONI_ELIMINA_FK extends AZIONI_FORM
{
	/**
	 * Contiene array di tre elementi del tipo (tabella, nome_chiave_esterna_in_tabella, nome_umano_tabella) con le informazioni di check e stampa messaggio per l'eliminazione safe di una riga
	 * @var array
	 */
	protected $verifica_eliminazione;

	/**
	 * Controlla che l'eliminazione di un utente non violi i vincoli di integrit� referenziale con altre tabelle
	 * @param GESTORE $rs
	 */
	public function elimina(&$rs)
	{
		for($i = 0; $i < count($this->verifica_eliminazione); $i++) {
			$g = new GESTORE($this->verifica_eliminazione[$i][0]);
			$n = $g -> conta(equ($this->verifica_eliminazione[$i][1], $this->parametri["ID"]));
			if ($n > 0) {
				throw new AppException("Esistono $n ".$this->verifica_eliminazione[$i][2]." correlati, impossibile eliminare il record.");
			}
			unset($g);
		}

		// se arrivo qui, non ho sollevato nessun errore, quindi posso eliminare tranquillamente

		parent::elimina($rs);
	}

	/**
	 * Inizializza {@link verifica_eliminazione} ad un array vuoto
	 */
	public function __construct($defaults = array())
	{
		$this->verifica_eliminazione = array();

		AZIONI_FORM::__construct($defaults);
	}
}

/**
 * Azioni personalizzate per la gestione della tabella degli utenti
 * @deprecated
 * @package gateway
 */
class AZIONI_UTENTI extends AZIONI_ELIMINA_FK
{

	/**
	 * Se non ho specificato la password, la rimuovo dal $_POST, altrimenti va a NULL
	 * @param GESTORE $rs
	 */
	public function aggiorna(&$rs)
	{
		$_pwd_field = constant_def('PASSWORD_FIELD', 'c_password');

		$_id = array_get("ID", $this->parametri);
		if (in_array($_pwd_field, $rs->campi) && !strlen($_POST[$_pwd_field][$_id])) {
			unset($_POST[$_pwd_field][$_id]);
		}
		parent::aggiorna($rs);
	}

	/**
	 * Se non ho specificato la password, la rimuovo dal $_POST, altrimenti va a NULL
	 * @param GESTORE $rs
	 */
	public function aggiorna_pwd(&$rs)
	{
		$_pwd_field = constant_def('PASSWORD_FIELD', 'c_password');

		$rs->carica(c_and(equ('i_acc_utente_id', $_SESSION['i_acc_utente_id']), equ($_pwd_field, $_POST[$_pwd_field.'_old'])));
		if ($rs->eof) {
			throw new AppException('Vecchia password non valida.');
		} else {
			$rs->aggiorna($_SESSION['i_acc_utente_id'], emula_post($_SESSION['i_acc_utente_id'], array($_pwd_field => $_POST[$_pwd_field])));
		}
	}

	/**
	 * Memorizza l'id dell'utente passato via GET sull'url in una variabile di sessione, quindi richiaman il metodo originale; utilizzata per la visualizzazione dei diritti
	 * @param GESTORE $rs
	 */
	public function mostra_riga(&$rs)
	{
		$_SESSION["i_utente_gestito_id"] = array_get("ID", $this->parametri);
		parent::mostra_riga($rs);
	}

	/**
	 * Rimuove la presonalizzazione del diritti indicato
	 * @param GESTORE $rs
	 */
	public function elimina_diritto(&$rs)
	{
		$rs_diritti = new GESTORE("t_p_acc_utente_sezione");
		$rs_diritti->elimina(array_get("ID", $this->parametri));
	}

	/**
	 * Crea una riga di personalizzazione di un diritto, copiando i diritti attuali ereditati dai gruppi di appartenenza
	 * @param GESTORE $rs
	 */
	public function crea_diritto(&$rs)
	{
		// carico i diritti del gruppo
		$rs_diritti = new GESTORE("v_acc_diritto_gruppo_utente", "i_acc_sezione_id");
		$rs_diritti->carica(c_and(equ("i_acc_sezione_id", $this->parametri["ID"]), equ("i_acc_utente_id", $_SESSION["i_utente_gestito_id"])));
		
		// e li replico per l'utente
		$valori = $rs_diritti->valori_per_query(array("i_acc_sezione_id", "b_r", "b_w", "b_rw", "b_d"));
		$valori["i_acc_utente_id"]["-1"] = $_SESSION["i_utente_gestito_id"];

		$rs_diritti_utente = new GESTORE("t_p_acc_utente_sezione");
		$rs_diritti_utente->crea($valori);
	}

	/** 
	 * Aggiorna tutti i diritti personalizzati dell'utente in una volta sola
	 * @param GESTORE $rs
	 */
	public function aggiorna_diritti(&$rs)
	{
		// faccio un update sulla tabella ponte dei diritti per ogni riga che viene postata
		$rs_diritti = new GESTORE("t_p_acc_utente_sezione");
		if (isSet($_POST["i_id"])) {
			$chiavi = $_POST["i_id"];
			unset($_POST["i_id"]);
			foreach($chiavi as $id) {
				$rs_diritti->aggiorna($id, $_POST);
			}
		}
	}

}

/**
 * Contiene una clausola not_in di filtro dei gruppi o degli utenti gi� legati, in modo da non farli apparire nella combo di creazione
 * @deprecated
 * @global string $non_miei
 */
$non_miei = "";

/**
 * Azioni personalizzate per la gestione dei legami utente - gruppo, in entrambe le direzioni
 * @deprecated
 * @package gateway
 */
class AZIONI_ACC_UT_GRUPPO extends AZIONI_FORM
{
	/**
	 * Aggiunge a $_POST o il gruppo o l'utente correntemente gestito, quindi richiama il metodo originale per eseguire la creazione sul database
	 * @param GESTORE $rs
	 */
	public function crea(&$rs)
	{
		if (isSet($_SESSION["i_acc_utente_gestito_id"])) {
			$_POST["i_acc_utente_id"]["-1"] = $_SESSION["i_acc_utente_gestito_id"];
		} elseif (isSet($_SESSION["i_acc_gruppo_id"])) {
			$_POST["i_acc_gruppo_id"]["-1"] = $_SESSION["i_acc_gruppo_id"];
		} else {
			throw new AppException("Nessun utente o gruppo selezionato per la gestione dei legami di accesso.");
		}

		parent::crea($rs);								
	}

	/**
	 * Carica le righe relative al gruppo o all'utente correntemente gestito, e inizializza la variabile {@link $non_miei}
	 * @param GESTORE $rs
	 */
	protected function carica(&$rs)
	{
		$rs2 = new GESTORE("t_p_acc_utente_gruppo");
		if (isSet($_SESSION["i_acc_gruppo_id"])) {
			$rs->carica(equ("i_acc_gruppo_id", $_SESSION["i_acc_gruppo_id"]));
			$rs2->carica(equ("i_acc_gruppo_id", $_SESSION["i_acc_gruppo_id"]));
			$GLOBALS['non_miei'] = not_in('i_acc_utente_id', $rs2->valori_colonna("i_acc_utente_id"));
		} else {
			$rs2->carica(equ("i_acc_utente_id",$_SESSION["i_acc_utente_gestito_id"]));
			$rs->carica(equ("i_acc_utente_id",$_SESSION["i_acc_utente_gestito_id"]));
			$GLOBALS['non_miei'] = not_in("i_acc_gruppo_id", $rs2->valori_colonna("i_acc_gruppo_id"));
		}
		unSet($rs2);
	}

	/**
	 * Memorizza l'id del gruppo di accesso passato via GET sull'url in una variabile di sessione, annullando l'id dell'utente, quindi richiama la 	{@link AZIONI_FORM::mostra_riga}
	 * @param GESTORE $rs
	 */
	public function mostra_utenti(&$rs)
	{
		$_SESSION["i_acc_gruppo_id"] = array_get("ID", $this->parametri);
		unSet($_SESSION["i_acc_utente_gestito_id"]);
		parent::mostra_riga($rs);
	}

	/**
	 * Memorizza l'id dell'utente passato via GET sull'url in una variabile di sessione, annullando l'id del gruppo di accesso, quindi richiama la 	{@link AZIONI_FORM::mostra_riga}
	 * @param GESTORE $rs
	 */
	public function mostra_gruppi(&$rs)
	{
		$_SESSION["i_acc_utente_gestito_id"] = array_get("ID", $this->parametri);
		unSet($_SESSION["i_acc_gruppo_id"]);
		parent::mostra_riga($rs);
	}

}

/**
 * Azioni personalizzate per la gestione del login
 * @deprecated
 * @package gateway
 */
class AZIONI_LOGIN extends AZIONI_FORM
{
	/**
	 * Annulla il metodo carica - tutto viene svolto in {@link login}
	 * @param GESTORE $rs
	 */
	protected function carica(&$rs)
	{
		// non devo caricare nulla, ho gi� fatto tutto nell'azione
	}

	/**
	 * Distrugge la sezione e rimanda alla pagina di login
	 * @param GESTORE $rs
	 */
	public function logout(&$rs)
	{
		if (isSet($_SESSION["i_acc_utente_id"]) && in_array("b_loggato", $rs->campi)) {
			$rs->aggiorna_dove(array("b_loggato" => 0), equ("i_acc_utente_id", $_SESSION["i_acc_utente_id"]));
		}
		session_destroy();
		header("Location: ".constant_def('LOGIN_URL', '/'.path(VDIR, '')));
		exit;
	}

	public function carica_diritti()
	{
		$_SESSION["diritti"] = array();
		
		$tb_diritti = new GESTORE("v_acc_diritto_utente", "i_acc_utente_id");
		$tb_diritti->carica_righe_id($_SESSION["i_acc_utente_id"]);
		
		// se non ho diritti, errorone 
		if ($tb_diritti->eof) {
			throw new AppException("L'utente non ha alcun diritto, probabilmente non � stato inserito in alcun gruppo di accesso.");

		} else {
			// metto in sessione i diritti
			while (!$tb_diritti->eof) {

				$_SESSION["diritti"]
					[strtoupper($tb_diritti->valore("t_sezione"))] =

					array(
						"R"  => $tb_diritti->valore("b_r"),
						"W"  => $tb_diritti->valore("b_w"),
						"RW" => $tb_diritti->valore("b_rw"),
						"D"  => $tb_diritti->valore("b_d")
					);
				
				$tb_diritti->move_next();
			}
		}

		$this->parametri["DIRITTI"] = $_SESSION["diritti"];
	}

	 /**
	 * Cerca il login passato via POST, e se lo trova ne carica i diritti e memorizza in $_SESSION['i_acc_utente_id'] l'id dell'utente loggato, altrimenti solleva un'errore; inoltre, se il parametro SINGLE_LOGIN � stato definito ed � uguale a 1, allora se l'utente � gi� loggato impedisce un ulteriore login sollevando un'errore.
	 * @param GESTORE $rs
	 */
	public function login(&$rs)
	{
		$_pwd_field = constant_def('PASSWORD_FIELD', 'c_password');

		$rs->carica(c_and(equ("t_login",$_POST["user"]), equ($_pwd_field, $_POST["password"])));

		 // se arriva alla fine del recordset vuol dire che user e password sono inesistenti, e d� errore
		if ($rs -> eof) {

			throw new AppException("Utente o password non validi.");

		// altrimenti trova la riga e mette l'i_acc_utente_id in sessione
		// e si carica i diritti
		
		} elseif (constant_def('SINGLE_LOGIN', FALSE) && in_array("b_loggato", $rs->campi) && $rs->valore("b_loggato")) {
		
			throw new AppException("Utente gi� loggato.");

		} else {
			
			
			$_SESSION["i_acc_utente_id"] = $rs->valore("i_acc_utente_id");

			$this->carica_diritti();
			
			// memorizzo i gruppi in un array di sessione ... da questo viene generato un array con i gruppi di appartenenza
			// per interagire con l'help dentro pagina.xsl
			$tb_gruppi = new GESTORE("t_p_acc_utente_gruppo", "i_acc_utente_id");
			$tb_gruppi -> carica_righe_id($_SESSION["i_acc_utente_id"]);

			$_SESSION['i_acc_gruppi_utente_id'] = $tb_gruppi -> valori_colonna('i_acc_gruppo_id');
			$_SESSION['i_acc_gruppi_utente_id_js'] = "help_groups = new Array('".implode("', '", $_SESSION['i_acc_gruppi_utente_id'])."');";

			if (constant_def('SINGLE_LOGIN', FALSE) && in_array("b_loggato", $rs->campi)) {
				$rs->aggiorna_dove(array("b_loggato" => 1), equ("i_acc_utente_id", $_SESSION["i_acc_utente_id"]));
			}
		}
	}
}


?>
