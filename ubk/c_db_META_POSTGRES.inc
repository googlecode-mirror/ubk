<?php

/**
 * Query metadati per POSTGRESQL
 * @package db
 * @author Ubik <emiliano.leporati@gmail.com>
 */
class DB_META_POSTGRES extends DB_META
{

	private static function t_data_type()
	{
		return
			'case tp.typname '.
				"when 'int2' then 'int' ".
				"when 'int4' then 'int' ".
				"when 'int8' then 'int' ".
				"when 'float4' then 'float' ".
				"when 'float8' then 'float' ".
				"when 'bpchar' then 'char' ".
				"when 'bool' then 'bit' ".
				"else tp.typname ".
			'end as t_data_type';
	}

	private static function i_length()
	{
		return 'case when a.attlen < 0 and a.atttypmod < 0 then NULL when a.attlen < 0 then a.atttypmod else a.attlen end '.
			' - case when tp.typname in (\'bpchar\',\'char\',\'varchar\', \'text\') then 4 else 0 end as i_length';
	}

	private static function i_type_length()
	{
		return 'tp.typlen '.
			' - case when tp.typname in (\'bpchar\',\'char\',\'varchar\', \'text\') then 4 else 0 end as i_length';
	}

	private static function t_default()
	{
		return 'regexp_replace(regexp_replace(regexp_replace(def.adsrc, E\'::\\\\w+$\', \'\'), E\'^\\\'\', \'\'), E\'\\\'$\', \'\') as t_default';
	}

	public static function set_db($db = NULL)
	{
		// non ho bisogno di fare nulla. su mssql invece, ad esempio, devo salvarmi il nome del db per le query sullo schema o sulle systables (credo)
	}

	public static function tables() 
	{
		// t_table

		return	'select tablename as t_table '.
				'from pg_tables '.
				'where schemaname=\'public\' '.
				'order by 1';
	}


	public static function table_fields($table) 
	{
		// t_column, t_default, b_nullable, t_data_type, i_length, i_prec, i_scale, t_source

		return 'select lower(a.attname) as t_column, '.
				self::t_default().', '.
				'not a.attnotnull as b_nullable, '.
				self::t_data_type().', '.
				self::i_length().', '.
				'NULL as i_prec, '.
				'NULL as i_scale, '.
				'NULL as t_source '.
			'from pg_attribute a '.
				'inner join pg_class tbl on (a.attrelid = tbl.oid) '.
				'inner join pg_type tp on (a.atttypid = tp.oid) '.
				'left join pg_attrdef def on (a.attrelid = def.adrelid and a.attnum = def.adnum) '.
			'where '.
				'upper(tbl.relname) = '.qt(strtoupper($table)).' and '.
				'not a.attisdropped and '.
				'a.attnum > 0 '.
				'order by a.attnum';
	}


	public static function procedure_fields($procedure)
	{
		// t_column, t_default, b_nullable, t_data_type, i_length, i_prec
/*
select lower(a.attname) as t_column, 
def.adsrc as t_default,
not a.attnotnull as b_nullable,
case tp.typname
when 'int2' then 'int'
when 'int4' then 'int'
when 'int8' then 'int'
when 'float4' then 'float'
when 'float8' then 'float'
when 'bpchar' then 'char'
when 'bool' then 'bit'
else tp.typname
end as t_data_type,
case when a.attlen < 0 and a.atttypmod < 0 then NULL when a.attlen < 0 then a.atttypmod else a.attlen end
 - case when tp.typname in ('bpchar','char','varchar', 'text') then 4 else 0 end as i_length,
NULL as i_prec
from pg_proc p
join pg_type tr on (p.prorettype = tr.oid)
join pg_attribute a on (a.attrelid = tr.typrelid)
join pg_type tp on (a.atttypid = tp.oid)
left join pg_attrdef def on (a.attrelid = def.adrelid and a.attnum = def.adnum)
where upper(p.proname) = 'USER_FORUMS_RIGHTS'
and a.attnum > 0 and not a.attisdropped
order by a.attnum
*/

		return 'select lower(a.attname) as t_column, '.
				self::t_default().', '.
				'not a.attnotnull as b_nullable, '.
				self::t_data_type().', '.
				self::i_length().', '.
				'NULL as i_prec '.
				'from pg_proc p '.
					'join pg_type tr on (p.prorettype = tr.oid) '.
					'join pg_attribute a on (a.attrelid = tr.typrelid) '.
					'join pg_type tp on (a.atttypid = tp.oid) '.
					'left join pg_attrdef def on (a.attrelid = def.adrelid and a.attnum = def.adnum) '.
				'where upper(p.proname) = '.qt(strtoupper($procedure)).' '.
					'and a.attnum > 0 and not a.attisdropped '.
				'order by a.attnum';
	}

