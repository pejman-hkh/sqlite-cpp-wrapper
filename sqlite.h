#pragma once
#include "var.h"
#include <cmath>
#define SQLITE_HAS_CODEC
#include <sqlcipher/sqlite3.h>

using namespace vars;

class sqlite {
private:

   sqlite3 *_db;
   int _rc;
   std::string _error;
   var _sql;
   std::string _table_name;
   sqlite3_stmt *_stmt;
   int _wilcard = 0;
   var _paginate;

   bool paginate( var data ) {
      int number = _paginate["n"].to_int();
      int page = _paginate["p"].to_int();
      _paginate.unset();

      if( ! number ) return false;

      var exp = _sql.explode("FROM");

      var temp_sql = _sql;

      var fetch = sql( "SELECT COUNT(*) as count FROM " + exp[exp.count()-1].string() ).find_one( data );

      int count = fetch["count"].to_int();

      int limit = 8;

      int n_p = std::ceil( (float)count / (float)number );

      int start = ( page - limit ) <= 0  ? 1 : page - limit ; 
      int end = ( page + limit >= n_p ) ? n_p : page + limit;


      int next = page >= std::ceil( (float)count / (float)number ) ? page : page + 1;
      int prev = page <= 1 ? 1 : page - 1;


      var pagin;
      for( int i = start; i <= end ; i++ )
      {
         pagin[ i ] = i;
      }
      
      loop_pagination["loop"] = pagin; 
      loop_pagination["count"] = count;
      loop_pagination["next"] = next;
      loop_pagination["prev"] = prev;

      _wilcard = 0;
      _sql = temp_sql+" LIMIT "+( page * number - number )+", "+number;


      return true;
   }

   bool exec_a() {
      if (sqlite3_step( _stmt ) != SQLITE_DONE) {
         _error = sqlite3_errmsg( _db );
         return false;         
      }

      sqlite3_finalize( _stmt ); 

      _wilcard = 0;

      return true;     
   }

   var get_column_name() {
      int cols = sqlite3_column_count( _stmt );
      var out;
      for ( int i = 0; i < cols; i++ ) {

         out[ sqlite3_column_name(_stmt, i) ] = sqlite3_column_decltype(_stmt, i);
 
      }


      return out;     
   }

   bool bind( var &data ) {
      if( prepare() ) {
         
         for( auto &x : data  ) {
            var &val = data[x];

            if( val.type() == VAR_INT ) {

               if( ! bind( val.to_int() ) ) {
                  return false;
               }

            } else if( val.type() == VAR_DOUBLE ) {

               if( ! bind( val.to_num() ) ) {
                  return false;
               }

            } else {
               if( ! bind( val.string() ) ) {
                  return false;
               }               
            }

         }

         return true;
      }

      return false;
   }

   bool prepare() {

      if ( sqlite3_prepare_v2( _db, _sql.string().c_str(), -1, &_stmt, 0) != SQLITE_OK ) {

         _error = sqlite3_errmsg( _db );
         _error += " \r\nerror in preparing\r\n ";

         return false;
      }

      return true;
   }

   bool bind( const double &val ) {
      ++_wilcard;
      if ( sqlite3_bind_double( _stmt, _wilcard, val ) != SQLITE_OK ) {
         _error = sqlite3_errmsg( _db );
         _error += "\r\nerror in bind double !\r\n ";
         return false;
      }
      return true;
   }


   bool bind( const int &val ) {

      ++_wilcard;

      if ( sqlite3_bind_int( _stmt, _wilcard, val ) != SQLITE_OK ) {
         _error = sqlite3_errmsg( _db );
         _error += "\r\nerror in bind int !\r\n ";
         return false;
      }
      return true;
   }

   bool bind( const std::string &val ) {
      ++_wilcard;

      if ( sqlite3_bind_text( _stmt, _wilcard, val.c_str(), val.length(), SQLITE_TRANSIENT ) != SQLITE_OK ) {
         _error = sqlite3_errmsg( _db );
         _error += "\r\nerror in bind string !\r\n "+_wilcard;
         return false;
      }
      return true;
   }

public:

   var loop_pagination;


   sqlite &begin() {
      statement("BEGIN TRANSACTION");
      return *this;
   }

   sqlite &commit() {
      statement("COMMIT");
      return *this;
   }

