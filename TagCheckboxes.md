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


## UBK:CHECKBOX ##

This one represents a checkbox.
When **mode** is R, the checkbox is displayed as:
  * a label, if attribute **etichette** is specified
  * an image, if attribute **img** is specified
  * a default label, else (Vero / Falso)

| attribute | required | default | domain | notes |
|:----------|:---------|:--------|:-------|:------|
| **nome**  | X        |         |        | the name of the database field |
| **mode**  |          | environment | R, W, RW | tells the framework how to show the field: as a label, as a blank field, as a field filled with current value. Also the name of the field is affected by this property |
| **usa-id** |          | TRUE if **mode** = RW, FALSE if **mode** = W | TRUE, FALSE | tells the framework which id should be used to identify the field in the page: when FALSE, -1 is used, else **valore-id** if exists, else the current row-id |
| **valore** |          | '1'     | mixed  | the value of the field |
| **valore-id** |          |         | mixed  | the id value to be used to identify the field |
| **multiplo** |          | FALSE   | TRUE, FALSE | if true, the field is not uniquely identified, but becomes part of an array |
| **js-name** |          | **nome** |        | base part of the final html _name_ attribute |
| **js-id** |          | **nome** |        | base part of the final html _id_ attribute |
| **diritti** |          | environment | array  | Hash of type [=> [right => bool](section.md)] containing current user rights. If no rights are presente, field is displayed in any case |
| **sezione** |          | environment | mixed  | section of user rights to be used to check right needed to display the field. If no section is present, field is displayed in any case|
| **diritto** |          | **mode** | R, W, RW, D | the right needed to display this field, checked against current environment rights |
| **class** |          | environment |        | the css class to be applied to the field; if **mode** is R, then the value is entagged in a _span_ with that class, if **img** attribute is not set; defaul class _check_ is always present |
| **etichette** |          |         | string|string | labels for true and false values respectively to be shown if **mode** == R |
| **img**   |          |         | string | prefix name of a couple of images lying under _img/_ dir, called **prefix** SI **DEF\_IMG\_EXT** and **prefix** NO **DEF\_IMG\_EXT** used to represent the checkbox if **mode** == R (for example img/chkSI.png, img/chkNO.png) |
| **valore-default** |          | database field default value |        | used only if **mode** is W |
| **tabindex** |          | environment | int    |       |
| **bloccato** |          | FALSE   | TRUE, FALSE | if true, the field's html attribute _disabled_ is set |
| **js-onclick** |          |         | js     | javascript code to be executed on click |

## UBK:CHECKBOX-IMG ##

This one represents a graphical checkbox: it needs four images, representing the two states (checked, not checked) in two modalities (editing, reading).
Those images should lie in a directory into _img/_, called for example _chk/_; then each image has for prefix the same name as the directory (_chk_), for suffix the four values _SI_, _NO_, _SIR_, _NOR_, for extension the one specified by **DEF\_IMG\_EXT**.

You can define tons of graphical checkboxes with different appearence, putting those four images under different dirs.

This class needs a javascript extension (CUbkCheckbox).

| attribute | required | default | domain | notes |
|:----------|:---------|:--------|:-------|:------|
| **nome**  | X        |         |        | the name of the database field |
| **mode**  |          | environment | R, W, RW | tells the framework how to show the field: as a label, as a blank field, as a field filled with current value. Also the name of the field is affected by this property |
| **usa-id** |          | TRUE if **mode** = RW, FALSE if **mode** = W | TRUE, FALSE | tells the framework which id should be used to identify the field in the page: when FALSE, -1 is used, else **valore-id** if exists, else the current row-id |
| **valore** |          | '1'     | mixed  | the value of the field |
| **valore-id** |          |         | mixed  | the id value to be used to identify the field |
| **multiplo** |          | FALSE   | TRUE, FALSE | if true, the field is not uniquely identified, but becomes part of an array |
| **js-name** |          | **nome** |        | base part of the final html _name_ attribute |
| **js-id** |          | **nome** |        | base part of the final html _id_ attribute |
| **diritti** |          | environment | array  | Hash of type [=> [right => bool](section.md)] containing current user rights. If no rights are presente, field is displayed in any case |
| **sezione** |          | environment | mixed  | section of user rights to be used to check right needed to display the field. If no section is present, field is displayed in any case|
| **diritto** |          | **mode** | R, W, RW, D | the right needed to display this field, checked against current environment rights |
| **class** |          | environment |        | the css class to be applied to the image; default class _check_ is always present |
| **img**   | X        |         | string | prefix name of images used to represent the checkbox |
| **alt**   | X        |         | string | alternate description for the checkbox-img |
| **valore-default** |          | database field default value |        | used only if **mode** is W |
| **tabindex** |          | environment | int    |       |
| **bloccato** |          | FALSE   | TRUE, FALSE | if true, the R version of images is used, and no javascript-change of the value is allowed |
| **js-onclick** |          |         | js     | javascript code to be executed on click |

## UBK:CHECKBOX-FK ##

This one represents a checkbox that displays the presence of a related value to the current row in another place (table).

| attribute | required | default | domain | notes |
|:----------|:---------|:--------|:-------|:------|
| **nome**  | X        |         |        | the name of the database field |
| **mode**  |          | environment | R, W, RW | tells the framework how to show the field: as a label, as a blank field, as a field filled with current value. Also the name of the field is affected by this property |
| **usa-id** |          | TRUE if **mode** = RW, FALSE if **mode** = W | TRUE, FALSE | tells the framework which id should be used to identify the field in the page: when FALSE, -1 is used, else **valore-id** if exists, else the current row-id |
| **valore** |          | '1'     | mixed  | the value of the field |
| **valore-id** |          |         | mixed  | the id value to be used to identify the field |
| **multiplo** |          | FALSE   | TRUE, FALSE | if true, the field is not uniquely identified, but becomes part of an array |
| **js-name** |          | **nome** |        | base part of the final html _name_ attribute |
| **js-id** |          | **nome** |        | base part of the final html _id_ attribute |
| **diritti** |          | environment | array  | Hash of type [=> [right => bool](section.md)] containing current user rights. If no rights are presente, field is displayed in any case |
| **sezione** |          | environment | mixed  | section of user rights to be used to check right needed to display the field. If no section is present, field is displayed in any case|
| **diritto** |          | **mode** | R, W, RW, D | the right needed to display this field, checked against current environment rights |
| **class** |          | environment |        | the css class to be applied to the image; default class _check_ is always present, eventually as a span entagging labels |
| **tabella** | X        |         | string | the table where the data should be searched |
| **campo-id-link** | X        | **nome** | string | the column of **table** where the data should be searched |
| **filtro** |          |         | SQL    | optional additional search condition (default and always present is **campo-id-link** = **current\_table**.**nome**) |
| **img**   |          |         | string | prefix name of a couple of images lying under _img/_ dir, called **prefix** SI **DEF\_IMG\_EXT** and **prefix** NO **DEF\_IMG\_EXT** used to represent the checkbox if **mode** == R (for example img/chkSI.png, img/chkNO.png) |
| **tabindex** |          | environment | int    |       |
| **bloccato** |          | FALSE   | TRUE, FALSE | if true, the field's html attribute disabled is set |