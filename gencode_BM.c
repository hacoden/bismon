// file gencode_BM.c
#include "bismon.h"


//// for the method prepare_routine:basiclo_function
extern objrout_sigBM ROUTINEOBJNAME_BM (_5mnsT1wsdWs_2Qnqsf3wqaP);

value_tyBM
ROUTINEOBJNAME_BM (_5mnsT1wsdWs_2Qnqsf3wqaP)    // prepare_routine:basiclo_function
(const closure_tyBM * clos,
 struct stackframe_stBM * stkf,
 const value_tyBM arg1,
 const value_tyBM arg2, const value_tyBM arg3,
 const quasinode_tyBM * restargs)
{
  enum
  {
    constix_arguments,
    constix_block,
    constix_locals,
    constix_constants,
    constix_simple_routine_preparation,
    constix_body,
    constix_bindings,
    constix__LAST
  };
  LOCALFRAME_BM ( /*prev: */ stkf, /*descr: */ NULL,
                 const closure_tyBM * clos; value_tyBM recv;
                 objectval_tyBM * modgenob;
                 value_tyBM args; value_tyBM curarg;
                 objectval_tyBM * routprepob; value_tyBM prepvalset;
                 anyassoc_tyBM * assocbind;
    );
  assert (isclosure_BM ((const value_tyBM) clos));
  _.clos = clos;
  const objectval_tyBM *closconn = closureconn_BM ((const value_tyBM) clos);
  assert (closconn != NULL);
  const node_tyBM *constnod = nodecast_BM (objpayload_BM (closconn));
  /*** constnod is
      * const (arguments block locals constants simple_routine_preparation)
   ***/
  WEAKASSERT_BM (isnode_BM ((value_tyBM) constnod)
                 && valhash_BM ((value_tyBM) constnod) == 286038615
                 && nodewidth_BM ((value_tyBM) constnod) == constix__LAST);
  const objectval_tyBM *k_arguments =   //
    objectcast_BM (nodenthson_BM
                   ((const value_tyBM) constnod, constix_arguments));
  const objectval_tyBM *k_block =       //
    objectcast_BM (nodenthson_BM
                   ((const value_tyBM) constnod, constix_block));
  const objectval_tyBM *k_locals =      //
    objectcast_BM (nodenthson_BM
                   ((const value_tyBM) constnod, constix_locals));
  const objectval_tyBM *k_constants =   //
    objectcast_BM (nodenthson_BM
                   ((const value_tyBM) constnod, constix_constants));
  const objectval_tyBM *k_simple_routine_preparation =  //
    objectcast_BM (nodenthson_BM ((const value_tyBM) constnod,
                                  constix_simple_routine_preparation));
  const objectval_tyBM *k_body =        //
    objectcast_BM (nodenthson_BM ((const value_tyBM) constnod,
                                  constix_body));
  const objectval_tyBM *k_bindings =    //
    objectcast_BM (nodenthson_BM ((const value_tyBM) constnod,
                                  constix_bindings));
  // retrieve arguments
  _.recv = /*function object */ (arg1);
  _.modgenob = /*module generation object */ objectcast_BM (arg2);
  _.prepvalset = /* set of routines */ (value_tyBM) setcast_BM (arg3);
  DBGPRINTF_BM
    ("start prepare_routine:basiclo_function _5mnsT1wsdWs_2Qnqsf3wqaP recv=%s",
     objectdbg_BM (_.recv));
  DBGPRINTF_BM
    ("prepare_routine:basiclo_function modgenob=%s is a %s\n",
     objectdbg_BM (_.modgenob), objectdbg1_BM (objclass_BM (_.modgenob)));
  ////
  /// body: _36MhHxakHKT_1ZzV5Xoj5LX  
  /// assign _3jiDSyFdbdk_2PQpjqDkEUJ:
  _.routprepob = makeobj_BM ();
  /// assign _0cBpCT3lEHS_338HjZpF4UE:
  _.assocbind = make_assoc_BM (10);
  /// run _2hJWe55ydpv_5kaGdOUymFI:
#warning the putattr of bindings is wrong, since an assoc
  objputattr_BM (_.routprepob, k_bindings, _.assocbind);
  /// assign _9YS82HkgghD_71BLsDAKRfg
  _.args = objgetattr_BM (_.recv, k_arguments);
  /// foreach_in_tuple(args curarg curargix args ...) _0UqUZ7mQGG5_69QlCmcSwTi
  unsigned nbargs = tuplesize_BM (_.args);
  for (unsigned curargix = 0; curargix < nbargs; curargix++)
    {
      _.curarg = tuplecompnth_BM (_.args, curargix);
      if (assoc_getattr_BM (_.assocbind, _.curarg))
        {
        }
    }
#warning incomplete  prepare_routine:basiclo_function _5mnsT1wsdWs_2Qnqsf3wqaP
  return NULL;
}                               /* end  prepare_routine:basiclo_function _5mnsT1wsdWs_2Qnqsf3wqaP */



