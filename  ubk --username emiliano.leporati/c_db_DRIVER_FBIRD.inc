<?php

/**
 * Driver per FIREBIRD / INTERBASE
 * @package db
 * @author Ubik <emiliano.leporati@gmail.com>
 */
class DB_DRIVER_FBIRD extends DB_DRIVER
{
	private static $tr = NULL;
	private static $tr_nest = 0;

	public static function connect($db_connection)
	{
		if (is_null(self::$tr)) {

			check_db_connection($db_connection);

			$_res_conn = ibase_pconnect($db_connection["DB_SERVER"].":".$db_connection["DB_NAME"], $db_connection["DB_USERNAME"], $db_connection["DB_PASSWORD"]);
			if (!$_res_conn)
				throw new DbException(__CLASS__.'::'.__METHOD__, ibase_errmsg());

			return $_res_conn;
		} else {
			return self::$tr;
		}
	}

	
	public static function disconnect(&$conn_id)
	{
		if (is_null(self::$tr)) {

			$_result = @ibase_close($conn_id);
			if (!$_result)
				throw new DbException(__CLASS__.'::'.__METHOD__, ibase_errmsg());

		}
		$conn_id = NULL;
	}

	public static function get_last_message($conn_id = NULL)
	{
		return ibase_errmsg();
	}


	public static function errmsg($sql, $conn_id = NULL)
	{

		$_errmsg = ibase_errmsg();
		if (inizia_per($_errmsg, "Attempt to store duplicate value")) {
			return "Si e' tentato di inserire una riga con valori duplicati, impossibile procedere:".BR.$sql;
		} elseif (inizia_per($_errmsg, "exception")) {
			return substr($_errmsg, strpos($_errmsg, " ", 11)).BR.$sql;
		} else {
			return $_errmsg;
		}
	}


	public static function query($sql, $conn_id)
	{
		assert(!is_null($conn_id));

		DB_DRIVER::sql_push($sql);

		$_res_rs = @ibase_query($conn_id, $sql);
		if (!$_res_rs)
			throw new DbException(__CLASS__.'::'.__METHOD__, self::errmsg($sql));

		return $_res_rs;
	}
	

	public static function & fetch_assoc($rs)
	{

		$_row = ibase_fetch_assoc($rs, IBASE_TEXT);
		if (is_array($_row)) {
			$_row = array_change_key_case($_row, CASE_LOWER);
			return $_row;
		} else {
			return $_row;
		}
	}


	public static function & fetch_row($rs)
	{
		$_row = ibase_fetch_row($rs, IBASE_TEXT);
		if (is_array($_row)) {
			$_row = array_change_key_case($_row, CASE_LOWER);
			return $_row;
		} else {
			return $_row;
		}
	}


	public static function free_result($rs)
	{
		if (is_resource($rs)){
			@ibase_free_result($rs);
		}
	}

	// ???????
	public static function num_rows_select($rs)
	{
		return ibase_affected_rows($rs);

		for($i = 0; ibase_fetch_row($rs); $i ++);
		return $i;
	}


	public static function num_fields($rs)
	{
		return ibase_num_fields($rs);
	}


	public static function field_info_rs($rs, $i)
	{
		// name, length, type
		$_data = ibase_field_info($rs, $i);
		unSet($_data['relation']);
		$_data['name'] = strtolower($_data['alias']);
		unSet($_data['alias']);

		return $_data;

	}


