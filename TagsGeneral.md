## UBK:PHP ##

All the code inside the tag is treated as php code, and evaluated. The environment array **$parametri** is available.
```
<ubk:php>
$GLOBALS['myvar'] = my_smart_function($parametri['MODE']);
if (is_null($GLOBALS['myvar'])) my_log_error_function('Something wrong.');
</ubk:php>
```

## UBK:VAR ##

All the code inside the tag is treated as php expression, evaluated, and the result displayed. The environment array **$parametri** is available.

```
<ubk:var>$GLOBALS['myvar']</ubk:var>
<ubk:var>number_format($GLOBALS['myvar'], 2, '.', ',')</ubk:var>
```

## UBK:CONST ##

This tag has no content, just a required attribute named **nome**, that can contain one or more php constant names joined by the vertical pipe; each constant is displayed.
Useful (for me) to print out HTML entities in XML pages without relying on DTDs:
```
define('agrave', '&agrave;'); 
define('NBSP', '&nbsp;'); 

<ubk:const nome="agrave|NBSP|agrave"/>
```