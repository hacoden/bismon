// generated persistent module first_misc_module
// in file modbm_3BFt4NfJmZC_7iYi2dwM38B.c -- DONT EDIT
// this generated code is GPLv3+ licensed.


#include "bismon.h"

#ifdef BISMON_MODID
DECLARE_MODULE_BM (_3BFt4NfJmZC_7iYi2dwM38B);
extern moduleinit_sigBM modulinit_3BFt4NfJmZC_7iYi2dwM38B_BM;
#endif /*BISMON_MODID */

#ifdef BISMON_MOMD5
const char modulmd5chksum_3BFt4NfJmZC_7iYi2dwM38B_BM[] = BISMON_MOMD5;
#endif /*BISMON_MOMD5 */



// declare 15 routines

extern const char *const routid_3BFt4NfJmZC_7iYi2dwM38B_BM[15 + 1];


// no failure

extern objrout_sigBM crout_0JktC4mAHCa_1KNr6uMIqIi_BM;  //#0 !dump_scan°list_object minifunc for list_object o1 & dumperobj o2

extern objrout_sigBM crout_0SinERsgdvQ_57dLCP63i9r_BM;  //#1 !forgetname#command_handler

extern objrout_sigBM crout_0YOXUe7rEJC_3jFMjOAQRs9_BM;  //#2 element_of_closed_minifunc

extern objrout_sigBM crout_22Zd68WPKUO_6AoIuBcbUb2_BM;  //#3 !dump_scan°dict_object minifunc for dict-object odict and dumperobj odumper

extern objrout_sigBM crout_2bZ4BfCci5V_3WTzG7V25y4_BM;  //#4 !put°dict_object minifunc

extern objrout_sigBM crout_3iiht57Wrs1_5NbiUi27oBn_BM;  //#5 !prepend°list_object minifunc

extern objrout_sigBM crout_3yQlckX4DRh_4b9l9FBSSSL_BM;  //#6 same_as_closed_minifunc

extern objrout_sigBM crout_3HAeZZQ0q7b_7qyDsekaz7Y_BM;  //#7 !clear°list_object minifunc

extern objrout_sigBM crout_4iEFTEcHxeb_6lH464uFkTC_BM;  //#8 equal_to_closed_minifunc

extern objrout_sigBM crout_4qlegfkPzrC_17skUfJP7v8_BM;  //#9 !dump_data°dict_object minifun

extern objrout_sigBM crout_4M7lr5cU6VD_5hxmeH3baBf_BM;  //#10 !append°list_object minifunction

extern objrout_sigBM crout_5j8LpQULt1p_9oeOaqmfYYr_BM;  //#11 !dump_data°list_object minifunc

extern objrout_sigBM crout_82ho9wUUDji_4peMLha4PXl_BM;  //#12 findobj_scan_minifunc

extern objrout_sigBM crout_8imWClcIFKt_6w2H2ouB4I6_BM;  //#13 iterate_sequence_minifunc

extern objrout_sigBM crout_9CG8SKNs6Ql_4PiHd8cnydn_BM;  //#14 !clear°dict_object minifun


// declare 21 constants
objectval_tyBM *constob_3BFt4NfJmZC_7iYi2dwM38B_BM[21 + 1];
const char *const constid_3BFt4NfJmZC_7iYi2dwM38B_BM[21 + 1];


// define 15 routines


value_tyBM crout_0JktC4mAHCa_1KNr6uMIqIi_BM     //#0
  (struct stackframe_stBM *stkf,        //
//!dump_scan°list_object minifunc for list_object o1 & dumperobj o2
   // 2 arguments
   const value_tyBM arg0,       // o1
   const value_tyBM arg1,       // o2
   const value_tyBM arg2_ __attribute__ ((unused)),
   const value_tyBM arg3_ __attribute__ ((unused)),
   const quasinode_tyBM * restargs_ __attribute__ ((unused)))
{                               // start of _0JktC4mAHCa_1KNr6uMIqIi
  struct frame_0JktC4mAHCa_1KNr6uMIqIi_BMst
  {
    typedhead_tyBM stkfram_head;
    struct stackframe_stBM *stkfram_prev;
    objectval_tyBM *stkfram_descr;
    value_tyBM stkfram_callfun;
    int stkfram_state;
    int stkfram_extra;
    /// 4 local values:
    value_tyBM v_2BkOxf2KbS8_3bswrKbmgjy;       // v_node
    value_tyBM v_3qw9bnATIhq_8loBraLeXzQ;       // v_r
    objectval_tyBM *o_93FBSk1ML0f_2WjSuKa9TNU;  // o2
    objectval_tyBM *o_97rnxb0KFqq_44veN75DG2E;  // o1
    /// 2 local numbers:
    intptr_t n_2W7Ckt2WxEM_4NVYgEWE73B; // ii
    intptr_t n_833x2jYqhZh_8vDIuJhwz1k; // leni
  } _;
  memset (&_, 0, sizeof (struct frame_0JktC4mAHCa_1KNr6uMIqIi_BMst));
  _.stkfram_head.htyp = typayl_StackFrame_BM;
  _.stkfram_head.hgc = 0;
  _.stkfram_head.rlen = 4;
  _.stkfram_descr =
    constob_3BFt4NfJmZC_7iYi2dwM38B_BM[0] /*|_0JktC4mAHCa_1KNr6uMIqIi */ ;
  ASSERT_BM (!stkf || stkf->stkfram_pA.htyp == typayl_StackFrame_BM
             || stkf->stkfram_pA.htyp == typayl_SpecialFrame_BM);
  _.stkfram_prev = stkf;
  // fetch 2 arguments:
  _.o_97rnxb0KFqq_44veN75DG2E = objectcast_BM (arg0);   // o1
  _.o_93FBSk1ML0f_2WjSuKa9TNU = objectcast_BM (arg1);   // o2
  // fetched 2 arguments.
  // no closed values in _0JktC4mAHCa_1KNr6uMIqIi.
  // routine _0JktC4mAHCa_1KNr6uMIqIi body:

startblock__9xHQlk4snoU_6unWwGT6PcG:__attribute__ ((unused));
  {                             /* +block _9xHQlk4snoU_6unWwGT6PcG */
    /*v_node: */ _.v_2BkOxf2KbS8_3bswrKbmgjy =
      //assign _8ggHPTQI1iG_02hVE5KybEL  
      /*objlisttonodepayl: */
      (value_tyBM)
      objlisttonodepayl_BM (( /*o1: */ _.o_97rnxb0KFqq_44veN75DG2E),
                            ((constob_3BFt4NfJmZC_7iYi2dwM38B_BM[15]
                              /*|node */ )));


    /*leni: */ _.n_833x2jYqhZh_8vDIuJhwz1k =
      //assign _0zhpGzEVWGP_7WoL4LJocuS  
      /*nodewidth: */
      (intptr_t) nodewidth_BM ( /*v_node: */ _.v_2BkOxf2KbS8_3bswrKbmgjy);

//+ #2/4 of block _9xHQlk4snoU_6unWwGT6PcG :: 

    /*ii: */ _.n_2W7Ckt2WxEM_4NVYgEWE73B =
      //assign _0fygl91r6x5_8b3aCDptjAi  
      0;



//// +while _1w1gkKpMOzk_6mZXddz3Y4L 
  startblock__1w1gkKpMOzk_6mZXddz3Y4L: /*+!while */ ;
    {                           /* +while _1w1gkKpMOzk_6mZXddz3Y4L */
      if (!( /*while _1w1gkKpMOzk_6mZXddz3Y4L cond: */ ( /*less_int: */ ( /*ii: */ _.n_2W7Ckt2WxEM_4NVYgEWE73B) < ( /*leni: */ _.n_833x2jYqhZh_8vDIuJhwz1k))))  //: testwhile _1w1gkKpMOzk_6mZXddz3Y4L failing   
        goto endblock__1w1gkKpMOzk_6mZXddz3Y4L; //failwhile   
// while _1w1gkKpMOzk_6mZXddz3Y4L body:   
// while _1w1gkKpMOzk_6mZXddz3Y4L comp#0:  
      {                         // start cexpansion _4048rnGI686_6URARAINwQH - obdumpscanvalue   
        /*obdumpscanvalue: */
        obdumpscanvalue_BM (( /*o2: */ _.o_93FBSk1ML0f_2WjSuKa9TNU),
                            (   /*nodenthson: */
                              nodenthson_BM (( /*v_node: */ _.
                                              v_2BkOxf2KbS8_3bswrKbmgjy),
                                             ( /*ii: */ _.
                                              n_2W7Ckt2WxEM_4NVYgEWE73B))),
                            (0));
      }                         // end cexpansion _4048rnGI686_6URARAINwQH - obdumpscanvalue   
// while _1w1gkKpMOzk_6mZXddz3Y4L comp#1:  
      /*ii: */ _.n_2W7Ckt2WxEM_4NVYgEWE73B =
        //assign _5fibWpWyCTN_8xn6wySOLhA   
        /*add_int: */ (( /*ii: */ _.n_2W7Ckt2WxEM_4NVYgEWE73B) + (1));

      goto startblock__1w1gkKpMOzk_6mZXddz3Y4L; // repeatwhile  
    endblock__1w1gkKpMOzk_6mZXddz3Y4L: /*endingwhile */ ;
    }
  /*-endwhile _1w1gkKpMOzk_6mZXddz3Y4L */


/* !endingblock _9xHQlk4snoU_6unWwGT6PcG */
    goto endblock__9xHQlk4snoU_6unWwGT6PcG;
  endblock__9xHQlk4snoU_6unWwGT6PcG:;
  }
  /*-block _9xHQlk4snoU_6unWwGT6PcG */

epilog_0JktC4mAHCa_1KNr6uMIqIi:__attribute__ ((unused));
  // routine _0JktC4mAHCa_1KNr6uMIqIi epilogue:
  if (stkf)
    stkf->stkfram_callfun = NULL;
  return /*v_r: */ _.v_3qw9bnATIhq_8loBraLeXzQ;
}                               // end _0JktC4mAHCa_1KNr6uMIqIi routine#0 _0JktC4mAHCa_1KNr6uMIqIi




