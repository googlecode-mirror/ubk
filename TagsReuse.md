## UBK:INCLUDE ##

This tag lets you include external files in another, as many times as you need, wherever you want, optionally with different parameters.

Once the file is first loaded, the object structure representing it is cached, so next calls will be faster.

Also, inclusion parameters are stacked (they remain valid for just the very evaluation of the call).

The tag has only one mandatory parameter, the file name to be included; all other parameters are pushed (and popped) in the evaluation environment.

| attribute | required | default | domain | notes |
|:----------|:---------|:--------|:-------|:------|
| **file**  | X        |         | string | the (existing) file to be included |
| **any**   |          |         | mixed  | new / replacing environment parameter |

Example: define row.xml
```
<tr>
    <td><ubk:campo nome="t_name"/></td>
    <td><ubk:campo nome="t_description"/></td>
</tr>
```
.. and use it in table.xml
```
<table>
    <thead>
        <th>Name</th>
        <th>Description</th>
    </thead>
    <tbody>
        <ubk:include file="row.xml" mode="W"/>
        <ubk:righe>
            <ubk:include file="row.xml" mode="RW"/>
        </ubk:righe>
    </tbody>
</table>
```

## UBK:SNIPPET ##

**UBK:SNIPPET** acts really like **UBK:INCLUDE**: difference is that snippets can be defined in the same xml source, so you don't need to put that piece of code in another file.

This lets you use **UBK:INCLUDE** to share xml blocks between different files, and **UBK:SNIPPET** to re-user xml blocks inside a file.

Obviously, you can define a library of snippets in a separate file, include it where you need, and use those shared snippets ....

This tag has two forms: the _definition_ and the _call_.
To define a snippet, just give it a name and put the xml code inside.
To call a snippet, just call its name, eventually with parameters (environment stacked like in **UBK:INCLUDE**).

Attributes used to define a snippet:
| attribute | required | default | domain | notes |
|:----------|:---------|:--------|:-------|:------|
| **nome**  | X        |         | string | the name to be given to the snippet |

Attributes used to call (evaluate) a snippet:
| attribute | required | default | domain | notes |
|:----------|:---------|:--------|:-------|:------|
| **call**  | X        |         | string | the name of the (existing) snippet to be evaluated |
| **any**   |          |         | mixed  | new / replacing environment parameter |

The example above, using a snippet, becomes

Example: define row.xml
```
<ubk:snippet nome="row">
  <tr>
    <td><ubk:campo nome="t_name"/></td>
    <td><ubk:campo nome="t_description"/></td>
  </tr>
</ubk:snippet>

<table>
    <thead>
        <th>Name</th>
        <th>Description</th>
    </thead>
    <tbody>
        <ubk:snippet call="row" mode="W"/>
        <ubk:righe>
            <ubk:snippet call="row" mode="RW"/>
        </ubk:righe>
    </tbody>
</table>
```