/**************************************************************************************/
/**                                                                                \n**/
/**                 f  p  r  i  n  t  p  f  t  .  c                                \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function prints PFT variables                                              \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://gitlab.pik-potsdam.de/lpjml                                   \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

void fprintpft(FILE *file,    /**< pointer to text file */
               const Pft *pft /**< pointer to PFT variables */
              )
{
  fprintf(file,"PFT:\t\t%s\n",pft->par->name);
  fprintf(file,"fpc:\t\t%g\n",pft->fpc);
  fprintf(file,"nind:\t\t%g (1/m2)\n",pft->nind);
  fprintf(file,"wscal:\t\t%g\n",pft->wscal_mean);
  fprintf(file,"aphen:\t\t%g\n",pft->aphen);
  fprintf(file,"bminc:\t\t%g (gC/m2)\n",pft->bm_inc);
  fprintf(file,"gdd:\t\t%g\n",pft->gdd); 
  /* Print type specific variables */
  pft->par->fprint(file,pft);
} /* of 'fprintpft' */