value_tyBM crout_0SinERsgdvQ_57dLCP63i9r_BM     //#1
  (struct stackframe_stBM * stkf,       //
//!forgetname#command_handler
   // 1 arguments
   const value_tyBM arg0,       // v_arg
   const value_tyBM arg1_ __attribute__ ((unused)),
   const value_tyBM arg2_ __attribute__ ((unused)),
   const value_tyBM arg3_ __attribute__ ((unused)),
   const quasinode_tyBM * restargs_ __attribute__ ((unused)))
{                               // start of _0SinERsgdvQ_57dLCP63i9r
  struct frame_0SinERsgdvQ_57dLCP63i9r_BMst
  {
    typedhead_tyBM stkfram_head;
    struct stackframe_stBM *stkfram_prev;
    objectval_tyBM *stkfram_descr;
    value_tyBM stkfram_callfun;
    int stkfram_state;
    int stkfram_extra;
    /// 3 local values:
    value_tyBM v_3qw9bnATIhq_8loBraLeXzQ;       // v_r
    value_tyBM v_4Q4WBLjAPTV_5WVLmhAvCJY;       // v_arg
    objectval_tyBM *o_7daWyW7jPyL_0EvyGgObjJm;  // o_arg
    /// 0 local numbers:
  } _;
  memset (&_, 0, sizeof (struct frame_0SinERsgdvQ_57dLCP63i9r_BMst));
  _.stkfram_head.htyp = typayl_StackFrame_BM;
  _.stkfram_head.hgc = 0;
  _.stkfram_head.rlen = 3;
  _.stkfram_descr =
    constob_3BFt4NfJmZC_7iYi2dwM38B_BM[1] /*|_0SinERsgdvQ_57dLCP63i9r */ ;
  ASSERT_BM (!stkf || stkf->stkfram_pA.htyp == typayl_StackFrame_BM
             || stkf->stkfram_pA.htyp == typayl_SpecialFrame_BM);
  _.stkfram_prev = stkf;
  // fetch 1 arguments:
  _.v_4Q4WBLjAPTV_5WVLmhAvCJY = arg0;   // v_arg
  // fetched 1 arguments.
  // no closed values in _0SinERsgdvQ_57dLCP63i9r.
  // routine _0SinERsgdvQ_57dLCP63i9r body:

startblock__84ATc5kpHjd_2wlYHZEUoeb:__attribute__ ((unused));
  {                             /* +block _84ATc5kpHjd_2wlYHZEUoeb */
    if                          /*cond _0Vnb5tJpsaA_9C18YQWoCmH */
/*when _7aRr2DYoXS7_0jbH59RVcFR:*/
      (                         /*is_objectv */
        isobject_BM ( /*v_arg: */ _.v_4Q4WBLjAPTV_5WVLmhAvCJY))
      {                         //body when _7aRr2DYoXS7_0jbH59RVcFR  
        /*o_arg: */ _.o_7daWyW7jPyL_0EvyGgObjJm =
          //assign _1eDeH7xPojd_3uCQMEEBsHc   
          /*val_to_object: */
          objectcast_BM ( /*v_arg: */ _.v_4Q4WBLjAPTV_5WVLmhAvCJY);

        {                       // run _5cqLFnQUJtD_2QMjkdfqr6q  
          forgetnamedobject_BM ( /*o_arg: */ _.o_7daWyW7jPyL_0EvyGgObjJm);
        }                       // end run _5cqLFnQUJtD_2QMjkdfqr6q  

        {                       // run _8y60zCwYsj4_4MKReQrfwGI  
          if (pthread_self () == mainthreadid_BM)
            {
              log_begin_message_BM ();
              log_puts_message_BM ("Forgot name of object ");
              log_object_message_BM ( /*o_arg: */ _.
                                     o_7daWyW7jPyL_0EvyGgObjJm);
              log_puts_message_BM (".");
              log_end_message_BM ();
            }
          ;
        }                       // end run _8y60zCwYsj4_4MKReQrfwGI  
        /*return _8X0BDdDTJe6_8OoRGA5RDeF: *//*v_r: */ _.
          v_3qw9bnATIhq_8loBraLeXzQ =
          // returned  
          /*o_arg: */ _.o_7daWyW7jPyL_0EvyGgObjJm;
        goto epilog_0SinERsgdvQ_57dLCP63i9r;

      }                         //endwhen _7aRr2DYoXS7_0jbH59RVcFR  
    else if
/*when _3674ULyT2hz_5gMJA95w9aB:*/
      (                         /*issequence: */
        issequence_BM ( /*v_arg: */ _.v_4Q4WBLjAPTV_5WVLmhAvCJY))
      {                         //body when _3674ULyT2hz_5gMJA95w9aB  

        {                       // run _2Gx3OIyj6WH_36HZRfp3Slv  
          (apply1_BM (((constob_3BFt4NfJmZC_7iYi2dwM38B_BM[17]
                        /*!iterate_sequence_minifunc */ )),
                      ((struct stackframe_stBM *) &_),
                      ((constob_3BFt4NfJmZC_7iYi2dwM38B_BM[1]
                        /*!_0SinERsgdvQ_57dLCP63i9r */ ))));
        }                       // end run _2Gx3OIyj6WH_36HZRfp3Slv  

      }                         //endwhen _3674ULyT2hz_5gMJA95w9aB  
/*endcond _0Vnb5tJpsaA_9C18YQWoCmH*/

/* !endingblock _84ATc5kpHjd_2wlYHZEUoeb */
    goto endblock__84ATc5kpHjd_2wlYHZEUoeb;
  endblock__84ATc5kpHjd_2wlYHZEUoeb:;
  }
  /*-block _84ATc5kpHjd_2wlYHZEUoeb */

epilog_0SinERsgdvQ_57dLCP63i9r:__attribute__ ((unused));
  // routine _0SinERsgdvQ_57dLCP63i9r epilogue:
  if (stkf)
    stkf->stkfram_callfun = NULL;
  return /*v_r: */ _.v_3qw9bnATIhq_8loBraLeXzQ;
}                               // end _0SinERsgdvQ_57dLCP63i9r routine#1 _0SinERsgdvQ_57dLCP63i9r




value_tyBM crout_0YOXUe7rEJC_3jFMjOAQRs9_BM     //#2+
  (struct stackframe_stBM * stkf,       // element_of_closed_minifunc
//!return argument if it is contained in the closed set
   // 1 arguments
   const value_tyBM arg0,       // v_arg
   const value_tyBM arg1_ __attribute__ ((unused)),
   const value_tyBM arg2_ __attribute__ ((unused)),
   const value_tyBM arg3_ __attribute__ ((unused)),
   const quasinode_tyBM * restargs_ __attribute__ ((unused)))
{                               // start of element_of_closed_minifunc
  struct frame_0YOXUe7rEJC_3jFMjOAQRs9_BMst
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
    value_tyBM v_5EEITf6IFH7_4DbKG99zcPW;       // v1
    objectval_tyBM *o_97rnxb0KFqq_44veN75DG2E;  // o1
    /// 0 local numbers:
  } _;
  memset (&_, 0, sizeof (struct frame_0YOXUe7rEJC_3jFMjOAQRs9_BMst));
  _.stkfram_head.htyp = typayl_StackFrame_BM;
  _.stkfram_head.hgc = 0;
  _.stkfram_head.rlen = 4;
  _.stkfram_descr =
    constob_3BFt4NfJmZC_7iYi2dwM38B_BM[2] /*|element_of_closed_minifunc */ ;
  ASSERT_BM (!stkf || stkf->stkfram_pA.htyp == typayl_StackFrame_BM
             || stkf->stkfram_pA.htyp == typayl_SpecialFrame_BM);
  _.stkfram_prev = stkf;
  // fetch 1 arguments:
  _.v_4Q4WBLjAPTV_5WVLmhAvCJY = arg0;   // v_arg
  // fetched 1 arguments.
  {                             // fetch 1 closed values in element_of_closed_minifunc:
    const closure_tyBM *callclos_0YOXUe7rEJC_3jFMjOAQRs9 =
      (stkf && stkf->stkfram_pA.htyp == typayl_StackFrame_BM)
      ? (closurecast_BM (stkf->stkfram_callfun)) : NULL;
    unsigned nbclosed_0YOXUe7rEJC_3jFMjOAQRs9 =
      closurewidth_BM ((const value_tyBM) callclos_0YOXUe7rEJC_3jFMjOAQRs9);
    if (nbclosed_0YOXUe7rEJC_3jFMjOAQRs9 > 0)
      _.v_5EEITf6IFH7_4DbKG99zcPW = callclos_0YOXUe7rEJC_3jFMjOAQRs9->nodt_sons[0];     // v1
  }                             // fetched 1 closed values in _0YOXUe7rEJC_3jFMjOAQRs9.
  // routine _0YOXUe7rEJC_3jFMjOAQRs9 body:

startblock__7gnMvoTqaQY_3kAwj2rwZEM:__attribute__ ((unused));
  {                             /* +block _7gnMvoTqaQY_3kAwj2rwZEM */
    /*o1: */ _.o_97rnxb0KFqq_44veN75DG2E =
      //assign _11EvX29IHDi_1da6fY8nL7Y  
      /*val_to_object: */
      objectcast_BM ( /*v_arg: */ _.v_4Q4WBLjAPTV_5WVLmhAvCJY);


    if                          /*cond _0WAyTs536mS_7eZ58PLoYjP */
/*when _2U8EKCOdizu_8BNXvpmwd43:*/
      ((                        /*set_contains: */
         setcontains_BM (( /*v1: */ _.v_5EEITf6IFH7_4DbKG99zcPW),
                         ( /*o1: */ _.o_97rnxb0KFqq_44veN75DG2E))))
      {                         //body when _2U8EKCOdizu_8BNXvpmwd43  
        /*return _4rFgWAf3aqA_0UBP8b4hESV: *//*v_r: */ _.
          v_3qw9bnATIhq_8loBraLeXzQ =
          // returned  
          /*o1: */ _.o_97rnxb0KFqq_44veN75DG2E;
        goto epilog_0YOXUe7rEJC_3jFMjOAQRs9;

      }                         //endwhen _2U8EKCOdizu_8BNXvpmwd43  
/*endcond _0WAyTs536mS_7eZ58PLoYjP*/

/* !endingblock _7gnMvoTqaQY_3kAwj2rwZEM */
    goto endblock__7gnMvoTqaQY_3kAwj2rwZEM;
  endblock__7gnMvoTqaQY_3kAwj2rwZEM:;
  }
  /*-block _7gnMvoTqaQY_3kAwj2rwZEM */

epilog_0YOXUe7rEJC_3jFMjOAQRs9:__attribute__ ((unused));
  // routine _0YOXUe7rEJC_3jFMjOAQRs9 epilogue:
  if (stkf)
    stkf->stkfram_callfun = NULL;
  return /*v_r: */ _.v_3qw9bnATIhq_8loBraLeXzQ;
}                               // end _0YOXUe7rEJC_3jFMjOAQRs9 routine#2 element_of_closed_minifunc