//// for the method emit_c_type:c_opaque
extern objrout_sigBM ROUTINEOBJNAME_BM (_6gRlN8loM4E_4pSsNmiCeIa);

value_tyBM
ROUTINEOBJNAME_BM (_6gRlN8loM4E_4pSsNmiCeIa)    // emit_c_type:c_opaque
(const closure_tyBM * clos,
 struct stackframe_stBM * stkf,
 const value_tyBM arg1,
 const value_tyBM arg2, const value_tyBM arg3,
 const quasinode_tyBM * restargs)
{
  LOCALFRAME_BM ( /*prev: */ stkf, /*descr: */ NULL,
                 const closure_tyBM * clos; value_tyBM recv;
                 objectval_tyBM * bufob;
    );
  assert (isclosure_BM ((const value_tyBM) clos));
  _.clos = clos;
  // retrieve arguments
  _.recv = /*ctype */ (arg1);
  WEAKASSERT_BM (isobject_BM (_.recv));
  _.bufob = objectcast_BM (arg2);
  WEAKASSERT_BM (objhasstrbuffer_BM (_.bufob));
  DBGPRINTF_BM
    ("start  emit_c_type:c_opaque _6gRlN8loM4E_4pSsNmiCeIa recv=%s",
     objectdbg_BM (_.recv));
  const char *nam = findobjectname_BM (_.recv);
  char idbuf[32] = { };
  idtocbuf32_BM (objid_BM (_.recv), idbuf);
  if (nam)
    {
      objstrbufferprintf_BM (_.bufob, "\n// opaque named type\n");
      objstrbufferprintf_BM (_.bufob, "typedef void* %s_TyBM;\n", nam);
      objstrbufferprintf_BM (_.bufob, "#define %s_TyBM %s_TyBM\n", idbuf,
                             nam);
    }
  else
    {
      objstrbufferprintf_BM (_.bufob, "\n// opaque anonymous type\n");
      objstrbufferprintf_BM (_.bufob, "typedef void* %s_TyBM;\n", idbuf);
    }
  return _.recv;
}                               /* end emit_c_type:c_opaque _6gRlN8loM4E_4pSsNmiCeIa */




//// for the method emit_c_type:c_enum
extern objrout_sigBM ROUTINEOBJNAME_BM (_979hCujWp2g_9tjRbM8Ht1l);

value_tyBM
ROUTINEOBJNAME_BM (_979hCujWp2g_9tjRbM8Ht1l)    // emit_c_type:c_enum
(const closure_tyBM * clos,
 struct stackframe_stBM * stkf,
 const value_tyBM arg1,
 const value_tyBM arg2, const value_tyBM arg3,
 const quasinode_tyBM * restargs)
{
  enum
  {
    constix_extending,
    constix_own_enumeration,
    constix_enumeration,
    constix__LAST
  };
  LOCALFRAME_BM ( /*prev: */ stkf, /*descr: */ NULL,
                 const closure_tyBM * clos;
                 value_tyBM recv;
                 objectval_tyBM * prbufob;
                 const objectval_tyBM * extendingob;
                 const tupleval_tyBM * ownenumtup;
    );
  assert (isclosure_BM ((const value_tyBM) clos));
  _.clos = clos;
  // retrieve arguments
  _.recv = /*ctype */ (arg1);
  WEAKASSERT_BM (isobject_BM (_.recv));
  WEAKASSERT_BM (objhasstrbuffer_BM (arg2));
  _.prbufob = objectcast_BM (arg2);
  DBGPRINTF_BM
    ("start emit_c_type:c_enum _979hCujWp2g_9tjRbM8Ht1l recv=%s",
     objectdbg_BM (_.recv));
  const objectval_tyBM *closconn = closureconn_BM ((const value_tyBM) clos);
  assert (closconn != NULL);
  const node_tyBM *constnod = nodecast_BM (objpayload_BM (closconn));
  /*** constnod is
      * const (extending own_enumeration enumeration)
   ***/
  WEAKASSERT_BM (isnode_BM ((value_tyBM) constnod)
                 && valhash_BM ((value_tyBM) constnod) == 1519557249
                 && nodewidth_BM ((value_tyBM) constnod) == constix__LAST);
  const objectval_tyBM *k_extending =   //
    objectcast_BM (nodenthson_BM
                   ((const value_tyBM) constnod, constix_extending));
  const objectval_tyBM *k_own_enumeration =     //
    objectcast_BM (nodenthson_BM
                   ((const value_tyBM) constnod, constix_own_enumeration));
  const objectval_tyBM *k_enumeration = //
    objectcast_BM (nodenthson_BM
                   ((const value_tyBM) constnod, constix_enumeration));
  //
  const char *nam = findobjectname_BM (_.recv);
  char idbuf[32] = { };
  idtocbuf32_BM (objid_BM (_.recv), idbuf);
  _.extendingob = objectcast_BM (objgetattr_BM (_.recv, k_extending));
  _.ownenumtup = tuplecast_BM (objgetattr_BM (_.recv, k_own_enumeration));
#warning emit_c_type:c_enum _979hCujWp2g_9tjRbM8Ht1l unimplemented
  return NULL;
}                               /* end emit_c_type:c_enum _979hCujWp2g_9tjRbM8Ht1l  */


