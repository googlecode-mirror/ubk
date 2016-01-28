# Introduction #

Ubk is a data-driven and data-oriented framework, meaning that data structure and data format, nevertheless data base interaction, are a key feature and focus of the project.

# From the database to the page #

To render pages, the framework concerns mainly about:
  * **data format**: the information should appear in the same way, whatever is the dbms laying under and the format it uses to internally store data; moreover, we can define logic data types without caring about the dbms (like reversible encrypted strings, and so on)
  * **data constraints**: the type, the length, the nullableness, the default value of the fields defined in the db should be taken in consideration when rendering fields
  * **data names**: a "well named" database structure should help us in building pages and in using referred tables

# From the page to the database #

The framework tries to free the programmer of the weight of writing down tons of SQL code, using informations to directly build SQL statemens:
  * **metadata**: they tell us how a table is formed
  * **post data**: they carry us the informations that the user has put on the web page
  * **get data**: they tell us what the user wants to do with the **post data**
So a flow is like:
  * the user tells the framework to do an update on the row _n_ of the table _t_ with the data on the page (tells via a framework-generated link)
  * the framework filters the post data wrt the structure of the table _t_
  * the framework builds up the correct SQL statement and executes it

# A database driver under ubk #

For the points above, when the framework first was developed, years ago, wasn't used a common library to interact with the db: the goal was not to easily write tons of sql code in an objective way, but simply avoid objective manipulation of the database, going straight, and in the straighter way, to the result: the SQL statement.

So now, a driver in the framework is formed by three classes:
  * _DB\_DRIVER

&lt;DBTYPE&gt;

**: this class wraps the php primitives of the specific database interaction, and defines the transformations between data representation in the database space and in the user space
  ***DB\_META_

&lt;DBTYPE&gt;

**: this one returns SQL-queries to investigate about database objects structure
  ***DB\_SQL

&lt;DBTYPE&gt;

**: this one has to build up queries from the get/post data, and expose methods to abstract logical SQL operators, to build WHERE clauses in a php-way, that is, without using the SQL directly, to let the applications be database independent**

All of the three classes above inherits from base abstract classes that implements the very common logic between different dbms.