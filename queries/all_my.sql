SELECT 
    Level,
    Role,
    Object_Name,
    DB_Name,
    Object_Type,
    Privileges,
    Is_SuperUser,
    rolinherit,
    rolcreaterole,
    rolcreatedb,
    rolcanlogin,
    memberof,
    rolbypassrls
FROM (
    -- Server Permissions (Global Privileges)
    SELECT 
        'Server_Permissions' AS Level,
        CONCAT(u.User, '@', u.Host) AS Role,
        'N/A' AS Object_Name,
        'N/A' AS DB_Name,
        'N/A' AS Object_Type,
        'N/A' AS Privileges,
        u.Super_priv AS Is_SuperUser,
        'N/A' AS rolinherit,
        u.Create_user_priv AS rolcreaterole,
        u.Create_priv AS rolcreatedb,
        (u.account_locked = 'N') AS rolcanlogin,
        (SELECT GROUP_CONCAT(CONCAT(r.from_user, '@', r.from_host) SEPARATOR ', ')
         FROM mysql.role_edges r
         WHERE r.to_user = u.User AND r.to_host = u.Host) AS memberof,
        'N/A' AS rolbypassrls
    FROM mysql.user u
    WHERE u.User NOT IN ('mysql.sys', 'mysql.session', 'mysql.infoschema', 'root')

    UNION ALL

    -- Database Ownership (Users with GRANT OPTION on a Database)
    SELECT 
        'DB_Ownership' AS Level,
        CONCAT(User, '@', Host) AS Role,
        'N/A' AS Object_Name,
        Db AS DB_Name,
        'N/A' AS Object_Type,
        'N/A' AS Privileges,
        'N/A' AS Is_SuperUser,
        'N/A' AS rolinherit,
        'N/A' AS rolcreaterole,
        'N/A' AS rolcreatedb,
        'N/A' AS rolcanlogin,
        'N/A' AS memberof,
        'N/A' AS rolbypassrls
    FROM mysql.db
    WHERE Grant_priv = 'Y'

    UNION ALL

    -- Schema Permissions (Database-Level Privileges)
    SELECT 
        'Database_Permissions' AS Level,
        CONCAT(User, '@', Host) AS Role,
        'N/A' AS Object_Name,
        Db AS DB_Name,
        'N/A' AS Object_Type,
        CONCAT_WS('+',
            NULLIF(CONCAT_WS('',
                IF(Create_priv = 'Y', 'Create,', ''),
                IF(Alter_priv = 'Y', 'Alter,', ''),
                IF(Drop_priv = 'Y', 'Drop,', ''),
                IF(Grant_priv = 'Y', 'Grant,', ''),
                IF(Insert_priv = 'Y', 'Insert,', ''),
                IF(Select_priv = 'Y', 'Select,', ''),
                IF(Update_priv = 'Y', 'Update,', ''),
                IF(Delete_priv = 'Y', 'Delete,', ''),
                IF(Index_priv = 'Y', 'Index,', ''),
                IF(Create_view_priv = 'Y', 'CreateView,', ''),
                IF(Show_view_priv = 'Y', 'ShowView,', '')
            ), '')
        ) AS Privileges,
        'N/A' AS Is_SuperUser,
        'N/A' AS rolinherit,
        'N/A' AS rolcreaterole,
        'N/A' AS rolcreatedb,
        'N/A' AS rolcanlogin,
        'N/A' AS memberof,
        'N/A' AS rolbypassrls
    FROM mysql.db
    WHERE Db NOT IN ('information_schema', 'mysql', 'performance_schema', 'sys')

    UNION ALL

    -- Table Ownership (Table-Level Privileges)
    SELECT 
        'Table_Permissions' AS Level,
        CONCAT(User, '@', Host) AS Role,
        Table_name AS Object_Name,
        Db AS DB_Name,
        'Table' AS Object_Type,
        GROUP_CONCAT(Table_priv SEPARATOR ', ') AS Privileges,
        'N/A' AS Is_SuperUser,
        'N/A' AS rolinherit,
        'N/A' AS rolcreaterole,
        'N/A' AS rolcreatedb,
        'N/A' AS rolcanlogin,
        'N/A' AS memberof,
        'N/A' AS rolbypassrls
    FROM mysql.tables_priv
    GROUP BY User, Host, Db, Table_name

    UNION ALL

    -- Object Permissions (Columns and Routines)
    -- Columns
    SELECT 
        'Object_Permissions' AS Level,
        CONCAT(User, '@', Host) AS Role,
        CONCAT(Table_name, '.', Column_name) AS Object_Name,
        Db AS DB_Name,
        'Column' AS Object_Type,
        GROUP_CONCAT(Column_priv SEPARATOR ', ') AS Privileges,
        'N/A' AS Is_SuperUser,
        'N/A' AS rolinherit,
        'N/A' AS rolcreaterole,
        'N/A' AS rolcreatedb,
        'N/A' AS rolcanlogin,
        'N/A' AS memberof,
        'N/A' AS rolbypassrls
    FROM mysql.columns_priv
    GROUP BY User, Host, Db, Table_name, Column_name

    UNION ALL

    -- Routines (Procedures/Functions)
    SELECT 
        'Object_Permissions' AS Level,
        CONCAT(User, '@', Host) AS Role,
        Routine_name AS Object_Name,
        Db AS DB_Name,
        Routine_type AS Object_Type,
        GROUP_CONCAT(Proc_priv SEPARATOR ', ') AS Privileges,
        'N/A' AS Is_SuperUser,
        'N/A' AS rolinherit,
        'N/A' AS rolcreaterole,
        'N/A' AS rolcreatedb,
        'N/A' AS rolcanlogin,
        'N/A' AS memberof,
        'N/A' AS rolbypassrls
    FROM mysql.procs_priv
    GROUP BY User, Host, Db, Routine_name, Routine_type
) AS combined_data
ORDER BY Level, Role;