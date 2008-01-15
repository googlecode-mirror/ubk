<?php

/**
 * Driver per SQL SERVER
 * @package db
 * @author Ubik <emiliano.leporati@gmail.com>
 */
class DB_DRIVER_MSSQL extends DB_DRIVER
{
	public static function connect($db_connection)
	{
		$c =& self::current_connection();

		if (is_null($c->get_info()) || $c->different_from($db_connection)) {

			check_db_connection($db_connection);
	
			$conn_id = mssql_connect($db_connection["DB_SERVER"], $db_connection["DB_USERNAME"], $db_connection["DB_PASSWORD"]);
			if (!$conn_id)
				throw new DbException(__METHOD__, mssql_get_last_message());
	
			if (!mssql_select_db($db_connection["DB_NAME"], $conn_id))
				throw new DbException(__CLASS__.'::'.__METHOD__, "Database ".$db_connection["DB_NAME"]." non presente su ".$db_connection["DB_SERVER"]);

			self::add_connection(new DB_CONNECTION($db_connection, $conn_id));

		} else {
			$c->references ++;
		}
	}

	public static function disconnect()
	{
		$c =& self::current_connection();
		$c->references --;

		if ($c->references <= 0) {

			$_result = mssql_close($c->get_id());
			if (!$_result)
				throw new DbException(__CLASS__.'::'.__METHOD__, mssql_get_last_message());

			self::remove_connection();
		}
	}
	
	public static function get_last_message()
	{
		return mssql_get_last_message();
	}

	public static function errmsg($sql)
	{

		$_errmsg  = mssql_get_last_message();
		if ($_errmsg == "Impossibile eseguire un'operazione full-text. A clause of the query contained only ignored words. ") {
			return "Impossibile eseguire una ricerca libera su congiunzioni, avverbi, articoli o simili.";
		} elseif (is_array($sql)) {
			return $_errmsg.":".BR.$sql["SQL"];
		} else {
			return $_errmsg.":".BR.$sql;
		}
	}


	public static function query($sql)
	{
		if (is_array($sql)) {
			DB_DRIVER::sql_push($sql['SQL']);

			$_res_rs = @mssql_query($sql["SQL"], self::current_connection()->get_id());
			if ($_res_rs && $sql["PAGE"] > 0) {
				mssql_data_seek($_res_rs, $sql["PAGE"] * $sql["PAGE-SIZE"]);
			}

		} elseif (is_string($sql)) {
			DB_DRIVER::sql_push($sql);
			$_res_rs = @mssql_query($sql, self::current_connection()->get_id());
		
		} else {
			throw new CodeException("Query non valida:".BR."$sql");
		}

		if (!$_res_rs && !preg_match('/(?i)INSERT|UPDATE|DELETE/', $sql))
			throw new DbException(__CLASS__.'::'.__METHOD__, self::errmsg($sql));

		return $_res_rs;
	}
	

	public static function & fetch_assoc($rs)
	{
		return mssql_fetch_assoc($rs);
	}


	public static function & fetch_row($rs)
	{
		return mssql_fetch_row($rs);
	}


	public static function free_result($rs)
	{
		if (is_resource($rs)){
			@mssql_free_result($rs);
		}
	}


	public static function num_rows_select($rs)
	{
		return mssql_num_rows($rs);
	}


	public static function num_fields($rs)
	{
		return mssql_num_fields($rs);
	}


	public static function field_info_rs($rs, $i)
	{
		$_data = array();
		$_data['name']   = mssql_field_name($rs, $i);
		$_data['type']   = mssql_field_type($rs, $i);
		$_data['length'] = mssql_field_length($rs, $i);

		return $_data;
	}


