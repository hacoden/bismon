// generated module first_misc_module in file modbm_3BFt4NfJmZC_7iYi2dwM38B.c


#include "bismon.h"

#ifdef BISMON_MODID
DECLARE_MODULE_BM (_3BFt4NfJmZC_7iYi2dwM38B);
extern moduleinit_sigBM modulinit_3BFt4NfJmZC_7iYi2dwM38B_BM;
#endif /*BISMON_MODID */



// declare 1 routines

extern const char *const routid_3BFt4NfJmZC_7iYi2dwM38B_BM[1 + 1];

extern objrout_sigBM crout_8imWClcIFKt_6w2H2ouB4I6_BM;  //#0 iterate_sequence_minifunc


// declare 1 constants
objectval_tyBM *constob_3BFt4NfJmZC_7iYi2dwM38B_BM[1 + 1];
const char *const constid_3BFt4NfJmZC_7iYi2dwM38B_BM[1 + 1];


// define 1 routines


value_tyBM crout_8imWClcIFKt_6w2H2ouB4I6_BM     //#0+
  (struct stackframe_stBM *stkf,        // iterate_sequence_minifunc
   // 3 arguments
   const value_tyBM arg0,       // v_seq
   const value_tyBM arg1,       // v_fun
   const value_tyBM arg2,       // v_arg
   const value_tyBM arg3_ __attribute__ ((unused)),
   const quasinode_tyBM * restargs_ __attribute__ ((unused)))
{                               // start of iterate_sequence_minifunc
  struct frame_8imWClcIFKt_6w2H2ouB4I6_BMst
  {
    typedhead_tyBM stkfram_head;
    struct stackframe_stBM *stkfram_prev;
    objectval_tyBM *stkfram_descr;
    value_tyBM stkfram_callfun;
    int stkfram_state;
    int stkfram_extra;
    /// 4 local values:
    value_tyBM v_3qw9bnATIhq_8loBraLeXzQ;       // v_r
    value_tyBM v_4Q4WBLjAPTV_5WVLmhAvCJY;       // v_arg
    value_tyBM v_8jKgolslEuE_4dxzrK5R2B4;       // v_fun
    value_tyBM v_9vc0quxdnhj_3zLw1MEwl1e;       // v_seq
    /// 2 local numbers:
    intptr_t n_2W7Ckt2WxEM_4NVYgEWE73B; // ii
    intptr_t n_833x2jYqhZh_8vDIuJhwz1k; // leni
  } _;
  memset (&_, 0, sizeof (struct frame_8imWClcIFKt_6w2H2ouB4I6_BMst));
  _.stkfram_head.htyp = typayl_StackFrame_BM;
  _.stkfram_head.hgc = 0;
  _.stkfram_head.rlen = 4;
  _.stkfram_descr =
    constob_3BFt4NfJmZC_7iYi2dwM38B_BM[0] /*|iterate_sequence_minifunc */ ;
  ASSERT_BM (!stkf || stkf->stkfram_pA.htyp == typayl_StackFrame_BM
             || stkf->stkfram_pA.htyp == typayl_SpecialFrame_BM);
  _.stkfram_prev = stkf;
  // fetch 3 arguments:
  _.v_9vc0quxdnhj_3zLw1MEwl1e = arg0;
  _.v_8jKgolslEuE_4dxzrK5R2B4 = arg1;
  _.v_4Q4WBLjAPTV_5WVLmhAvCJY = arg2;
  // fetched 3 arguments.
  // routine _8imWClcIFKt_6w2H2ouB4I6 body:

startblock__2W6NObLb2d4_6OfkP3dYlF3:__attribute__ ((unused));
  {                             /* +block _2W6NObLb2d4_6OfkP3dYlF3 */
    _.n_833x2jYqhZh_8vDIuJhwz1k =       //assign _3EakP4EXPLZ_10btDL0iqAt  
/*seqlength*/ sequencesize_BM (_.v_9vc0quxdnhj_3zLw1MEwl1e);


    _.n_2W7Ckt2WxEM_4NVYgEWE73B =       //assign _1Z2mN7VIEbD_4RJunrL2QcV  
      0;



  startblock__3vvVJBONOyK_3kn86nuYwr1:__attribute__ ((unused));
    {                           /* +loop _3vvVJBONOyK_3kn86nuYwr1 */
    startloop__3vvVJBONOyK_3kn86nuYwr1:__attribute__ ((unused));


      if                        /*cond _7pBaII8qZHM_8xoaIKeilBk */
        /*when _8qsLnTz2QdD_9nZub2hBMEb: */
        ((                      /*greaterequal_int: */
           (_.n_2W7Ckt2WxEM_4NVYgEWE73B) >= (_.n_833x2jYqhZh_8vDIuJhwz1k)))
        {
// exit _9dmQwUU17M1_1km5cSGzJly   
          goto endblock__3vvVJBONOyK_3kn86nuYwr1;

        }                       //endwhen _8qsLnTz2QdD_9nZub2hBMEb   
/*endcond _7pBaII8qZHM_8xoaIKeilBk*/



/* !endingloop _3vvVJBONOyK_3kn86nuYwr1 */
      goto startloop__3vvVJBONOyK_3kn86nuYwr1;
    endblock__3vvVJBONOyK_3kn86nuYwr1:__attribute__ ((unused));
    }
  /*-loop _3vvVJBONOyK_3kn86nuYwr1 */
/* !endingblock _2W6NObLb2d4_6OfkP3dYlF3 */
    goto endblock__2W6NObLb2d4_6OfkP3dYlF3;
  endblock__2W6NObLb2d4_6OfkP3dYlF3:;
  }
  /*-block _2W6NObLb2d4_6OfkP3dYlF3 */

epilog_8imWClcIFKt_6w2H2ouB4I6:__attribute__ ((unused));
  // routine _8imWClcIFKt_6w2H2ouB4I6 epilogue:
  if (stkf)
    stkf->stkfram_callfun = NULL;
  return _.v_3qw9bnATIhq_8loBraLeXzQ;
}                               // end _8imWClcIFKt_6w2H2ouB4I6 routine#0 iterate_sequence_minifunc




