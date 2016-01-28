## UBK:IF ##

This tag is an implementation of common
```
if (test) then
  <block>
[else
  <block>
] end if 
```
control structure.
The _else_ part is optional, while the _then_ and the _test_ are mandatory.
Into _then_ and _else_, you can put everything you want (until it's XML).

The tag can be used in two ways:
```
<ubk:if test="php_condition">
  <ubk:then>
  ...
  </ubk:then>
  <ubk:else>
  ...
  </ubk:else>
</ubk:if>
```
or
```
<ubk:if>
  <ubk:test>php_condition</ubk:test>
  <ubk:then>
  ...
  </ubk:then>
  <ubk:else>
  ...
  </ubk:else>
</ubk:if>
```
The two are really equivalent; i prefer the second just when the condition is long or needs a bit of computation.


_php\_condition_ is a php expression that evaluates to a boolean, so almost any expression; you can express it in basically two different ways:
```
<ubk:if test="isset($GLOBALS['myvar'])">
  <ubk:then>hello <b>world</b>!</ubk:then>
</ubk:if>
```
or
```
<ubk:if>
  <ubk:test>
    $n = count($GLOBALS['myarray']);
    return ($n &gt; 5 and $n &lt; 10);
  </ubk:test>
  <ubk:then>hello <b>world</b>!</ubk:then>
</ubk:if>
```
The point is that the framework adds _return_ and the ending semicolon if they are missing.


## UBK:WHEN ##

This very flexible tag implements a multi-control control structure, with two different behaviours based on the value of the boolean attribute _tutte_. When _tutte_ is true (default behaviour), the tag acts like this:
```
if (test_1) then
  <block>
[else
  <block>
] end if
[if (test_2) then
  <block>
[else
  <block>
] end if]
...
[if (test_n) then
  <block>
[else
  <block>
] end if]
```

When _tutte_ is false, instead:
```
if (test_1) then
  <block>
[else
  [<block>]
  if (test_2) then
    <block>
  [else
    [<block>]
  ... ] end if
] end if
```
that means:
```
go to next test
if _test_ is positive
  then display its _then_ block
  else
    if exists a corresponding _else_ block, display it
    restart iteration
```

Writing it down with a single, last _else_ block, with _tutte_ to false, it is equivalent to a
```
if (test_1) then
  <block>
else if (test_2) then
  <block>
...
else
  <block>
end if
```
structure.

Here are a couple of examples:
```

<ubk:when tutte="true">

  <ubk:test>canDo('bau')</ubk:test>
  <ubk:then>bau</ubk:then>
  <ubk:else>not a dog !</ubk:else>

  <ubk:test>canDo('miao')</ubk:test>
  <ubk:then>miao</ubk:then>
  <ubk:else>not a cat !</ubk:else>

  <ubk:test>canDo('snort')</ubk:test>
  <ubk:then>snort</ubk:then>
  <ubk:else>not a dragon !</ubk:else>

</ubk:when>


<ubk:when tutte="false">

  <ubk:test>$GLOBALS['n'] &lt; 10</ubk:test>
  <ubk:then>small</ubk:then>

  <ubk:test>$GLOBALS['n'] &lt; 20</ubk:test>
  <ubk:then>normal</ubk:then>

  <ubk:else>big</ubk:else>
</ubk:when>

<ubk:when tutte="false">

  <ubk:test>$GLOBALS['n'] &lt; 10</ubk:test>
  <ubk:then>small</ubk:then>
  <ubk:else>not small: i'm </ubk:else>

  <ubk:test>$GLOBALS['n'] &gt 10</ubk:test>
  <ubk:then>big</ubk:then>
  <ubk:else>ten</ubk:else>

</ubk:when>


```
Same rules as **UBK:IF** are valid for _test_, _then_ and _else_ sub-tags.

## UBK:SWITCH ##

This is an implementation of common _switch_ control structure: a sequence of cases with corresponding blocks to be written if the _case_ matches the _test_ expression, with an optional default (_else_) block when no _case_ is matched.

```
<ubk:switch>

  <ubk:test>$GLOBALS['myvar']</ubk:test>

  <ubk:case>'A'</ubk:case>
  <ubk:then>
    Hey, I'm the first ... and uppercase !
  </ubk:then>

  <ubk:case>'z'</ubk:case>
  <ubk:then>
    Hey, I'm the last ... and lowercase :(
  </ubk:then>

  <ubk:else>
    Unuseful middleware ...
  </ubk:else>

</ubk:switch>
```