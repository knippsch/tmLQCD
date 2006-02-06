/* $Id$ */

#ifdef HAVE_CONFIG_H
# include<config.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#ifdef MPI
# include <mpi.h>
#endif
#include "global.h"
#include "mpi_init.h"

#ifdef MPI
/* Datatypes for the data exchange */
MPI_Datatype gauge_point;
MPI_Datatype gauge_time_slice_cont;
MPI_Datatype gauge_time_slice_split;
MPI_Datatype deri_point;
MPI_Datatype deri_time_slice_cont;
MPI_Datatype deri_time_slice_split;

MPI_Datatype field_point;
MPI_Datatype field_time_slice_cont;
MPI_Datatype gauge_x_slice_cont;
MPI_Datatype gauge_x_subslice;
MPI_Datatype gauge_x_eo_subslice;
MPI_Datatype gauge_x_slice_gath;
MPI_Datatype gauge_x_slice_gath_split;
MPI_Datatype field_x_slice_cont;
MPI_Datatype field_x_subslice;
MPI_Datatype field_x_slice_gath;
MPI_Datatype deri_x_slice_cont;
MPI_Datatype deri_x_subslice;
MPI_Datatype deri_x_eo_subslice;
MPI_Datatype deri_x_slice_gath;
MPI_Datatype deri_x_slice_gath_split;
MPI_Datatype gauge_xt_edge_cont;
MPI_Datatype gauge_xt_edge_gath;
MPI_Datatype gauge_xt_edge_gath_split;

MPI_Datatype gauge_y_slice_gath;
MPI_Datatype gauge_y_slice_cont;
MPI_Datatype gauge_y_subslice;
MPI_Datatype gauge_y_eo_subslice;
MPI_Datatype gauge_y_slice_gath_split;

MPI_Datatype field_y_slice_gath;
MPI_Datatype field_y_slice_cont;
MPI_Datatype field_y_subslice;

MPI_Datatype field_z_slice_gath;
MPI_Datatype field_z_slice_cont;
MPI_Datatype field_z_subslice;

MPI_Datatype deri_y_slice_cont;
MPI_Datatype deri_y_subslice;
MPI_Datatype deri_y_eo_subslice;
MPI_Datatype deri_y_slice_gath;
MPI_Datatype deri_y_slice_gath_split;

MPI_Datatype gauge_yx_edge_cont;
MPI_Datatype gauge_yx_edge_gath;
MPI_Datatype gauge_yx_edge_gath_split;

MPI_Datatype gauge_ty_edge_cont;
MPI_Datatype gauge_ty_edge_gath;
MPI_Datatype gauge_ty_edge_gath_split;

MPI_Datatype gauge_z_slice_gath;
MPI_Datatype gauge_z_slice_cont;
MPI_Datatype gauge_z_subslice;
MPI_Datatype gauge_z_eo_subslice;
MPI_Datatype gauge_z_slice_gath_split;

MPI_Datatype field_z_slice_gath;
MPI_Datatype field_z_slice_cont;
MPI_Datatype field_z_subslice;

MPI_Datatype deri_z_slice_cont;
MPI_Datatype deri_z_subslice;
MPI_Datatype deri_z_eo_subslice;
MPI_Datatype deri_z_slice_gath;
MPI_Datatype deri_z_slice_gath_split;

MPI_Datatype gauge_zx_edge_cont;
MPI_Datatype gauge_zx_edge_gath;
MPI_Datatype gauge_zx_edge_gath_split;

MPI_Datatype gauge_tz_edge_cont;
MPI_Datatype gauge_tz_edge_gath;
MPI_Datatype gauge_tz_edge_gath_split;

MPI_Datatype gauge_zy_edge_cont;
MPI_Datatype gauge_zy_edge_gath;
MPI_Datatype gauge_zy_edge_gath_split;

#ifdef PARALLELXYZT
spinor * field_buffer_z;
#endif

MPI_Comm mpi_time_slices;
#endif

int mpi_time_rank;

