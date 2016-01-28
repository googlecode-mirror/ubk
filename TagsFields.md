# Introduction #

All fields share the same logic for building _name_ and _id_ attributes:
  * default behaviour is having the two values equal to 

&lt;db-name&gt;

[

&lt;row-id&gt;

]*** when**multiplo**attribute is set, then _name_ becomes**

&lt;db-name&gt;

[.md](.md)**, while _id_ keeps default behaviour
  * when**js-name**is set, then _name_ becomes**

&lt;js-name&gt;

[

&lt;row-id&gt;

]**(or**

&lt;js-name&gt;

[.md](.md)**)
  * when**js-id**is set, then _id_ becomes**

&lt;js-id&gt;

[

&lt;row-id&gt;

]



&lt;row-id&gt;

**itself is computed this way:
  * default behaviour is**-1**for new fields (**usa-id**FALSE) and**database row id**for existing fields (**usa-id**TRUE)
  * when**valore-id**is set and**usa-id**TRUE, then the value is the**valore-id**'s value**


## UBK:CAMPO ##

This one represents a generic text field - string, date, time, timestamp, and numeric values.
Javascript controls to ensure correct data format and length are automatically generated on focus loose.

| attribute | required | default | domain | notes |
|:----------|:---------|:--------|:-------|:------|
| **nome**  | X        |         |        | the name of the database field |
| **mode**  |          | environment | R, W, RW | tells the framework how to show the field: as a label, as a blank field, as a field filled with current value. Also the name of the field is affected by this property |
| **usa-id** |          | TRUE if **mode** = RW, FALSE if **mode** = W | TRUE, FALSE | tells the framework which id should be used to identify the field in the page: when FALSE, -1 is used, else **valore-id** if exists, else the current row-id |
| **valore-id** |          |         | mixed  | the id value to be used to identify the field |
| **multiplo** |          | FALSE   | TRUE, FALSE | if true, the field is not uniquely identified, but becomes part of an array |
| **js-name** |          | **nome** |        | base part of the final html _name_ attribute |
| **js-id** |          | **nome** |        | base part of the final html _id_ attribute |
| **diritti** |          | environment | array  | Hash of type [=> [right => bool](section.md)] containing current user rights. If no rights are presente, field is displayed in any case |
| **sezione** |          | environment | mixed  | section of user rights to be used to check right needed to display the field. If no section is present, field is displayed in any case|
| **diritto** |          | **mode** | R, W, RW, D | the right needed to display this field, checked against current environment rights |
| **tipo**  |          | text    | text, hidden, password, radio, textarea | type of the field |
| **class** |          | environment |        | the css class to be applied to the field; if **mode** is R, then the value is entagged in a _span_ with that class |
| **valore-default** |          | database field default value |        | used only if **mode** is W |
| **se-null** |          |         |        | value to be displayed if field IS NULL; used only if **mode** is R |
| **formato** |          | _nl2br_ if **tipo** is TEXTAREA |        | specifies the name of a php function of type _mixed_ => _mixed_ that is applyed to database / **se-null** value to format it (before the eventual _span_/**class** entagging); used in every display **mode** |
| **trim**  |          |         | int    | maximum string characters to display (ellipsis added after); used only if **mode** = R |
| **maxlength** |          | database field length | int    | _maxlength_ html attribute value |
| **size**  |          | 2,50 for **tipo** = TEXTAREA, 12 for date fields, 5 for time fields, 6 for integers and floating points | int, (int,int), DB | the _size_ html attribute value, or values for _rows_ and _cols_ respectively if **tipo** = TEXTAREA; when DB then the database field length is used |
| **tabindex** |          | environment | int    |       |
| **align** |          | _right_ for date, time and numeric fields, _left_ otherwise | right, left, center | alignment of field content |
| **bloccato** |          | FALSE   | TRUE, FALSE | if true, the field's html attribute _disabled_ is set |
| **js-onblur** |          |         | js     | javascript code to be executed on focus loose, after default type-checks |
| **js-onfocus** |          |         | js     | javascript code to be executed on focus gain |
| **js-onchange** |          |         | js     | javascript code to be executed on field value change |
| **secolo** |          | 20      | int    | the century, used to check date soundness; used only on date fields |
| **richiesto** |          | FALSE   | TRUE, FALSE | if TRUE, forces the field to be required on data submit and displays an asterisk after the field; if the global parameter **STAR** is set, then the asterisk is entagged in a _span_ of class _star_ |

## UBK:CAMPO-MAIL ##

A simple extension to **UBK:CAMPO**: when in R **mode**, if **tipo** is different from _hidden_ and _password_, it displays itself as a _mailto:_ link to its value; in W or RW **mode**, it checks correct e-mail format for its value.

## UBK:COMBO ##

This one represents a select or radio group, whose values are taken from an other database table.

| attribute | required | default | domain | notes |
|:----------|:---------|:--------|:-------|:------|
| **tabella** | X        |         |        | the name of the database table containing values to display / select |
| **campo-id-tabella** |          | **tabella**'s id field |        | the name of the database field containing ids (primary key) |
| **nome**  |          | **campo-id-tabella** |        | the name of the database field in the source table (foreign key) |
| **campi** | X        |         | string[|string..] | sequence of strings representing **tabella**'s fields and separators, alternatively (example: t\_name| - |t\_description) |
| **filtro** |          |         | SQL    | filter condition over values in **tabella** |
| **ordine** |          | **campi**[0 ](.md) | SQL    | sort condition |
| **distinct** |          | FALSE   | FALSE, TRUE | tells the framework if values in the select should be forced to be unique |
| **mode**  |          | environment | R, W, RW | tells the framework how to show the field: as a label, as a blank field, as a field filled with current value. Also the name of the field is affected by this property |
| **usa-id** |          | TRUE if **mode** = RW, FALSE if **mode** = W | TRUE, FALSE | tells the framework which id should be used to identify the field in the page: when FALSE, -1 is used, else **valore-id** if exists, else the current row-id |
| **valore-id** |          |         | mixed  | the id value to be used to identify the field; if **mode** == R, then it's the value used as source, regardless of the current row value for **nome** |
| **multiplo** |          | FALSE   | TRUE, FALSE | if true, the field is not uniquely identified, but becomes part of an array |
| **js-name** |          | **nome** |        | base part of the final html _name_ attribute |
| **js-id** |          | **nome** |        | base part of the final html _id_ attribute |
| **diritti** |          | environment | array  | Hash of type [=> [right => bool](section.md)] containing current user rights. If no rights are presente, field is displayed in any case |
| **sezione** |          | environment | mixed  | section of user rights to be used to check right needed to display the field. If no section is present, field is displayed in any case|
| **diritto** |          | **mode** | R, W, RW, D | the right needed to display this field, checked against current environment rights |
| **tipo**  |          | combo   | combo,radio | type of the field |
| **riga-vuota** |          | TRUE    | FALSE, TRUE | if FALSE and **tipo** == COMBO, no empty option is displayed (empty option is displayed by default even if field is required) |
| **class** |          | environment |        | the css class to be applied to the field; if **mode** is R, then the value is entagged in a _span_ with that class |
| **valore-default** |          | database field default value |        | used only if **mode** is W |
| **se-null** |          |         |        | value to be displayed if field IS NULL; used only if **mode** is R |
| **formato** |          |         |        | specifies the name of a php function of type _mixed_ => _mixed_ that is applyed to database / **se-null** value to format it (before the eventual _span_/**class** entagging); used in every display **mode** |
| **trim**  |          |         | int    | maximum string characters to display (ellipsis added after); used in every display **mode** |
| **tabindex** |          | environment | int    |       |
| **js-onclick** |          |         | js     | javascript code to be executed on field value change |
| **richiesto** |          | FALSE   | TRUE, FALSE | if TRUE, forces the field to be required on data submit and displays an asterisk after the field; if the global parameter **STAR** is set, then the asterisk is entagged in a _span_ of class _star_ |