value_tyBM crout_22Zd68WPKUO_6AoIuBcbUb2_BM     //#3
  (struct stackframe_stBM * stkf,       //
//!dump_scan°dict_object minifunc for dict-object odict and dumperobj odumper
   // 2 arguments
   const value_tyBM arg0,       // odict
   const value_tyBM arg1,       // odumper
   const value_tyBM arg2_ __attribute__ ((unused)),
   const value_tyBM arg3_ __attribute__ ((unused)),
   const quasinode_tyBM * restargs_ __attribute__ ((unused)))
{                               // start of _22Zd68WPKUO_6AoIuBcbUb2
  struct frame_22Zd68WPKUO_6AoIuBcbUb2_BMst
  {
    typedhead_tyBM stkfram_head;
    struct stackframe_stBM *stkfram_prev;
    objectval_tyBM *stkfram_descr;
    value_tyBM stkfram_callfun;
    int stkfram_state;
    int stkfram_extra;
    /// 5 local values:
    value_tyBM v_167WczZjWtG_3FofhJxQWJi;       // v_str
    objectval_tyBM *o_3cTC8W7d75H_1vteOyEoAao;  // odumper
    value_tyBM v_3qw9bnATIhq_8loBraLeXzQ;       // v_r
    value_tyBM v_5jRQCD1qTko_61I0j04ugAA;       // v_val
    objectval_tyBM *o_7n2SLLuWOjH_4ArhFUjmckz;  // odict
    /// 0 local numbers:
  } _;
  memset (&_, 0, sizeof (struct frame_22Zd68WPKUO_6AoIuBcbUb2_BMst));
  _.stkfram_head.htyp = typayl_StackFrame_BM;
  _.stkfram_head.hgc = 0;
  _.stkfram_head.rlen = 5;
  _.stkfram_descr =
    constob_3BFt4NfJmZC_7iYi2dwM38B_BM[5] /*|_22Zd68WPKUO_6AoIuBcbUb2 */ ;
  ASSERT_BM (!stkf || stkf->stkfram_pA.htyp == typayl_StackFrame_BM
             || stkf->stkfram_pA.htyp == typayl_SpecialFrame_BM);
  _.stkfram_prev = stkf;
  // fetch 2 arguments:
  _.o_7n2SLLuWOjH_4ArhFUjmckz = objectcast_BM (arg0);   // odict
  _.o_3cTC8W7d75H_1vteOyEoAao = objectcast_BM (arg1);   // odumper
  // fetched 2 arguments.
  // no closed values in _22Zd68WPKUO_6AoIuBcbUb2.
  // routine _22Zd68WPKUO_6AoIuBcbUb2 body:

startblock__7csjYG7rNV4_68bJZT4W2BO:__attribute__ ((unused));
  {                             /* +block _7csjYG7rNV4_68bJZT4W2BO */
    /*v_str: */ _.v_167WczZjWtG_3FofhJxQWJi =
      //assign _5mCmWSYOW7T_4G9YRC6tvRf  
      /*objdictfirstkeypayl: */
      (value_tyBM)
      objdictfirstkeypayl_BM (( /*odict: */ _.o_7n2SLLuWOjH_4ArhFUjmckz));



//// +while _0SWi4IZCeP3_0VPExGvOasL 
  startblock__0SWi4IZCeP3_0VPExGvOasL: /*+!while */ ;
    {                           /* +while _0SWi4IZCeP3_0VPExGvOasL */
      if (!( /*while _0SWi4IZCeP3_0VPExGvOasL cond: *//*v_str: */ _.v_167WczZjWtG_3FofhJxQWJi)) //: testwhile _0SWi4IZCeP3_0VPExGvOasL failing  
        goto endblock__0SWi4IZCeP3_0VPExGvOasL; //failwhile  
// while _0SWi4IZCeP3_0VPExGvOasL body:  
// while _0SWi4IZCeP3_0VPExGvOasL comp#0:  
      /*v_val: */ _.v_5jRQCD1qTko_61I0j04ugAA =
        //assign _2dCjJpUS20B_3NM9eVa84hQ   
        /*objdictgetpayl: */
        objdictgetpayl_BM (( /*odict: */ _.o_7n2SLLuWOjH_4ArhFUjmckz),
                           stringcast_BM ( /*v_str: */ _.
                                          v_167WczZjWtG_3FofhJxQWJi));
// while _0SWi4IZCeP3_0VPExGvOasL comp#1:  
      {                         // start cexpansion _0LKRJrD5wqK_5ZE495IAA7d - obdumpscanvalue   
        /*obdumpscanvalue: */
        obdumpscanvalue_BM (( /*odumper: */ _.o_3cTC8W7d75H_1vteOyEoAao),
                            ( /*v_val: */ _.v_5jRQCD1qTko_61I0j04ugAA),
                            (1));
      }                         // end cexpansion _0LKRJrD5wqK_5ZE495IAA7d - obdumpscanvalue   
// while _0SWi4IZCeP3_0VPExGvOasL comp#2:  
      /*v_str: */ _.v_167WczZjWtG_3FofhJxQWJi =
        //assign _5UiotUrFshh_9xWCmys30Pe   
        /*objdictkeyafterpayl: */
        (value_tyBM)
        objdictkeyafterpayl_BM (( /*odict: */ _.o_7n2SLLuWOjH_4ArhFUjmckz),
                                stringcast_BM ( /*v_str: */ _.
                                               v_167WczZjWtG_3FofhJxQWJi));

      goto startblock__0SWi4IZCeP3_0VPExGvOasL; // repeatwhile  
    endblock__0SWi4IZCeP3_0VPExGvOasL: /*endingwhile */ ;
    }
  /*-endwhile _0SWi4IZCeP3_0VPExGvOasL */



    /*return _8pmMgF3SSuE_2COzwjyI8lS: *//*v_r: */ _.
      v_3qw9bnATIhq_8loBraLeXzQ =
      // returned 
      /*odict: */ _.o_7n2SLLuWOjH_4ArhFUjmckz;
    goto epilog_22Zd68WPKUO_6AoIuBcbUb2;

/* !endingblock _7csjYG7rNV4_68bJZT4W2BO */
    goto endblock__7csjYG7rNV4_68bJZT4W2BO;
  endblock__7csjYG7rNV4_68bJZT4W2BO:;
  }
  /*-block _7csjYG7rNV4_68bJZT4W2BO */

epilog_22Zd68WPKUO_6AoIuBcbUb2:__attribute__ ((unused));
  // routine _22Zd68WPKUO_6AoIuBcbUb2 epilogue:
  if (stkf)
    stkf->stkfram_callfun = NULL;
  return /*v_r: */ _.v_3qw9bnATIhq_8loBraLeXzQ;
}                               // end _22Zd68WPKUO_6AoIuBcbUb2 routine#3 _22Zd68WPKUO_6AoIuBcbUb2




value_tyBM crout_2bZ4BfCci5V_3WTzG7V25y4_BM     //#4
  (struct stackframe_stBM * stkf,       //
//!put°dict_object minifunc
   // 3 arguments
   const value_tyBM arg0,       // odict
   const value_tyBM arg1,       // v_str
   const value_tyBM arg2,       // v_val
   const value_tyBM arg3_ __attribute__ ((unused)),
   const quasinode_tyBM * restargs_ __attribute__ ((unused)))
{                               // start of _2bZ4BfCci5V_3WTzG7V25y4
  struct frame_2bZ4BfCci5V_3WTzG7V25y4_BMst
  {
    typedhead_tyBM stkfram_head;
    struct stackframe_stBM *stkfram_prev;
    objectval_tyBM *stkfram_descr;
    value_tyBM stkfram_callfun;
    int stkfram_state;
    int stkfram_extra;
    /// 4 local values:
    value_tyBM v_167WczZjWtG_3FofhJxQWJi;       // v_str
    value_tyBM v_3qw9bnATIhq_8loBraLeXzQ;       // v_r
    value_tyBM v_5jRQCD1qTko_61I0j04ugAA;       // v_val
    objectval_tyBM *o_7n2SLLuWOjH_4ArhFUjmckz;  // odict
    /// 0 local numbers:
  } _;
  memset (&_, 0, sizeof (struct frame_2bZ4BfCci5V_3WTzG7V25y4_BMst));
  _.stkfram_head.htyp = typayl_StackFrame_BM;
  _.stkfram_head.hgc = 0;
  _.stkfram_head.rlen = 4;
  _.stkfram_descr =
    constob_3BFt4NfJmZC_7iYi2dwM38B_BM[6] /*|_2bZ4BfCci5V_3WTzG7V25y4 */ ;
  ASSERT_BM (!stkf || stkf->stkfram_pA.htyp == typayl_StackFrame_BM
             || stkf->stkfram_pA.htyp == typayl_SpecialFrame_BM);
  _.stkfram_prev = stkf;
  // fetch 3 arguments:
  _.o_7n2SLLuWOjH_4ArhFUjmckz = objectcast_BM (arg0);   // odict
  _.v_167WczZjWtG_3FofhJxQWJi = arg1;   // v_str
  _.v_5jRQCD1qTko_61I0j04ugAA = arg2;   // v_val
  // fetched 3 arguments.
  // no closed values in _2bZ4BfCci5V_3WTzG7V25y4.
  // routine _2bZ4BfCci5V_3WTzG7V25y4 body:

startblock__0VJTIqFfNXf_5chzuIlqcJP:__attribute__ ((unused));
  {                             /* +block _0VJTIqFfNXf_5chzuIlqcJP */
    if                          /*cond _838sw9thRfn_03qQ7lMn8Ij */
/*when _2aMZ0ZBgKj2_35QKBjbmq4w:*/
      (                         /*objhasdictpayl: */
        objhasdictpayl_BM (( /*odict: */ _.o_7n2SLLuWOjH_4ArhFUjmckz)))
      {                         //body when _2aMZ0ZBgKj2_35QKBjbmq4w  
        if                      /*cond _58oNVcHAUe4_3Z3PsPWQQWC */
/*when _5Zj20zoTMgi_2wX9L155Qet:*/
          (                     /*isstring: */
            isstring_BM ( /*v_str: */ _.v_167WczZjWtG_3FofhJxQWJi))
          {                     //body when _5Zj20zoTMgi_2wX9L155Qet   
            {                   // start cexpansion _7T2FZv58UYu_1HHqslZTor2 - objdictputpayl    
              /*objdictputpayl: */
              objdictputpayl_BM (( /*odict: */ _.o_7n2SLLuWOjH_4ArhFUjmckz),
                                 ( /*v_str: */ _.v_167WczZjWtG_3FofhJxQWJi),
                                 ( /*v_val: */ _.
                                  v_5jRQCD1qTko_61I0j04ugAA));
            }                   // end cexpansion _7T2FZv58UYu_1HHqslZTor2 - objdictputpayl    
            /*return _3Q8xwomyj4D_36oIs4VyEfB: *//*v_r: */ _.
              v_3qw9bnATIhq_8loBraLeXzQ =
              // returned   
              /*odict: */ _.o_7n2SLLuWOjH_4ArhFUjmckz;
            goto epilog_2bZ4BfCci5V_3WTzG7V25y4;

          }                     //endwhen _5Zj20zoTMgi_2wX9L155Qet   
/*endcond _58oNVcHAUe4_3Z3PsPWQQWC*/

      }                         //endwhen _2aMZ0ZBgKj2_35QKBjbmq4w  
/*endcond _838sw9thRfn_03qQ7lMn8Ij*/

/* !endingblock _0VJTIqFfNXf_5chzuIlqcJP */
    goto endblock__0VJTIqFfNXf_5chzuIlqcJP;
  endblock__0VJTIqFfNXf_5chzuIlqcJP:;
  }
  /*-block _0VJTIqFfNXf_5chzuIlqcJP */

epilog_2bZ4BfCci5V_3WTzG7V25y4:__attribute__ ((unused));
  // routine _2bZ4BfCci5V_3WTzG7V25y4 epilogue:
  if (stkf)
    stkf->stkfram_callfun = NULL;
  return /*v_r: */ _.v_3qw9bnATIhq_8loBraLeXzQ;
}                               // end _2bZ4BfCci5V_3WTzG7V25y4 routine#4 _2bZ4BfCci5V_3WTzG7V25y4




value_tyBM crout_3iiht57Wrs1_5NbiUi27oBn_BM     //#5
  (struct stackframe_stBM * stkf,       //
//!prepend°list_object minifunc
   // 2 arguments
   const value_tyBM arg0,       // o1
   const value_tyBM arg1,       // v2
   const value_tyBM arg2_ __attribute__ ((unused)),
   const value_tyBM arg3_ __attribute__ ((unused)),
   const quasinode_tyBM * restargs_ __attribute__ ((unused)))
{                               // start of _3iiht57Wrs1_5NbiUi27oBn
  struct frame_3iiht57Wrs1_5NbiUi27oBn_BMst
  {
    typedhead_tyBM stkfram_head;
    struct stackframe_stBM *stkfram_prev;
    objectval_tyBM *stkfram_descr;
    value_tyBM stkfram_callfun;
    int stkfram_state;
    int stkfram_extra;
    /// 3 local values:
    value_tyBM v_09RxU8xDco3_2Zxwfb13oDZ;       // v2
    value_tyBM v_3qw9bnATIhq_8loBraLeXzQ;       // v_r
    objectval_tyBM *o_97rnxb0KFqq_44veN75DG2E;  // o1
    /// 0 local numbers:
  } _;
  memset (&_, 0, sizeof (struct frame_3iiht57Wrs1_5NbiUi27oBn_BMst));
  _.stkfram_head.htyp = typayl_StackFrame_BM;
  _.stkfram_head.hgc = 0;
  _.stkfram_head.rlen = 3;
  _.stkfram_descr =
    constob_3BFt4NfJmZC_7iYi2dwM38B_BM[7] /*|_3iiht57Wrs1_5NbiUi27oBn */ ;
  ASSERT_BM (!stkf || stkf->stkfram_pA.htyp == typayl_StackFrame_BM
             || stkf->stkfram_pA.htyp == typayl_SpecialFrame_BM);
  _.stkfram_prev = stkf;
  // fetch 2 arguments:
  _.o_97rnxb0KFqq_44veN75DG2E = objectcast_BM (arg0);   // o1
  _.v_09RxU8xDco3_2Zxwfb13oDZ = arg1;   // v2
  // fetched 2 arguments.
  // no closed values in _3iiht57Wrs1_5NbiUi27oBn.
  // routine _3iiht57Wrs1_5NbiUi27oBn body:

startblock__9BLJq5DadtZ_6I89t7ioVRS:__attribute__ ((unused));
  {                             /* +block _9BLJq5DadtZ_6I89t7ioVRS */
    if                          /*cond _2cKxScfQVIk_8j9yMA8y0D2 */
/*when _9Eu315oGSiW_2T0LUr58JnT:*/
      (                         /*objhaslistpayl: */
        objhaslistpayl_BM (( /*o1: */ _.o_97rnxb0KFqq_44veN75DG2E)))
      {                         //body when _9Eu315oGSiW_2T0LUr58JnT  
        {                       // start cexpansion _8K97qV68UiQ_0ssdaZQW9h5 - objlistprependpayl   
          /*objlistprependpayl: */
          objlistprependpayl_BM (( /*o1: */ _.o_97rnxb0KFqq_44veN75DG2E),
                                 ( /*v2: */ _.v_09RxU8xDco3_2Zxwfb13oDZ));
        }                       // end cexpansion _8K97qV68UiQ_0ssdaZQW9h5 - objlistprependpayl   
        /*return _3Hj8WH5oi59_3L07vytBdDJ: *//*v_r: */ _.
          v_3qw9bnATIhq_8loBraLeXzQ =
          // returned  
          /*o1: */ _.o_97rnxb0KFqq_44veN75DG2E;
        goto epilog_3iiht57Wrs1_5NbiUi27oBn;

      }                         //endwhen _9Eu315oGSiW_2T0LUr58JnT  
/*endcond _2cKxScfQVIk_8j9yMA8y0D2*/

/* !endingblock _9BLJq5DadtZ_6I89t7ioVRS */
    goto endblock__9BLJq5DadtZ_6I89t7ioVRS;
  endblock__9BLJq5DadtZ_6I89t7ioVRS:;
  }
  /*-block _9BLJq5DadtZ_6I89t7ioVRS */

epilog_3iiht57Wrs1_5NbiUi27oBn:__attribute__ ((unused));
  // routine _3iiht57Wrs1_5NbiUi27oBn epilogue:
  if (stkf)
    stkf->stkfram_callfun = NULL;
  return /*v_r: */ _.v_3qw9bnATIhq_8loBraLeXzQ;
}                               // end _3iiht57Wrs1_5NbiUi27oBn routine#5 _3iiht57Wrs1_5NbiUi27oBn




