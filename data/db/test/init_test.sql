create table Var(
    Name varchar(128) PRIMARY KEY NOT NULL,
    Morph varchar(128),
    Possible varchar(256)
);

create table Var_depend(
    Name varchar(128) PRIMARY KEY NOT NULL,
    Val varchar(128) NOT NULL,
    Dependent varchar(128) NOT NUll
);