	public static function table_foreign_keys($table)
	{
		// t_update_rule, t_delete_rule, t_foreign_column, t_foreign_table, t_primary_column, t_primary_table, t_constraint, b_nullable

		return 'select '.
			'case c.confupdtype '.
				'when \'a\' then \'no action\' '.
				'when \'c\' then \'cascade\' '.
				'when \'n\' then \'set null\' '.
			'end as t_update_rule, '.
			'case c.confdeltype '.
				'when \'a\' then \'no action\' '.
				'when \'c\' then \'cascade\' '.
				'when \'n\' then \'set null\' '.
			'end as t_delete_rule, '.
			'lower(ftblc.attname) as t_foreign_column, '.
			'lower(ftbl.relname) as t_foreign_table, '.
			'lower(tblc.attname) as t_primary_column, '.
			'lower(tbl.relname) as t_primary_table, '.
			'lower(c.conname) as t_constraint, '.
			'not ftblc.attnotnull as b_nullable '.
		'from pg_class ftbl '.
			'inner join pg_constraint c on (ftbl.oid = c.conrelid) '.
			'inner join pg_class tbl on (c.confrelid = tbl.oid) '.
			'inner join pg_attribute tblc on (tbl.oid = tblc.attrelid and tblc.attnum = ANY( c.confkey) ) '.
			'inner join pg_attribute ftblc on (ftbl.oid = ftblc.attrelid and ftblc.attnum = ANY (c.conkey) ) '.
		'where '.
			'upper(ftbl.relname) = '.qt(strtoupper($table)).' and '.
			'c.contype = \'f\'';
	}


	public static function table_indices($table)
	{
		// t_index, b_unique

		return 'select lower(ii.relname) as t_index, indisunique as b_unique '.
			'from pg_index i '.
				'join pg_class ii on (i.indexrelid = ii.oid) '.
				'join pg_class tbl on (i.indrelid = tbl.oid) '.
			'where upper(tbl.relname) = '.qt(strtoupper($table)).' order by 1';
	}


	public static function table_index_keys($table, $index)
	{
		return 'select a.attname, null as i_order '.
			'from pg_index i '.
				'join pg_class ii on (i.indexrelid = ii.oid) '.
				'join pg_class tbl on (i.indrelid = tbl.oid) '.
				'join pg_attribute a on (a.attrelid = tbl.oid and a.attnum = ANY(i.indkey)) '.
			'where upper(tbl.relname) = '.qt(strtoupper($table)).' and upper(ii.relname) = '.qt(strtoupper($index));
	}