////////////////

//// for the method prepare_routine°basiclo_minifunction
extern objrout_sigBM ROUTINEOBJNAME_BM (_07qYMXftJRR_9dde2ASz4e9);

value_tyBM
ROUTINEOBJNAME_BM (_07qYMXftJRR_9dde2ASz4e9)    //  prepare_routine°basiclo_minifunction
(const closure_tyBM * clos,
 struct stackframe_stBM * stkf,
 const value_tyBM arg1,
 const value_tyBM arg2, const value_tyBM arg3,
 const quasinode_tyBM * restargs)
{
  enum
  {
    constix_arguments,
    constix_constants,
    constix_result,
    constix_closed,
    constix_locals,
    constix_numbers,
    constix_body,
    constix_simple_routine_preparation,
    constix_hset_object,
    constix_blocks,
    constix_prepare_routine,
    constix_in,
    constix_collect_blocks,
    constix__LAST
  };
  LOCALFRAME_BM ( /*prev: */ stkf, /*descr: */ NULL,
                 const closure_tyBM * clos; objectval_tyBM * modgen;
                 setval_tyBM * prepvalset; objectval_tyBM * curprepob;
                 objectval_tyBM * routprep; objectval_tyBM * obhsetblock;
                 value_tyBM recv;
#warning routassoc is not a value, but an extendedval
                 anyassoc_tyBM * routassoc;
                 const tupleval_tyBM * tupargs;
                 const tupleval_tyBM * tupclosed;
                 objectval_tyBM * obresult;
                 const setval_tyBM * setlocals;
                 const setval_tyBM * setnumbers;
                 const setval_tyBM * setconsts;
                 objectval_tyBM * curvar; value_tyBM curol; value_tyBM oldrol;
                 value_tyBM bodyv;
                 value_tyBM collbl;
    );
  assert (isclosure_BM ((const value_tyBM) clos));
  _.clos = clos;
  const objectval_tyBM *closconn = closureconn_BM ((const value_tyBM) clos);
  assert (closconn != NULL);
  const node_tyBM *constnod = nodecast_BM (objpayload_BM (closconn));
  /*** constnod is
      * const (arguments result constants closed locals numbers 
               body simple_routine_preparation
	       hset_object blocks prepare_routine in
	       collect_blocks)
   ***/
  WEAKASSERT_BM (isnode_BM ((value_tyBM) constnod)
                 && valhash_BM ((value_tyBM) constnod) == 3438389410
                 && nodewidth_BM ((value_tyBM) constnod) == constix__LAST);
  const objectval_tyBM *k_arguments =   //
    objectcast_BM (nodenthson_BM
                   ((const value_tyBM) constnod, constix_arguments));
  const objectval_tyBM *k_constants =   //
    objectcast_BM (nodenthson_BM
                   ((const value_tyBM) constnod, constix_constants));
  const objectval_tyBM *k_result =      //
    objectcast_BM (nodenthson_BM
                   ((const value_tyBM) constnod, constix_result));
  const objectval_tyBM *k_closed =      //
    objectcast_BM (nodenthson_BM
                   ((const value_tyBM) constnod, constix_closed));
  const objectval_tyBM *k_locals =      //
    objectcast_BM (nodenthson_BM
                   ((const value_tyBM) constnod, constix_locals));
  const objectval_tyBM *k_numbers =     //
    objectcast_BM (nodenthson_BM
                   ((const value_tyBM) constnod, constix_numbers));
  const objectval_tyBM *k_body =        //
    objectcast_BM (nodenthson_BM ((const value_tyBM) constnod, constix_body));
  const objectval_tyBM *k_simple_routine_preparation =  //
    objectcast_BM (nodenthson_BM ((const value_tyBM) constnod,
                                  constix_simple_routine_preparation));
  const objectval_tyBM *k_hset_object = //
    objectcast_BM (nodenthson_BM ((const value_tyBM) constnod,
                                  constix_hset_object));
  const objectval_tyBM *k_blocks =      //
    objectcast_BM (nodenthson_BM ((const value_tyBM) constnod,
                                  constix_blocks));
  const objectval_tyBM *k_prepare_routine =     //
    objectcast_BM (nodenthson_BM ((const value_tyBM) constnod,
                                  constix_prepare_routine));
  const objectval_tyBM *k_in =  //
    objectcast_BM (nodenthson_BM ((const value_tyBM) constnod,
                                  constix_in));
  const objectval_tyBM *k_collect_blocks =      //
    objectcast_BM (nodenthson_BM ((const value_tyBM) constnod,
                                  constix_collect_blocks));
  // retrieve arguments
  _.recv = (arg1);
  WEAKASSERT_BM (isobject_BM (_.recv));
  _.modgen = objectcast_BM (arg2);
  _.prepvalset = setcast_BM (arg3);     /// unneeded, set of all routines
  WEAKASSERT_BM (_.modgen != NULL);
  WEAKASSERT_BM (_.prepvalset != NULL);
  unsigned nbprep = setcardinal_BM (_.prepvalset);
  DBGPRINTF_BM
    ("start prepare_routine°basiclo_minifunction _07qYMXftJRR_9dde2ASz4e9 recv=%s modgen=%s nbprep=%u",
     objectdbg_BM (_.recv), objectdbg1_BM (_.modgen), nbprep);
  _.tupargs = tuplecast_BM (objgetattr_BM (_.recv, k_arguments));
  _.tupclosed = tuplecast_BM (objgetattr_BM (_.recv, k_closed));
  _.obresult = objectcast_BM (objgetattr_BM (_.recv, k_result));
  _.setlocals = setcast_BM (objgetattr_BM (_.recv, k_locals));
  _.setnumbers = setcast_BM (objgetattr_BM (_.recv, k_numbers));
  _.setconsts = setcast_BM (objgetattr_BM (_.recv, k_constants));
  _.bodyv = objgetattr_BM (_.recv, k_body);
  if (!isobject_BM (_.bodyv))
    {
      fprintf (stderr, "bad body minifunction %s\n", objectdbg1_BM (_.recv));
      return NULL;
    }
  _.routprep = makeobj_BM ();
  objputclass_BM (_.routprep,
                  (objectval_tyBM *) k_simple_routine_preparation);
  unsigned nbargs = tuplesize_BM (_.tupargs);
  unsigned nbclosed = tuplesize_BM (_.tupclosed);
  unsigned nblocals = setcardinal_BM (_.setlocals);
  unsigned nbnumbers = setcardinal_BM (_.setnumbers);
  unsigned nbconsts = setcardinal_BM (_.setconsts);
  DBGPRINTF_BM
    ("start prepare_routine°basiclo_minifunction recv %s nbargs=%u nbclosed=%u nblocals=%u nbnumbers=%u nbconsts=%u",
     objectdbg_BM (_.recv), nbargs, nbclosed, nblocals, nbnumbers, nbconsts);
  _.routassoc =
    make_assoc_BM (2 + nbargs + nbclosed + nblocals + nbnumbers + nbconsts);
  /// bind the arguments
  for (unsigned argix = 0; argix < nbargs; argix++)
    {
      _.curvar = tuplecompnth_BM (_.tupargs, argix);
      DBGPRINTF_BM
        ("start prepare_routine°basiclo_minifunction argix=%u argum curvar=%s",
         argix, objectdbg_BM (_.curvar));
      _.oldrol = assoc_getattr_BM (_.routassoc, _.curvar);
      if (_.oldrol)
        {
          fprintf (stderr, "argument#%u %s is not fresh in minifunction %s\n",
                   argix, objectdbg_BM (_.curvar), objectdbg1_BM (_.recv));
          return NULL;
        }
      _.curol = makenodevar_BM (k_arguments, taggedint_BM (argix), NULL);
      _.routassoc = assoc_addattr_BM (_.routassoc, _.curvar, _.curol);
      _.curol = NULL;
    }
  /// bind the closed
  for (unsigned cloix = 0; cloix < nbclosed; cloix++)
    {
      _.curvar = tuplecompnth_BM (_.tupclosed, cloix);
      DBGPRINTF_BM
        ("start prepare_routine°basiclo_minifunction cloix=%u closed curvar=%s",
         cloix, objectdbg_BM (_.curvar));
      _.oldrol = assoc_getattr_BM (_.routassoc, _.curvar);
      if (_.oldrol)
        {
          fprintf (stderr, "closed#%u %s is not fresh in minifunction %s\n",
                   cloix, objectdbg_BM (_.curvar), objectdbg1_BM (_.recv));
          return NULL;
        }
      _.curol = makenodevar_BM (k_closed, taggedint_BM (cloix), NULL);
      _.routassoc = assoc_addattr_BM (_.routassoc, _.curvar, _.curol);
      _.curol = NULL;
    }
  /// bind the locals
  for (unsigned locix = 0; locix < nblocals; locix++)
    {
      _.curvar = setelemnth_BM (_.setlocals, locix);
      DBGPRINTF_BM
        ("start prepare_routine°basiclo_minifunction locix=%u local curvar=%s",
         locix, objectdbg_BM (_.curvar));
      _.oldrol = assoc_getattr_BM (_.routassoc, _.curvar);
      if (_.oldrol)
        {
          fprintf (stderr, "local#%u %s is not fresh in minifunction %s\n",
                   locix, objectdbg_BM (_.curvar), objectdbg1_BM (_.recv));
          return NULL;
        }
      _.curol = makenodevar_BM (k_locals, taggedint_BM (locix), NULL);
      _.routassoc = assoc_addattr_BM (_.routassoc, _.curvar, _.curol);
      _.curol = NULL;
    }
  // bind the numbers
  for (unsigned numix = 0; numix < nbnumbers; numix++)
    {
      _.curvar = setelemnth_BM (_.setnumbers, numix);
      DBGPRINTF_BM
        ("start prepare_routine°basiclo_minifunction numix=%u number curvar=%s",
         numix, objectdbg_BM (_.curvar));
      _.oldrol = assoc_getattr_BM (_.routassoc, _.curvar);
      if (_.oldrol)
        {
          fprintf (stderr, "number#%u %s is not fresh in minifunction %s\n",
                   numix, objectdbg_BM (_.curvar), objectdbg1_BM (_.recv));
          return NULL;
        }
      _.curol = makenodevar_BM (k_numbers, taggedint_BM (numix), NULL);
      _.routassoc = assoc_addattr_BM (_.routassoc, _.curvar, _.curol);
      _.curol = NULL;
    }
  // bind the constants
  for (unsigned constix = 0; constix < nbconsts; constix++)
    {
      _.curvar = setelemnth_BM (_.setconsts, constix);
      DBGPRINTF_BM
        ("start prepare_routine°basiclo_minifunction constix=%u constant curvar=%s",
         constix, objectdbg_BM (_.curvar));
      _.oldrol = assoc_getattr_BM (_.routassoc, _.curvar);
      if (_.oldrol)
        {
          fprintf (stderr, "constant#%u %s is not fresh in minifunction %s\n",
                   constix, objectdbg_BM (_.curvar), objectdbg1_BM (_.recv));
          return NULL;
        }
      _.curol = makenodevar_BM (k_constants, taggedint_BM (constix), NULL);
      _.routassoc = assoc_addattr_BM (_.routassoc, _.curvar, _.curol);
      _.curol = NULL;
    }
  objputpayload_BM (_.routprep, _.routassoc);
  _.obhsetblock = makeobj_BM ();
  objputpayload_BM (_.obhsetblock, hashsetobj_grow_BM (NULL, 15));
  objputclass_BM (_.obhsetblock, (objectval_tyBM *) k_hset_object);
  objputattr_BM (_.routprep, k_blocks, _.obhsetblock);
  objputattr_BM (_.routprep, k_prepare_routine, _.recv);
  objputattr_BM (_.routprep, k_in, _.modgen);
  objputattr_BM (_.obhsetblock, k_in, _.routprep);
  objtouchnow_BM (_.obhsetblock);
  objtouchnow_BM (_.routprep);
  DBGPRINTF_BM
    ("start prepare_routine°basiclo_minifunction before collect_blocks recv %s routprep %s",
     objectdbg_BM (_.recv), objectdbg1_BM (_.routprep));
  _.collbl = send2_BM (_.bodyv, k_collect_blocks,
                       (struct stackframe_stBM *) &_,
                       _.routprep, taggedint_BM (0));
  if (!_.collbl)
    {
      fprintf (stderr,
               "collect_blocks failed for block %s in minifunction %s with routprep %s\n",
               objectdbg_BM (_.bodyv), objectdbg1_BM (_.recv),
               objectdbg2_BM (_.routprep));
      return NULL;
    }
  DBGPRINTF_BM
    ("start prepare_routine°basiclo_minifunction after collect_blocks recv %s routprep %s",
     objectdbg_BM (_.recv), objectdbg1_BM (_.routprep));
  return _.routprep;
}                               /* end prepare_routine°basiclo_minifunction  _07qYMXftJRR_9dde2ASz4e9  */




