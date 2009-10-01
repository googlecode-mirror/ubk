<?php

/**
 * Driver per POSTGRES
 * @package db
 * @author Ubik <emiliano.leporati@gmail.com>
 */
class DB_DRIVER_POSTGRES extends DB_DRIVER
{
	public static function connect($db_connection)
	{
		$c =& self::current_connection();

		if (is_null($c->get_info()) || $c->different_from($db_connection)) {

			check_db_connection($db_connection);

			$str = 'host='.$db_connection["DB_SERVER"].
				' port=5432'.
				' dbname='.$db_connection["DB_NAME"].
				' user='.$db_connection["DB_USERNAME"].
				' password='.$db_connection["DB_PASSWORD"];

			$_res_conn = pg_pconnect($str);
			if (!$_res_conn)
				throw new DbException(__CLASS__.'::'.__METHOD__, pg_last_error());
			pg_set_client_encoding($_res_conn, "WIN1254");
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
				$_result = @pg_close($c->get_id());
				if (!$_result)
					throw new DbException(__CLASS__.'::'.__METHOD__, pg_last_error());

				self::remove_connection();
			}
		}
	}

	public static function get_last_message()
	{
		return pg_last_error();
	}


	public static function errmsg($sql)
	{

		$_errmsg = pg_last_error().BR.$sql;
/*		if (inizia_per($_errmsg, "Attempt to store duplicate value")) {
			return "Si e' tentato di inserire una riga con valori duplicati, impossibile procedere:".BR.$sql;
		} elseif (inizia_per($_errmsg, "exception")) {
			return substr($_errmsg, strpos($_errmsg, " ", 11)).BR.$sql;
		} else {
			return $_errmsg;
		}*/
		return $_errmsg;
	}


	public static function query($sql)
	{
		DB_DRIVER::sql_push($sql);

		$_res_rs = @pg_query(self::current_connection()->get_id(), $sql);
		if (!$_res_rs)
			throw new DbException(__CLASS__.'::'.__METHOD__, self::errmsg($sql));

		return $_res_rs;
	}


	public static function & fetch_assoc($rs)
	{
		$_row = pg_fetch_assoc($rs);
		if (is_array($_row)) {
			$_row = array_change_key_case($_row, CASE_LOWER);
			return $_row;
		} else {
			return $_row;
		}
	}


	public static function & fetch_row($rs)
	{
		$_row = pg_fetch_row($rs);
		return $_row;
	}


	public static function free_result($rs)
	{
		if (is_resource($rs)){
			pg_free_result($rs);
		}
	}

	public static function num_rows_select($rs)
	{
		return pg_affected_rows($rs);
	}


	public static function num_fields($rs)
	{
		return pg_num_fields($rs);
	}


	public static function field_info_rs($rs, $i)
	{
		// name, length, type
		$_data = array(
			'name' => strtolower(pg_field_name($rs, $i))
			,'length' => pg_field_prtlen($rs, $i)
			,'type' => pg_field_type($rs, $i)
		);

		return $_data;

	}


	public static function field_info_table($table)
	{
		$_meta =& DB_CLASS_FACTORY::get_meta('POSTGRES');
		$_sqlf =& DB_CLASS_FACTORY::get_sql('POSTGRES');

		$_info['fields'] = array();
		$_info['length'] = array();
		$_info['type']   = array();
		$_info['default'] = array();
		$_info['null']   = array();

		if (strpos($table, "(") !== FALSE) {
			$table = substr($table, 0, strpos($table, "("));
			$_sql = $_meta->procedure_fields($table);
		} else {
			$_rs  = @pg_query(self::current_connection()->get_id(), 'SELECT NOT EXISTS(SELECT 1 FROM pg_proc WHERE upper(proname) = '.qt(strtoupper($table)).')');
			if (!$_rs)
				throw new DbException(__CLASS__.'::'.__METHOD__, pg_last_error());

			$_row = pg_fetch_row($_rs);
			pg_free_result($_rs);

			if ($_row[0]) {
				$_sql = $_meta->table_fields($table);
			} else {
				$_sql = $_meta->procedure_fields($table);
			}
		}

		$_res_rs = self::query($_sql);

		while ($_riga = self::fetch_assoc($_res_rs)) {

			$campo = trim(strtolower($_riga["t_column"]));

			$_info['fields'][] = $campo;
			$_info['type'][$campo]     = $_riga["t_data_type"];
			$_info['null'][$campo]     = $_sqlf->db_2_utente('b', $_riga["b_nullable"]);
			$_info['length'][$campo]   = $_riga["i_length"];
			switch($campo[0]) {
			case 'd':
			case 'h':
			case 'r':
				$_info['default'][$campo] = NULL; break;
			default:
				$_info['default'][$campo]  = $_sqlf->db_2_utente($campo, $_riga["t_default"]);
			}
		}
		self::free_result($_res_rs);

		return $_info;
	}


	public static function field_info_procedure($procedure)
	{
		$_meta =& DB_CLASS_FACTORY::get_meta('POSTGRES');

		$_info['params'] = array();
		$_info['length'] = array();
		$_info['type']   = array();

		$_res_rs = self::query($_meta->procedure_parameters($procedure));


		while ($_riga = self::fetch_assoc($_res_rs)) {

			$param = trim(strtolower($_riga["t_parameter"]));
			$_info['params'][] = $param;
			$_info['type'][$param]     = $_riga["t_data_type"];
			$_info['length'][$param]   = $_riga["i_length"];

		}
		self::free_result($_res_rs);

		unSet($_meta);

		return $_info;
	}


	public static function identity($table)
	{
		$_gen = strtolower($table.'_'.campo_id_tabella($table).'_seq');

		$_res_rs = @pg_query(self::current_connection()->get_id(), 'SELECT currval('.qt($_gen).') AS i_numero');
		if (!$_res_rs)
			throw new DbException(__CLASS__.'::'.__METHOD__, pg_last_error());

		return $_res_rs;
	}

	public static function exec_procedure($procedure, $params)
	{
		if (!is_array($params))
			$params = array($params);

		$_sql = 'SELECT '.$procedure.par(implode(',', $params));

		self::query($_sql);
	}


	public static function tr_begin()
	{
		$c =& self::current_connection();
		if (is_null($c) || is_null($c->get_info())) {
			throw new DbException(__CLASS__.'::'.__METHOD__, 'Cant begin transaction without a connection.');
		} else {
			if ($c->is_transaction()) {
				$c->references ++;
			} else {
				$_res_rs = @pg_query($c->get_id(), 'BEGIN WORK');
				if (!$_res_rs)
					throw new DbException(__CLASS__.'::'.__METHOD__, pg_last_error());
				@pg_free_result($_res_rs);

				self::add_connection(new DB_CONNECTION($c->get_info(), $c->get_id(), TRUE));
			}
		}
	}

	public static function tr_commit()
	{
		$c =& self::current_connection();

		$_res_rs = pg_query($c->get_id(), 'COMMIT WORK');
		if (!$_res_rs)
			throw new DbException(__CLASS__.'::'.__METHOD__, pg_last_error());
		@pg_free_result($_res_rs);

		self::remove_connection();
	}

	public static function tr_rollback()
	{
		$c =& self::current_connection();

		$_res_rs = @pg_query($c->get_id(), 'ROLLBACK WORK');
		if (!$_res_rs)
			throw new DbException(__CLASS__.'::'.__METHOD__, pg_last_error());
		@pg_free_result($_res_rs);

		self::remove_connection();
	}

}


?>
