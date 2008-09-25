<?php
/**
 * Classe di encrypt / decrypt
 *  
 * @package general
 * @author Ubik <emiliano.leporati@gmail.com>
 */

/**
 * Crittazione e decrittazione di valori
 *  
 * @package general
 */
class CRYPTER 
{
	/**
	 * Tabella di crittazione
	 * @var resource
	 */
	private $td;
	/**
	 * Chiave di crittazione
	 * @var string
	 */
	private $key;
	/**
	 * Vettore di inizializzazione
	 * @var string
	 */
	private $iv;

	/**
	 * Inizializza le variabili private, solleva eccezione se manca il modulo Mcrypt
	 */
	public function __construct($key)
	{
		if (function_exists("mcrypt_module_open")) {
			srand();
			$this->td = mcrypt_module_open(MCRYPT_RIJNDAEL_128, '', 'ecb', '');
			$this->key = substr($key, 0, mcrypt_enc_get_key_size($this->td));
			$iv_size = mcrypt_enc_get_iv_size($this->td);
			$this->iv = mcrypt_create_iv($iv_size, MCRYPT_RAND);
		} else {
			throw new FatalException("Modulo Mcrypt non installato, impossibile utilizzare campi crittati.");
		}
	}


	/**
	 * Chiude il modulo di crittazione
	 */
	public function __destruct()
	{
		mcrypt_module_close($this->td);
	}

	/**
	 * Esegue una codifica reversibile della stringa
	 * @param string $string La stringa da crittare
	 * @return string La stringa crittata
	 */
	public function encrypt($string)
	{
		if (!$string) return $string;

		mcrypt_generic_init($this->td, $this->key, $this->iv);
		$result = bin2hex(mcrypt_generic($this->td, $string));
		mcrypt_generic_deinit($this->td);

		return $result;
	}

	/**
	 * Decodifica una stringa crittata con {@link encrypt}
	 * @param string $string La stringa crittata
	 * @return string La stringa de-crittata
	 */
	public function decrypt($string)
	{
		if (!$string) return $string;

		mcrypt_generic_init($this->td, $this->key, $this->iv);
		$result = rtrim(mdecrypt_generic($this->td, hex2bin($string)));
		mcrypt_generic_deinit($this->td);

		return $result;
	}

	/**
	 * Esegue una codifica irreversibile di una stringa; solleva eccezione se manca il modulo Mcrypt
	 * @static
	 * @param string $string La stringa da crittare
	 * @return string La stringa crittata
	 */
	public static function hash($string)
	{
		if (function_exists("mhash")) {
			return bin2hex(mhash(MHASH_SHA1, $string));
		} else {
			throw new FatalException("Modulo Mhash non installato, impossibile utilizzare campi crittati.");
		}
	}

}


?>