// for the method to complete_module in basiclo_temporary_module &
// basiclo_dumpable_module

extern objrout_sigBM ROUTINEOBJNAME_BM (_10XOFm9ui6R_06F8qZQynnA);
value_tyBM
ROUTINEOBJNAME_BM (_10XOFm9ui6R_06F8qZQynnA)    //
(const closure_tyBM * clos, struct stackframe_stBM * stkf,      //
 const value_tyBM arg1,         // recieving module
 const value_tyBM arg2,         // module generator
 const value_tyBM arg3,         // preparation
 const quasinode_tyBM * restargs __attribute__ ((unused)))
{
  enum
  {
    constix_functions_set,
    constix_plain_module,
    constix_basiclo_minifunction,
    constix__LAST
  };
  LOCALFRAME_BM ( /*prev: */ stkf, /*descr: */ NULL,
                 const closure_tyBM * clos; value_tyBM recv;
                 value_tyBM modgen;
                 value_tyBM funset; objectval_tyBM * plainmod;
                 objectval_tyBM * curfun;
    );
  assert (isclosure_BM ((const value_tyBM) clos));
  _.clos = clos;
  const objectval_tyBM *closconn = closureconn_BM ((const value_tyBM) clos);
  assert (closconn != NULL);
  // retrieve arguments
  _.recv = (arg1);
  WEAKASSERT_BM (isobject_BM (_.recv));
  _.modgen = (arg2);
  WEAKASSERT_BM (isobject_BM (_.modgen));
  const node_tyBM *constnod = nodecast_BM (objpayload_BM (closconn));
  /*** constnod is
      * const (functions_set plain_module basiclo_minifunction)
   ***/
  WEAKASSERT_BM (isnode_BM ((value_tyBM) constnod)
                 && valhash_BM ((value_tyBM) constnod) == 2107755858
                 && nodewidth_BM ((value_tyBM) constnod) == constix__LAST);
  const objectval_tyBM *k_functions_set =       //
    objectcast_BM (nodenthson_BM
                   ((const value_tyBM) constnod, constix_functions_set));
  const objectval_tyBM *k_plain_module =        //
    objectcast_BM (nodenthson_BM
                   ((const value_tyBM) constnod, constix_plain_module));
  const objectval_tyBM *k_basiclo_minifunction =        //
    objectcast_BM (nodenthson_BM ((const value_tyBM) constnod,
                                  constix_basiclo_minifunction));
  DBGPRINTF_BM ("complete_module°basiclo*module start recv=%s modgen=%s",
                objectdbg_BM (_.recv), objectdbg1_BM (_.modgen));
  _.funset = setcast_BM (objgetattr_BM (_.modgen, k_functions_set));
  unsigned nbfuns = setcardinal_BM (_.funset);
  _.plainmod = objectcast_BM (objgetattr_BM (_.modgen, k_plain_module));
  DBGPRINTF_BM
    ("complete_module°basiclo*module nbfuns=%u plainmod=%s",
     nbfuns, objectdbg_BM (_.plainmod));
  unsigned nbtinyfuns = 0;
  for (unsigned ix = 0; ix < nbfuns; ix++)
    {
      _.curfun = setelemnth_BM (_.funset, ix);
      if (objectisinstance_BM (_.curfun, k_basiclo_minifunction))
        {
          nbtinyfuns++;
        }
    }
  if (nbtinyfuns == nbfuns)
    {
      DBGPRINTF_BM
        ("complete_module°basiclo*module gives all %u tinyfuncs recv=%s modgen=%s",
         nbtinyfuns, objectdbg_BM (_.recv), objectdbg1_BM (_.modgen));
      return _.funset;
    }
  DBGPRINTF_BM
    ("complete_module°basiclo*module nbtinyfuns=%u < nbfuns=%u incomplete",
     nbtinyfuns, nbfuns);
#warning complete_module°basiclo*module unimplemented
  return NULL;
}                               /* end complete_module°basiclo*module _10XOFm9ui6R_06F8qZQynnA  */


