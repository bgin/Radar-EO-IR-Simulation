



/*
    Based on VOLK project.
    Modified by Bernard Gingold on:
    Date: 26-09-2020 09:52AM +00200
    contact: beniekg@gmail.com
    Few modification were added to original
    implementation (ICC pragmas, alignment directives and code layout rescheduled,
    unrolling completely 2-iteration for-loops)
    Converted to AVX512 implementation.
    
*/

/*
 * Copyright 2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#include <immintrin.h>
#include "GMS_32f_asin_32f.h"
#include "GMS_cephes.h"


	  void dsp_32f_asin_32f_u_avx512_looped(float * __restrict b,
	                                        float * __restrict a,
						const int32_t npoints) {
                   DSP_32F_ASIN_32F_AVX512_BLOCK
#if defined __ICC || defined __INTEL_COMPILER
#pragma code_align(32)
#endif
                   for(; idx != len; ++idx) {
                        _mm_prefetch((const char*)&a+32,_MM_HINT_T0); 
                        aVal = _mm512_loadu_ps(a);
                        aVal = _mm512_div_ps(aVal,
                                  _mm512_sqrt_ps(_mm512_mul_ps(_mm512_add_ps(fones, aVal),
                                                                    _mm512_sub_ps(fones, aVal))));
                        z = aVal;
			condition = _mm512_cmp_ps_mask(z,fzeroes,_CMP_LT_OQ);
			z = _mm512_maskz_sub_ps(condition,z,_mm512_mul_ps(z,ftwos));
			condition = _mm512_cmp_ps_mask(z,fones,_CMP_LT_OQ);
			x = _mm512_maskz_add_ps(condition,z,
			                               _mm512_sub_ps(
						                 _mm512_div_ps(fones,z),z));
			 // Original code contained here a 2-cycle loop
		       /*
                        for (i = 0; i < 2; i++) {
                                x = _mm256_add_ps(x, _mm256_sqrt_ps(_mm256_fmadd_ps(x, x, fones)));
                        }
                        */
		       t0 = _mm512_sqrt_ps(_mm512_fmadd_ps(x, x, fones));
		       x = _mm512_add_ps(x,t0 );
		       x = _mm512_add_ps(x,t0);
		       x = _mm512_div_ps(fones, x);
                       y = fzeroes;
		       // Original code contained here a 2-cycle loop
		        /*
                          for (j = ASIN_TERMS - 1; j >= 0; j--) {
                               y = _mm256_fmadd_ps(
                               y, _mm256_mul_ps(x, x), _mm256_set1_ps(pow(-1, j) / (2 * j + 1)));
                          }*/
		       y = _mm512_fmadd_ps(
                                  y, _mm512_mul_ps(x, x),n_third); // removing call to pow
		       y = _mm512_fmadd_ps(
                                  y, _mm512_mul_ps(x, x), p_third);  // removed call to pow
		       y = _mm512_mul_ps(y, _mm512_mul_ps(x, ffours)); 
		       condition = _mm512_cmp_ps_mask(z,fones,_CMP_GT_OQ);
		       y = _mm512_maskz_add_ps(condition,y,_mm512_fnmadd_ps(y,ftwos,pio2));
		       arcsine = y;
		       condition = _mm512_cmp_ps_mask(aVal,fzeroes,_CMP_LT_OQ);
		       arcsine   = _mm512_maskz_sub_ps(condition, arcsine,
		                                        _mm512_mul_ps(arcsine,ftwos));
		       _mm512_storeu_ps(b,arcsine);
		       a += 16;
		       b += 16;
		                                 
		 }
                  idx = len*16;