   bool bind_bulk( var &data ) {
      if( prepare() ) {
         
         for( auto &x : data  ) {
            _wilcard = 0;
            for( auto &d : data[x] ) {
               var &val = data[x][d];

   
               if( val.type() == VAR_INT ) {

                  if( ! bind( val.to_int() ) ) {
                     return false;
                  }

               } else if( val.type() == VAR_DOUBLE ) {

                  if( ! bind( val.to_num() ) ) {
                     return false;
                  }

               } else {
                  if( ! bind( val.string() ) ) {
                     return false;
                  }               
               }

            }

            sqlite3_step(_stmt);
            sqlite3_clear_bindings(_stmt);
            sqlite3_reset(_stmt);
            
         }

         sqlite3_finalize( _stmt ); 

         return true;
      }

      return false;
   }


   std::string type() {
      return "sqlite";
   }


   sqlite() : _stmt(0), _table_name("") {
   	
   }

   ~sqlite() {
    	sqlite3_close( _db );

   }

   std::string get_error() {
		return _error;
   }

   bool select( const std::string &table_name ) {

      if( sqlite3_open( (""+table_name+".db").c_str(), &_db)  != SQLITE_OK ) {

         _error = sqlite3_errmsg( _db );
         _error += "\r\nerror in open db file "+table_name;
         return false;

      } else {
         return true;
      }
   }

   bool set_key( const std::string &key ) {
      if ( sqlite3_key(_db, key.c_str(), key.length() ) != SQLITE_OK) {
         _error = sqlite3_errmsg( _db );
         _error += "\r\nerror in set key";
         return false;
      }

      return true;
   }
   

   bool statement( const std::string &sql ) {
      char *zErrMsg = 0;

      if( sqlite3_exec( _db, sql.c_str(), 0, 0, &zErrMsg ) != SQLITE_OK ) {

         _error = zErrMsg;
         _error += "\r\nerror in statement !"+ sql;

         sqlite3_free( zErrMsg );

         return false;
      } else {
         return true;
      }
      
   }
   
   sqlite &pagination( const int &number, const int &page ) {

      _paginate["n"] = number;
      _paginate["p"] = page;

      return *this;
   }

   sqlite &table( const std::string &table_name ) {
      _wilcard = 0;
      _table_name = table_name;
      return *this;
   }

   sqlite &sql( const std::string &sql ) {

      _wilcard = 0;
      _sql = sql;
      loop_pagination = "";

      return *this;
   }

   var find( var data = "" ) {
      _wilcard = 0;

      if( _paginate.count() > 0 ) {
         paginate( data );
      }

      if( bind( data ) ) {

         var columns = get_column_name();

         var ret;
         int j = 0;
         while( sqlite3_step( _stmt ) == SQLITE_ROW ) {

            var data;
            int i = 0;
            for( auto &key : columns ) {
               var &value = columns[ key ];

               if( value == "TEXT" ) {
                  data[ key ] = sqlite3_column_text( _stmt, i );
               } else if( value == "INT" ) {
                  data[ key ] = sqlite3_column_int( _stmt, i );
               } else if( value.string().find("CHAR") != std::string::npos ) {
                  data[ key ] = sqlite3_column_text( _stmt, i );
               } else if( value == "REAL" ) {
                  data[ key ] = sqlite3_column_double( _stmt, i );
               } else {
                  data[ key ] = sqlite3_column_int( _stmt, i );
               }
               i++;
            }
            ret[ j ] = data;
            data.unset();
            j++;
         }

         sqlite3_finalize( _stmt ); 
         return ret;
      }
      return {};
   }

   var find_one( var data = "" ) {
      _sql += " LIMIT 0,1";
      return find( data )[0];
   }

   int insert( var data ) {
      _wilcard = 0;
      var colum_name;
      var values;
      for( int i = 0; i < data.count(); i++ ) {
         colum_name[i] = (var)"`"+data.key( i )+(var)"`";
         values[i] = "?";
      }

      std::string column_str = colum_name.implode(",").string();
      std::string values_str = values.implode(",").string();

      _sql = "INSERT INTO `"+_table_name+"`("+column_str+") VALUES("+values_str+")";
   
      _table_name = "";

      if ( bind( data ) ) {
         if( exec_a() ) {
            return sqlite3_last_insert_rowid( _db );
         }

      } else {
         return 0;
      }

   }

   bool exec( var data = "" ) {
      _wilcard = 0;
      if ( bind(  data ) ) {
         if( exec_a() ) {
            return true;
         }
      }

      return false;
   }



};