// end of 1 generated routines


// the constant ids for 1 constants:
const char *const constid_3BFt4NfJmZC_7iYi2dwM38B_BM[1 + 1] = {
  /*0: */
  "_8imWClcIFKt_6w2H2ouB4I6",   //iterate_sequence_minifunc
  NULL
};                              // end 1 constant ids



// the routine ids for 1 routines:
const char *const routid_3BFt4NfJmZC_7iYi2dwM38B_BM[1 + 1] = {
  /*0: */
  "_8imWClcIFKt_6w2H2ouB4I6",   //iterate_sequence_minifunc

  NULL
};                              // end 1 routine ids



// basiclo*module initialization
#ifdef BISMON_MODID
value_tyBM
modulinit_3BFt4NfJmZC_7iYi2dwM38B_BM (struct stackframe_stBM *stkf,     //
                                      const value_tyBM arg1,    //
                                      const value_tyBM arg2,    //
                                      const value_tyBM arg3,    //
                                      void *dlh)
{
  return simple_module_initialize_BM (arg1, arg2, arg3, //
                                      "_3BFt4NfJmZC_7iYi2dwM38B",       //
                                      constob_3BFt4NfJmZC_7iYi2dwM38B_BM,       //
                                      constid_3BFt4NfJmZC_7iYi2dwM38B_BM,       //
                                      routid_3BFt4NfJmZC_7iYi2dwM38B_BM,        //
                                      dlh, stkf);
}                               // end modulinit_3BFt4NfJmZC_7iYi2dwM38B_BM
#endif /*BISMON_MODID */



// end of generated module first_misc_module in file modbm_3BFt4NfJmZC_7iYi2dwM38B.c