void mpi_init(int argc,char *argv[]) {
  g_proc_coords[0] = 0;
  g_proc_coords[1] = 0;
  g_proc_coords[2] = 0;
  g_proc_coords[3] = 0;

#ifdef MPI
  int periods[] = {1,1,1,1};
  int dims[] = {0,0,0,0};
  int ndims = 0;
  int reorder = 1, namelen;
  char processor_name[MPI_MAX_PROCESSOR_NAME];
  

#ifdef PARALLELT
  ndims = 1;
#  ifndef FIXEDVOLUME
  N_PROC_X = 1;
  N_PROC_Y = 1;
  N_PROC_Z = 1;
#  endif
#endif
#if defined PARALLELXT
  ndims = 2;
#  ifndef FIXEDVOLUME
  N_PROC_Y = 1;
  N_PROC_Z = 1;
#endif
#endif
#if defined PARALLELXYT
  ndims = 3;
#  ifndef FIXEDVOLUME
  N_PROC_Z = 1;
#  endif
#endif
#if defined PARALLELXYZT
  ndims = 4;
#endif
  dims[1] = N_PROC_X;
  dims[2] = N_PROC_Y;
  dims[3] = N_PROC_Z;


  MPI_Comm_size(MPI_COMM_WORLD, &g_nproc);
  MPI_Comm_rank(MPI_COMM_WORLD, &g_proc_id);
  MPI_Get_processor_name(processor_name, &namelen);
  MPI_Dims_create(g_nproc, ndims, dims);
  if(g_proc_id == 0){
    printf("Creating the following cartesian grid for a %d dimensional parallelisation:\n%d x %d x %d x %d\n"
	   , ndims, dims[0], dims[1], dims[2], dims[3]);
  }

  g_nproc_t = dims[0];
  g_nproc_x = dims[1];
  g_nproc_y = dims[2];
  g_nproc_z = dims[3];

#ifndef FIXEDVOLUME
  N_PROC_X = g_nproc_x;
  N_PROC_Y = g_nproc_y;
  N_PROC_Z = g_nproc_z;
  T = T_global/g_nproc_t;
  LX = LX/g_nproc_x;
  LY = LY/g_nproc_y;
  LZ = LZ/g_nproc_z;
  VOLUME = (T*LX*LY*LZ);
#  ifdef PARALLELT  
  RAND = (2*LX*LY*LZ);
  EDGES = 0;
#  elif defined PARALLELXT
  RAND = 2*LZ*(LY*LX + T*LY);
  EDGES = 4*LZ*LY;
  /* Note that VOLUMEPLUSRAND not equal to VOLUME+RAND in this case */
  /* VOLUMEPLUSRAND rather includes the edges */
#  elif defined PARALLELXYT
  RAND = 2*LZ*(LY*LX + T*LY + T*LX);
  EDGES = 4*LZ*(LY + T + LX);
#  elif defined PARALLELXYZT
  RAND = 2*LZ*LY*LX + 2*LZ*T*LY + 2*LZ*T*LX + 2*T*LX*LY;
  EDGES = 4*LZ*LY + 4*LZ*T + 4*LZ*LX + 4*LY*T + 4*LY*LX + 4*T*LX;
#  else
  RAND = 0;
  EDGES = 0;
#  endif
  /* Note that VOLUMEPLUSRAND is not always equal to VOLUME+RAND */
  /* VOLUMEPLUSRAND rather includes the edges */
  VOLUMEPLUSRAND = VOLUME + RAND + EDGES;
#endif
  g_dbw2rand = (RAND + 2*EDGES);

#ifdef PARALLELXYZT
  field_buffer_z = (spinor*)malloc(T*LX*LY/2*sizeof(spinor));
#endif

  MPI_Cart_create(MPI_COMM_WORLD, ndims, dims, periods, reorder, &g_cart_grid);
  MPI_Comm_rank(g_cart_grid, &g_cart_id);
  MPI_Cart_coords(g_cart_grid, g_cart_id, ndims, g_proc_coords);

  fprintf(stdout,"Process %d of %d on %s: cart_id %d, coordinates (%d %d %d %d)\n",
	  g_proc_id, g_nproc, processor_name, g_cart_id, 
	  g_proc_coords[0], g_proc_coords[1], g_proc_coords[2], g_proc_coords[3]);
  fflush(stdout);

  if(g_stdio_proc == -1){
    g_stdio_proc = g_proc_id;
  }

  MPI_Cart_shift(g_cart_grid, 0, 1, &g_nb_t_dn, &g_nb_t_up);
#if (defined PARALLELXT || defined PARALLELXYT || defined PARALLELXYZT)
  MPI_Cart_shift(g_cart_grid, 1, 1, &g_nb_x_dn, &g_nb_x_up);
#endif
#if (defined PARALLELXYT  || defined PARALLELXYZT)
  MPI_Cart_shift(g_cart_grid, 2, 1, &g_nb_y_dn, &g_nb_y_up);
#endif
#if defined PARALLELXYZT
  MPI_Cart_shift(g_cart_grid, 3, 1, &g_nb_z_dn, &g_nb_z_up);
#endif

  /* With internal boundary we mean the fields that are send */
  /* to another processor. It is located wihtin the local    */
  /* volume, whereas the external boundary is the boundary   */
  /* received from another processor lying on the RAND.      */
  /* In general the external bondaries are continuous in     */
  /* memory, while this is not always true for the internal  */
  /* one. */

  /* first the gauge fields */
  /* This is a gauge field on one space-time point */
  MPI_Type_contiguous(72, MPI_DOUBLE, &gauge_point);
  /* This is a type for one gauge time slice continuous */ 
  MPI_Type_contiguous(LX*LY*LZ, gauge_point, &gauge_time_slice_cont);
  /* This is a type for one gauge time slice dis-continuous -> NEW_GEOMETRY */
  /* This are 2 continuous ensembles of gauge_points of length LX*LY*LZ/2   */
  /* separated in memory by (VOLUME)/2 gauge_points                         */
  MPI_Type_vector(2, LX*LY*LZ/2, (VOLUME)/2, gauge_point, &gauge_time_slice_split);
  /* Commit the new types */
  MPI_Type_commit(&gauge_time_slice_split);
  MPI_Type_commit(&gauge_time_slice_cont);

  /* Continuous x-slice as it is found in the external memory.*/
  MPI_Type_contiguous(T*LY*LZ, gauge_point, &gauge_x_slice_cont);
  /* this is a continuoues gauge xt-slice */
  MPI_Type_contiguous(LY*LZ, gauge_point, &gauge_x_subslice);
  /* Put T of the latter together, each of which has length 1 (in units */
  /* of gauge_yy_subslice). They are separated by LX of those.          */
  /* This is as the gauge fields are located in the internal memory     */
  MPI_Type_vector(T, 1, LX, gauge_x_subslice, &gauge_x_slice_gath);
  MPI_Type_commit(&gauge_x_slice_gath);
  MPI_Type_commit(&gauge_x_slice_cont);

  /* Continuous y-slice as it is found in the external memory.*/
  MPI_Type_contiguous(T*LX*LZ, gauge_point, &gauge_y_slice_cont);
  /* this is a continuoues gauge xyt-slice */
  MPI_Type_contiguous(LZ, gauge_point, &gauge_y_subslice);
  /* Put T*LX together, separated by LY of those */
  MPI_Type_vector(T*LX, 1, LY, gauge_y_subslice, &gauge_y_slice_gath);
  MPI_Type_commit(&gauge_y_slice_cont);
  MPI_Type_commit(&gauge_y_slice_gath);

  /* Continuous z-slice as it is found in the external memory.*/
  MPI_Type_contiguous(T*LX*LY, gauge_point, &gauge_z_slice_cont);
  /* Put T*LX*LY gauge-points together, separated by LZ of those */
  MPI_Type_vector(T*LX*LY, 1, LZ, gauge_point, &gauge_z_slice_gath);
  MPI_Type_commit(&gauge_z_slice_cont);
  MPI_Type_commit(&gauge_z_slice_gath);


  /* external edges: on x-Rand send in t-direction*/
  MPI_Type_contiguous(2*LY*LZ ,gauge_point, &gauge_xt_edge_cont);
  MPI_Type_commit(&gauge_xt_edge_cont);
  /* internal edges, lying in memory nevertheless in the boundary */
  MPI_Type_vector(2, 1, T, gauge_x_subslice, &gauge_xt_edge_gath);
  MPI_Type_commit(&gauge_xt_edge_gath);

  /* external edges: y-Rand send in x-direction */
  MPI_Type_contiguous(2*T*LZ ,gauge_point, &gauge_yx_edge_cont);
  MPI_Type_commit(&gauge_yx_edge_cont);
  /* internal edges */
  MPI_Type_vector(2*T, LZ, LX*LZ, gauge_point, &gauge_yx_edge_gath);
  MPI_Type_commit(&gauge_yx_edge_gath);

  /* external edges: t-Rand send in y-direction */
  MPI_Type_contiguous(2*LX*LZ ,gauge_point, &gauge_ty_edge_cont);
  MPI_Type_commit(&gauge_ty_edge_cont);
  /* internal edges */
  MPI_Type_vector(2*LX, LZ, LY*LZ, gauge_point, &gauge_ty_edge_gath);
  MPI_Type_commit(&gauge_ty_edge_gath);

  /* external edges: z-Rand send in x-direction */
  /* zx-edge */
  MPI_Type_contiguous(2*T*LY ,gauge_point, &gauge_zx_edge_cont);
  MPI_Type_commit(&gauge_zx_edge_cont);
  /* internal edges */
  MPI_Type_vector(2*T, LY, LY*LX, gauge_point, &gauge_zx_edge_gath);
  MPI_Type_commit(&gauge_zx_edge_gath);

  /* external edges: t-Rand send in z-direction */
  /* tz-edge */
  MPI_Type_contiguous(2*LX*LY ,gauge_point, &gauge_tz_edge_cont);
  MPI_Type_commit(&gauge_tz_edge_cont);
  /* internal edges */
  MPI_Type_vector(2*LX*LY, 1, LZ, gauge_point, &gauge_tz_edge_gath);
  MPI_Type_commit(&gauge_tz_edge_gath);

  /* external edges: z-Rand send in y-direction */
  /* zy-edge */
  MPI_Type_contiguous(2*T*LX ,gauge_point, &gauge_zy_edge_cont);
  MPI_Type_commit(&gauge_zy_edge_cont);
  /* internal edges */
  MPI_Type_vector(2*T*LX, 1, LY, gauge_point, &gauge_zy_edge_gath);
  MPI_Type_commit(&gauge_zy_edge_gath);

  /* For _NEW_GEOMETRY -> even/odd geometry also in the gauge fields */
  /* Now we need a half continuoues gauge xt-slice */
  MPI_Type_contiguous((LY*LZ)/2, gauge_point, &gauge_x_eo_subslice); 
  /* We need to put 2*T of those of length one together separated by LX */
  MPI_Type_vector(2*T, 1, LX, gauge_x_eo_subslice, &gauge_x_slice_gath_split);
  MPI_Type_commit(&gauge_x_slice_gath_split);

  MPI_Type_vector(4, 1, T, gauge_x_eo_subslice, &gauge_xt_edge_gath_split);
  MPI_Type_commit(&gauge_xt_edge_gath_split);

  /* Now we need a half continuoues gauge xyt-slice */
  MPI_Type_contiguous((LZ)/2, gauge_point, &gauge_y_eo_subslice); 
  /* We need to put 2*T*LX of those together separated by LY */
  MPI_Type_vector(2*T*LX, 1, LY, gauge_y_eo_subslice, &gauge_y_slice_gath_split);
  MPI_Type_commit(&gauge_y_slice_gath_split);
  /* END _NEW_GEOMETRY */

  /* The spinor fields */
  /* this is a single spinor field on one space-time point */
  MPI_Type_contiguous(24, MPI_DOUBLE, &field_point);
  /* Tis is an even or odd spinor field time slice, continuous */
  MPI_Type_contiguous(LX*LY*LZ/2, field_point, &field_time_slice_cont); 
  /* Commit the new types */
  MPI_Type_commit(&field_time_slice_cont);

  /* This is an even or odd continuous spinor field x-slice */
  MPI_Type_contiguous(T*LY*LZ/2, field_point, &field_x_slice_cont);
  /* this is an even or odd continuoues spinor field xt-slice */
  MPI_Type_contiguous(LY*LZ/2, field_point, &field_x_subslice);
  /* this type puts T xt-slices together being the internal x-boundary in */
  /* even/odd ordered spinor fields */
  MPI_Type_vector(T, 1, LX, field_x_subslice, &field_x_slice_gath);
  MPI_Type_commit(&field_x_slice_gath);
  MPI_Type_commit(&field_x_slice_cont);

  /* This is an even or odd continuous spinor field y-slice */
  MPI_Type_contiguous(T*LX*LZ/2, field_point, &field_y_slice_cont);
  /* this is an even or odd continuoues spinor field xt-slice */
  MPI_Type_contiguous(LZ/2, field_point, &field_y_subslice);
  /* this type puts T*LX xt-slices together being the internal x-boundary in */
  /* even/odd ordered spinor fields */
  MPI_Type_vector(T*LX, 1, LY, field_y_subslice, &field_y_slice_gath);
  MPI_Type_commit(&field_y_slice_gath);
  MPI_Type_commit(&field_y_slice_cont);

  /* This is an even or odd continuous spinor field z-slice */
  MPI_Type_contiguous(T*LX*LY/2, field_point, &field_z_slice_cont);
  /* this type puts T*LX xt-slices together being the internal x-boundary in */
  /* even/odd ordered spinor fields */
  MPI_Type_vector(T*LX*LY/2, 1, LZ/2, field_point, &field_z_slice_gath);
  MPI_Type_commit(&field_z_slice_gath);
  MPI_Type_commit(&field_z_slice_cont);

  /* Now the derivative fields */
  /* this is a derivative field on one space-time point */
  MPI_Type_contiguous(32, MPI_DOUBLE, &deri_point);
  /* This is a type for one derivative time slice continuous */
  MPI_Type_contiguous(LX*LY*LZ, deri_point, &deri_time_slice_cont);
  /* This is a type for one derivative time slice dis-continuous -> NEW_GEOMETRY */
  MPI_Type_vector(2, LX*LY*LZ/2, VOLUME/2, deri_point, &deri_time_slice_split);
  /* Commit the new types */
  MPI_Type_commit(&deri_time_slice_split);
  MPI_Type_commit(&deri_time_slice_cont);

  MPI_Type_contiguous(T*LY*LZ, deri_point, &deri_x_slice_cont);
  MPI_Type_contiguous(LY*LZ, deri_point, &deri_x_subslice);
  MPI_Type_vector(T, 1, LX, deri_x_subslice, &deri_x_slice_gath);
  MPI_Type_commit(&deri_x_slice_gath);
  MPI_Type_commit(&deri_x_slice_cont);

  MPI_Type_contiguous(T*LX*LZ, deri_point, &deri_y_slice_cont);
  MPI_Type_contiguous(LZ, deri_point, &deri_y_subslice);
  MPI_Type_vector(T*LX, 1, LY, deri_y_subslice, &deri_y_slice_gath);
  MPI_Type_commit(&deri_y_slice_gath);
  MPI_Type_commit(&deri_y_slice_cont);

  MPI_Type_contiguous(T*LX*LY, deri_point, &deri_z_slice_cont);
  MPI_Type_vector(T*LX*LY, 1, LZ, deri_point, &deri_z_slice_gath);
  MPI_Type_commit(&deri_z_slice_gath);
  MPI_Type_commit(&deri_z_slice_cont);

  /* For _NEW_GEOMETRY */
  MPI_Type_contiguous(LY*LZ/2, deri_point, &deri_x_eo_subslice);
  MPI_Type_vector(2*T, 1, LX, deri_x_eo_subslice, &deri_x_slice_gath_split);
  MPI_Type_commit(&deri_x_slice_gath_split);

  MPI_Type_contiguous(LZ/2, deri_point, &deri_y_eo_subslice);
  MPI_Type_vector(2*T*LX, 1, LY, deri_y_eo_subslice, &deri_y_slice_gath_split);
  MPI_Type_commit(&deri_y_slice_gath_split);


  /* For observables we need communicators for catesian time slices */

  MPI_Comm_split(g_cart_grid, g_proc_coords[0], g_cart_id, &mpi_time_slices);
  MPI_Comm_rank(mpi_time_slices, &mpi_time_rank);
/*   fprintf(stderr, "My mpi_time_rank = %d, g_proc_coords = (%d,%d)\n", mpi_time_rank, g_proc_coords[0], g_proc_coords[1]); */

#else
  g_nproc = 1;
  g_proc_id = 0;
  g_nproc_x = 1;
  g_nproc_y = 1;
  g_nproc_z = 1;
  g_nproc_t = 1;
  g_cart_id = 0;
  mpi_time_rank = 0;
  g_stdio_proc = 0;

#ifndef FIXEDVOLUME
  T = T_global;
  VOLUME = (T*LX*LY*LZ);
  RAND = 0;
  EDGES = 0;
  VOLUMEPLUSRAND = VOLUME;
  N_PROC_X = 1;
  N_PROC_Y = 1;
  N_PROC_Z = 1;
#endif
  g_dbw2rand = 0;
#endif
  if(LZ%2 != 0) {
    fprintf(stderr, "LZ must be even!\n Aborting prgram...\n");
#ifdef MPI
    MPI_Finalize();
#endif
    exit(-1);
  }
}

static char const rcsid[] = "$Id$";