value_tyBM crout_3yQlckX4DRh_4b9l9FBSSSL_BM     //#6+
  (struct stackframe_stBM * stkf,       // same_as_closed_minifunc
//!return argument if it is the same as the closed value
   // 1 arguments
   const value_tyBM arg0,       // v_arg
   const value_tyBM arg1_ __attribute__ ((unused)),
   const value_tyBM arg2_ __attribute__ ((unused)),
   const value_tyBM arg3_ __attribute__ ((unused)),
   const quasinode_tyBM * restargs_ __attribute__ ((unused)))
{                               // start of same_as_closed_minifunc
  struct frame_3yQlckX4DRh_4b9l9FBSSSL_BMst
  {
    typedhead_tyBM stkfram_head;
    struct stackframe_stBM *stkfram_prev;
    objectval_tyBM *stkfram_descr;
    value_tyBM stkfram_callfun;
    int stkfram_state;
    int stkfram_extra;
    /// 3 local values:
    value_tyBM v_3qw9bnATIhq_8loBraLeXzQ;       // v_r
    value_tyBM v_4Q4WBLjAPTV_5WVLmhAvCJY;       // v_arg
    value_tyBM v_5EEITf6IFH7_4DbKG99zcPW;       // v1
    /// 0 local numbers:
  } _;
  memset (&_, 0, sizeof (struct frame_3yQlckX4DRh_4b9l9FBSSSL_BMst));
  _.stkfram_head.htyp = typayl_StackFrame_BM;
  _.stkfram_head.hgc = 0;
  _.stkfram_head.rlen = 3;
  _.stkfram_descr =
    constob_3BFt4NfJmZC_7iYi2dwM38B_BM[8] /*|same_as_closed_minifunc */ ;
  ASSERT_BM (!stkf || stkf->stkfram_pA.htyp == typayl_StackFrame_BM
             || stkf->stkfram_pA.htyp == typayl_SpecialFrame_BM);
  _.stkfram_prev = stkf;
  // fetch 1 arguments:
  _.v_4Q4WBLjAPTV_5WVLmhAvCJY = arg0;   // v_arg
  // fetched 1 arguments.
  {                             // fetch 1 closed values in same_as_closed_minifunc:
    const closure_tyBM *callclos_3yQlckX4DRh_4b9l9FBSSSL =
      (stkf && stkf->stkfram_pA.htyp == typayl_StackFrame_BM)
      ? (closurecast_BM (stkf->stkfram_callfun)) : NULL;
    unsigned nbclosed_3yQlckX4DRh_4b9l9FBSSSL =
      closurewidth_BM ((const value_tyBM) callclos_3yQlckX4DRh_4b9l9FBSSSL);
    if (nbclosed_3yQlckX4DRh_4b9l9FBSSSL > 0)
      _.v_5EEITf6IFH7_4DbKG99zcPW = callclos_3yQlckX4DRh_4b9l9FBSSSL->nodt_sons[0];     // v1
  }                             // fetched 1 closed values in _3yQlckX4DRh_4b9l9FBSSSL.
  // routine _3yQlckX4DRh_4b9l9FBSSSL body:

startblock__8seRhfNtGTR_4rOgARd0hVM:__attribute__ ((unused));
  {                             /* +block _8seRhfNtGTR_4rOgARd0hVM */
    if                          /*cond _8aGb7reuzLX_3sgkigRSOpu */
/*when _5Ad0ZdurQuC_1A6Kn7qopbm:*/
      ((                        /*same_val: */
         ( /*v1: */ _.v_5EEITf6IFH7_4DbKG99zcPW) ==
         ( /*v_arg: */ _.v_4Q4WBLjAPTV_5WVLmhAvCJY)))
      {                         //body when _5Ad0ZdurQuC_1A6Kn7qopbm  
        /*return _1qFf9AUO2W9_8azcQljqB3I: *//*v_r: */ _.
          v_3qw9bnATIhq_8loBraLeXzQ =
          // returned  
          /*v_arg: */ _.v_4Q4WBLjAPTV_5WVLmhAvCJY;
        goto epilog_3yQlckX4DRh_4b9l9FBSSSL;

      }                         //endwhen _5Ad0ZdurQuC_1A6Kn7qopbm  
/*endcond _8aGb7reuzLX_3sgkigRSOpu*/

/* !endingblock _8seRhfNtGTR_4rOgARd0hVM */
    goto endblock__8seRhfNtGTR_4rOgARd0hVM;
  endblock__8seRhfNtGTR_4rOgARd0hVM:;
  }
  /*-block _8seRhfNtGTR_4rOgARd0hVM */

epilog_3yQlckX4DRh_4b9l9FBSSSL:__attribute__ ((unused));
  // routine _3yQlckX4DRh_4b9l9FBSSSL epilogue:
  if (stkf)
    stkf->stkfram_callfun = NULL;
  return /*v_r: */ _.v_3qw9bnATIhq_8loBraLeXzQ;
}                               // end _3yQlckX4DRh_4b9l9FBSSSL routine#6 same_as_closed_minifunc




value_tyBM crout_3HAeZZQ0q7b_7qyDsekaz7Y_BM     //#7
  (struct stackframe_stBM * stkf,       //
//!clear°list_object minifunc
   // 1 arguments
   const value_tyBM arg0,       // o1
   const value_tyBM arg1_ __attribute__ ((unused)),
   const value_tyBM arg2_ __attribute__ ((unused)),
   const value_tyBM arg3_ __attribute__ ((unused)),
   const quasinode_tyBM * restargs_ __attribute__ ((unused)))
{                               // start of _3HAeZZQ0q7b_7qyDsekaz7Y
  struct frame_3HAeZZQ0q7b_7qyDsekaz7Y_BMst
  {
    typedhead_tyBM stkfram_head;
    struct stackframe_stBM *stkfram_prev;
    objectval_tyBM *stkfram_descr;
    value_tyBM stkfram_callfun;
    int stkfram_state;
    int stkfram_extra;
    /// 2 local values:
    value_tyBM v_3qw9bnATIhq_8loBraLeXzQ;       // v_r
    objectval_tyBM *o_97rnxb0KFqq_44veN75DG2E;  // o1
    /// 0 local numbers:
  } _;
  memset (&_, 0, sizeof (struct frame_3HAeZZQ0q7b_7qyDsekaz7Y_BMst));
  _.stkfram_head.htyp = typayl_StackFrame_BM;
  _.stkfram_head.hgc = 0;
  _.stkfram_head.rlen = 2;
  _.stkfram_descr =
    constob_3BFt4NfJmZC_7iYi2dwM38B_BM[9] /*|_3HAeZZQ0q7b_7qyDsekaz7Y */ ;
  ASSERT_BM (!stkf || stkf->stkfram_pA.htyp == typayl_StackFrame_BM
             || stkf->stkfram_pA.htyp == typayl_SpecialFrame_BM);
  _.stkfram_prev = stkf;
  // fetch 1 arguments:
  _.o_97rnxb0KFqq_44veN75DG2E = objectcast_BM (arg0);   // o1
  // fetched 1 arguments.
  // no closed values in _3HAeZZQ0q7b_7qyDsekaz7Y.
  // routine _3HAeZZQ0q7b_7qyDsekaz7Y body:

startblock__4wSCE1PqaMS_2XYzFgB0waZ:__attribute__ ((unused));
  {                             /* +block _4wSCE1PqaMS_2XYzFgB0waZ */
    if                          /*cond _3eTPhEBroKh_3F8wQMduy1J */
/*when _0IRlnxNrCtM_6W21xnJjzmn:*/
      (                         /*objputlistpayl: */
        objputlistpayl_BM ( /*o1: */ _.o_97rnxb0KFqq_44veN75DG2E))
      {                         //body when _0IRlnxNrCtM_6W21xnJjzmn  
        /*return _516tXf2eeNb_8VGPLYEode5: *//*v_r: */ _.
          v_3qw9bnATIhq_8loBraLeXzQ =
          // returned  
          /*o1: */ _.o_97rnxb0KFqq_44veN75DG2E;
        goto epilog_3HAeZZQ0q7b_7qyDsekaz7Y;

      }                         //endwhen _0IRlnxNrCtM_6W21xnJjzmn  
/*endcond _3eTPhEBroKh_3F8wQMduy1J*/

/* !endingblock _4wSCE1PqaMS_2XYzFgB0waZ */
    goto endblock__4wSCE1PqaMS_2XYzFgB0waZ;
  endblock__4wSCE1PqaMS_2XYzFgB0waZ:;
  }
  /*-block _4wSCE1PqaMS_2XYzFgB0waZ */

epilog_3HAeZZQ0q7b_7qyDsekaz7Y:__attribute__ ((unused));
  // routine _3HAeZZQ0q7b_7qyDsekaz7Y epilogue:
  if (stkf)
    stkf->stkfram_callfun = NULL;
  return /*v_r: */ _.v_3qw9bnATIhq_8loBraLeXzQ;
}                               // end _3HAeZZQ0q7b_7qyDsekaz7Y routine#7 _3HAeZZQ0q7b_7qyDsekaz7Y




