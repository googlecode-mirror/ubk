<?php

/**
 * Query metadati per SQL SERVER
 * @package db
 * @author Ubik <emiliano.leporati@gmail.com>
 */
class DB_META_MSSQL extends DB_META
{
	private static $db = NULL;


	public static function set_db($db = NULL)
	{
		self::$db = $db;
	}

	public static function tables() 
	{
		// t_table

		return	"SELECT TABLE_NAME t_table FROM INFORMATION_SCHEMA.TABLES ".
				"WHERE TABLE_TYPE <> 'VIEW' AND TABLE_NAME NOT LIKE 'sys%' AND TABLE_NAME NOT LIKE 'dtpro%'";
		/// TODO: perchè non funzia? self::db non viene valorizzato 
		// AND TABLE_CATALOG = '{self::db}'";
	}


	public static function table_fields($table) 
	{
		// t_column, t_default, b_nullable, t_data_type, i_length
		
		if (($_p = strpos($table, "(")) !== FALSE) {
			$table = trim(substr($table, 0, $_p));
		}

		return	"SELECT DISTINCT C.NAME t_column, DEF.TEXT as t_default, C.ISNULLABLE b_nullable, TY.NAME t_data_type, C.LENGTH i_length, ".
					"CASE WHEN C.ISCOMPUTED = 1 THEN COMP.TEXT ELSE NULL END as t_compute_expression ".
				"FROM SYSCOLUMNS C ".
					"LEFT JOIN SYSCOMMENTS DEF  ON (C.CDEFAULT = DEF.ID) ".
					"LEFT JOIN SYSCOMMENTS COMP ON (C.ID = COMP.ID) ".
					"LEFT JOIN SYSTYPES TY ON (C.XTYPE = TY.XTYPE) ".
				"WHERE C.ID = OBJECT_ID('$table') ";
//					"AND C.NAME NOT IN ".
//						"(SELECT K.COLUMN_NAME as NAME ".
//						"FROM INFORMATION_SCHEMA.REFERENTIAL_CONSTRAINTS R ".
//							"INNER JOIN INFORMATION_SCHEMA.CONSTRAINT_COLUMN_USAGE K ON (R.CONSTRAINT_NAME = K.CONSTRAINT_NAME) ".
//							"INNER JOIN INFORMATION_SCHEMA.CONSTRAINT_COLUMN_USAGE P ON (P.CONSTRAINT_NAME = R.UNIQUE_CONSTRAINT_NAME) ".
//							"INNER JOIN INFORMATION_SCHEMA.COLUMNS C ON (K.TABLE_NAME = C.TABLE_NAME AND K.COLUMN_NAME = C.COLUMN_NAME) ".
//						"WHERE UPPER(K.TABLE_NAME) = UPPER('$table'))";
	}

	public static function procedure_fields($table) 
	{
		// t_column, t_default, b_nullable, t_data_type, i_length
	}

	public static function table_foreign_keys($table) 
	{
		// t_update_rule, t_delete_rule, t_foreign_column, t_foreign_table, t_primary_column, t_primary_table, t_constraint, b_nullable

		return "SELECT DISTINCT R.UPDATE_RULE t_update_rule, R.DELETE_RULE t_delete_rule, ".
					"K.COLUMN_NAME as t_foreign_column, K.TABLE_NAME t_foreign_table, ".
					"P.TABLE_NAME as PRIMARY_TABLE, P.COLUMN_NAME as PRIMARY_KEY, R.CONSTRAINT_NAME t_constraint,  ".
					"CASE C.IS_NULLABLE WHEN 'YES' THEN 1 ELSE 0 END b_nullable, ".
				"FROM INFORMATION_SCHEMA.REFERENTIAL_CONSTRAINTS R ".
					"INNER JOIN INFORMATION_SCHEMA.CONSTRAINT_COLUMN_USAGE K ON (R.CONSTRAINT_NAME = K.CONSTRAINT_NAME)".
					"INNER JOIN INFORMATION_SCHEMA.CONSTRAINT_COLUMN_USAGE P ON (P.CONSTRAINT_NAME = R.UNIQUE_CONSTRAINT_NAME) ".
					"INNER JOIN INFORMATION_SCHEMA.COLUMNS C ON (K.TABLE_NAME = C.TABLE_NAME AND K.COLUMN_NAME = C.COLUMN_NAME) ".
				"WHERE K.TABLE_NAME = '$table'";

	}


