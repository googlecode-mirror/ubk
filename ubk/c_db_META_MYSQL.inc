<?php

/**
 * Query metadati per MYSQL
 * @package db
 * @author Ubik <emiliano.leporati@gmail.com>
 */
class DB_META_MYSQL extends DB_META
{
	private static $db = NULL;
	
	private static function t_data_type()
	{
		return
				'case lower(data_type) '.
					'when "char" then \'bit\' '.
					'when "date" then \'date\' '.
					'when "time" then \'time\' '.
					'when "timestamp" then \'timestamp\' '.
					'when "int" then \'int\' '.
					'when "bigint" then \'int\' '.
					'when "float" then \'float\' '.
					'when "real" then \'float\' '.
					'when "double precision" then \'float\' '.
					'when "varchar" then \'varchar\' '.
					'when "blob" then \'text\' '.
					'when "mediumblob" then \'text\' '.
					'when "longblob" then \'text\' '.
					'when "text" then \'text\' '.
					'when "mediumtext" then \'text\' '.
					'when "longtext" then \'text\' '.
					'else lower(data_type) '.
				'end t_data_type';
	}


	public static function set_db($db = NULL)
	{
		self::$db = strtolower($db);
	}

	public static function tables() 
	{
		// t_table

		return	'select lower(table_name) as t_table '.
				'from information_schema.tables '.
				'where lower(table_schema) = '.qt(self::$db).' '.
				'order by 1';
	}


	public static function table_fields($table) 
	{
		// t_column, t_default, b_nullable, t_data_type, i_length, i_prec
		return	'select column_name t_column, column_default t_default, case when is_nullable = "YES" then 1 else 0 end b_nullable, '.
				self::t_data_type().', '.
				'character_maximum_length i_length, '.
				'numeric_precision i_prec, '.
				'numeric_scale i_scale '.
				'from information_schema.columns '.
				'where lower(table_name) = '.qt($table).
					' and lower(table_schema) = '.qt(self::$db).
				' order by column_name';
	}


	public static function procedure_fields($table) 
	{
		// t_column, t_default, b_nullable, t_data_type, i_length, i_prec
/*		
		return 'SELECT p.rdb$parameter_name t_column, f.rdb$default_value t_default, 1 - case when f.rdb$null_flag is null then 0 else f.rdb$null_flag end b_nullable, '.
				self::t_data_type().', '.
				'f.rdb$field_length i_length, '.
				'f.rdb$field_precision i_prec, '.
				'f.rdb$field_scale i_scale '.
				'FROM rdb$fields f '.
				'	INNER JOIN rdb$procedure_parameters p ON (p.rdb$field_source = f.rdb$field_name) '.
				'WHERE p.rdb$parameter_type = 1 AND p.rdb$procedure_name = '.qt(strtoupper($table));
				' ORDER BY rdb$parameter_type, rdb$parameter_number';
*/	}

	public static function table_foreign_keys($table) 
	{
		// t_update_rule, t_delete_rule, t_foreign_column, t_foreign_table, t_primary_column, t_primary_table, t_constraint, b_nullable
/*
		return 'select fk.rdb$update_rule t_update_rule, fk.rdb$delete_rule t_delete_rule, '.
					's.rdb$field_name t_foreign_column, tc.rdb$relation_name t_foreign_table, '.
					'ss.rdb$field_name t_primary_column, ii.rdb$relation_name t_primary_table, '.
					'fk.rdb$constraint_name t_constraint, '.
					'1 - case when rf.rdb$null_flag is null then 0 else rf.rdb$null_flag end b_nullable '.
				'from rdb$indices i '.
					'inner join rdb$index_segments s on (i.rdb$index_name = s.rdb$index_name) '.
					'inner join rdb$relation_constraints tc on (tc.rdb$index_name = i.rdb$index_name) '.
					'inner join rdb$ref_constraints fk on (fk.rdb$constraint_name = tc.rdb$constraint_name) '.
					'inner join rdb$indices ii on (ii.rdb$index_name = i.rdb$foreign_key) '.
					'inner join rdb$index_segments ss on (ii.rdb$index_name = ss.rdb$index_name) '.
					'inner join rdb$relation_fields rf on (tc.rdb$relation_name = rf.rdb$relation_name and s.rdb$field_name = rf.rdb$field_name) '.
				'where i.rdb$relation_name = '.qt(strtoupper($table)).
						'and tc.rdb$constraint_type = \'FOREIGN KEY\' '.
				'order by s.rdb$field_name';
*/	}