value_tyBM crout_4iEFTEcHxeb_6lH464uFkTC_BM     //#8+
  (struct stackframe_stBM * stkf,       // equal_to_closed_minifunc
//!return argument if it is the same as the closed value
   // 1 arguments
   const value_tyBM arg0,       // v_arg
   const value_tyBM arg1_ __attribute__ ((unused)),
   const value_tyBM arg2_ __attribute__ ((unused)),
   const value_tyBM arg3_ __attribute__ ((unused)),
   const quasinode_tyBM * restargs_ __attribute__ ((unused)))
{                               // start of equal_to_closed_minifunc
  struct frame_4iEFTEcHxeb_6lH464uFkTC_BMst
  {
    typedhead_tyBM stkfram_head;
    struct stackframe_stBM *stkfram_prev;
    objectval_tyBM *stkfram_descr;
    value_tyBM stkfram_callfun;
    int stkfram_state;
    int stkfram_extra;
    /// 3 local values:
    value_tyBM v_3qw9bnATIhq_8loBraLeXzQ;       // v_r
    value_tyBM v_4Q4WBLjAPTV_5WVLmhAvCJY;       // v_arg
    value_tyBM v_5EEITf6IFH7_4DbKG99zcPW;       // v1
    /// 0 local numbers:
  } _;
  memset (&_, 0, sizeof (struct frame_4iEFTEcHxeb_6lH464uFkTC_BMst));
  _.stkfram_head.htyp = typayl_StackFrame_BM;
  _.stkfram_head.hgc = 0;
  _.stkfram_head.rlen = 3;
  _.stkfram_descr =
    constob_3BFt4NfJmZC_7iYi2dwM38B_BM[10] /*|equal_to_closed_minifunc */ ;
  ASSERT_BM (!stkf || stkf->stkfram_pA.htyp == typayl_StackFrame_BM
             || stkf->stkfram_pA.htyp == typayl_SpecialFrame_BM);
  _.stkfram_prev = stkf;
  // fetch 1 arguments:
  _.v_4Q4WBLjAPTV_5WVLmhAvCJY = arg0;   // v_arg
  // fetched 1 arguments.
  {                             // fetch 1 closed values in equal_to_closed_minifunc:
    const closure_tyBM *callclos_4iEFTEcHxeb_6lH464uFkTC =
      (stkf && stkf->stkfram_pA.htyp == typayl_StackFrame_BM)
      ? (closurecast_BM (stkf->stkfram_callfun)) : NULL;
    unsigned nbclosed_4iEFTEcHxeb_6lH464uFkTC =
      closurewidth_BM ((const value_tyBM) callclos_4iEFTEcHxeb_6lH464uFkTC);
    if (nbclosed_4iEFTEcHxeb_6lH464uFkTC > 0)
      _.v_5EEITf6IFH7_4DbKG99zcPW = callclos_4iEFTEcHxeb_6lH464uFkTC->nodt_sons[0];     // v1
  }                             // fetched 1 closed values in _4iEFTEcHxeb_6lH464uFkTC.
  // routine _4iEFTEcHxeb_6lH464uFkTC body:

startblock__60FKeHfNUo3_9OSN4Cqm8zI:__attribute__ ((unused));
  {                             /* +block _60FKeHfNUo3_9OSN4Cqm8zI */
    if                          /*cond _7wPbopKi9ZB_1ifk5LEKSqu */
/*when _8hUpJbSLQUS_88aqogJAi2Q:*/
      ((                        /*equal_val: */
         valequal_BM (( /*v1: */ _.v_5EEITf6IFH7_4DbKG99zcPW),
                      ( /*v_arg: */ _.v_4Q4WBLjAPTV_5WVLmhAvCJY))))
      {                         //body when _8hUpJbSLQUS_88aqogJAi2Q  
        /*return _3dJ2b1hmBuw_7Pu5iDjauLr: *//*v_r: */ _.
          v_3qw9bnATIhq_8loBraLeXzQ =
          // returned  
          /*v1: */ _.v_5EEITf6IFH7_4DbKG99zcPW;
        goto epilog_4iEFTEcHxeb_6lH464uFkTC;

      }                         //endwhen _8hUpJbSLQUS_88aqogJAi2Q  
/*endcond _7wPbopKi9ZB_1ifk5LEKSqu*/

/* !endingblock _60FKeHfNUo3_9OSN4Cqm8zI */
    goto endblock__60FKeHfNUo3_9OSN4Cqm8zI;
  endblock__60FKeHfNUo3_9OSN4Cqm8zI:;
  }
  /*-block _60FKeHfNUo3_9OSN4Cqm8zI */

epilog_4iEFTEcHxeb_6lH464uFkTC:__attribute__ ((unused));
  // routine _4iEFTEcHxeb_6lH464uFkTC epilogue:
  if (stkf)
    stkf->stkfram_callfun = NULL;
  return /*v_r: */ _.v_3qw9bnATIhq_8loBraLeXzQ;
}                               // end _4iEFTEcHxeb_6lH464uFkTC routine#8 equal_to_closed_minifunc




value_tyBM crout_4qlegfkPzrC_17skUfJP7v8_BM     //#9
  (struct stackframe_stBM * stkf,       //
//!dump_data°dict_object minifun
   // 3 arguments
   const value_tyBM arg0,       // odict
   const value_tyBM arg1,       // odumper
   const value_tyBM arg2,       // osbuf
   const value_tyBM arg3_ __attribute__ ((unused)),
   const quasinode_tyBM * restargs_ __attribute__ ((unused)))
{                               // start of _4qlegfkPzrC_17skUfJP7v8
  struct frame_4qlegfkPzrC_17skUfJP7v8_BMst
  {
    typedhead_tyBM stkfram_head;
    struct stackframe_stBM *stkfram_prev;
    objectval_tyBM *stkfram_descr;
    value_tyBM stkfram_callfun;
    int stkfram_state;
    int stkfram_extra;
    /// 6 local values:
    value_tyBM v_167WczZjWtG_3FofhJxQWJi;       // v_str
    objectval_tyBM *o_3cTC8W7d75H_1vteOyEoAao;  // odumper
    value_tyBM v_3qw9bnATIhq_8loBraLeXzQ;       // v_r
    value_tyBM v_5jRQCD1qTko_61I0j04ugAA;       // v_val
    objectval_tyBM *o_6opEz55NyNu_3JKpBgxLijw;  // osbuf
    objectval_tyBM *o_7n2SLLuWOjH_4ArhFUjmckz;  // odict
    /// 0 local numbers:
  } _;
  memset (&_, 0, sizeof (struct frame_4qlegfkPzrC_17skUfJP7v8_BMst));
  _.stkfram_head.htyp = typayl_StackFrame_BM;
  _.stkfram_head.hgc = 0;
  _.stkfram_head.rlen = 6;
  _.stkfram_descr =
    constob_3BFt4NfJmZC_7iYi2dwM38B_BM[11] /*|_4qlegfkPzrC_17skUfJP7v8 */ ;
  ASSERT_BM (!stkf || stkf->stkfram_pA.htyp == typayl_StackFrame_BM
             || stkf->stkfram_pA.htyp == typayl_SpecialFrame_BM);
  _.stkfram_prev = stkf;
  // fetch 3 arguments:
  _.o_7n2SLLuWOjH_4ArhFUjmckz = objectcast_BM (arg0);   // odict
  _.o_3cTC8W7d75H_1vteOyEoAao = objectcast_BM (arg1);   // odumper
  _.o_6opEz55NyNu_3JKpBgxLijw = objectcast_BM (arg2);   // osbuf
  // fetched 3 arguments.
  // no closed values in _4qlegfkPzrC_17skUfJP7v8.
  // routine _4qlegfkPzrC_17skUfJP7v8 body:

startblock__3oMW0T4HlXx_8r7NLpyK1J8:__attribute__ ((unused));
  {                             /* +block _3oMW0T4HlXx_8r7NLpyK1J8 */
    {                           // start cexpansion _4QfB21rab5P_26PMGm9N4Dl - objstrbufferclearindentpayl  
      /*objstrbufferclearindentpayl: */
      objstrbufferclearindentpayl_BM (( /*osbuf: */ _.
                                       o_6opEz55NyNu_3JKpBgxLijw));
    }                           // end cexpansion _4QfB21rab5P_26PMGm9N4Dl - objstrbufferclearindentpayl  


    {                           // start cexpansion _15WmvoAu2Kx_9K4XFDxMyU4 - objstrbufferappendcstrpayl  
      /*objstrbufferappendcstrpayl: */
      objstrbufferappendcstrpayl_BM (( /*osbuf: */ _.
                                      o_6opEz55NyNu_3JKpBgxLijw),
                                     ("!~ todo (~\n"));
    }                           // end cexpansion _15WmvoAu2Kx_9K4XFDxMyU4 - objstrbufferappendcstrpayl  

//+ #2/9 of block _3oMW0T4HlXx_8r7NLpyK1J8 ::  

    /*v_str: */ _.v_167WczZjWtG_3FofhJxQWJi =
      //assign _5mCmWSYOW7T_4G9YRC6tvRf  
      /*objdictfirstkeypayl: */
      (value_tyBM)
      objdictfirstkeypayl_BM (( /*odict: */ _.o_7n2SLLuWOjH_4ArhFUjmckz));


    {                           // start cexpansion _3pnZ6MbNCtX_2IZKwMgn9y1 - objstrbuffersetindentpayl  
      /*objstrbuffersetindentpayl: */
      objstrbuffersetindentpayl_BM (( /*osbuf: */ _.
                                     o_6opEz55NyNu_3JKpBgxLijw), (1));
    }                           // end cexpansion _3pnZ6MbNCtX_2IZKwMgn9y1 - objstrbuffersetindentpayl  

//+ #4/9 of block _3oMW0T4HlXx_8r7NLpyK1J8 ::  


    {                           // run _40XgQGIUZQY_246JbHmppCN 
      (send3_BM (((constob_3BFt4NfJmZC_7iYi2dwM38B_BM[18] /*|clear */ )),
                 /*sel: */
                 ((constob_3BFt4NfJmZC_7iYi2dwM38B_BM[4] /*|dump_value */ )),
                 ((struct stackframe_stBM *) &_),
                 ( /*osbuf: */ _.o_6opEz55NyNu_3JKpBgxLijw),
                 ( /*odumper: */ _.o_3cTC8W7d75H_1vteOyEoAao),
                 (((value_tyBM) 0x3 /*!1 */ ))));
    }                           // end run _40XgQGIUZQY_246JbHmppCN 



//// +while _9gJT0jotyox_8JQNlJn3qh6 
  startblock__9gJT0jotyox_8JQNlJn3qh6: /*+!while */ ;
    {                           /* +while _9gJT0jotyox_8JQNlJn3qh6 */
      if (!( /*while _9gJT0jotyox_8JQNlJn3qh6 cond: *//*isstring: */ isstring_BM ( /*v_str: */ _.v_167WczZjWtG_3FofhJxQWJi)))   //: testwhile _9gJT0jotyox_8JQNlJn3qh6 failing   
        goto endblock__9gJT0jotyox_8JQNlJn3qh6; //failwhile   
// while _9gJT0jotyox_8JQNlJn3qh6 body:   
// while _9gJT0jotyox_8JQNlJn3qh6 comp#0:  
      /*v_val: */ _.v_5jRQCD1qTko_61I0j04ugAA =
        //assign _6mzawBHvh0i_13Wpf0Lmgig   
        /*objdictgetpayl: */
        objdictgetpayl_BM (( /*odict: */ _.o_7n2SLLuWOjH_4ArhFUjmckz),
                           stringcast_BM ( /*v_str: */ _.
                                          v_167WczZjWtG_3FofhJxQWJi));
// while _9gJT0jotyox_8JQNlJn3qh6 comp#1:  
      {                         // start cexpansion _8QIiIH1xKGD_0ByaWRWwCAO - objstrbufferappendcstrpayl   
        /*objstrbufferappendcstrpayl: */
        objstrbufferappendcstrpayl_BM (( /*osbuf: */ _.
                                        o_6opEz55NyNu_3JKpBgxLijw),
                                       (" !&\n"));
      }                         // end cexpansion _8QIiIH1xKGD_0ByaWRWwCAO - objstrbufferappendcstrpayl   
// while _9gJT0jotyox_8JQNlJn3qh6 comp#2:  

      {                         // run _4BOu5jhqyAD_3HGfVAY5hFx  
        (send3_BM (((constob_3BFt4NfJmZC_7iYi2dwM38B_BM[19] /*|put */ )),
                   /*sel: */
                   ((constob_3BFt4NfJmZC_7iYi2dwM38B_BM[4]      /*|dump_value */
                    )), ((struct stackframe_stBM *) &_),
                   ( /*osbuf: */ _.o_6opEz55NyNu_3JKpBgxLijw),
                   ( /*odumper: */ _.o_3cTC8W7d75H_1vteOyEoAao),
                   (((value_tyBM) 0x3 /*!1 */ ))));
      }                         // end run _4BOu5jhqyAD_3HGfVAY5hFx  
// while _9gJT0jotyox_8JQNlJn3qh6 comp#3:  
      {                         // start cexpansion _8TB4Kpuoc3f_0Dhb8Tm0xrJ - objstrbufferappendcstrpayl   
        /*objstrbufferappendcstrpayl: */
        objstrbufferappendcstrpayl_BM (( /*osbuf: */ _.
                                        o_6opEz55NyNu_3JKpBgxLijw), ("\t"));
      }                         // end cexpansion _8TB4Kpuoc3f_0Dhb8Tm0xrJ - objstrbufferappendcstrpayl   
// while _9gJT0jotyox_8JQNlJn3qh6 comp#4:  

      {                         // run _634ecJogXve_7sycLMYAfdT  
        (send3_BM (( /*v_str: */ _.v_167WczZjWtG_3FofhJxQWJi),  /*sel: */
                   ((constob_3BFt4NfJmZC_7iYi2dwM38B_BM[4]      /*|dump_value */
                    )),
                   ((struct stackframe_stBM *) &_),
                   ( /*osbuf: */ _.o_6opEz55NyNu_3JKpBgxLijw),
                   ( /*odumper: */ _.o_3cTC8W7d75H_1vteOyEoAao),
                   (((value_tyBM) 0x3 /*!1 */ ))));
      }                         // end run _634ecJogXve_7sycLMYAfdT  
// while _9gJT0jotyox_8JQNlJn3qh6 comp#5:  
      {                         // start cexpansion _3fFn9uY0hNS_2YU6fn9QOSh - objstrbuffernewlinepayl   
        /*objstrbuffernewlinepayl: */ objstrbuffernewlinepayl_BM (
                                                                   /*osbuf: */
                                                                   _.o_6opEz55NyNu_3JKpBgxLijw);
      }                         // end cexpansion _3fFn9uY0hNS_2YU6fn9QOSh - objstrbuffernewlinepayl   
// while _9gJT0jotyox_8JQNlJn3qh6 comp#6:  

      {                         // run _1aLOinNfz2n_0WpWbC84CAL  
        (send3_BM (( /*v_val: */ _.v_5jRQCD1qTko_61I0j04ugAA),  /*sel: */
                   ((constob_3BFt4NfJmZC_7iYi2dwM38B_BM[4]      /*|dump_value */
                    )),
                   ((struct stackframe_stBM *) &_),
                   ( /*osbuf: */ _.o_6opEz55NyNu_3JKpBgxLijw),
                   ( /*odumper: */ _.o_3cTC8W7d75H_1vteOyEoAao),
                   (((value_tyBM) 0x3 /*!1 */ ))));
      }                         // end run _1aLOinNfz2n_0WpWbC84CAL  
// while _9gJT0jotyox_8JQNlJn3qh6 comp#7:  
      /*v_str: */ _.v_167WczZjWtG_3FofhJxQWJi =
        //assign _5VEaDZcNd5x_9CGhCudUvGs   
        /*objdictkeyafterpayl: */
        (value_tyBM)
        objdictkeyafterpayl_BM (( /*odict: */ _.o_7n2SLLuWOjH_4ArhFUjmckz),
                                stringcast_BM ( /*v_str: */ _.
                                               v_167WczZjWtG_3FofhJxQWJi));

      goto startblock__9gJT0jotyox_8JQNlJn3qh6; // repeatwhile  
    endblock__9gJT0jotyox_8JQNlJn3qh6: /*endingwhile */ ;
    }
  /*-endwhile _9gJT0jotyox_8JQNlJn3qh6 */


//+ #6/9 of block _3oMW0T4HlXx_8r7NLpyK1J8 :: 

    {                           // start cexpansion _1UfGW420Q3X_3osvimj3Myj - objstrbufferclearindentpayl  
      /*objstrbufferclearindentpayl: */
      objstrbufferclearindentpayl_BM (( /*osbuf: */ _.
                                       o_6opEz55NyNu_3JKpBgxLijw));
    }                           // end cexpansion _1UfGW420Q3X_3osvimj3Myj - objstrbufferclearindentpayl  


    {                           // start cexpansion _3urNBgu4IsT_4oyZ9YgJmbx - objstrbufferappendcstrpayl  
      /*objstrbufferappendcstrpayl: */
      objstrbufferappendcstrpayl_BM (( /*osbuf: */ _.
                                      o_6opEz55NyNu_3JKpBgxLijw), ("\n~)\n"));
    }                           // end cexpansion _3urNBgu4IsT_4oyZ9YgJmbx - objstrbufferappendcstrpayl  


    /*return _9IH3fglmCon_9bHWhFstP4K: *//*v_r: */ _.
      v_3qw9bnATIhq_8loBraLeXzQ =
      // returned 
      /*odict: */ _.o_7n2SLLuWOjH_4ArhFUjmckz;
    goto epilog_4qlegfkPzrC_17skUfJP7v8;

/* !endingblock _3oMW0T4HlXx_8r7NLpyK1J8 */
    goto endblock__3oMW0T4HlXx_8r7NLpyK1J8;
  endblock__3oMW0T4HlXx_8r7NLpyK1J8:;
  }
  /*-block _3oMW0T4HlXx_8r7NLpyK1J8 */

epilog_4qlegfkPzrC_17skUfJP7v8:__attribute__ ((unused));
  // routine _4qlegfkPzrC_17skUfJP7v8 epilogue:
  if (stkf)
    stkf->stkfram_callfun = NULL;
  return /*v_r: */ _.v_3qw9bnATIhq_8loBraLeXzQ;
}                               // end _4qlegfkPzrC_17skUfJP7v8 routine#9 _4qlegfkPzrC_17skUfJP7v8




