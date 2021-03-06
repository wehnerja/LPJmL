/**************************************************************************************/
/**                                                                                \n**/
/**                 o  p  e  n  r  e  s  t  a  r  t  .  c                          \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function opens restart file and seeks to first grid cell as                \n**/
/**     specified in LPJ configuration                                             \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

FILE *openrestart(const char *filename, /**< filename of restart file */
                  Config *config,       /**< LPJ configuration */
                  int ntotpft,          /**< Total number of PFTs */
                  Bool *swap            /**< Byte order has to be changed */
                 )                      /** \return file pointer or NULL */
{
  FILE *file;
  Header header;
  Restartheader restartheader;
  int offset,version;
  long long offsetl;
  /* Open file */
  file=fopen(filename,"rb");
  if(file==NULL)
  {
    if(isroot(*config))
      printfopenerr(filename);
    return NULL;
  }
  /* Read restart header */
  version=READ_VERSION;
  if(freadheader(file,&header,swap,RESTART_HEADER,&version))
  {
    if(isroot(*config))
      fprintf(stderr,"ERROR154: Invalid header in '%s'.\n",filename);
    fclose(file);
    return NULL;
  }
  if(version!=RESTART_VERSION)
  {
    if(isroot(*config))
      fprintf(stderr,"ERROR154: Invalid version %d in '%s'.\n",
              version,filename);
    fclose(file);
    return NULL;
  }
  if(header.cellsize_lon!=config->resolution.lon)
  {
    if(isroot(*config))
      fprintf(stderr,"ERROR154: Cell size longitude %g different from %g in '%s'.\n",
              header.cellsize_lon,config->resolution.lon,filename);
    fclose(file);
    return NULL;
  }
  if(header.cellsize_lat!=config->resolution.lat)
  {
    if(isroot(*config))
      fprintf(stderr,"ERROR154: Cell size latitude %g different from %g in '%s'.\n",
              header.cellsize_lat,config->resolution.lat,filename);
    fclose(file);
    return NULL;
  }
  if(freadrestartheader(file,&restartheader,*swap))
  {
    if(isroot(*config))
      fprintf(stderr,"ERROR154: Invalid restart header in '%s'.\n",filename);
    fclose(file);
    return NULL;
  }
  if(restartheader.landuse && (config->withlanduse==NO_LANDUSE))
  {
    if(isroot(*config))
      fputs("ERROR180: Land-use setting is different in restart file.\n",stderr);
    fclose(file);
    return NULL;
  }
  config->landuse_restart=restartheader.landuse;
  if(restartheader.river_routing!=config->river_routing)
  {
    if(isroot(*config))
      fputs("ERROR181: River-routing setting is different in restart file.\n",stderr);
    fclose(file);
    return NULL;
  }
  config->sdate_option_restart=restartheader.sdate_option;
  if(isroot(*config) && restartheader.sdate_option==NO_FIXED_SDATE && config->sdate_option>NO_FIXED_SDATE)
    fputs("WARNING024: Sowing dates are missing in restart file.\n",stderr);
  if((sizeof(Real)==sizeof(float)  && header.datatype!=LPJ_FLOAT) ||
     (sizeof(Real)==sizeof(double) && header.datatype!=LPJ_DOUBLE))
  {
    if(isroot(*config))
      fputs("ERROR182: Real datatype does not match in restart file.\n",stderr);
    fclose(file);
    return NULL;
  }
  if(header.nbands!=ntotpft)
  {
    if(isroot(*config))
      fprintf(stderr,
              "ERROR183: Number of PFTs=%d does not match %d in restart file.\n",header.nbands,ntotpft);
    fclose(file);
    return NULL;
  }
  if(config->nspinup==0 && header.firstyear!=config->firstyear-1 &&
     isroot(*config))
    fprintf(stderr,
            "WARNING005: Year of restartfile=%d not equal start year=%d-1.\n",
            header.firstyear,config->firstyear);

  offset=config->startgrid-header.firstcell;
  if(offset<0)
  {
    fprintf(stderr,
            "WARNING006: First grid cell not in restart file, set to %d.\n",
            header.firstcell);
    config->startgrid=header.firstcell;
    offset=0;
  }
  if(offset>0)
  {
    if(offset>=header.ncell)
    {
      fputs("ERROR155: First grid cell not in restart file.\n",stderr);
      fclose(file);
      return NULL;
    }
    fseek(file,offset*sizeof(long long),SEEK_CUR);
  }
  /* read index from file */
  freadlong1(&offsetl,*swap,file);
  /* skip to index */
  if(fseek(file,offsetl,SEEK_SET))
  {
    fputs("ERROR156: Cannot seek to offset in restart file.\n",stderr);
    fclose(file);
    return NULL;
  }
  if(header.ncell<config->ngridcell)
  {
    fprintf(stderr,
            "WARNING007: Restart file too short, grid truncated to %d.\n",
            header.ncell);
    config->ngridcell=header.ncell;
  }

  return file;
} /* of 'openrestart' */