// for the method to collect_blocks in basiclo_block-s 

extern objrout_sigBM ROUTINEOBJNAME_BM (_0gkYrIdnOg2_0wLEAh1QuYu);
value_tyBM
ROUTINEOBJNAME_BM (_0gkYrIdnOg2_0wLEAh1QuYu)    //
(const closure_tyBM * clos, struct stackframe_stBM * stkf,      //
 const value_tyBM arg1,         // recieving block
 const value_tyBM arg2,         // routine preparation
 const value_tyBM arg3,         // depth
 const quasinode_tyBM * restargs __attribute__ ((unused)))
{
  enum
  {
    closix_nbvars,
    closix_nbargs,
    closix__LAST
  };
  enum
  {
    constix_miniscan_var,
    constix_miniscan_expr,
    constix_miniscan_block,
    constix_miniscan_stmt,
    constix_basiclo_block,
    constix_basiclo_statement,
    constix__LAST
  };
  LOCALFRAME_BM ( /*prev: */ stkf, /*descr: */ NULL,
                 const closure_tyBM * clos;
                 objectval_tyBM * recv; objectval_tyBM * routprep;
                 objectval_tyBM * curob; value_tyBM curexp; value_tyBM resv;
                 anyassoc_tyBM * routassoc;
    );
  assert (isclosure_BM ((const value_tyBM) clos));
  _.clos = clos;
  const objectval_tyBM *closconn = closureconn_BM ((const value_tyBM) clos);
  assert (closconn != NULL);
  const node_tyBM *constnod = nodecast_BM (objpayload_BM (closconn));
  /*** constnod is
   * const (miniscan_var miniscan_expr miniscan_block
            miniscan_stmt basiclo_block basiclo_statement)
  ***/
  WEAKASSERT_BM (isnode_BM ((value_tyBM) constnod)
                 && valhash_BM ((value_tyBM) constnod) == 1795820440
                 && nodewidth_BM ((value_tyBM) constnod) == constix__LAST);
  const objectval_tyBM *k_miniscan_var =        //
    objectcast_BM (nodenthson_BM
                   ((const value_tyBM) constnod, constix_miniscan_var));
  const objectval_tyBM *k_miniscan_expr =       //
    objectcast_BM (nodenthson_BM
                   ((const value_tyBM) constnod, constix_miniscan_expr));
  const objectval_tyBM *k_miniscan_block =      //
    objectcast_BM (nodenthson_BM ((const value_tyBM) constnod,
                                  constix_miniscan_block));
  const objectval_tyBM *k_miniscan_stmt =       //
    objectcast_BM (nodenthson_BM ((const value_tyBM) constnod,
                                  constix_miniscan_stmt));
  const objectval_tyBM *k_basiclo_block =       //
    objectcast_BM (nodenthson_BM ((const value_tyBM) constnod,
                                  constix_basiclo_block));
  const objectval_tyBM *k_basiclo_statement =   //
    objectcast_BM (nodenthson_BM ((const value_tyBM) constnod,
                                  constix_basiclo_statement));
  // retrieve arguments
  _.recv = objectcast_BM (arg1);
  WEAKASSERT_BM (isobject_BM (_.recv));
  _.routprep = objectcast_BM (arg2);
  WEAKASSERT_BM (isobject_BM (_.routprep));
  WEAKASSERT_BM (istaggedint_BM (arg3));
  int depth = getint_BM (arg3);
  int nbvars = 0, nbargs = 0;
  // retrieve closed integers
  WEAKASSERT_BM (closurewidth_BM ((const value_tyBM) clos) >= closix__LAST);
  {
    value_tyBM nbvarsv =
      closurenthson_BM ((const value_tyBM) clos, closix_nbvars);
    WEAKASSERT_BM (istaggedint_BM (nbvarsv));
    nbvars = getint_BM (nbvarsv);
  }
  {
    value_tyBM nbargsv =
      closurenthson_BM ((const value_tyBM) clos, closix_nbargs);
    WEAKASSERT_BM (istaggedint_BM (nbargsv));
    nbargs = getint_BM (nbargsv);
  }
  DBGPRINTF_BM
    ("collect_blocks°basiclo_block _0gkYrIdnOg2_0wLEAh1QuYu start recv=%s routprep=%s depth=%d nbvar=%d nbargs=%d",
     objectdbg_BM (_.recv), objectdbg1_BM (_.routprep), depth, (int) nbargs,
     (int) nbvars);
  _.routassoc = assoccast_BM (objpayload_BM (_.routprep));
  if (!_.routassoc)
    {
      fprintf (stderr, "collect_blocks°basiclo_block bad routprep %s\n",
               objectdbg_BM (_.routprep));
      return NULL;
    }
  for (int varix = 0; varix < nbvars; varix++)
    {
      _.curob = objectcast_BM (objgetcomp_BM (_.recv, varix));
      DBGPRINTF_BM
        ("collect_blocks°basiclo_block varix#%d curob %s", varix,
         objectdbg_BM (_.curob));
      if (!_.curob)
        {
          fprintf (stderr,
                   "collect_blocks°basiclo_block bad variable for varix#%d\n",
                   varix);
          return NULL;
        }
      if (!assoc_getattr_BM (_.routassoc, _.curob))
        {
          fprintf (stderr,
                   "collect_blocks°basiclo_block unknown variable %s for varix#%d\n",
                   objectdbg_BM (_.curob), varix);
          return NULL;
        }
      /// should send k_miniscan_var
      _.resv = send3_BM (_.curob,
                         k_miniscan_var,
                         (struct stackframe_stBM *) &_,
                         _.routprep, taggedint_BM (depth), _.recv);
      if (!_.resv)
        {
          fprintf (stderr,
                   "collect_blocks°basiclo_block miniscan_variable of variable %s for varix#%d failed\n",
                   objectdbg_BM (_.curob), varix);
          return NULL;
        }
    }
  for (int argix = 0; argix < nbargs; argix++)
    {
      _.curexp = (objgetcomp_BM (_.recv, argix + nbvars));
      DBGPRINTF_BM ("collect_blocks°basiclo_block argix#%d curexp %s",
                    argix,
                    debug_outstr_value_BM (_.curexp,
                                           (struct stackframe_stBM *) &_, 0));
      /// should send k_miniscan_expr
      _.resv = send3_BM (_.curexp,
                         k_miniscan_expr,
                         (struct stackframe_stBM *) &_,
                         _.routprep, taggedint_BM (depth), _.recv);
      if (!_.resv)
        {
          fprintf (stderr,
                   "collect_blocks°basiclo_block miniscan_expr for expix#%d failed\n",
                   argix);
          return NULL;
        }
    }
  int off = nbvars + nbargs;
  int nbblocks = objnbcomps_BM (_.recv) - off;
  for (int blockix = 0; blockix < nbblocks; blockix++)
    {
      _.curob = objectcast_BM (objgetcomp_BM (_.recv, blockix + off));
      DBGPRINTF_BM
        ("collect_blocks°basiclo_block blockix#%d curob %s", blockix,
         objectdbg_BM (_.curob));
      if (!_.curob)
        {
          fprintf (stderr,
                   "collect_blocks°basiclo_block bad block for blockix#%d\n",
                   blockix);
          return NULL;
        }
      if (objectisinstance_BM (_.curob, k_basiclo_block))
        {
          /// should send k_miniscan_block
          _.resv = send3_BM (_.curob,
                             k_miniscan_block,
                             (struct stackframe_stBM *) &_,
                             _.routprep, taggedint_BM (depth), _.recv);
          if (!_.resv)
            {
              fprintf (stderr,
                       "collect_blocks°basiclo_block miniscan_block for %s blockix#%d failed\n",
                       objectdbg_BM (_.curob), blockix);
              return NULL;
            }
        }
      else if (objectisinstance_BM (_.curob, k_basiclo_statement))
        {
          /// should send k_miniscan_stmt
          _.resv = send3_BM (_.curob,
                             k_miniscan_stmt,
                             (struct stackframe_stBM *) &_,
                             _.routprep, taggedint_BM (depth), _.recv);
          if (!_.resv)
            {
              fprintf (stderr,
                       "collect_blocks°basiclo_block miniscan_stmt for %s blockix#%d failed\n",
                       objectdbg_BM (_.curob), blockix);
              return NULL;
            }
        }
      else
        {
          fprintf (stderr,
                   "collect_blocks°basiclo_block invalid block %s for blockix#%d\n",
                   objectdbg_BM (_.curob), blockix);
          return NULL;
        }
    }
  DBGPRINTF_BM
    ("collect_blocks°basiclo_block _0gkYrIdnOg2_0wLEAh1QuYu done recv=%s routprep=%s depth %d",
     objectdbg_BM (_.recv), objectdbg1_BM (_.routprep), depth);
  return _.routprep;
}                               /* end collect_blocks°basiclo_block _0gkYrIdnOg2_0wLEAh1QuYu */