value_tyBM crout_4M7lr5cU6VD_5hxmeH3baBf_BM     //#10
  (struct stackframe_stBM * stkf,       //
//!append°list_object minifunction
   // 2 arguments
   const value_tyBM arg0,       // o1
   const value_tyBM arg1,       // v2
   const value_tyBM arg2_ __attribute__ ((unused)),
   const value_tyBM arg3_ __attribute__ ((unused)),
   const quasinode_tyBM * restargs_ __attribute__ ((unused)))
{                               // start of _4M7lr5cU6VD_5hxmeH3baBf
  struct frame_4M7lr5cU6VD_5hxmeH3baBf_BMst
  {
    typedhead_tyBM stkfram_head;
    struct stackframe_stBM *stkfram_prev;
    objectval_tyBM *stkfram_descr;
    value_tyBM stkfram_callfun;
    int stkfram_state;
    int stkfram_extra;
    /// 3 local values:
    value_tyBM v_09RxU8xDco3_2Zxwfb13oDZ;       // v2
    value_tyBM v_3qw9bnATIhq_8loBraLeXzQ;       // v_r
    objectval_tyBM *o_97rnxb0KFqq_44veN75DG2E;  // o1
    /// 0 local numbers:
  } _;
  memset (&_, 0, sizeof (struct frame_4M7lr5cU6VD_5hxmeH3baBf_BMst));
  _.stkfram_head.htyp = typayl_StackFrame_BM;
  _.stkfram_head.hgc = 0;
  _.stkfram_head.rlen = 3;
  _.stkfram_descr =
    constob_3BFt4NfJmZC_7iYi2dwM38B_BM[12] /*|_4M7lr5cU6VD_5hxmeH3baBf */ ;
  ASSERT_BM (!stkf || stkf->stkfram_pA.htyp == typayl_StackFrame_BM
             || stkf->stkfram_pA.htyp == typayl_SpecialFrame_BM);
  _.stkfram_prev = stkf;
  // fetch 2 arguments:
  _.o_97rnxb0KFqq_44veN75DG2E = objectcast_BM (arg0);   // o1
  _.v_09RxU8xDco3_2Zxwfb13oDZ = arg1;   // v2
  // fetched 2 arguments.
  // no closed values in _4M7lr5cU6VD_5hxmeH3baBf.
  // routine _4M7lr5cU6VD_5hxmeH3baBf body:

startblock__883QaWUOedh_9zmHk2A3RPm:__attribute__ ((unused));
  {                             /* +block _883QaWUOedh_9zmHk2A3RPm */
    if                          /*cond _9l1kqklfeho_8gvTSpVHJs0 */
/*when _4bt3mp8e1a2_7XsOs7auvrU:*/
      (                         /*objhaslistpayl: */
        objhaslistpayl_BM (( /*o1: */ _.o_97rnxb0KFqq_44veN75DG2E)))
      {                         //body when _4bt3mp8e1a2_7XsOs7auvrU  
        {                       // start cexpansion _8WKnIBYQNvy_0t8M4F48joR - objlistappendpayl   
          /*objlistappendpayl: */
          objlistappendpayl_BM (( /*o1: */ _.o_97rnxb0KFqq_44veN75DG2E),
                                ( /*v2: */ _.v_09RxU8xDco3_2Zxwfb13oDZ));
        }                       // end cexpansion _8WKnIBYQNvy_0t8M4F48joR - objlistappendpayl   
        /*return _86rGd2LLytX_4xsXHoCmw1Y: *//*v_r: */ _.
          v_3qw9bnATIhq_8loBraLeXzQ =
          // returned  
          /*o1: */ _.o_97rnxb0KFqq_44veN75DG2E;
        goto epilog_4M7lr5cU6VD_5hxmeH3baBf;

      }                         //endwhen _4bt3mp8e1a2_7XsOs7auvrU  
/*endcond _9l1kqklfeho_8gvTSpVHJs0*/

/* !endingblock _883QaWUOedh_9zmHk2A3RPm */
    goto endblock__883QaWUOedh_9zmHk2A3RPm;
  endblock__883QaWUOedh_9zmHk2A3RPm:;
  }
  /*-block _883QaWUOedh_9zmHk2A3RPm */

epilog_4M7lr5cU6VD_5hxmeH3baBf:__attribute__ ((unused));
  // routine _4M7lr5cU6VD_5hxmeH3baBf epilogue:
  if (stkf)
    stkf->stkfram_callfun = NULL;
  return /*v_r: */ _.v_3qw9bnATIhq_8loBraLeXzQ;
}                               // end _4M7lr5cU6VD_5hxmeH3baBf routine#10 _4M7lr5cU6VD_5hxmeH3baBf




