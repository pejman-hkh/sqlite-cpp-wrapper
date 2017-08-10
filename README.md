# PPP_sqlite-cpp-wrapper
PPP_sqlite is Sqlite3 & Sqlcipher c++ wrapper


# Usage

```c++
//Connect to sqlite
ppp_sqlite::sqlite db;
db.select("test");
std::string create_table = "CREATE TABLE test("  \
"id INTEGER PRIMARY KEY AUTOINCREMENT," \
"title TEXT NOT NULL," \
"date INT NOT NULL);";

if( db.statement( create_table ) ) {
//echo( "table created\r\n" );
}


//Fetch with accosiated array
var loop = db.sql("SELECT id, title FROM test ").find();
print_r( loop );


//Insert
var insert;
insert["title"] = "peji";
insert["date"] = "123456";
int id = db.table("test").insert( insert );


//Update
db.sql("UPDATE test SET title = ? WHERE id = ? ").exec( { "ppp", 1 } );

//Pagination
var loop1 = db.sql("SELECT id, title FROM test ").pagination(5,1).find();
print_r( db.loop_pagination );

//Fetch one record
var fetch = db.sql("SELECT id, title FROM test ").find_one();
print_r( fetch );



//Fetch one record with assign value
var fetch1 = db.sql("SELECT id, title FROM test WHERE id = ? ").find_one( { 1 } );
print_r( fetch1 );


//Fetch with assign value
var loop2 = db.sql("SELECT id, title FROM test WHERE title = ? ").find( { "test" } );
print_r( loop2 );


//Transaction test
var data;
data[0] = kv({ { "title" , "test ast" }, { "date", 12345 } });
data[1] = kv({ { "title" , "test1 ast" }, { "date", 12345 } });
data[2] = kv({ { "title" , "test2 ast" }, { "date", 12345 } });

db.begin().sql("INSERT INTO test(`title`, `date`) VALUES(?, ?)").bind_bulk( data );
db.commit();

echo( db.get_error() );
```

# C++ vars
See [C++ vars](https://github.com/pejman-hkh/cpp-vars) for var usage

# Sqlcipher
You can use set_key method for set key on your db file & lock it

# Building
```
g++ -std=c++11 main.cpp -o test -lsqlcipher
```
OR

```
g++ -std=c++11 main.cpp -o test -lsqlite3
```
# Mysql wrapper
see my mysql wrapper too :

[Mysql c++ wrapper](https://github.com/pejman-hkh/mysql-cpp-wrapper)

# Use it in 4php

You can use this sqlite wrapper in 4php

[4php](https://github.com/pejman-hkh/4php)