#if defined __ICC || defined __INTEL_COMPILER
#pragma loop_count min(1),avg(4),max(7)
#endif
                   for(; idx != npoints; ++idx) {
                        b[i] = ceph_asinf(a[i]);
	           }
	  }


	  
	  void dsp_32f_asin_32f_a_avx512_looped(float * __restrict __ATTR_ALIGN__(64) b,
	                                        float * __restrict __ATTR_ALIGN__(64) a,
						const int32_t npoints) {
                   DSP_32F_ASIN_32F_AVX512_BLOCK
#if defined __ICC || defined __INTEL_COMPILER
              __assume_aligned(b,64);
	      __assume_aligned(a,64);
#elif defined __GNUC__ && !defined __INTEL_COMPILER
              b = (float*)__builtin_assume_aligned(b,64);
	      a = (float*)__builtin_assume_aligned(a,64);
#endif		   
#if defined __ICC || defined __INTEL_COMPILER
#pragma code_align(32)
#endif
                   for(; idx != len; ++idx) {
                        _mm_prefetch((const char*)&a+32,_MM_HINT_T0); 
                        aVal = _mm512_load_ps(a);
                        aVal = _mm512_div_ps(aVal,
                                  _mm512_sqrt_ps(_mm512_mul_ps(_mm512_add_ps(fones, aVal),
                                                                    _mm512_sub_ps(fones, aVal))));
                        z = aVal;
			condition = _mm512_cmp_ps_mask(z,fzeroes,_CMP_LT_OQ);
			z = _mm512_maskz_sub_ps(condition,z,_mm512_mul_ps(z,ftwos));
			condition = _mm512_cmp_ps_mask(z,fones,_CMP_LT_OQ);
			x = _mm512_maskz_add_ps(condition,z,
			                               _mm512_sub_ps(
						                 _mm512_div_ps(fones,z),z));
			 // Original code contained here a 2-cycle loop
		       /*
                        for (i = 0; i < 2; i++) {
                                x = _mm256_add_ps(x, _mm256_sqrt_ps(_mm256_fmadd_ps(x, x, fones)));
                        }
                        */
		       t0 = _mm512_sqrt_ps(_mm512_fmadd_ps(x, x, fones));
		       x = _mm512_add_ps(x,t0 );
		       x = _mm512_add_ps(x,t0);
		       x = _mm512_div_ps(fones, x);
                       y = fzeroes;
		       // Original code contained here a 2-cycle loop
		        /*
                          for (j = ASIN_TERMS - 1; j >= 0; j--) {
                               y = _mm256_fmadd_ps(
                               y, _mm256_mul_ps(x, x), _mm256_set1_ps(pow(-1, j) / (2 * j + 1)));
                          }*/
		       y = _mm512_fmadd_ps(
                                  y, _mm512_mul_ps(x, x),n_third); // removing call to pow
		       y = _mm512_fmadd_ps(
                                  y, _mm512_mul_ps(x, x), p_third);  // removed call to pow
		       y = _mm512_mul_ps(y, _mm512_mul_ps(x, ffours)); 
		       condition = _mm512_cmp_ps_mask(z,fones,_CMP_GT_OQ);
		       y = _mm512_maskz_add_ps(condition,y,_mm512_fnmadd_ps(y,ftwos,pio2));
		       arcsine = y;
		       condition = _mm512_cmp_ps_mask(aVal,fzeroes,_CMP_LT_OQ);
		       arcsine   = _mm512_maskz_sub_ps(condition, arcsine,
		                                        _mm512_mul_ps(arcsine,ftwos));
		       _mm512_store_ps(b,arcsine);
		       a += 16;
		       b += 16;
		                                 
		 }
                  idx = len*16;
#if defined __ICC || defined __INTEL_COMPILER
#pragma loop_count min(1),avg(4),max(7)
#endif
                   for(; idx != npoints; ++idx) {
                        b[i] = ceph_asinf(a[i]);
	           }
	  }


       
          __m512 dsp_32f_asin_32_avx512(const __m512 v) {

                                                   
	          register __m512 aVal, pio2, x, y, z, arcsine;              
                  register __m512 fzeroes, fones, ftwos, ffours;              
                  register __m512 n_third,p_third,t0;                         
	          __mmask16 condition = 0;
	          pio2 = _mm512_set1_ps(1.5707963267948966192f);              
	          fzeroes = _mm512_setzero_ps();                              
	          t0 = fzeroes;                                              
                  fones = _mm512_set1_ps(1.0f);                               
                  ftwos = _mm512_set1_ps(2.0f);                               
                  ffours  = _mm512_set1_ps(4.0f);                             
                  n_third = _mm512_set1_ps(-0.3333333333333333333333333f);     
                  p_third = _mm512_set1_ps(0.3333333333333333333333333f);
		  aVal = v;
		  aVal = _mm512_div_ps(aVal,
                                  _mm512_sqrt_ps(_mm512_mul_ps(_mm512_add_ps(fones, aVal),
                                                                    _mm512_sub_ps(fones, aVal))));
                  z = aVal;
		  condition = _mm512_cmp_ps_mask(z,fzeroes,_CMP_LT_OQ);
		  z = _mm512_maskz_sub_ps(condition,z,_mm512_mul_ps(z,ftwos));
		  condition = _mm512_cmp_ps_mask(z,fones,_CMP_LT_OQ);
		  x = _mm512_maskz_add_ps(condition,z,
			                               _mm512_sub_ps(
						                 _mm512_div_ps(fones,z),z));
			 // Original code contained here a 2-cycle loop
		       /*
                        for (i = 0; i < 2; i++) {
                                x = _mm256_add_ps(x, _mm256_sqrt_ps(_mm256_fmadd_ps(x, x, fones)));
                        }
                        */
		   t0 = _mm512_sqrt_ps(_mm512_fmadd_ps(x, x, fones));
		   x = _mm512_add_ps(x,t0 );
		   x = _mm512_add_ps(x,t0);
		   x = _mm512_div_ps(fones, x);
                   y = fzeroes;
		       // Original code contained here a 2-cycle loop
		        /*
                          for (j = ASIN_TERMS - 1; j >= 0; j--) {
                               y = _mm256_fmadd_ps(
                               y, _mm256_mul_ps(x, x), _mm256_set1_ps(pow(-1, j) / (2 * j + 1)));
                          }*/
		    y = _mm512_fmadd_ps(
                                  y, _mm512_mul_ps(x, x),n_third); // removing call to pow
		    y = _mm512_fmadd_ps(
                                  y, _mm512_mul_ps(x, x), p_third);  // removed call to pow
		    y = _mm512_mul_ps(y, _mm512_mul_ps(x, ffours)); 
		    condition = _mm512_cmp_ps_mask(z,fones,_CMP_GT_OQ);
		    y = _mm512_maskz_add_ps(condition,y,_mm512_fnmadd_ps(y,ftwos,pio2));
		    arcsine = y;
		    condition = _mm512_cmp_ps_mask(aVal,fzeroes,_CMP_LT_OQ);
		    arcsine   = _mm512_maskz_sub_ps(condition, arcsine,
		                                        _mm512_mul_ps(arcsine,ftwos));
		    return (arcsine);
	  }