	public static function table_indices($table)
	{
		// t_index, b_unique
		return "SELECT name AS t_index, indexproperty(object_id('$table'), name, 'IsUnique') FROM sysindexes WHERE id = object_id('$table') and (status & 64) = 0";
	}


	public static function table_index_keys($table, $index)
	{
		$_sql = array();

		for ($i = 1; $i <= 16; $i ++) {
			$_sql[] = "SELECT index_col('$table', indid, $i) AS t_field, $i AS i_order ".
						"FROM sysindexes WHERE name = '$index' ".
						"AND (status & 64) = 0 and index_col('$table', indid, $i) IS NOT NULL ";
		}

		return implode(" UNION ALL ", $_sql);
	}

	public static function table_triggers($table)
	{
		// t_trigger, t_when, t_event, i_order
		return	"SELECT name AS t_trigger, ".
				"CASE WHEN ObjectProperty( id, 'ExecIsAfterTrigger') = 1 THEN 'after' ELSE 'before' END AS t_when, ".
				"CASE ".
				"	WHEN ObjectProperty( id, 'ExecIsUpdateTrigger') = 1 and ObjectProperty( id, 'ExecIsDeleteTrigger') = 1 and ObjectProperty( id, 'ExecIsInsertTrigger') = 1 THEN 'insert,update,delete' ".
				"	WHEN ObjectProperty( id, 'ExecIsUpdateTrigger') = 1 and ObjectProperty( id, 'ExecIsInsertTrigger') = 1 THEN 'insert,update' ".
				"	WHEN ObjectProperty( id, 'ExecIsDeleteTrigger') = 1 and ObjectProperty( id, 'ExecIsInsertTrigger') = 1 THEN 'insert,delete' ".
				"	WHEN ObjectProperty( id, 'ExecIsUpdateTrigger') = 1 and ObjectProperty( id, 'ExecIsInsertTrigger') = 1 THEN 'update,delete' ".
				"	WHEN ObjectProperty( id, 'ExecIsUpdateTrigger') = 1 THEN 'update' ".
				"	WHEN ObjectProperty( id, 'ExecIsDeleteTrigger') = 1 THEN 'delete' ".
				"	WHEN ObjectProperty( id, 'ExecIsInsertTrigger') = 1 THEN 'insert' ".
				"	ELSE NULL ".
				"END AS t_event, ".
				"NULL AS i_order ".
				"FROM sysobjects WHERE type = 'TR' AND parent_obj = object_id('$table')";
	}



	public static function table_trigger_body($trigger)
	{
		return "SELECT text AS t_trigger_body FROM syscomments WHERE id = object_id('$trigger')";
	}


	public static function views()
	{
		return "SELECT TABLE_NAME FROM INFORMATION_SCHEMA.VIEWS WHERE TABLE_TYPE = 'VIEW' AND TABLE_NAME NOT LIKE 'sys%' AND TABLE_CATALOG = '{self::$db}'";
	}


	public static function view_fields($view)
	{
		// t_column, t_default, b_nullable, t_data_type, i_length
		return	"SELECT C.NAME t_column, TY.NAME t_data_type, C.LENGTH i_length ".
				"FROM SYSCOLUMNS C ".
					"LEFT JOIN SYSTYPES TY ON (C.XTYPE = TY.XTYPE) ".
				"WHERE C.ID = OBJECT_ID('$table') ";
	}


	public static function view_body($view)
	{
		return "SELECT text AS t_view_body FROM syscomments WHERE id = object_id('$trigger')";
	}


	public static function procedures()
	{
		return "SELECT routine_name as t_procedure FROM information_schema.routines WHERE ROUTINE_TYPE = 'PROCEDURE' and routine_name not like 'dt_%'";
	}


	public static function procedure_parameters($procedure)
	{
		return	"SELECT C.NAME t_column, ".
				"CASE WHEN C.IsOutParam = 1 then 'output' else 'input' END t_parameter_type, ".
				"TY.NAME t_data_type, C.LENGTH i_length ".
				"FROM SYSCOLUMNS C ".
					"LEFT JOIN SYSTYPES TY ON (C.XTYPE = TY.XTYPE) ".
				"WHERE C.ID = OBJECT_ID('$procedure') ";

	}

	public static function procedure_body($procedure)
	{
		return "SELECT text AS t_procedure_body FROM syscomments WHERE id = object_id('$procedure')";
	}

}

?>