	public static function field_info_table($table, $conn_id)
	{
		assert(!is_null($conn_id));

		$_meta =& DB_CLASS_FACTORY::get_meta('FBIRD');

		$_info['fields'] = array();
		$_info['length'] = array();
		$_info['type']   = array();
		$_info['default'] = array();
		$_info['null']   = array();

		if (strpos($table, "(") !== FALSE) {
			$table = substr($table, 0, strpos($table, "("));
			$_sql = $_meta->procedure_fields($table);
		} else {
			$_rs  = @ibase_query($conn_id, 'SELECT COUNT(*) FROM rdb$relation_fields WHERE rdb$relation_name = '.qt(strtoupper($table)));
			if (!$_rs)
				throw new DbException(__CLASS__.'::'.__METHOD__, ibase_errmsg());

			$_row = ibase_fetch_row($_rs);
			ibase_free_result($_rs);

			if ($_row[0]) {
				$_sql = $_meta->table_fields($table);
			} else {
				$_sql = $_meta->procedure_fields($table);
			}
		}

		$_res_rs = self::query($_sql, $conn_id);
		
		while ($_riga = self::fetch_assoc($_res_rs)) {

			$campo = trim(strtolower($_riga["t_column"]));

			array_push($_info['fields'], $campo);

			$_info['type'][$campo]     = $_riga["t_data_type"];
			$_info['null'][$campo]     = $_riga["b_nullable"];
			$_info['length'][$campo]   = $_riga["i_length"];
			
			// gestione dei valori default di campi
			switch (ord($_riga["t_default"][2])){
				case 8: // long
					$val =  ord($_riga["t_default"][4]) + 
							ord($_riga["t_default"][5]) * pow(2, 8) +
							ord($_riga["t_default"][6]) * pow(2, 16) +
							ord($_riga["t_default"][7]) * pow(2, 24);

					$_info['default'][$campo]  = $val;
					break;
				case 14: // txt 
				case 15: // txt
				case 27: // double
					$_info['default'][$campo]  = substr($_riga["t_default"], 7, -1);
					break;
				default:
					$_info['default'][$campo] = null;
			}
		}
		self::free_result($_res_rs);
		
		return $_info;
	}

	
	public static function field_info_procedure($procedure, $conn_id)
	{
		assert(!is_null($conn_id));

		$_meta =& DB_CLASS_FACTORY::get_meta('FBIRD');

		$_info['params'] = array();
		$_info['length'] = array();
		$_info['type']   = array();
		
		$_res_rs = self::query($_meta->procedure_parameters($procedure), $conn_id);

		
		while ($_riga = self::fetch_assoc($_res_rs)) {

			$param = trim(strtolower($_riga["t_parameter"]));
			array_push($_info['params'], $param);

			$_info['type'][$param]     = $_riga["t_data_type"];
			$_info['length'][$param]   = $_riga["i_length"];
			
		}
		self::free_result($_res_rs);
		
		unSet($_meta);

		return $_info;
	}

	
	public static function identity($table, $conn_id) 
	{
		assert(!is_null($conn_id));

		$_gen = constant_def('GEN_PREFIX', 'g_').(defined('TAB_STRIP') ? substr($table, TAB_STRIP) : $table);

		$_res_rs = @ibase_query($conn_id, "SELECT FIRST 1 GEN_ID($_gen, 0) AS i_numero FROM $table");
		if (!$_res_rs)
			throw new DbException(__CLASS__.'::'.__METHOD__, ibase_errmsg());

		return $_res_rs;
	}



