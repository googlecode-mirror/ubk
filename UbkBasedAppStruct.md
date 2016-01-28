# Introduction #

Here we give an overview about the directory structure and the base files of a ubk-based application.


# Tree and Files #

  * **params.inc**: this file has to parse the .ini (see below) and should be included first in every php file that contains the logic of the application
  * /**system**
    * **params.ini**: contains definitions of base parameters used by the app to connect to the database
    * **errore.xml**: the file used to display an error (those manageable by _set\_error\_handler_), parsed by the default error handler (a default file will be used if missing)
    * **errore-exc.xml**: the file used to display an unhandled exception, parsed by the default exception handler (a default file will be used if missing)
    * /**xsl**: contains app-specific xslt templates files, used to pre-transform the xml page definitions
      * **transform.xsl**: the file searched by the framework, used to include all app-specific xslt files; path here are relative to the framework's root, you can use the keyword **OWN** as a placemarker for the phisical directory of the app
    * /**log**: here the framework stores error, notices, exceptions, and so on, divided by client IP; this happens 'cos of the default error and exception handlers. The directory is created automatically if it doesn't exists
    * /**cache**:
      * /**metadata**: here are stored the metadata informations about tables, views, stored procedures
      * /**xml**: here lies the parsed, objectized version of the xml page definitions

# Parameters #
Parameters used by the framework, usually defined in params.ini
  * **TAB\_STRIP**: used to predict the table id field from the table name: ubk works when a table named _my\_table_ has an id named _i\_my\_table\_id_, usually i categorize table with a 4-digits prefix, so i set **TAB\_STRIP** to 4
  * **CACHE\_METADATI**: boolean, if the framework should cache metadata (default 0)
  * **CACHE\_XSL**: boolean, if the framework should cache the bundle of xsl transformations (default 0)
  * **CACHE\_DB**: boolean, if the framework should cache query results (default 0, functionality not working)
  * **CACHE\_XML**: boolean, if the framework should cache transformed xml page defitions (default 0)
  * **STAR**: boolean, if the framework should show an asterisk beside required fields (default 0)
  * **DEF\_IMG\_EXT**: which is the default extension for rollovered images (deprecated, default _.gif_)
  * **LOG\_STATS**: boolean, if the framework should log performance about the page transformation process (default 0)
  * **LOG\_STATS**: boolean, if the framework should use a checksum validation of his generated links to avoid misuses or abuses (default 0, not working with ajax tags)
  * **TIMEZONE**: suitable timezone referring to _date\_default\_timezone\_set_
  * **UBK\_ROOT**: the phisical directory where the framework lies
  * **PHDIR**: the phisical directory where the application lies
  * **VDIR**: the virtual directory under which the application is web-accessible
  * **MATCH**: if present in a section of the .ini file, all parameters under this section are parsed only if the value of **MATCH** matches with the 

&lt;host&gt;

:

&lt;port&gt;

 from where the http request comes - so you can set different parameters according from the provenience, i.e. the virtual host configuration of the http server

Other parameters defines the database connections: we can manage multiple database connections in an applications, even if usually we will use just one.
Every database connection lies in a **[DB\_something ](.md)** section of the ini files, defining these parameters:
  * **DB\_TYPE**: the ubk-type of the database; valid values are FBIRD, MSSQL, ODBC, MYSQL
  * **DB\_SERVER**: the host where the dbms runs
  * **DB\_NAME**: the name of the database
  * **DB\_USERNAME**
  * **DB\_PASSWORD**
If not specified else, the framework always tries to use the connection defined in section **[DB\_DEFAULT ](.md)**