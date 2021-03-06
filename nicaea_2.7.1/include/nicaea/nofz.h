/* ============================================================ *
 * nofz.h							*
 * Martin Kilbinger 2009					*
 * ============================================================ */

#ifndef __NOFZ_H
#define __NOFZ_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "config.h"
#include "errorlist.h"
#include "io.h"
#include "maths.h"
#include "cosmo.h"

#define redshift_base                     -1700
#define redshift_nnz                      -1 + redshift_base
#define redshift_zmax                     -2 + redshift_base
#define redshift_parnzerror               -3 + redshift_base
#define redshift_betap                    -4 + redshift_base
#define redshift_unknown                  -5 + redshift_base
#define redshift_Nzbin                    -6 + redshift_base
#define redshift_narrow_hist              -7 + redshift_base
#define redshift_type                     -8 + redshift_base
#define redshift_dz                       -9 + redshift_base
#define redshift_integration             -10 + redshift_base

/* Very narrow redshift bins cause a bug in the shear correlation function */
#define MINIMUM_ZBIN_WIDTH 0.1

/* Maximum redshift in case of photometric redshift errors, where the *
 * distribution formally goes to infinity.                            */
#define ZMAX 7

/* Distribution types and functions */
typedef enum {ludo, jonben, ymmk, ymmk0const, cfhtlens, hist, single} nofz_t;
#define snofz_t(i) ( \
		    i==ludo   ?     "ludo" : \
		    i==jonben ?     "jonben" : \
		    i==ymmk   ?     "ymmk" : \
		    i==ymmk0const ? "ymmk" : \
		    i==cfhtlens   ? "cfhtlens" : \
		    i==hist ?       "hist" : \
		    i==single ?     "single" : \
		    "")
#define Nnofz_t 7

/* Photometric redshift error types */
typedef enum {photz_no, photz_gauss} photz_t;
#define sphotz_t(i) ( \
	i==photz_no    ? "photz_no" : \
	i==photz_gauss ? "photz_gauss" : \
	"")
#define Nphotz_t 2

#define Nnzmode_t 2
typedef enum {nz_fit_hist, nz_read_from_files} nzmode_t;
#define snzmode_t(i) ( \
 i==nz_fit_hist         ? "nz_fit_hist" : \
 i==nz_read_from_files  ? "nz_read_from_files" : \
 "")

typedef struct {
  int Nzbin;          /* Number of redshift bins          */
  int *Nnz;           /* Number of parameters in each bin */
  int Nnz_max;        /* max(Nnz) */
  nofz_t *nofz;       /* Redshift distribution type for each bin */
  photz_t *photz;     /* Photometric redshift error type, convoling whatever type is */
  double *par_nz;     /* Redshift Parameter (Nzbin x Nnz_max) */
  double *prob_norm;  /* Redshift distribution normalisation */
  double *z_rescale;  /* Rescaling n(z) -> n(z_rescale*z) */
} redshift_t;

typedef struct {
  int i;
  redshift_t *self;
} redshiftANDint;

typedef struct {
  double z;
  redshift_t *redshift;
  int n_bin, Npz;
  error **err;
} params_photz_t;

typedef struct {
  double sigma_z, z_bias, c_cal, f_out, sigma_z_out, z_bias_out, c_cal_out;
} params_photz_gauss;


/* The usual inits, copys and updates */
redshift_t *init_redshift_empty(int Nzbin, int Nnz_max, error **err);
redshift_t *init_redshift(int Nzbin, const int *Nnz, const nofz_t *nofz, const photz_t *photz,
			  const double *par_nz, const double *z_rescale, error **err);
redshift_t *copy_redshift(const redshift_t *REDSHIFT, error **err);
void updateFrom_redshift(redshift_t *avant, redshift_t *apres);
void free_redshift(redshift_t **self);
void free_and_reinit_redshift(redshift_t **self, int Nzbin, int Nnz_max, error **err);
void dump_redshift(redshift_t *self, FILE *F, error **err);
void dump_redshift_nostruct(int Nzbin, int *Nnz, nofz_t *nofz, photz_t *photz, double *par_nz, int Nnz_max, FILE *F);

/* Reading from files */
void read_redshift_info(redshift_t **self, FILE *F, error **err);
void read_redshift_slice(redshift_t *self, int n_bin, const char *name, error **err);
void fill_redshift_slice(redshift_t *self, int n_bin, nofz_t nofz, photz_t photz, error **err, ...);
void get_nofz_t_file(const char *name, nofz_t *nofz, photz_t *photz, error **err);
void Nnz_from_file(const char *name, int *Nnz, error **err);
double *read_par_nz_hist(const char *name, int *Nnz, error **err);
redshift_t *init_redshift_from_files(const char **name, int Nzbin, error **err);
redshift_t *init_redshift_from_histogram_file(const char *name, error **err);


/* Redshift distribution */
double get_zmin(const redshift_t *redshift, int n_bin);
double get_zmax(const redshift_t *redshift, int n_bin);
double get_amin(const redshift_t *redshift, error **err);
double get_zmin_ph(const redshift_t *redshift, int n_bin);
double get_zmax_ph(const redshift_t *redshift, int n_bin);
double prob_unnorm(double z, void *intpar, error **err);
double dpn_dz(double zp, void *intpar);
double gaussian(double z, double mu, double sigma);
double prob_photz(double z, double zp, photz_t photz, const double *par_pz, error **err);
double prob_unnorm_nofz(double z, int n_bin, const redshift_t *self, error **err);
double prob(redshift_t *self, double z, int n_bin, error **err);
double int_for_zmean(double z, void *intpar, error **err);
double zmean(redshift_t*, int n_bin, error **err);
double zmedian(redshift_t *self, int n_bin, error **err);

int get_and_check_Nzbin(int Nzcorr, int Nzbin, error **err);

#define CHANGE(fct) int change_##fct(redshift_t*, redshift_t*)
CHANGE(prob);
CHANGE(redshift);
#undef CHANGE

int change_zmean(redshift_t *, redshift_t *, error **err);


#endif
