/*
   Copyright (C) 2011  Statoil ASA, Norway. 
   The file 'summary2csv.c' is part of ERT - Ensemble based Reservoir Tool. 
    
   ERT is free software: you can redistribute it and/or modify 
   it under the terms of the GNU General Public License as published by 
   the Free Software Foundation, either version 3 of the License, or 
   (at your option) any later version. 
    
   ERT is distributed in the hope that it will be useful, but WITHOUT ANY 
   WARRANTY; without even the implied warranty of MERCHANTABILITY or 
   FITNESS FOR A PARTICULAR PURPOSE.   
    
   See the GNU General Public License at <http://www.gnu.org/licenses/gpl.html> 
   for more details. 
*/

#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <stdbool.h>

#include <ert/util/util.h>
#include <ert/util/stringlist.h>

#include <ert/ecl/ecl_sum.h>




static void build_key_list( const ecl_sum_type * ecl_sum , stringlist_type * key_list ) {
  int iw;
  int last_step = ecl_sum_get_data_length( ecl_sum ) - 1;
  stringlist_type * well_list = ecl_sum_alloc_well_list( ecl_sum , NULL );
  for (iw = 0; iw < stringlist_get_size( well_list ); iw++) {
    const char * well = stringlist_iget( well_list , iw );
    char * wopt_key = ecl_sum_alloc_well_key( ecl_sum  , "WOPR", well);
    if (ecl_sum_has_key( ecl_sum , wopt_key) && (ecl_sum_get_well_var( ecl_sum , last_step , well , "WOPT") > 0 )) {
      /* 
         We add all the keys unconditionally here; and then let the
         ecl_sum_fprintf() function print a message on stderr if it is
         missing. 
      */
      stringlist_append_owned_ref( key_list , ecl_sum_alloc_well_key( ecl_sum  , "WOPR", well) );
      stringlist_append_owned_ref( key_list , ecl_sum_alloc_well_key( ecl_sum  , "WOPT", well) );

      stringlist_append_owned_ref( key_list , ecl_sum_alloc_well_key( ecl_sum  , "WGPR", well) );
      stringlist_append_owned_ref( key_list , ecl_sum_alloc_well_key( ecl_sum  , "WGPT", well) );

      stringlist_append_owned_ref( key_list , ecl_sum_alloc_well_key( ecl_sum  , "WWPR", well) );
      stringlist_append_owned_ref( key_list , ecl_sum_alloc_well_key( ecl_sum  , "WWPT", well) );
    } else
      printf("Ignoring well: %s \n",well);
    free( wopt_key );
  }
  stringlist_free( well_list );
}


int main(int argc , char ** argv) {
  {
    ecl_sum_fmt_type fmt;
    bool           include_restart = true;
    int            arg_offset      = 1;  
    
    if (argc != 2) {
      printf("You must supply the name of a case as:\n\n   summary2csv.exe  ECLIPSE_CASE\n\nThe case can optionally contain a leading path component.\n");
      exit(1);
    }

    {
      char         * data_file = argv[arg_offset];
      ecl_sum_type * ecl_sum;
    
      ecl_sum_fmt_init_csv( &fmt );
      ecl_sum = ecl_sum_fread_alloc_case__( data_file , ":" , include_restart);
      if (ecl_sum != NULL) {
        stringlist_type * key_list = stringlist_alloc_new( );
        build_key_list( ecl_sum , key_list );
        {
          char * csv_file = util_alloc_filename( NULL , ecl_sum_get_base(ecl_sum) , "txt");  // Weill save to current path; can use ecl_sum_get_path() to save to target path instead.
          FILE * stream = util_fopen( csv_file , "w");
          ecl_sum_fprintf(ecl_sum , stream , key_list , false , &fmt);
          fclose( stream );
          free( csv_file );
        }     
        stringlist_free( key_list );
        ecl_sum_free(ecl_sum);
      } else 
        fprintf(stderr,"summary2csv: No summary data found for case:%s\n", data_file );
    }
  }
}
