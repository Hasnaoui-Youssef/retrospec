--list tables
select table_name
from information_schema.tables;

--list columns
select table_name, column_name
from information_schema.columns;