	public static function table_indices($table)
	{
		// t_index, b_unique
/*
		return	'select i.rdb$index_name t_index, case when i.rdb$unique_flag = 1 then \'1\' else \'0\' end b_unique '.
				'from rdb$indices i '.
				'left join rdb$relation_constraints rc on (i.rdb$index_name = rc.rdb$index_name) '.
				'where (rc.rdb$constraint_type is null or rc.rdb$constraint_type not in (\'PRIMARY KEY\', \'FOREIGN KEY\')) '.
					' and i.rdb$relation_name = '.qt(strtoupper($table)).
				' order by i.rdb$index_name';
*/	}


	public static function table_index_keys($table, $index)
	{
		// t_field, i_order
/*
		return	'select s.rdb$field_name t_field, s.rdb$field_position i_order '.
				'from rdb$indices i '.
				'inner join rdb$index_segments s on (i.rdb$index_name = s.rdb$index_name) '.
				'where i.rdb$relation_name = '.qt(strtoupper($table)).' AND i.rdb$index_name = '.qt(strtoupper($index)).
				' order by s.rdb$field_position';
*/	}

	public static function table_triggers($table)
	{
		// t_trigger, t_when, t_event, i_order, b_active
/*
		return 'select rdb$trigger_name t_trigger, '.
				'case '.
					'when rdb$trigger_type in (1,3,5,11,13,17,21,25,27,53,59,77,89,107,113) then \'before\' '.
					'when rdb$trigger_type in (2,4,6,12,14,18,22,26,28,54,60,78,90,108,114) then \'after\' '.
					'else NULL '.
				'end t_when, '.
				'case '.
				'	when rdb$trigger_type in (1,2) then \'insert\' '.
				'	when rdb$trigger_type in (3,4) then \'update\' '.
				'	when rdb$trigger_type in (5,6) then \'delete\' '.
				'	when rdb$trigger_type in (11,12,17,18) then \'insert|update\' '.
				'	when rdb$trigger_type in (13,14,25,26) then \'insert|delete\' '.
				'	when rdb$trigger_type in (21,22,27,28) then \'update|delete\' '.
				'	when rdb$trigger_type in (53,59,77,89,107,113,54,60,78,90,108,114) then \'insert|update|delete\' '.
				'	else NULL '.
				'end t_event, '.
				'rdb$trigger_sequence i_order, '.
				'case when rdb$trigger_inactive = 0 then \'1\' else \'0\' end  b_active '.
				'from rdb$triggers '.
				'where (rdb$system_flag = 0 or rdb$system_flag is null) and rdb$relation_name = '.qt(strtoupper($table)).
				' order by rdb$trigger_type, rdb$trigger_sequence, rdb$trigger_name';
*/	}



	public static function table_trigger_body($trigger)
	{
/*		return	'select rdb$trigger_source t_trigger_body '.
				'from rdb$triggers '.
				'where rdb$trigger_name = '.qt(strtoupper($trigger));
*/	}


	public static function views()
	{
		return	'select table_name as t_view '.
				'from information_schema.views '.
				'where lower(table_schema) = '.qt(self::$db).' '.
				'order by 1';
	}


	public static function view_fields($view)
	{
		// t_column, t_data_type, i_length
		
		return	'select column_name t_column, '.
				self::t_data_type().', '.
				'character_maximum_length i_length '.
				'from information_schema.columns '.
				'where lower(table_name) = '.qt($table).
					' and lower(table_schema) = '.qt(self::$db).
				' order by column_name';
	}


	public static function view_body($view)
	{
		return	'select view_definition as t_view_body '.
				'from information_schema.views '.
				'where lower(table_name) = '.qt($table).
					' and lower(table_schema) = '.qt(self::$db);
	}


	public static function procedures()
	{
/*		return	'select rdb$procedure_name as t_procedure '.
				'from rdb$procedures '.
				'where (rdb$system_flag is null or rdb$system_flag = 0) '.
				'order by 1';
*/	}


	public static function procedure_parameters($procedure)
	{
/*		return	'select rdb$parameter_name t_parameter, '.
				'case rdb$parameter_type when 0 then \'input\' when 1 then \'output\' else \'undefined\' end t_parameter_type, '.
				self::t_data_type().', '.
				'f.rdb$field_length i_length, '.
				'f.rdb$field_precision i_prec, '.
				'f.rdb$field_scale i_scale '.
				'from rdb$procedure_parameters p '.
				'	inner join rdb$fields f on (p.rdb$field_source = f.rdb$field_name) '.
				'where rdb$procedure_name = '.qt(strtoupper($procedure)).' and rdb$parameter_type = 0'.
				' order by rdb$parameter_type, rdb$parameter_number';
*/	}


	public static function procedure_body($procedure)
	{
/*		return	'select rdb$procedure_source as t_procedure_body '.
				'from rdb$procedures '.
				'where rdb$procedure_name = '.qt(strtoupper($procedure));
*/	}

}

?>