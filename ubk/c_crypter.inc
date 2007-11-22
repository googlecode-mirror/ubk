<?php
/**
 * Crittazione e decrittazione
 *  
 * @package general
 * @author Ubik <emiliano.leporati@gmail.com>
 */

class CRYPTER 
{
	private $td;
	private $key;
	private $iv;

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


	public function __destruct()
	{
		mcrypt_module_close($this->td);
	}

	public function decrypt($string)
	{
		mcrypt_generic_init($this->td, $this->key, $this->iv);
		$result = rtrim(mdecrypt_generic($this->td, hex2bin($string)));
		mcrypt_generic_deinit($this->td);

		return $result;
	}

	public function encrypt($string)
	{
		mcrypt_generic_init($this->td, $this->key, $this->iv);
		$result = bin2hex(mcrypt_generic($this->td, $string));
		mcrypt_generic_deinit($this->td);

		return $result;
	}

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