	public static function table_triggers($table)
	{
		// t_trigger, t_when, t_event, i_order, b_active

		$table = qt(strtoupper($table));

		return 'select lower(tr.tgname) as t_trigger, '.
			'case '.
				'when tr.tgtype & (1 << 1) = (1 << 1) then \'before\' '.
				'else \'after\' '.
			'end as t_when, '.
			'case '.
				'when tr.tgtype & (1 << 2 | 1 << 3 | 1 << 4) = (1 << 2 | 1 << 3 | 1 << 4) then \'insert|update|delete\' '.
				'when tr.tgtype & (1 << 2 | 1 << 3) = (1 << 2 | 1 << 3) then \'insert|delete\' '.
				'when tr.tgtype & (1 << 2 | 1 << 4) = (1 << 2 | 1 << 4) then \'insert|update\' '.
				'when tr.tgtype & (1 << 3 | 1 << 4) = (1 << 3 | 1 << 3) then \'update|delete\' '.
				'when tr.tgtype & (1 << 2) = (1 << 2) then \'insert\' '.
				'when tr.tgtype & (1 << 3) = (1 << 3) then \'delete\' '.
				'when tr.tgtype & (1 << 4) = (1 << 4) then \'update\' '.
			'end as t_event, '.
			'(select count(*) from pg_trigger tt join pg_class cc on tt.tgrelid = cc.oid and not tt.tgisconstraint and upper(cc.relname) = '.$table.' and tt.tgname < tr.tgname) as i_order, '.
			'tr.tgenabled != \'D\' as b_active '.
			'from pg_trigger tr '.
				'join pg_class c on (c.oid = tr.tgrelid) '.
			'where not tr.tgisconstraint and upper(c.relname) = '.$table.
			'order by i_order';
	}



	public static function table_trigger_body($trigger)
	{
		return	'select lower(p.proname) as t_trigger_body '.
				'from pg_trigger t join pg_proc p on (t.tgfoid = p.oid) '.
				'where upper(t.tgname) = '.qt(strtoupper($trigger));
	}


	public static function views()
	{
		return	'select lower(viewname) as t_view '.
				'from pg_views '.
				'where schemaname=\'public\' '.
				'order by 1';
	}


	public static function view_fields($view)
	{
		// t_column, t_data_type, i_length

		return 'select lower(a.attname) as t_column, '.
			self::t_data_type().', '.
			self::i_length().
		'from pg_attribute a '.
			'inner join pg_class tbl on (a.attrelid = tbl.oid) '.
			'inner join pg_type tp on (a.atttypid = tp.oid) '.
		'where '.
			'upper(tbl.relname) = '.qt(strtoupper($view)).' and '.
			'not a.attisdropped and '.
			'a.attnum > 0 '.
			'order by a.attnum ';
	}


	public static function view_body($view)
	{
		return	'select definition as t_view_body '.
				'from pg_views '.
				'where schemaname=\'public\' and upper(viewname) = '.qt(strtoupper($view)).
				' order by 1';
	}


	public static function procedures()
	{
		return	'select lower(proname) as t_procedure '.
				'from pg_proc p join pg_namespace n on (p.pronamespace = n.oid) '.
				'where n.nspname=\'public\' '.
				'order by 1';
	}

	public static function procedure_parameters($procedure)
	{
		// t_parameter, t_parameter_type (input, output, input-output), t_data_type, i_length, i_prec, i_scale 

		return 'select lower(p.proargnames[s.i + 1]) as t_parameter, '.
				'case coalesce(proargmodes[s.i + 1], \'i\') '.
					'when \'i\' then \'input\' '.
					'when \'o\' then \'output\' '.
					'when \'b\' then \'input-output\' '.
					'else \'undefined\' '.
				'end as t_parameter_type, '.
				self::t_data_type().', '.
				self::i_type_length().', '.
				'NULL as i_prec, '.
				'NULL as i_scale '.
			'from pg_proc p '.
				'inner join (select * from sequence(30)) s (i) on (s.i < p.pronargs) '.
				'inner join pg_type tp on (case when p.proallargtypes is null then p.proargtypes[s.i] else p.proallargtypes[s.i + 1] end = tp.oid) '.
			'where upper(p.proname) = '.qt(strtoupper($procedure)).' order by s.i';
	}


	public static function procedure_body($procedure)
	{
		return	'select prosrc as t_procedure_body '.
				'from pg_proc p join pg_namespace n on (p.pronamespace = n.oid) '.
				'where n.nspname=\'public\' and upper(p.proname) = '.qt(strtoupper($procedure));
	}

}

?>