value_tyBM crout_5j8LpQULt1p_9oeOaqmfYYr_BM     //#11
  (struct stackframe_stBM * stkf,       //
//!dump_data°list_object minifunc
   // 3 arguments
   const value_tyBM arg0,       // o1
   const value_tyBM arg1,       // o2
   const value_tyBM arg2,       // o3
   const value_tyBM arg3_ __attribute__ ((unused)),
   const quasinode_tyBM * restargs_ __attribute__ ((unused)))
{                               // start of _5j8LpQULt1p_9oeOaqmfYYr
  struct frame_5j8LpQULt1p_9oeOaqmfYYr_BMst
  {
    typedhead_tyBM stkfram_head;
    struct stackframe_stBM *stkfram_prev;
    objectval_tyBM *stkfram_descr;
    value_tyBM stkfram_callfun;
    int stkfram_state;
    int stkfram_extra;
    /// 5 local values:
    objectval_tyBM *o_1Bl1iFQ8boR_1xM6MOfHV1N;  // o3
    value_tyBM v_2BkOxf2KbS8_3bswrKbmgjy;       // v_node
    value_tyBM v_3qw9bnATIhq_8loBraLeXzQ;       // v_r
    objectval_tyBM *o_93FBSk1ML0f_2WjSuKa9TNU;  // o2
    objectval_tyBM *o_97rnxb0KFqq_44veN75DG2E;  // o1
    /// 2 local numbers:
    intptr_t n_2W7Ckt2WxEM_4NVYgEWE73B; // ii
    intptr_t n_833x2jYqhZh_8vDIuJhwz1k; // leni
  } _;
  memset (&_, 0, sizeof (struct frame_5j8LpQULt1p_9oeOaqmfYYr_BMst));
  _.stkfram_head.htyp = typayl_StackFrame_BM;
  _.stkfram_head.hgc = 0;
  _.stkfram_head.rlen = 5;
  _.stkfram_descr =
    constob_3BFt4NfJmZC_7iYi2dwM38B_BM[13] /*|_5j8LpQULt1p_9oeOaqmfYYr */ ;
  ASSERT_BM (!stkf || stkf->stkfram_pA.htyp == typayl_StackFrame_BM
             || stkf->stkfram_pA.htyp == typayl_SpecialFrame_BM);
  _.stkfram_prev = stkf;
  // fetch 3 arguments:
  _.o_97rnxb0KFqq_44veN75DG2E = objectcast_BM (arg0);   // o1
  _.o_93FBSk1ML0f_2WjSuKa9TNU = objectcast_BM (arg1);   // o2
  _.o_1Bl1iFQ8boR_1xM6MOfHV1N = objectcast_BM (arg2);   // o3
  // fetched 3 arguments.
  // no closed values in _5j8LpQULt1p_9oeOaqmfYYr.
  // routine _5j8LpQULt1p_9oeOaqmfYYr body:

startblock__9Hf4n8AnqR7_7ve3Bi3kWMV:__attribute__ ((unused));
  {                             /* +block _9Hf4n8AnqR7_7ve3Bi3kWMV */
    /*v_node: */ _.v_2BkOxf2KbS8_3bswrKbmgjy =
      //assign _8ggHPTQI1iG_02hVE5KybEL  
      /*objlisttonodepayl: */
      (value_tyBM)
      objlisttonodepayl_BM (( /*o1: */ _.o_97rnxb0KFqq_44veN75DG2E),
                            ((constob_3BFt4NfJmZC_7iYi2dwM38B_BM[15]
                              /*|node */ )));


    /*leni: */ _.n_833x2jYqhZh_8vDIuJhwz1k =
      //assign _0zhpGzEVWGP_7WoL4LJocuS  
      /*nodewidth: */
      (intptr_t) nodewidth_BM ( /*v_node: */ _.v_2BkOxf2KbS8_3bswrKbmgjy);

//+ #2/8 of block _9Hf4n8AnqR7_7ve3Bi3kWMV :: 

    /*ii: */ _.n_2W7Ckt2WxEM_4NVYgEWE73B =
      //assign _0fygl91r6x5_8b3aCDptjAi  
      0;


    {                           // start cexpansion _6yLpzYpq8F0_4ogwWJhDGnH - objstrbufferappendcstrpayl  
      /*objstrbufferappendcstrpayl: */
      objstrbufferappendcstrpayl_BM (( /*o3: */ _.
                                      o_1Bl1iFQ8boR_1xM6MOfHV1N),
                                     ("!~ todo (~"));
    }                           // end cexpansion _6yLpzYpq8F0_4ogwWJhDGnH - objstrbufferappendcstrpayl  

//+ #4/8 of block _9Hf4n8AnqR7_7ve3Bi3kWMV ::  


//// +while _9xjh4MB97y4_2d8ffcaNFYt 
  startblock__9xjh4MB97y4_2d8ffcaNFYt: /*+!while */ ;
    {                           /* +while _9xjh4MB97y4_2d8ffcaNFYt */
      if (!( /*while _9xjh4MB97y4_2d8ffcaNFYt cond: */ ( /*less_int: */ ( /*ii: */ _.n_2W7Ckt2WxEM_4NVYgEWE73B) < ( /*leni: */ _.n_833x2jYqhZh_8vDIuJhwz1k))))  //: testwhile _9xjh4MB97y4_2d8ffcaNFYt failing   
        goto endblock__9xjh4MB97y4_2d8ffcaNFYt; //failwhile   
// while _9xjh4MB97y4_2d8ffcaNFYt body:   
// while _9xjh4MB97y4_2d8ffcaNFYt comp#0:  
      {                         // start cexpansion _63n9WJxlgsW_7aRQkxcfyFp - objstrbuffersetindentpayl   
        /*objstrbuffersetindentpayl: */
        objstrbuffersetindentpayl_BM (( /*o3: */ _.
                                       o_1Bl1iFQ8boR_1xM6MOfHV1N), (1));
      }                         // end cexpansion _63n9WJxlgsW_7aRQkxcfyFp - objstrbuffersetindentpayl   
// while _9xjh4MB97y4_2d8ffcaNFYt comp#1:  
      {                         // start cexpansion _4L5hoWsh9Dl_112aUlLRWBa - objstrbuffernewlinepayl   
        /*objstrbuffernewlinepayl: */ objstrbuffernewlinepayl_BM ( /*o3: */ _.
                                                                  o_1Bl1iFQ8boR_1xM6MOfHV1N);
      }                         // end cexpansion _4L5hoWsh9Dl_112aUlLRWBa - objstrbuffernewlinepayl   
// while _9xjh4MB97y4_2d8ffcaNFYt comp#2:  
      {                         // start cexpansion _4ExmfQs4chn_6b5RsLwzSmp - objstrbufferappendcstrpayl   
        /*objstrbufferappendcstrpayl: */
        objstrbufferappendcstrpayl_BM (( /*o3: */ _.
                                        o_1Bl1iFQ8boR_1xM6MOfHV1N), (" &!"));
      }                         // end cexpansion _4ExmfQs4chn_6b5RsLwzSmp - objstrbufferappendcstrpayl   
// while _9xjh4MB97y4_2d8ffcaNFYt comp#3:  

      {                         // run _6HYDs1Zywvs_1w8OTn5XOZY  
        (send3_BM ((            /*nodenthson: */
                     nodenthson_BM (( /*v_node: */ _.
                                     v_2BkOxf2KbS8_3bswrKbmgjy), ( /*ii: */ _.n_2W7Ckt2WxEM_4NVYgEWE73B))),     /*sel: */
                   ((constob_3BFt4NfJmZC_7iYi2dwM38B_BM[4]      /*|dump_value */
                    )),
                   ((struct stackframe_stBM *) &_),
                   ( /*o3: */ _.o_1Bl1iFQ8boR_1xM6MOfHV1N),
                   ( /*o2: */ _.o_93FBSk1ML0f_2WjSuKa9TNU),
                   (((value_tyBM) 0x3 /*!1 */ ))));
      }                         // end run _6HYDs1Zywvs_1w8OTn5XOZY  
// while _9xjh4MB97y4_2d8ffcaNFYt comp#4:  
      /*ii: */ _.n_2W7Ckt2WxEM_4NVYgEWE73B =
        //assign _4nPRrjCznxv_354tRTC5amu   
        /*add_int: */ (( /*ii: */ _.n_2W7Ckt2WxEM_4NVYgEWE73B) + (1));

      goto startblock__9xjh4MB97y4_2d8ffcaNFYt; // repeatwhile  
    endblock__9xjh4MB97y4_2d8ffcaNFYt: /*endingwhile */ ;
    }
  /*-endwhile _9xjh4MB97y4_2d8ffcaNFYt */



    {                           // start cexpansion _39EI0paA4hR_8y7JANajq5t - objstrbuffernewlinepayl  
      /*objstrbuffernewlinepayl: */ objstrbuffernewlinepayl_BM ( /*o3: */ _.
                                                                o_1Bl1iFQ8boR_1xM6MOfHV1N);
    }                           // end cexpansion _39EI0paA4hR_8y7JANajq5t - objstrbuffernewlinepayl  

//+ #6/8 of block _9Hf4n8AnqR7_7ve3Bi3kWMV ::  

    {                           // start cexpansion _5flu8xdOTMC_79tshj3uSa4 - objstrbuffersetindentpayl  
      /*objstrbuffersetindentpayl: */
      objstrbuffersetindentpayl_BM (( /*o3: */ _.o_1Bl1iFQ8boR_1xM6MOfHV1N),
                                    (0));
    }                           // end cexpansion _5flu8xdOTMC_79tshj3uSa4 - objstrbuffersetindentpayl  


    {                           // start cexpansion _5YxDaNaTjQ7_0v9tZ2seGmF - objstrbufferappendcstrpayl  
      /*objstrbufferappendcstrpayl: */
      objstrbufferappendcstrpayl_BM (( /*o3: */ _.
                                      o_1Bl1iFQ8boR_1xM6MOfHV1N), ("~)"));
    }                           // end cexpansion _5YxDaNaTjQ7_0v9tZ2seGmF - objstrbufferappendcstrpayl  

/* !endingblock _9Hf4n8AnqR7_7ve3Bi3kWMV */
    goto endblock__9Hf4n8AnqR7_7ve3Bi3kWMV;
  endblock__9Hf4n8AnqR7_7ve3Bi3kWMV:;
  }
  /*-block _9Hf4n8AnqR7_7ve3Bi3kWMV */

epilog_5j8LpQULt1p_9oeOaqmfYYr:__attribute__ ((unused));
  // routine _5j8LpQULt1p_9oeOaqmfYYr epilogue:
  if (stkf)
    stkf->stkfram_callfun = NULL;
  return /*v_r: */ _.v_3qw9bnATIhq_8loBraLeXzQ;
}                               // end _5j8LpQULt1p_9oeOaqmfYYr routine#11 _5j8LpQULt1p_9oeOaqmfYYr




value_tyBM crout_82ho9wUUDji_4peMLha4PXl_BM     //#12+
  (struct stackframe_stBM * stkf,       // findobj_scan_minifunc
//!findobj_scan_minifunc scans, with the closed and locked o_findobj, the argument object o_scan; give NULL if o_scan was already scanned
   // 1 arguments
   const value_tyBM arg0,       // o_scan
   const value_tyBM arg1_ __attribute__ ((unused)),
   const value_tyBM arg2_ __attribute__ ((unused)),
   const value_tyBM arg3_ __attribute__ ((unused)),
   const quasinode_tyBM * restargs_ __attribute__ ((unused)))
{                               // start of findobj_scan_minifunc
  struct frame_82ho9wUUDji_4peMLha4PXl_BMst
  {
    typedhead_tyBM stkfram_head;
    struct stackframe_stBM *stkfram_prev;
    objectval_tyBM *stkfram_descr;
    value_tyBM stkfram_callfun;
    int stkfram_state;
    int stkfram_extra;
    /// 4 local values:
    value_tyBM v_3qw9bnATIhq_8loBraLeXzQ;       // v_r
    objectval_tyBM *o_4gykCX1QdLb_9Aooqa2FWTq;  // o_scan
    objectval_tyBM *o_4ITcQtvlMmO_30Pss9oAVDB;  // o_findobj
    objectval_tyBM *o_6Jkz6rphVkn_1KZto79j46r;  // o_vihset
    /// 0 local numbers:
  } _;
  memset (&_, 0, sizeof (struct frame_82ho9wUUDji_4peMLha4PXl_BMst));
  _.stkfram_head.htyp = typayl_StackFrame_BM;
  _.stkfram_head.hgc = 0;
  _.stkfram_head.rlen = 4;
  _.stkfram_descr =
    constob_3BFt4NfJmZC_7iYi2dwM38B_BM[16] /*|findobj_scan_minifunc */ ;
  ASSERT_BM (!stkf || stkf->stkfram_pA.htyp == typayl_StackFrame_BM
             || stkf->stkfram_pA.htyp == typayl_SpecialFrame_BM);
  _.stkfram_prev = stkf;
  // fetch 1 arguments:
  _.o_4gykCX1QdLb_9Aooqa2FWTq = objectcast_BM (arg0);   // o_scan
  // fetched 1 arguments.
  {                             // fetch 1 closed values in findobj_scan_minifunc:
    const closure_tyBM *callclos_82ho9wUUDji_4peMLha4PXl =
      (stkf && stkf->stkfram_pA.htyp == typayl_StackFrame_BM)
      ? (closurecast_BM (stkf->stkfram_callfun)) : NULL;
    unsigned nbclosed_82ho9wUUDji_4peMLha4PXl =
      closurewidth_BM ((const value_tyBM) callclos_82ho9wUUDji_4peMLha4PXl);
    if (nbclosed_82ho9wUUDji_4peMLha4PXl > 0)
      _.o_4ITcQtvlMmO_30Pss9oAVDB = objectcast_BM (callclos_82ho9wUUDji_4peMLha4PXl->nodt_sons[0]);     // o_findobj
  }                             // fetched 1 closed values in _82ho9wUUDji_4peMLha4PXl.
  // routine _82ho9wUUDji_4peMLha4PXl body:

startblock__6YnYKKsHo3f_9nIEOZMp744:__attribute__ ((unused));
  {                             /* +block _6YnYKKsHo3f_9nIEOZMp744 */

    {                           // run _7m9brpKxZkY_3BEI7YANJuJ 
      /*begin findobj_scan_minifunc */
      WEAKASSERT_BM (objectisinstance_BM
                     ( /*o_findobj: */ _.o_4ITcQtvlMmO_30Pss9oAVDB,
                      (constob_3BFt4NfJmZC_7iYi2dwM38B_BM[14]
                       /*|findrun_object */ )));;
    }                           // end run _7m9brpKxZkY_3BEI7YANJuJ 

/* !endingblock _6YnYKKsHo3f_9nIEOZMp744 */
    goto endblock__6YnYKKsHo3f_9nIEOZMp744;
  endblock__6YnYKKsHo3f_9nIEOZMp744:;
  }
  /*-block _6YnYKKsHo3f_9nIEOZMp744 */

epilog_82ho9wUUDji_4peMLha4PXl:__attribute__ ((unused));
  // routine _82ho9wUUDji_4peMLha4PXl epilogue:
  if (stkf)
    stkf->stkfram_callfun = NULL;
  return /*v_r: */ _.v_3qw9bnATIhq_8loBraLeXzQ;
}                               // end _82ho9wUUDji_4peMLha4PXl routine#12 findobj_scan_minifunc




