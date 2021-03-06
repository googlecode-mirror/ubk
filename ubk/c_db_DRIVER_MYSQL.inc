<?php

/**
 * Driver per MYSQL
 * @package db
 * @author Ubik <emiliano.leporati@gmail.com>
 */
class DB_DRIVER_MYSQL extends DB_DRIVER
{
	public static function connect($db_connection)
	{
		$c =& self::current_connection();

		if (is_null($c->get_info()) || $c->different_from($db_connection)) {

			check_db_connection($db_connection);

			$_res_conn = mysqli_connect($db_connection["DB_SERVER"], $db_connection["DB_USERNAME"], $db_connection["DB_PASSWORD"], $db_connection["DB_NAME"]);
			if (!$_res_conn)
				throw new DbException(__CLASS__.'::'.__METHOD__, mysqli_connect_error());

			mysqli_autocommit($_res_conn, TRUE);

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
			if ($c->is_transaction()) {
				self::rollback();
			} else {
				$_result = @mysqli_close($c->get_id());
				if (!$_result)
					throw new DbException(__CLASS__.'::'.__METHOD__, mysqli_error($c->get_id()));
				self::remove_connection();
			}

		}
	}

	public static function get_last_message()
	{
		return mysqli_error(self::current_connection()->get_id());
	}


	public static function errmsg($sql)
	{
		return mysqli_error(self::current_connection()->get_id());
	}


	public static function query($sql)
	{
		DB_DRIVER::sql_push($sql);

		$_res_rs = @mysqli_query(self::current_connection()->get_id(), $sql);
		if (!$_res_rs)
			throw new DbException(__CLASS__.'::'.__METHOD__, self::errmsg($sql));

		return $_res_rs;
	}
	

	public static function & fetch_assoc($rs)
	{

		$_row = mysqli_fetch_assoc($rs);
		if (is_array($_row)) {
			$_row = array_change_key_case($_row, CASE_LOWER);
			return $_row;
		} else {
			return $_row;
		}
	}


	public static function & fetch_row($rs)
	{
		$_row = mysqli_fetch_row($rs);
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
			@mysqli_free_result($rs);
		}
	}

	// ???????
	public static function num_rows_select($rs)
	{
		return mysqli_affected_rows($rs);

		for($i = 0; mysqli_fetch_row($rs); $i ++);
		return $i;
	}


	public static function num_fields($rs)
	{
		return mysqli_num_fields($rs);
	}


	public static function field_info_rs($rs, $i)
	{
		// name, length, type
		$_data = mysqli_fetch_field_direct($rs, $i);
		$_res  = array();
		$_res['name']    = strtolower($_data->name);
		$_res['type']    = $_data->type;
		$_res['length']  = $_data->length;
		$_res['default'] = $_data->def;

		return $_res;

	}


	public static function field_info_table($table)
	{
		$_meta =& DB_CLASS_FACTORY::get_meta('MYSQL');

		$_info['fields'] = array();
		$_info['length'] = array();
		$_info['type']   = array();
		$_info['default'] = array();
		$_info['null']   = array();

		$_sql = $_meta->table_fields($table);

		$_res_rs = self::query($_sql);
		
		while ($_riga = self::fetch_assoc($_res_rs)) {

			$campo = trim(strtolower($_riga["t_column"]));

			array_push($_info['fields'], $campo);

			$_info['type'][$campo]     = $_riga["t_data_type"];
			$_info['null'][$campo]     = $_riga["b_nullable"];
			$_info['length'][$campo]   = $_riga["i_length"];
			$_info['default'][$campo]  = $_riga['t_default'];

		}
		
		self::free_result($_res_rs);
		
		return $_info;
	}

	
	public static function field_info_procedure($procedure)
	{
		return array();
	}

	
	public static function identity($table) 
	{
		$_res_rs = self::query("SELECT MAX(".campo_id_tabella($table).") as i_numero FROM $table");
		if (!$_res_rs)
			throw new DbException(__CLASS__.'::'.__METHOD__, mysqli_error());

		return $_res_rs;
	}

	public static function exec_procedure($procedure, $params)
	{
		if (!is_array($params))
			$params = array($params);

		$_sql = 'CALL '.$procedure.par(implode(',', $params));
		
		self::query($_sql);
	}


	public static function tr_begin()
	{
		$c =& self::current_connection();
		if (is_null($c->get_info())) {
			throw new DbException(__CLASS__.'::'.__METHOD__, 'Cant begin transaction without a connection.');
		} else {
			if ($c->is_transaction()) {
				$c->references ++;
			} else {
				if (mysqli_autocommit($c->get_id(), FALSE)) {
					self::add_connection(new DB_CONNECTION($c->get_info(), $c->get_id(), TRUE));
				} else {
					throw new DbException(__CLASS__.'::'.__METHOD__, mysqli_error($c->get_id()));
				}
			}
		}
	}

	public static function tr_commit(&$tr)
	{
		$c =& self::current_connection();
		if (!mysqli_commit($c->get_id()))
			throw new DbException(__CLASS__.'::'.__METHOD__, mysqli_error($c->get_id()));

		self::remove_connection();
	}

	public static function tr_rollback(&$tr)
	{
		$c =& self::current_connection();
		if (!mysqli_rollback($c->get_id()))
			throw new DbException(__CLASS__.'::'.__METHOD__, mysqli_error($c->get_id()));

		self::remove_connection();
	}
}


?>