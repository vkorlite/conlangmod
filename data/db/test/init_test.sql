create table Var(
    Name varchar(128) PRIMARY KEY NOT NULL,
    Possible varchar(256)
);

create table Var_depend(
    Name varchar(128) PRIMARY KEY NOT NULL,
    Val varchar(128) NOT NULL,
    Dependent varchar(128) NOT NUll
);
