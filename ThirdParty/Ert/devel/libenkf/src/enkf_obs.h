/*
   Copyright (C) 2011  Statoil ASA, Norway. 
    
   The file 'enkf_obs.h' is part of ERT - Ensemble based Reservoir Tool. 
    
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

#ifndef __ENKF_OBS_H__
#define __ENKF_OBS_H__
#ifdef __cplusplus
extern "C" {
#endif
#include <time.h>

#include <hash.h>
#include <stringlist.h>

#include <history.h>

#include <ecl_sum.h>

#include <enkf_fs.h>
#include <enkf_state.h>
#include <meas_data.h>
#include <obs_data.h>
#include <obs_vector.h>
#include <local_obsset.h>
#include <enkf_types.h>

  bool            enkf_obs_have_obs( const enkf_obs_type * enkf_obs );
  const char    * enkf_obs_get_config_file( const enkf_obs_type * enkf_obs);
  enkf_obs_type * enkf_obs_alloc( const history_type * hist, double std_cutoff );
  
  void            enkf_obs_free(  enkf_obs_type * enkf_obs);
  
  obs_vector_type * enkf_obs_get_vector(const enkf_obs_type * , const char * );
  
  void              enkf_obs_load(enkf_obs_type * enkf_obs,
                                  const char           * config_file,
                                  const ecl_grid_type  * grid , 
                                  const ecl_sum_type   * refcase , 
                                  ensemble_config_type * ensemble_config);
  
  void            enkf_obs_reload( enkf_obs_type * enkf_obs , const ecl_grid_type * grid , const ecl_sum_type * refcase , ensemble_config_type * ensemble_config );                                
  
  void enkf_obs_get_obs_and_measure(
                                    const enkf_obs_type    * enkf_obs,
                                    enkf_fs_type           * fs,
                                    const int_vector_type  * step_list , 
                                    state_enum               state,
                                    int                      ens_size,
                                    const enkf_state_type ** ensemble ,
                                    meas_data_type         * meas_data,
                                    obs_data_type          * obs_data,
                                    const local_obsset_type * obsset);
  
  
  stringlist_type * enkf_obs_alloc_typed_keylist( enkf_obs_type * enkf_obs , obs_impl_type );
  hash_type * enkf_obs_alloc_data_map(enkf_obs_type * enkf_obs);
  
  const obs_vector_type * enkf_obs_user_get_vector(const enkf_obs_type * obs , const char  * full_key, char ** index_key );
  bool              enkf_obs_has_key(const enkf_obs_type * , const char * );
  
  hash_iter_type  * enkf_obs_alloc_iter( const enkf_obs_type * enkf_obs );
  
  stringlist_type * enkf_obs_alloc_matching_keylist(const enkf_obs_type * enkf_obs , const char * input_string);
  time_t            enkf_obs_iget_obs_time(enkf_obs_type * enkf_obs , int report_step);
  void              enkf_obs_fprintf_config( const enkf_obs_type * enkf_obs , FILE * stream);
  
#ifdef __cplusplus
}
#endif
#endif