## UBK:ITER ##

This tag acts much like a _for_ or _foreach_, over a range of numbers or over an array, specified in a variety of ways.

If used to loop over a range of numbers, two new environment variables are set:
  * ITER-START: the start value
  * ITER-CURRENT: the current value
and the attributes that the tag handles are the followings:
| attribute | required | default | domain | notes |
|:----------|:---------|:--------|:-------|:------|
| **inizio** | X        |         | int    | the start value |
| **fine**  | X        |         | int    | the end value |
| **step**  |          | 1       | int    | the increment for the index |
this modality is used if both **inizio** and **fine** are specified; no other attribute is considered.


If used to loop over an array, two new environment variables are set:
  * ITER-CURRENT-KEY: the index or the current key of the array
  * ITER-CURRENT: the current value
and the attributes that the tag handles are the followings:
| attribute | required | default | domain | notes |
|:----------|:---------|:--------|:-------|:------|
| **array** |          |         | string | the name of a global variable that actually contains the array to loop over |
| **dati**  |          |         | mixed  | any expression returning a string suitable to define an array (for example: dati="%implode(array(1,2,3,4,5))", dati="'one' => 1,'two' => 2") |
this modality is used if one of **array** or **dati** are specified; no other attribute is considered.

## UBK:RIGHE ##

This tag loops over the current result set, repeating its content for every tuple.
Some attributes can be specified to loop over just a part of the result set:

| attribute | required | default | domain | notes |
|:----------|:---------|:--------|:-------|:------|
| **page-size** | X        |         | int    | the number of records to display, starting from the first |
| **start** |          |         | int    | the record number to start from |
| **page**  |          | 0       | int    | the page number (assumed containing **page-size** record) to start from |
this modality is used if **page-size** is specified; if neither **start** nor **page** is specified, then the loop starts from the first record (like if **page** was set to 0).

## UBK:RIGHE-CORRELATE ##

This tag acts much like ubk:righe, but changing the result set to loop over, that is the set of tuple in another table somehow related to current tuple.

| attribute | required | default | domain | notes |
|:----------|:---------|:--------|:-------|:------|
| **tabella** | X        |         | string | the new table / view to loop over |
| **campo-id-tabella** |          | **tabella**'s primary key | string | the name of the field in **tabella** used as id |
| **campo-id-link** |          | **campo-id-tabella** | string | the name of the field in **tabella** used to find related tuples |
| **campo-id** |          | **campo-id-link** | string | the name of the field in current table used to find related tuples |
| **filtro** |          |         | SQL    | additional filter for **tabella**'s tuples |
| **ordine** |          |         | SQL    | additional sort clause for **tabella**'s tuples |
| **colonne** |          | | column|column[,column] | which columns should be fetched: specifying less columns in wide tables speeds up the process |
| **distinct** |          | FALSE   | FALSE, TRUE | should tuples be forced to be distinct |

A common and minimal use of this tag is the following:
```
<ubk:righe-correlate tabella="xxx" campo-id-link="i_yyy_id">
....
</ubk:righe-correlate>
```

When primary key in **tabella** and foreign key in current table have same name, you can just specify **tabella**.
Default filtering is done like this:
```
SELECT * FROM tabella WHERE campo-id-link = current_table[ campo-id ]
```
that usually is equivalent to
```
SELECT * FROM tabella WHERE primary-key = current_table[ tabella.primary-key ]
```

Some attributes can be specified to loop over just a part of the result set:

| attribute | required | default | domain | notes |
|:----------|:---------|:--------|:-------|:------|
| **page-size** | X        |         | int    | the number of records to display, starting from the first |
| **start** |          |         | int    | the record number to start from |
| **page**  |          | 0       | int    | the page number (assumed containing **page-size** record) to start from |
this modality is used if **page-size** is specified; if neither **start** nor **page** is specified, then the loop starts from the first record (like if **page** was set to 0).

## UBK:RIGHE-RICORSIVE ##

This tags loops over a table with tree structure, i.e. whos tuples contains a link to a "parent" tuple.

| attribute | required | default | domain | notes |
|:----------|:---------|:--------|:-------|:------|
| **tabella** | X        |         |        | the name of the database table containing values to display |
| **campo-id-figlio** |          | **tabella**'s id field |        | the name of the database field containing ids (primary key) |
| **campo-id-padre** | X        |         |        | the name of the database field containing the link to the parent node |
| **nodi-aperti** |          | null    | array  | _keys_ in this array contains ids of nodes whose children will be displayed; if no array is provided, all nodes will be printed; if an empty array is provided, only roots will be printed |
| **filtro** |          |         | SQL    | filter condition on root nodes (those having **campo-id-padre** NULL) |
| **filtro-generale** |          |         | SQL    | filter condition on internal nodes (those having **campo-id-padre** NOT NULL) |
| **ordine** |          | **campo-id-padre** asc | SQL    | sort clause for records; default clause is always pre-pended to any sort clause specified |
| **indentazione** | X        |         | int    | indentation amount for nested nodes, expressed in **um-ind** |
| **um-ind** |          | px      | px, em, pt, ... | CSS indentation unit measure |
| **incremento-ind** |          | 0       | int    | indentation increment for each level |
| **start-ind** |          | 0       | int    | starting value for indentation accumulator |
| **metodo-ind** |          | padding-left | padding-left, margin-left, ... | CSS indentation method |
| **child-display** |          | block   | block, inline, ... | CSS display value for node's children block |
| **tag**   |          | div     | HTML tag | tag used to build node and children blocks |
| **js-over** |          |         | js     | javascript to be executed on mouse over node |
| **js-out** |          |         | js     | javascript to be executed on mouse out node |
| **js-click** |          |         | js     | javascript to be executed on node click |
| **class** |          |         | string | the css class to be applied to the node |

During iteration, these environment variables are set:
  * RIGHE-RICORSIVE-LIVELLO: carries the nesting level, starting from 0 (root)
  * RIGHE-RICORSIVE-INDICE: a progressive index, incremented for every node printed out
  * RIGHE-RICORSIVE-FIGLI: the amount of current node's children


Initial filter _condition_ is built in this way:
  * if both **filtro** and **filtro-generale** are set, then _condition_ is : (**filtro** and **campo-id-padre** is null) or (**filtro-generale** and **campo-id-padre** is not null)
  * if only **filtro** is set, then _condition_ is: (**filtro** and **campo-id-padre** is null)
  * if only **filtro-generale** is set then _condition_ is **filtro-generale**

Then an additional _node condition_ , to build _whole filter condition_:
  * if at least **filtro** is set: (_condition_) and (**campo-id-padre** in **nodi-aperti**)
  * if **filtro-generale** is set: (_condition_) and ((**campo-id-padre** is null) or (**campo-id-padre** in **nodi-aperti**))
  * else: (**campo-id-padre** is null) or (**campo-id-padre** in **nodi-aperti**)

Seems a bit odd, but this ensure minimal tuple selection over the whole tree.