// for the method miniscan_stmt°basiclo_cond 
extern objrout_sigBM ROUTINEOBJNAME_BM (_0zzJJsAL6Qm_2uw3eoWQHEq);
value_tyBM
ROUTINEOBJNAME_BM (_0zzJJsAL6Qm_2uw3eoWQHEq)    //
(const closure_tyBM * clos, struct stackframe_stBM * stkf,      //
 const value_tyBM arg1,         // recieving cond
 const value_tyBM arg2,         // routine preparation
 const value_tyBM arg3,         // depth
 const quasinode_tyBM * restargs)       // arg4 is the containing block
{
  LOCALFRAME_BM ( /*prev: */ stkf, /*descr: */ NULL,
                 const closure_tyBM * clos;
                 objectval_tyBM * recv; objectval_tyBM * routprepob;
                 objectval_tyBM * fromblockob;
    );
  int depth = 0;
  _.clos = clos;
  _.recv = objectcast_BM (arg1);
  _.routprepob = objectcast_BM (arg2);
  depth = getint_BM (arg3);
  _.fromblockob = objectcast_BM (nodenthson_BM (restargs, 0));
  DBGPRINTF_BM
    ("miniscan_stmt°basiclo_cond start recv=%s routprepob=%s depth#%d fromblockob=%s unimplemented",
     objectdbg_BM (_.recv), objectdbg1_BM (_.routprepob), depth,
     objectdbg2_BM (_.fromblockob));
#warning miniscan_stmt°basiclo_cond unimplemented
  return NULL;
}                               /* end routine miniscan_stmt°basiclo_cond _0zzJJsAL6Qm_2uw3eoWQHEq */