	public static function field_info_table($table)
	{
		$_info['default'] = array();
		$_info['null']    = array();
		$_info['fields']  = array();
		$_info['length']  = array();
		$_info['type']    = array();
		
		$_meta =& DB_CLASS_FACTORY::get_meta('MSSQL');
		$_res_rs = self::query($_meta->table_fields($table));

		while ($_riga = mssql_fetch_assoc($_res_rs)) {
			$campo = trim(strtolower($_riga["t_column"]));
			array_push($_info['fields'], $campo);

			$_info['type'][$campo]     = $_riga["t_data_type"];
			$_info['null'][$campo]     = $_riga["b_nullable"];
			$_info['length'][$campo]   = $_riga["i_length"];
			
			$_def_val = $_riga["t_default"];
			// � una stringa
			if ($_def_val[1] == "'") {
				$_info['default'][$campo]  = substr($_def_val, 2, -2);
			} else {
				$_info['default'][$campo]  = substr($_def_val, 1, -1);
			}
						
		}
		mssql_free_result($_res_rs);
		
		return $_info;
	}

	
	public static function field_info_procedure($procedure)
	{
		$_info['params'] = array();
		$_info['length'] = array();
		$_info['type']   = array();
		
		// controllo che la stored esista
		$_rs_esiste = self::query("SELECT COUNT(*) FROM INFORMATION_SCHEMA.ROUTINES WHERE ROUTINE_NAME = ".qt($procedure));
		$_riga = mssql_fetch_array($_rs_esiste);
		mssql_free_result($_rs_esiste);
		if (!$_riga[0])
			throw new DbException(__CLASS__.'::'.__METHOD__, "Stored procedure inesistente: $procedure");

		// carico tranquillo i parametri
		$_res_rs = @mssql_query(meta_procedure_parameters($procedure), self::current_connection()->get_id());
		while ($_riga = mssql_fetch_assoc($_res_rs)) {
			if (strpos($_riga['MODE'],'IN') !== false){
				$param = trim(strtolower($_riga["NAME"]));
				if ($param[0] == "@") $param = substr($param, 1);

				array_push($_info['params'], $param);
				$_info['type'][$param]     = $_riga["TYPE"];
				$_info['length'][$param]   = $_riga["LENGTH"];
			}
		}
		mssql_free_result($_res_rs);
		
		return $_info;
	}

	
	public static function identity($table)
	{
		$_res_rs = @mssql_query("SELECT IDENT_CURRENT('$table') AS i_numero", self::current_connection()->get_id());
		if (!$_res_rs)
			throw new DbException(__CLASS__.'::'.__METHOD__, self::errmsg($sql));
		return $_res_rs;
	}



	public static function db_2_utente($colonna, $valore)
	{
		switch($colonna[0]) {
		case "d":
			// db: yyyymmgg   -   utente : gg/mm/yyyy; 
			return empty($valore) ? "" : substr($valore, -2)."/".substr($valore, 4, 2)."/".substr($valore, 0, 4);
		case "h":
			// db: hhmm - utente : hh.mm; 
			return empty($valore) ? "" : substr($valore, 0, 2).".".substr($valore, -2);

		case "r":
			//  db:yyyymmgghhmm  -  utente: gg/mm/yyyy hh.mm;
			return empty($valore) ? "" : 
				substr($valore, 6, 2)."/".substr($valore, 4, 2)."/".substr($valore, 0, 4)." ".
				substr($valore, 8, 2).".".substr($valore, 10, 2)
			;

		case "b":
			return ($valore == 1) ? true : false;
		
		case "i":
		case "f":
			return $valore;
		
		case "s":
			return strtoupper($valore);
		case "t":
			return $valore;
		
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


	public static function utente_2_db($colonna, $valore)
	{
		if ($valore === "" || is_null($valore)) return "NULL";

		switch($colonna[0]) {
		
		case "d":
			// utente : gg/mm/yyyy; db: yyyymmgg
			return qt(substr($valore, -4).substr($valore, 3, 2).substr($valore, 0, 2));
		case "h":
			// utente : hh:mm; db: hhmm
			return qt(substr($valore, 0 ,2).substr($valore, -2));
		case "r":
			// utente: gg/mm/yyyy hh:mm; db:yyyymmgghhmm
			return qt(
					substr($valore, 6, 4).substr($valore, 3, 2).substr($valore, 0, 2).
					substr($valore, 11, 2).substr($valore, 14, 2)
					);
		
		case "b":
			return empty($valore) ? 0 : 1;
		case "i":
		case "f":
			return $valore;

		case "s":
			return qt(htmlentities(utf8_decode(strtoupper(stripslashes($valore))), ENT_QUOTES));
		case "t":
			 return qt(htmlentities(utf8_decode(stripslashes($valore)), ENT_QUOTES));

		case "e":
			$cry = new CRYPTER("Chiave in chiaro da cambiare come si desidera");
			$value = $cry->encrypt($valore);
			unset($cry);

			return '0x'.$value;
		case "c":
			return qt('0x'.CRYPTER::hash($valore));

		default:
			throw new CodeException("Formato colonna sconosciuto o non supportato: $colonna");
		}
	}

	public static function exec_procedure($procedure, $params)
	{
		$_sql = "EXEC $procedure " . implode(',', $params);

		DB_DRIVER::sql_push($_sql);

		if (! @mssql_query($_sql, self::current_connection()->get_id()))
			throw new DbException(__CLASS__.'::'.__METHOD__, self::errmsg($_sql));
	}

}



?>