value_tyBM crout_8imWClcIFKt_6w2H2ouB4I6_BM     //#13+
  (struct stackframe_stBM * stkf,       // iterate_sequence_minifunc
//!iterate on sequence v_seq the function v_fun with 2nd argument v_arg
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
    constob_3BFt4NfJmZC_7iYi2dwM38B_BM[17] /*|iterate_sequence_minifunc */ ;
  ASSERT_BM (!stkf || stkf->stkfram_pA.htyp == typayl_StackFrame_BM
             || stkf->stkfram_pA.htyp == typayl_SpecialFrame_BM);
  _.stkfram_prev = stkf;
  // fetch 3 arguments:
  _.v_9vc0quxdnhj_3zLw1MEwl1e = arg0;   // v_seq
  _.v_8jKgolslEuE_4dxzrK5R2B4 = arg1;   // v_fun
  _.v_4Q4WBLjAPTV_5WVLmhAvCJY = arg2;   // v_arg
  // fetched 3 arguments.
  // no closed values in iterate_sequence_minifunc.
  // routine _8imWClcIFKt_6w2H2ouB4I6 body:

startblock__2W6NObLb2d4_6OfkP3dYlF3:__attribute__ ((unused));
  {                             /* +block _2W6NObLb2d4_6OfkP3dYlF3 */
    /*leni: */ _.n_833x2jYqhZh_8vDIuJhwz1k =
      //assign _3EakP4EXPLZ_10btDL0iqAt  
      /*seqlength */
      sequencesize_BM ( /*v_seq: */ _.v_9vc0quxdnhj_3zLw1MEwl1e);


    /*ii: */ _.n_2W7Ckt2WxEM_4NVYgEWE73B =
      //assign _1Z2mN7VIEbD_4RJunrL2QcV  
      0;



  startblock__3vvVJBONOyK_3kn86nuYwr1:__attribute__ ((unused));
    {                           /* +loop _3vvVJBONOyK_3kn86nuYwr1 */
    startloop__3vvVJBONOyK_3kn86nuYwr1:__attribute__ ((unused));


      if                        /*cond _7pBaII8qZHM_8xoaIKeilBk */
/*when _8qsLnTz2QdD_9nZub2hBMEb:*/
        ((                      /*greaterequal_int: */
           ( /*ii: */ _.n_2W7Ckt2WxEM_4NVYgEWE73B) >=
           ( /*leni: */ _.n_833x2jYqhZh_8vDIuJhwz1k)))
        {                       //body when _8qsLnTz2QdD_9nZub2hBMEb   
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
  return /*v_r: */ _.v_3qw9bnATIhq_8loBraLeXzQ;
}                               // end _8imWClcIFKt_6w2H2ouB4I6 routine#13 iterate_sequence_minifunc




value_tyBM crout_9CG8SKNs6Ql_4PiHd8cnydn_BM     //#14
  (struct stackframe_stBM * stkf,       //
//!clear°dict_object minifun
   // 1 arguments
   const value_tyBM arg0,       // odict
   const value_tyBM arg1_ __attribute__ ((unused)),
   const value_tyBM arg2_ __attribute__ ((unused)),
   const value_tyBM arg3_ __attribute__ ((unused)),
   const quasinode_tyBM * restargs_ __attribute__ ((unused)))
{                               // start of _9CG8SKNs6Ql_4PiHd8cnydn
  struct frame_9CG8SKNs6Ql_4PiHd8cnydn_BMst
  {
    typedhead_tyBM stkfram_head;
    struct stackframe_stBM *stkfram_prev;
    objectval_tyBM *stkfram_descr;
    value_tyBM stkfram_callfun;
    int stkfram_state;
    int stkfram_extra;
    /// 2 local values:
    value_tyBM v_3qw9bnATIhq_8loBraLeXzQ;       // v_r
    objectval_tyBM *o_7n2SLLuWOjH_4ArhFUjmckz;  // odict
    /// 0 local numbers:
  } _;
  memset (&_, 0, sizeof (struct frame_9CG8SKNs6Ql_4PiHd8cnydn_BMst));
  _.stkfram_head.htyp = typayl_StackFrame_BM;
  _.stkfram_head.hgc = 0;
  _.stkfram_head.rlen = 2;
  _.stkfram_descr =
    constob_3BFt4NfJmZC_7iYi2dwM38B_BM[20] /*|_9CG8SKNs6Ql_4PiHd8cnydn */ ;
  ASSERT_BM (!stkf || stkf->stkfram_pA.htyp == typayl_StackFrame_BM
             || stkf->stkfram_pA.htyp == typayl_SpecialFrame_BM);
  _.stkfram_prev = stkf;
  // fetch 1 arguments:
  _.o_7n2SLLuWOjH_4ArhFUjmckz = objectcast_BM (arg0);   // odict
  // fetched 1 arguments.
  // no closed values in _9CG8SKNs6Ql_4PiHd8cnydn.
  // routine _9CG8SKNs6Ql_4PiHd8cnydn body:

startblock__1lmLI285f7p_4im4Ls4Xewh:__attribute__ ((unused));
  {                             /* +block _1lmLI285f7p_4im4Ls4Xewh */
    if                          /*cond _58n2IHCyrnJ_19R71AKw5aF */
/*when _8QkV0tL7Jvt_9mc0QaZYbyv:*/
      (                         /*objputdictpayl: */
        objputdictpayl_BM (( /*odict: */ _.o_7n2SLLuWOjH_4ArhFUjmckz)))
      {                         //body when _8QkV0tL7Jvt_9mc0QaZYbyv  
        /*return _2RCw0JQb23v_2NfFku112jv: *//*v_r: */ _.
          v_3qw9bnATIhq_8loBraLeXzQ =
          // returned  
          /*odict: */ _.o_7n2SLLuWOjH_4ArhFUjmckz;
        goto epilog_9CG8SKNs6Ql_4PiHd8cnydn;

      }                         //endwhen _8QkV0tL7Jvt_9mc0QaZYbyv  
/*endcond _58n2IHCyrnJ_19R71AKw5aF*/


    if                          /*cond _4QR8kuWiKxV_9oKMeD79KDQ */
/*when _8bCPs7gML9M_12nXMrdhzvW:*/
      (                         /*objputdictpayl: */
        objputdictpayl_BM (( /*odict: */ _.o_7n2SLLuWOjH_4ArhFUjmckz)))
      {                         //body when _8bCPs7gML9M_12nXMrdhzvW  
        /*return _9198YxnrBs6_7u9nhmAA8Q6: *//*v_r: */ _.
          v_3qw9bnATIhq_8loBraLeXzQ =
          // returned  
          /*odict: */ _.o_7n2SLLuWOjH_4ArhFUjmckz;
        goto epilog_9CG8SKNs6Ql_4PiHd8cnydn;

      }                         //endwhen _8bCPs7gML9M_12nXMrdhzvW  
/*endcond _4QR8kuWiKxV_9oKMeD79KDQ*/

/* !endingblock _1lmLI285f7p_4im4Ls4Xewh */
    goto endblock__1lmLI285f7p_4im4Ls4Xewh;
  endblock__1lmLI285f7p_4im4Ls4Xewh:;
  }
  /*-block _1lmLI285f7p_4im4Ls4Xewh */

epilog_9CG8SKNs6Ql_4PiHd8cnydn:__attribute__ ((unused));
  // routine _9CG8SKNs6Ql_4PiHd8cnydn epilogue:
  if (stkf)
    stkf->stkfram_callfun = NULL;
  return /*v_r: */ _.v_3qw9bnATIhq_8loBraLeXzQ;
}                               // end _9CG8SKNs6Ql_4PiHd8cnydn routine#14 _9CG8SKNs6Ql_4PiHd8cnydn




// end of 15 generated routines


// the constant ids for 21 constants:
const char *const constid_3BFt4NfJmZC_7iYi2dwM38B_BM[21 + 1] = {
  /*0: */
  "_0JktC4mAHCa_1KNr6uMIqIi",   //! dump_scan°list_object minifunc for list_object o1 & dumperobj o2
  "_0SinERsgdvQ_57dLCP63i9r",   //! forgetname#command_handler
  "_0YOXUe7rEJC_3jFMjOAQRs9",   //element_of_closed_minifunc
  "_1dzZlwqKdLH_8HjkBsL9Mar",   //add
  "_1FEnnpEkGdI_5DAcVDL5XHG",   //dump_value
  /*5: */
  "_22Zd68WPKUO_6AoIuBcbUb2",   //! dump_scan°dict_object minifunc for dict-object odict and dumperobj odumper
  "_2bZ4BfCci5V_3WTzG7V25y4",   //! put°dict_object minifunc
  "_3iiht57Wrs1_5NbiUi27oBn",   //! prepend°list_object minifunc
  "_3yQlckX4DRh_4b9l9FBSSSL",   //same_as_closed_minifunc
  "_3HAeZZQ0q7b_7qyDsekaz7Y",   //! clear°list_object minifunc
  /*10: */
  "_4iEFTEcHxeb_6lH464uFkTC",   //equal_to_closed_minifunc
  "_4qlegfkPzrC_17skUfJP7v8",   //! dump_data°dict_object minifun
  "_4M7lr5cU6VD_5hxmeH3baBf",   //! append°list_object minifunction
  "_5j8LpQULt1p_9oeOaqmfYYr",   //! dump_data°list_object minifunc
  "_64UbCFBD19G_43TeBXhcYMy",   //findrun_object
  /*15: */
  "_7D8xcWnEiys_8oqOVSkCxkA",   //node
  "_82ho9wUUDji_4peMLha4PXl",   //findobj_scan_minifunc
  "_8imWClcIFKt_6w2H2ouB4I6",   //iterate_sequence_minifunc
  "_8lV53mLyu1H_3Z1SI8wDBK1",   //clear
  "_9pvzBeIKHXF_8YDPCrQ6OEK",   //put
  /*20: */
  "_9CG8SKNs6Ql_4PiHd8cnydn",   //! clear°dict_object minifun
  NULL
};                              // end 21 constant ids



// the routine ids for 15 routines:
const char *const routid_3BFt4NfJmZC_7iYi2dwM38B_BM[15 + 1] = {
  /*0: */
  "_0JktC4mAHCa_1KNr6uMIqIi",   //!dump_scan°list_object minifunc for list_object o1 & dumperobj o2
  "_0SinERsgdvQ_57dLCP63i9r",   //!forgetname#command_handler
  "_0YOXUe7rEJC_3jFMjOAQRs9",   //element_of_closed_minifunc
  "_22Zd68WPKUO_6AoIuBcbUb2",   //!dump_scan°dict_object minifunc for dict-object odict and dumperobj odumper
  "_2bZ4BfCci5V_3WTzG7V25y4",   //!put°dict_object minifunc;
  "_3iiht57Wrs1_5NbiUi27oBn",   //!prepend°list_object minifunc
  "_3yQlckX4DRh_4b9l9FBSSSL",   //same_as_closed_minifunc
  "_3HAeZZQ0q7b_7qyDsekaz7Y",   //!clear°list_object minifunc
  /*8: */
  "_4iEFTEcHxeb_6lH464uFkTC",   //equal_to_closed_minifunc
  "_4qlegfkPzrC_17skUfJP7v8",   //!dump_data°dict_object minifunc
  "_4M7lr5cU6VD_5hxmeH3baBf",   //!append°list_object minifunction
  "_5j8LpQULt1p_9oeOaqmfYYr",   //!dump_data°list_object minifunc;
  "_82ho9wUUDji_4peMLha4PXl",   //findobj_scan_minifunc
  "_8imWClcIFKt_6w2H2ouB4I6",   //iterate_sequence_minifunc
  "_9CG8SKNs6Ql_4PiHd8cnydn",   //!clear°dict_object minifunc

  NULL
};                              // end 15 routine ids



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



// end of generated persistent module first_misc_module in file modbm_3BFt4NfJmZC_7iYi2dwM38B.c