	public static static function db_2_utente($colonna, $valore)
	{
		switch($colonna[0]) {
		case "d":
			// db: yyyy-mm-dd -   utente : dd/mm/yyyy
			return empty($valore) ? "" : substr($valore, -2)."/".substr($valore, 5, 2)."/".substr($valore, 0, 4);
		case "h":
			// db: hh:mm:ss.sssss  -   utente : hh.mm 
			return empty($valore) ? "" : substr($valore, 0, 2).".".substr($valore, 3, 2);
		case "r":
			// db: yyyy-mm-dd hh:mm:ss -   utente : dd/mm/yyyy hh.mm
//			return $valore;
			return empty($valore) ? "" : 
				substr($valore, 8, 2)."/".substr($valore, 5, 2)."/".substr($valore, 0, 4)." ".
				substr($valore, 11, 2).".".substr($valore, 14, 2)
			;
		case "b":
			return ($valore == 0) ? FALSE : TRUE;
		case "i":
			return $valore;
		case "f":
			if (strpos($valore, '.') !== FALSE) {
				$valore = preg_replace('/[0]+$/', '', $valore);
				if (substr($valore, -1) == '.') $valore = substr($valore, 0, -1);
			}
			return $valore;
		case "s":
			return rtrim(strtoupper($valore));
		case "t":
			return rtrim($valore);
		case "e":
			$cry = new CRYPTER("Chiave in chiaro da cambiare come si desidera");
			$value = $cry->decrypt($valore);
			unset($cry);
			
			return $value;
		case "c":
			return $valore;
		default:
			throw new CodeException("Formato colonna sconosciuto o non supportato: $colonna");
		}
	}


	public static static function utente_2_db($colonna, $valore)
	{
		if ($valore === "" || is_null($valore)) return "NULL";

		switch($colonna[0]) {
		case "e":
			$cry = new CRYPTER("Chiave in chiaro da cambiare come si desidera");
			$value = $cry->encrypt($valore);
			unset($cry);

			return qt($value);
		case "c":
			return qt(CRYPTER::hash($valore));
		case "s":
			return qt(utf8_decode(htmlentities(strtoupper(stripslashes($valore)), ENT_QUOTES, 'UTF-8')));
		case "t":
			return qt(utf8_decode(htmlentities(stripslashes($valore), ENT_QUOTES, 'UTF-8')));
		case "i":
		case "f":
			return $valore;
		case "d":
			// utente : dd/mm/yyyy; db: yyyy-mm-dd
			return qt(substr($valore, -4).'-'.substr($valore, 3, 2).'-'.substr($valore, 0, 2));
		case "h":
			// utente : hh:mm; db: hh:mm:ss.ssss
			return qt(substr($valore, 0 ,2).':'.substr($valore, -2).':00.0000');
		case "r":
			// utente: dd/mm/yyyy hh:mm; db:yyyy-mm-dd hh:mm:ss.ssss
			return qt(
					substr($valore, 6, 4).'-'.substr($valore, 3, 2).'-'.substr($valore, 0, 2).' '.
					substr($valore, 11, 2).':'.substr($valore, 14, 2).':00.0000'
					);
		case "b":
			return qt(empty($valore) ? '0' : '1');
		default:
			throw new CodeException("Formato colonna sconosciuto o non supportato: $colonna");
		}
	}

	public static function exec_procedure($procedure, $params, $conn_id)
	{
		assert(!is_null($conn_id));

		if (!is_array($params))
			$params = array($params);

		$_sql = 'EXECUTE PROCEDURE '.$procedure.' '.implode(',', $params);
		
		self::query($_sql, $conn_id);
	}


	public static function tr_begin(&$conn_id)
	{
		if (!self::$tr) {
			self::$tr = ibase_trans(IBASE_DEFAULT | IBASE_WRITE | IBASE_CONCURRENCY, $conn_id);
			if (!self::$tr)
				throw new DbException(__CLASS__.'::'.__METHOD__, ibase_errmsg());
		} else {
			self::$tr_nest ++;
		}

		$conn_id = self::$tr;
	}

	public static function tr_commit(&$tr)
	{
		if (self::$tr_nest) {
			self::$tr_nest --;
		} else {
			if (!ibase_commit(self::$tr))
				throw new DbException(__CLASS__.'::'.__METHOD__, ibase_errmsg());

			self::$tr = NULL;
		}
		$tr = NULL;
	}

	public static function tr_rollback(&$tr)
	{
		if (!ibase_rollback(self::$tr))
			throw new DbException(__CLASS__.'::'.__METHOD__, ibase_errmsg());

		self::$tr_nest = 0;
		self::$tr = NULL;
		$tr = NULL;
	}

}


?>