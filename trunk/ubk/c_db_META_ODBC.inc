<?php

/**
 * Query metadati per ODBC
 * @package db
 * @author Ubik <emiliano.leporati@gmail.com>
 */
class DB_META_ODBC extends DB_META
{

	public static function set_db($db = NULL)
	{
		// non ho bisogno di fare nulla. su mssql invece, ad esempio, devo salvarmi il nome del db per le query sullo schema o sulle systables (credo)
	}

	public static function tables() 
	{
		// t_table

	}


	public static function table_fields($table) 
	{
		// t_column, t_default, b_nullable, t_data_type, i_length
		
	}

	public static function procedure_fields($table) 
	{
		// t_column, t_default, b_nullable, t_data_type, i_length
		
	}

	public static function table_foreign_keys($table) 
	{
		// t_update_rule, t_delete_rule, t_foreign_column, t_foreign_table, t_primary_column, t_primary_table, t_constraint, b_nullable

	}


	public static function table_indices($table)
	{
		// t_index, b_unique


	}


	public static function table_index_keys($table, $index)
	{
		// t_field, i_order
	}

	public static function table_triggers($table)
	{

	}



	public static function table_trigger_body($trigger)
	{
	}


	public static function views()
	{
	}


	public static function view_fields($view)
	{
	}


	public static function view_body($view)
	{
	}


	public static function procedures()
	{
	}


	public static function procedure_parameters($procedure)
	{
	}


	public static function procedure_body($procedure)
	{
	}

}

?>