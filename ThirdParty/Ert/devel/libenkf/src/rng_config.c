/*
   Copyright (C) 2011  Statoil ASA, Norway. 
    
   The file 'rng_config.c' is part of ERT - Ensemble based Reservoir Tool. 
    
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

#include <ert/util/mzran.h>
#include <ert/util/util.h>
#include <ert/util/rng.h>

#include <ert/config/config.h>

#include <ert/enkf/rng_config.h>
#include <ert/enkf/config_keys.h>
#include <ert/enkf/enkf_defaults.h>


struct rng_config_struct {
  rng_alg_type      type;
  char            * seed_load_file;    /* NULL: Do not store the seed. */
  char            * seed_store_file;   /* NULL: Do not load a seed from file. */
};



void rng_config_set_type( rng_config_type * rng_config , rng_alg_type type) {
  rng_config->type = type;
}

rng_alg_type rng_config_get_type(const rng_config_type * rng_config ) {
  return rng_config->type;
}

const char * rng_config_get_seed_load_file( const rng_config_type * rng_config ) {
  return rng_config->seed_load_file;
}

void rng_config_set_seed_load_file( rng_config_type * rng_config , const char * seed_load_file) {
  rng_config->seed_load_file = util_realloc_string_copy( rng_config->seed_load_file , seed_load_file);
}

const char * rng_config_get_seed_store_file( const rng_config_type * rng_config ) {
  return rng_config->seed_store_file;
}

void rng_config_set_seed_store_file( rng_config_type * rng_config , const char * seed_store_file) {
  rng_config->seed_store_file = util_realloc_string_copy( rng_config->seed_store_file , seed_store_file);
}


rng_config_type * rng_config_alloc( ) {
  rng_config_type * rng_config = util_malloc( sizeof * rng_config);

  rng_config_set_type( rng_config , MZRAN );  /* Only type ... */
  rng_config->seed_store_file = NULL;
  rng_config->seed_load_file = NULL;

  return rng_config;
}



void rng_config_free( rng_config_type * rng) {
  util_safe_free( rng->seed_load_file );
  util_safe_free( rng->seed_store_file );
  free( rng );
}

/*****************************************************************/

void rng_config_add_config_items( config_type * config ) {
  config_schema_item_type * item;

  item= config_add_schema_item( config , STORE_SEED_KEY , false , false );
  config_schema_item_set_argc_minmax(item , 1 , 1 , 1 , NULL );
  
  item = config_add_schema_item( config , LOAD_SEED_KEY , false , false );
  config_schema_item_set_argc_minmax(item , 1 , 1 , 1 , (const config_item_types [1]) { CONFIG_EXISTING_FILE});
}


void rng_config_init( rng_config_type * rng_config , config_type * config ) {
  if (config_item_set( config , STORE_SEED_KEY ))
    rng_config_set_seed_store_file( rng_config , config_iget(config , STORE_SEED_KEY ,0,0));

  if (config_item_set( config , LOAD_SEED_KEY ))
    rng_config_set_seed_load_file( rng_config , config_iget(config , LOAD_SEED_KEY ,0,0));
}


void rng_config_fprintf_config( rng_config_type * rng_config , FILE * stream ) {
  if (rng_config->seed_load_file != NULL) {
    fprintf( stream , CONFIG_KEY_FORMAT      , LOAD_SEED_KEY );
    fprintf( stream , CONFIG_ENDVALUE_FORMAT , rng_config->seed_load_file);
  }

  if (rng_config->seed_store_file != NULL) {
    fprintf( stream , CONFIG_KEY_FORMAT      , STORE_SEED_KEY );
    fprintf( stream , CONFIG_ENDVALUE_FORMAT , rng_config->seed_store_file);
  }
}
