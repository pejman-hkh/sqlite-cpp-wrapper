#include "var.h"
#include "sqlite.h"

using namespace vars;

var kv( var a ) {
	return a.to_kv();
}

int main( int argc, char** argv  ) {
	//Connect to sqlite
	sqlite db;
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
}
