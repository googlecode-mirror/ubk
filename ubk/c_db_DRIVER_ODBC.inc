<?php

/**
 * Driver ODBC
 * @package db
 * @author Ubik <emiliano.leporati@gmail.com>
 */
class DB_DRIVER_ODBC extends DB_DRIVER
{
	
	public static function connect($db_connection)
	{
		$c =& self::current_connection();

		if (is_null($c->get_info()) || $c->different_from($db_connection)) {
			check_db_connection($db_connection);
	
			$_res_conn = odbc_connect($db_connection["DB_NAME"], $db_connection["DB_USERNAME"], $db_connection["DB_PASSWORD"]);
			if (!$_res_conn)
				throw new DbException(__CLASS__.'::'.__METHOD__, odbc_errormsg());

			self::add_connection(new DB_CONNECTION($db_connection, $_res_conn));
		} else {
			$c->references ++;
		}
	}

	
	public static function disconnect()
	{
		$c =& self::current_connection();
		$c->references --;

		if ($c->references <= 0) {
			@odbc_close($c->get_id());
			self::remove_connection();
		}
	}

	public static function get_last_message()
	{
		return odbc_errormsg();
	}


	public static function errmsg($sql)
	{

		return odbc_errormsg().BR.$sql;
	}


	public static function query($sql)
	{
		DB_DRIVER::sql_push($sql);

		$_res_rs = @odbc_do(self::current_connection()->get_id(), $sql);

		if (!$_res_rs)
			throw new DbException(__CLASS__.'::'.__METHOD__, self::errmsg($sql));

		return $_res_rs;
	}
	

	public static function & fetch_assoc($rs)
	{

		$_row = odbc_fetch_array($rs);
		if (is_array($_row)) {
			$_row = array_change_key_case($_row, CASE_LOWER);
			return $_row;
		} else {
			return $_row;
		}
	}


	public static function & fetch_row($rs)
	{
		$_row = odbc_fetch_row($rs);
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
			@odbc_free_result($rs);
		}
	}

	public static function num_rows_select($rs)
	{
		return @odbc_num_rows($rs);
	}


	public static function num_fields($rs)
	{
		return odbc_num_fields($rs);
	}


	public static function field_info_rs($rs, $i)
	{
		$a = array();
		$a['name']   = odbc_field_name($rs, $i);
		$a['type']   = odbc_field_type($rs, $i);
		$a['length'] = odbc_field_len($rs, $i);

		return $a;
	}


	public static function field_info_table($table)
	{
		$info_table['default'] = array();
		$info_table['null']   = array();
		$info_table['fields'] = array();
		$info_table['length'] = array();
		$info_table['type']   = array();
		
		$_res_rs = 	@odbc_do(self::current_connection()->get_id(), "SELECT * FROM $table");

		$_riga = self::fetch_assoc($_res_rs);

		for($i = 1; $i <= self::num_fields($_res_rs); $i ++) {
			
			$campo = strtolower(odbc_field_name($_res_rs, $i));

			array_push($info_table['fields'], $campo );

			$info_table['type'][$campo]     = odbc_field_type($_res_rs, $i);
			$info_table['length'][$campo]   = odbc_field_len($_res_rs, $i);
			$info_table['null'][$campo]     = true;
			$info_table['default'][$campo]  = null;

		}
		odbc_free_result($_res_rs);
		
		return $info_table;
	}

	
	public static function field_info_procedure($procedure)
	{
	}

	
	public static function identity($table) 
	{
		return self::query("SELECT MAX(".DB_BASE::campo_id_tabella($tabella).") as i_numero FROM $tabella");
	}

	public static function exec_procedure($procedure, $params)
	{
	}

}


?>