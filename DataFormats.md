# Introduction #

Ubk own the concept of **abstract data format**: since we need to present data in the same way to the end-user without taking care of the format the database we're using memorizes data, well, we can do a step forward introducing types that possibly no database natively handles, as far as we can put them in a native database type.


# Database columns #

So, in a first approach we could lie on meta-data information to understand what kind of data we're working with; but when I had to manage some _smalldatetimes_ i came in serious problems due to the time-part management of _that_ database, so i choosed to memorize date and times as fixed, ordered strings: how could I then distinguish a simple string from a date ?

Ubk does it looking at the first character of the column name: with this trick, we get a strong-fixed way to write down column names that just adds clarity to the database structure, and the chance to forge new, abstract types for our needs.

Currently, Ubk implements these abstract/concrete types
  * **t**: the common string
  * **s**: a string always in upper case
  * **c**: a hashed string
  * **e**: an encoded string: it lies encoded on the db, but is reversible to clear text
  * **f**: a floating point
  * **i**: an integer
  * **b**: a boolean
  * **d**: a date (day, month, year)
  * **h**: a time (hour, minutes)
  * **r**: a timestamp (date + time)

These types lies over a concrete, standard-assumed implemented for the various databases with whom Ubk can work (for example, the boolean is implemented as a char(1) in firebird, the date as a char(8) in MSSQL, and so on).

Hashed and encoded strings are usually standard varchars, because the encoding is done in php - so we don't need a specific hash function in the database, mainly because it couldn't be available.

# Database tables #

Working with Ubk is easy if every table has a surrogate (auto-increment, sequence) for primary key - that doesn't mean you can forget UNIQUE constraints on the _real_ primary key. The best way to define table suitable for the framework is the following (assuming we're using Firebird):

```
create table genre(
i_genre_id integer not null primary key
,t_description varchar(50) not null
,unique(t_description)
);

create table artist(
i_artist_id integer not null primary key
,t_name varchar(50) not null
,t_surname varchar(50)
,d_born date
,unique(t_name, t_surname)
);

create table wish(
t_wish char(1) not null primary key
,t_description varchar(50) not null
,unique(t_description)
);

create table cd(
i_cd_id integer not null primary key
,t_title varchar(50) not null
,i_year integer
,i_artist_id integer not null references artist
,t_wish char(1) not null references wish on update cascade
,unique(i_artisti_id, t_title)
);

insert into wish values('H', 'Have');
insert into wish values('W', 'Want');
insert into wish values('N', 'Don''t care');

create generator g_genre;
create generator g_artist;
create generator g_cd;

-- write down triggers to set surrogates to the next sequence value

```