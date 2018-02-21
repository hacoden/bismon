// file inline_BM.h
#ifndef INLINE_BM_INCLUDED
#define INLINE_BM_INCLUDED

pid_t
gettid_BM (void)
{
  return syscall (SYS_gettid, 0L);
}                               /* end gettid_BM */


double
clocktime_BM (clockid_t clid)
{
  struct timespec ts = { 0, 0 };
  if (clock_gettime (clid, &ts))
    return NAN;
  return (double) ts.tv_sec + 1.0e-9 * ts.tv_nsec;
}

double
cputime_BM (void)
{
  return clocktime_BM (CLOCK_PROCESS_CPUTIME_ID);
}                               /* end cputime_BM */

double
elapsedtime_BM (void)
{
  struct timespec ts = { 0, 0 };
  if (clock_gettime (CLOCK_MONOTONIC, &ts))
    return NAN;
  return (double) (ts.tv_sec - startrealtimespec_BM.tv_sec)
    + 1.0e-9 * (ts.tv_nsec - startrealtimespec_BM.tv_nsec);
}                               /* end elapsedtime_BM */



bool
istaggedint_BM (value_tyBM v)
{
  return (uintptr_t) v & 1;
}                               /* end istaggedint_BM */

intptr_t
getint_BM (value_tyBM v)
{
  if (istaggedint_BM (v))
    return ((intptr_t) v) >> 1;
  return 0;
}                               /* end getint_BM */

value_tyBM
taggedint_BM (intptr_t i)
{
  return (value_tyBM) ((i << 1) | 1);
}                               /* end taggedint_BM */

int
valtype_BM (const value_tyBM v)
{
  if (!v)
    return tyNone_BM;
  if (istaggedint_BM (v))
    return tyInt_BM;
  if (((uintptr_t) v & 3) == 0)
    {
      typedhead_tyBM *ht = (typedhead_tyBM *) v;
      ASSERT_BM (ht->htyp != 0);
      return ht->htyp;
    }
  return tyNone_BM;
}                               /* end valtype_BM */


bool
isgenuineval_BM (const value_tyBM v)
{
  int ty = valtype_BM (v);
  if (ty > type_LASTREAL_BM)
    return false;
  return ty != tyNone_BM;
}                               /* end isgenuineval_BM */


objectval_tyBM *
valclass_BM (const value_tyBM v)
{
  if (!v)
    return NULL;
  if (istaggedint_BM (v))
    return BMP_int;
  if (((uintptr_t) v & 3) == 0)
    {
      typedhead_tyBM *ht = (typedhead_tyBM *) v;
      ASSERT_BM (ht->htyp != 0);
      int ty = ht->htyp;
      switch (ty)
        {
        case tyString_BM:
          return BMP_string;
        case tySet_BM:
          return BMP_set;
        case tyTuple_BM:
          return BMP_tuple;
        case tyNode_BM:
          return BMP_node;
        case tyClosure_BM:
          return BMP_closure;
        case tyObject_BM:
          return ((objectval_tyBM *) v)->ob_class ? : BMP_undefined;
        case tyUnspecified_BM:
          return BMP_unspecified;
        }
    }
#ifndef NDEBUG
  weakassertfailureat_BM ("valclass_BM with bad type", __FILE__, __LINE__);
#endif // NDEBUG
  return NULL;
}                               /* end valclass_BM */

hash_tyBM
valhash_BM (const value_tyBM v)
{
  if (!v)
    return 0;
  int ty = valtype_BM (v);
  switch (ty)
    {
    case tyInt_BM:
      {
        uintptr_t i = getint_BM (v);
        hash_tyBM hi = (i & 0x3fffffff) ^ (i % 132594613);
        if (hi == 0)
          hi = ((i % 594571) & 0xfffffff) + 17;
        ASSERT_BM (hi != 0);
        return hi;
      }
    case tyString_BM:
    case tySet_BM:
    case tyTuple_BM:
    case tyNode_BM:
    case tyClosure_BM:
    case tyObject_BM:
      return ((typedhead_tyBM *) v)->hash;
    case tyUnspecified_BM:
      return 8594623;
    default:
      return 0;
    }
}                               /* end valhash_BM */

bool
istuple_BM (const value_tyBM v)
{
  return (valtype_BM (v) == tyTuple_BM);
}                               /* end istuple_BM */

const tupleval_tyBM *
tuplecast_BM (const value_tyBM v)
{
  return istuple_BM (v) ? (const tupleval_tyBM *) v : NULL;
}                               /* end tuplecast_BM */


bool
valequal_BM (const value_tyBM v1, const value_tyBM v2)
{
  if (v1 == v2)
    return true;
  int ty1 = valtype_BM (v1);
  if (ty1 != valtype_BM (v2))
    return false;
  if (valhash_BM (v1) != valhash_BM (v2))
    return false;
  if (ty1 == tyObject_BM)
    return false;
  return valsamecontent_BM (v1, v2);
}                               /* end valequal_BM */

bool
validserial63_BM (serial63_tyBM s)
{
  return (s > MINSERIAL_BM && s < MAXSERIAL_BM);
}                               /* end validserial63_BM */


unsigned
bucknumserial63_BM (serial63_tyBM s)
{
  return s / (DELTASERIAL_BM / MAXBUCKETS_BM);
}

uint64_t
buckoffserial63_BM (serial63_tyBM s)
{
  return s % (DELTASERIAL_BM / MAXBUCKETS_BM);
}

bool
validid_BM (rawid_tyBM id)
{
  return (validserial63_BM (id.id_hi) && validserial63_BM (id.id_lo));
}                               /* end validid_BM */



hash_tyBM
hashid_BM (rawid_tyBM id)
{
  if (!validid_BM (id))
    return 0;
  hash_tyBM h = (id.id_hi % 1073741939) ^ (id.id_lo % 596789351);
  if (h == 0)
    h = (id.id_hi & 0xffffff) + (id.id_lo & 0x3ffffff) + 17;
  ASSERT_BM (h > 0);
  return h;
}                               /* end hashid_BM */



int
cmpid_BM (rawid_tyBM id1, rawid_tyBM id2)
{
  if (id1.id_hi == id2.id_hi)
    {
      if (id1.id_lo == id2.id_lo)
        return 0;
      else if (id1.id_lo < id2.id_lo)
        return -1;
      else
        return +1;
    }
  else if (id1.id_hi < id2.id_hi)
    return -1;
  else
    return +1;
}                               /* end cmpid_BM */


rawid_tyBM
objid_BM (const objectval_tyBM * obj)
{
  if ((valtype_BM ((const value_tyBM) obj) != tyObject_BM))
    return (rawid_tyBM)
    {
    0, 0};
  return obj->ob_id;
}                               /* end objid_BM */

double
objmtime_BM (const objectval_tyBM * obj)
{
  if ((valtype_BM ((const value_tyBM) obj) != tyObject_BM))
    return 0.0;
  return obj->ob_mtime;
}                               /* end objmtime_BM */

void
objtouchmtime_BM (objectval_tyBM * obj, double mtime)
{
  if ((valtype_BM ((const value_tyBM) obj) != tyObject_BM))
    return;
  obj->ob_mtime = mtime;
}                               /* end objtouchmtime_BM */


void
objtouchnow_BM (objectval_tyBM * obj)
{
  if ((valtype_BM ((const value_tyBM) obj) != tyObject_BM))
    return;
  obj->ob_mtime = clocktime_BM (CLOCK_REALTIME);
}                               /* end objtouchnow_BM */


objectval_tyBM *
objsignature_BM (const objectval_tyBM * obj)
{
  if ((valtype_BM ((const value_tyBM) obj) != tyObject_BM))
    return NULL;
  return obj->ob_sig;
}                               /* end objsignature_BM */

void *
objroutaddr_BM (const objectval_tyBM * obj, const objectval_tyBM * objsig)
{
  if ((valtype_BM ((const value_tyBM) obj) != tyObject_BM))
    return NULL;
  if (obj->ob_sig != objsig)
    return NULL;
  return obj->ob_routaddr;
}                               /* end objroutaddr_BM */

objectval_tyBM *
objclass_BM (const objectval_tyBM * obj)
{
  if ((valtype_BM ((const value_tyBM) obj) != tyObject_BM))
    return NULL;
  return obj->ob_class;
}                               /* end objclass_BM */

bool
equalid_BM (rawid_tyBM id1, rawid_tyBM id2)
{
  return id1.id_hi == id2.id_hi && id1.id_lo == id2.id_lo;
}                               /* end equalid_BM */



/// object support

bool
isobject_BM (const value_tyBM v)
{
  return valtype_BM (v) == tyObject_BM;
}                               /* end isobject_BM */

bool
objlock_BM (objectval_tyBM * obj)
{
  if (valtype_BM (obj) != tyObject_BM)
    return false;
  if (pthread_mutex_lock (&obj->ob_mutex))
    return false;
  if (curfailurehandle_BM)
    {
      ASSERT_BM (curfailurehandle_BM->failh_magic == FAILUREHANDLEMAGIC_BM);
      register_failock_BM (curfailurehandle_BM->failh_lockset, obj);
    }
  return true;
}                               /* end objlock_BM */

bool
objunlock_BM (objectval_tyBM * obj)
{

  if (valtype_BM (obj) != tyObject_BM)
    return false;
  if (pthread_mutex_unlock (&obj->ob_mutex))
    return false;
  if (curfailurehandle_BM)
    {
      ASSERT_BM (curfailurehandle_BM->failh_magic == FAILUREHANDLEMAGIC_BM);
      unregister_failock_BM (curfailurehandle_BM->failh_lockset, obj);
    }
  return true;
}                               /* end objunlock_BM */

bool
objtrylock_BM (objectval_tyBM * obj)
{
  if (valtype_BM (obj) != tyObject_BM)
    return false;
  if (pthread_mutex_trylock (&obj->ob_mutex))
    return false;
  if (curfailurehandle_BM)
    {
      ASSERT_BM (curfailurehandle_BM->failh_magic == FAILUREHANDLEMAGIC_BM);
      register_failock_BM (curfailurehandle_BM->failh_lockset, obj);
    };
  return true;
}                               /* end objtrylock_BM */


objectval_tyBM *
objectcast_BM (const value_tyBM v)
{
  return isobject_BM (v) ? (objectval_tyBM *) v : NULL;
}                               /* end objectcast_BM */

hash_tyBM
objecthash_BM (const objectval_tyBM * pob)
{
  if (!isobject_BM ((const value_tyBM) pob))
    return 0;
  hash_tyBM h = ((typedhead_tyBM *) pob)->hash;
  ASSERT_BM (h > 0);
  ASSERT_BM (h == hashid_BM (pob->ob_id));
  return h;
}                               /* end objecthash_BM */

unsigned
objspacenum_BM (const objectval_tyBM * obj)
{
  if (!isobject_BM ((const value_tyBM) obj))
    return 0;
  return obj->ob_space;
}                               /* end objspacenum_BM */

value_tyBM
objgetattr_BM (const objectval_tyBM * obj, const objectval_tyBM * objattr)
{
  if (!isobject_BM ((const value_tyBM) obj))
    return NULL;
  if (!isobject_BM ((const value_tyBM) objattr))
    return NULL;
  if (!obj->ob_attrassoc)
    return NULL;
  return assoc_getattr_BM (obj->ob_attrassoc, objattr);
}                               /* end objgetattr_BM */

unsigned
objnbattrs_BM (const objectval_tyBM * obj)
{
  if (!isobject_BM ((const value_tyBM) obj))
    return 0;
  if (!obj->ob_attrassoc)
    return 0;
  return assoc_nbkeys_BM (obj->ob_attrassoc);
}                               /* end objnbattrs_BM */



unsigned
objnbcomps_BM (const objectval_tyBM * obj)
{
  if (!isobject_BM ((const value_tyBM) obj))
    return 0;
  if (!obj->ob_compvec)
    return 0;
  return datavectlen_BM (obj->ob_compvec);
}                               /* end objnbcomps_BM */

value_tyBM *
objcompdata_BM (const objectval_tyBM * obj)
{
  if (!isobject_BM ((const value_tyBM) obj))
    return NULL;
  if (!obj->ob_compvec)
    return NULL;
  return (value_tyBM *) datavectdata_BM (obj->ob_compvec);
}                               /* end objcompdata_BM */


const setval_tyBM *
objsetattrs_BM (const objectval_tyBM * obj)
{
  if (!isobject_BM ((const value_tyBM) obj))
    return NULL;
  if (!obj->ob_attrassoc)
    return NULL;
  return assoc_setattrs_BM (obj->ob_attrassoc);
}                               /* end objsetattrs_BM */


value_tyBM
objgetcomp_BM (const objectval_tyBM * obj, int rk)
{
  if (!isobject_BM ((const value_tyBM) obj))
    return NULL;
  if (!obj->ob_compvec)
    return NULL;
  return datavectnth_BM (obj->ob_compvec, rk);
}                               /* end objgetcomp_BM */

void
objputcomp_BM (objectval_tyBM * obj, int rk, const value_tyBM valcomp)
{
  if (!isobject_BM ((const value_tyBM) obj))
    return;
  if (!obj->ob_compvec)
    return;
  if (!valcomp || isgenuineval_BM (valcomp))
    datavectputnth_BM (obj->ob_compvec, rk, valcomp);
}                               /* end objputcomp_BM */


void
objreservecomps_BM (objectval_tyBM * obj, unsigned gap)
{
  if (!isobject_BM ((const value_tyBM) obj))
    return;
  obj->ob_compvec = datavect_reserve_BM (obj->ob_compvec, gap);
}                               /* end objreservecomps_BM */


void
objresetcomps_BM (objectval_tyBM * obj, unsigned len)
{
  if (!isobject_BM ((const value_tyBM) obj))
    return;
  obj->ob_compvec = datavect_reserve_BM (NULL, len);
}                               /* end objresetcomps_BM */


void
objappendcomp_BM (objectval_tyBM * obj, value_tyBM compval)
{
  if (!isobject_BM ((const value_tyBM) obj))
    return;
  if (compval && !isgenuineval_BM (compval))
    return;
  obj->ob_compvec = datavect_append_BM (obj->ob_compvec, compval);
}                               /* end objappendcomp_BM */


void
objgrowcomps_BM (objectval_tyBM * obj, unsigned gap)
{
  if (!isobject_BM ((const value_tyBM) obj))
    return;
  obj->ob_compvec = datavect_grow_BM (obj->ob_compvec, gap);
}                               /* end objgrowcomps_BM */

extendedval_tyBM
objpayload_BM (const objectval_tyBM * obj)
{
  if (!isobject_BM ((const value_tyBM) obj))
    return NULL;
  return obj->ob_payl;
}                               /* end objpayload_BM */


void
objclearpayload_BM (objectval_tyBM * obj)
{
  if (!isobject_BM ((const value_tyBM) obj))
    return;
  extendedval_tyBM payl = obj->ob_payl;
  if (!payl)
    return;
  obj->ob_payl = NULL;
  deleteobjectpayload_BM (obj, payl);
}                               /* end objclearpayload_BM */

void
objputpayload_BM (objectval_tyBM * obj, extendedval_tyBM payl)
{
  if (!isobject_BM ((const value_tyBM) obj))
    return;
  extendedval_tyBM oldpayl = obj->ob_payl;
  if (oldpayl == payl)
    return;
  obj->ob_payl = NULL;
  if (oldpayl)
    deleteobjectpayload_BM (obj, oldpayl);
  obj->ob_payl = payl;
}                               /* end objputpayload_BM */

bool
objhasclassinfo_BM (const objectval_tyBM * obj)
{
  extendedval_tyBM payl = objpayload_BM (obj);
  if (!payl)
    return false;
  return (valtype_BM ((const value_tyBM) payl) == typayl_classinfo_BM);
}                               /* end objhasclassinfo_BM */

objectval_tyBM *
objgetclassinfosuperclass_BM (const objectval_tyBM * obj)
{
  if (!objhasclassinfo_BM (obj))
    return NULL;
  struct classinfo_stBM *clinf =        //
    (struct classinfo_stBM *) (obj->ob_payl);
  ASSERT_BM (valtype_BM ((const value_tyBM) clinf) == typayl_classinfo_BM);
  objectval_tyBM *superob = clinf->clinf_superclass;
  ASSERT_BM (!superob || isobject_BM ((const value_tyBM) superob));
  return superob;
}                               /* end objgetclassinfosuperclass_BM */

bool
objectisinstance_BM (const objectval_tyBM * obj,
                     const objectval_tyBM * obclass)
{
  if (!objhasclassinfo_BM (obclass))
    return false;
  if (!isobject_BM ((const value_tyBM) obj))
    return false;
  return objclassinfoissubclass_BM (obj->ob_class, obclass);
}                               /* end objectisinstance_BM */

const closure_tyBM *
objgetclassinfomethod_BM (const objectval_tyBM * obj,
                          const objectval_tyBM * obselector)
{
  if (!objhasclassinfo_BM (obj))
    return NULL;
  if (!isobject_BM ((const value_tyBM) obselector))
    return NULL;
  struct classinfo_stBM *clinf =        //
    (struct classinfo_stBM *) (obj->ob_payl);
  ASSERT_BM (valtype_BM ((const value_tyBM) clinf) == typayl_classinfo_BM);
  const closure_tyBM *clos = (const closure_tyBM *)     //
    assoc_getattr_BM (clinf->clinf_dictmeth,
                      obselector);
  ASSERT_BM (!clos || isclosure_BM ((const value_tyBM) clos));
  return clos;
}                               /* end objgetclassinfomethod_BM */

const setval_tyBM *             //
objgetclassinfosetofselectors_BM (const objectval_tyBM * obj)
{
  if (!objhasclassinfo_BM (obj))
    return NULL;
  struct classinfo_stBM *clinf =        //
    (struct classinfo_stBM *) (obj->ob_payl);
  ASSERT_BM (valtype_BM ((const value_tyBM) clinf) == typayl_classinfo_BM);
  const setval_tyBM *set =      //
    assoc_setattrs_BM (clinf->clinf_dictmeth);
  ASSERT_BM (!set || valtype_BM ((const value_tyBM) set) == tySet_BM);
  return set;
}                               /* end objgetclassinfosetofselectors_BM */

////////////////////////////////////////////////////////////////
bool
objhasstrbuffer_BM (const objectval_tyBM * obj)
{
  extendedval_tyBM payl = objpayload_BM (obj);
  if (!payl)
    return false;
  return (valtype_BM ((const value_tyBM) payl) == typayl_strbuffer_BM);
}                               /* end objhasstrbuffer_BM */



struct strbuffer_stBM *
objgetstrbufferpayload_BM (objectval_tyBM * obj)
{
  extendedval_tyBM payl = objpayload_BM (obj);
  if (!payl)
    return NULL;
  if (valtype_BM ((const value_tyBM) payl) == typayl_strbuffer_BM)
    return (struct strbuffer_stBM *) payl;
  return NULL;
}                               /* end objgetstrbufferpayload_BM */


////////////////
int
objectcmp_BM (const objectval_tyBM * ob1, const objectval_tyBM * ob2)
{
  if (ob1 == ob2)
    return 0;
  if (ob1
      && (((intptr_t) ob1 & 3)
          || ((typedhead_tyBM *) ob1)->htyp != tyObject_BM))
    FATAL_BM ("bad ob1@%p for objectcmp_BM", ob1);
  if (ob2
      && (((intptr_t) ob1 & 3)
          || ((typedhead_tyBM *) ob2)->htyp != tyObject_BM))
    FATAL_BM ("bad ob2@%p for objectcmp_BM", ob2);
  if (!ob1)
    return -1;
  if (!ob2)
    return +1;
  return cmpid_BM (ob1->ob_id, ob2->ob_id);
}                               /* end objectcmp_BM */



int
objectnamedcmp_BM (const objectval_tyBM * ob1, const objectval_tyBM * ob2)
{
  if (ob1 == ob2)
    return 0;
  if (!ob1)
    return -1;
  if (!ob2)
    return +1;
  ASSERT_BM (isobject_BM ((const value_tyBM) ob1));
  ASSERT_BM (isobject_BM ((const value_tyBM) ob2));
  const char *n1 = findobjectname_BM (ob1);
  const char *n2 = findobjectname_BM (ob2);
  if (n1 && n2)
    return strcmp (n1, n2);
  if (n1)
    return -1;
  if (n2)
    return +1;
  return objectcmp_BM (ob1, ob2);
}                               /* end objectnamedcmp_BM */


////////////////////////////////////////////////////////////////
bool
isassoc_BM (const value_tyBM v)
{
  int ty = valtype_BM (v);
  return ty == typayl_assocbucket_BM || ty == typayl_assocpairs_BM;
}                               /* end isassoc_BM */

anyassoc_tyBM *
assoccast_BM (value_tyBM v)
{
  if (isassoc_BM (v))
    return (anyassoc_tyBM *) v;
  return NULL;
}                               /* end assoccast_BM */

unsigned
assoc_nbkeys_BM (const anyassoc_tyBM * assoc)
{
  int ty = valtype_BM ((value_tyBM) assoc);
  if (ty == typayl_assocpairs_BM)
    {
      struct assocpairs_stBM *apair = (struct assocpairs_stBM *) assoc;
      return ((typedsize_tyBM *) apair)->size;
    }
  else if (ty == typayl_assocbucket_BM)
    {
      struct assocbucket_stBM *abuck = (struct assocbucket_stBM *) assoc;
      return ((typedsize_tyBM *) abuck)->size;
    }
  return 0;
}                               /* end assoc_nbkeys_BM */

anyassoc_tyBM *
make_assoc_BM (unsigned len)
{
  anyassoc_tyBM *res = NULL;
  assoc_reorganize_BM (&res, len);
  return res;
}                               /* end make_assoc_BM */

unsigned
datavectlen_BM (const struct datavectval_stBM *dvec)
{
  if (valtype_BM ((const value_tyBM) dvec) != typayl_vectval_BM)
    return 0;
  return ((typedsize_tyBM *) dvec)->size;
}                               /* end datavectlen_BM */

const value_tyBM *
datavectdata_BM (const struct datavectval_stBM *dvec)
{
  if (valtype_BM ((const value_tyBM) dvec) != typayl_vectval_BM)
    return NULL;
  return dvec->vec_data;
}                               /* end datavectdata_BM */

value_tyBM
datavectnth_BM (const struct datavectval_stBM * dvec, int rk)
{
  unsigned sz = datavectlen_BM (dvec);
  if (rk < 0)
    rk += (int) sz;
  if (rk >= 0 && rk < (int) sz)
    return dvec->vec_data[rk];
  return NULL;
}                               /* end datavectnth_BM */

void
datavectputnth_BM (struct datavectval_stBM *dvec,
                   int rk, const value_tyBM valcomp)
{
  unsigned sz = datavectlen_BM (dvec);
  if (rk < 0)
    rk += (int) sz;
  if (rk >= 0 && rk < (int) sz)
    dvec->vec_data[rk] = valcomp;
}                               /* end datavectputnth_BM */

struct datavectval_stBM *
datavect_insertone_BM (struct datavectval_stBM *dvec, int rk, value_tyBM val)
{
  return datavect_insert_BM (dvec, rk, &val, 1);
}                               /* end datavect_insertone_BM  */

////////////////
bool
isset_BM (const value_tyBM v)
{
  return (valtype_BM (v) == tySet_BM);
}                               /* end isset_BM */

const setval_tyBM *
setcast_BM (const value_tyBM v)
{
  return isset_BM (v) ? (const setval_tyBM *) v : NULL;
}                               /* end setcast_BM */

bool
setcontains_BM (const setval_tyBM * setv, const objectval_tyBM * obelem)
{
  if (!isset_BM ((const value_tyBM) setv))
    return false;
  if (!isobject_BM ((const value_tyBM) obelem))
    return false;
  return setelemindex_BM (setv, obelem) >= 0;
}                               /* end setcontains_BM */

////////////////
bool
issequence_BM (const value_tyBM v)
{
  int ty = valtype_BM (v);
  return (ty == tySet_BM || ty == tyTuple_BM);
}                               /* end issequence_BM */


const seqobval_tyBM *
sequencecast_BM (const value_tyBM v)
{
  return issequence_BM (v) ? (const seqobval_tyBM *) v : NULL;
}                               /* end sequencecast_BM */

bool
isstring_BM (const value_tyBM v)
{
  return (valtype_BM (v) == tyString_BM);
}                               /* end isstring_BM */

const stringval_tyBM *
stringcast_BM (const value_tyBM v)
{
  return isstring_BM (v) ? (const stringval_tyBM *) v : NULL;
}                               /* end stringcast_BM */

////////////////
struct hashsetobj_stBM *
hashsetobjcast_BM (const value_tyBM v)
{
  if (valtype_BM ((const value_tyBM) v) != typayl_hashsetobj_BM)
    return NULL;
  return (struct hashsetobj_stBM *) v;
}                               /* end hashsetobjcast_BM */

struct hashsetobj_stBM *
objputhashsetpayload_BM (objectval_tyBM * obj, unsigned inisiz)
{
  if (!isobject_BM (obj))
    return NULL;
  ASSERT_BM (inisiz < MAXSIZE_BM);
  struct hashsetobj_stBM *hset = hashsetobj_grow_BM (NULL, inisiz);
  objputpayload_BM (obj, hset);
  return hset;
}                               /* end objputhashsetpayload_BM */


struct hashsetobj_stBM *
objhashsetpayload_BM (objectval_tyBM * obj)
{
  if (!isobject_BM (obj))
    return NULL;
  void *payl = objpayload_BM (obj);
  if (valtype_BM (payl) == typayl_hashsetobj_BM)
    return (struct hashsetobj_stBM *) payl;
  return NULL;
}                               /* end objhashsetpayload_BM */


unsigned
hashsetobj_cardinal_BM (struct hashsetobj_stBM *hset)
{
  if (valtype_BM ((const value_tyBM) hset) != typayl_hashsetobj_BM)
    return 0;
  return ((typedsize_tyBM *) hset)->size;
}                               /* end hashsetobj_cardinal_BM */

////////////////

bool
islist_BM (const value_tyBM v)
{
  return (valtype_BM (v) == typayl_listtop_BM);
}

value_tyBM
listfirst_BM (const struct listtop_stBM * lis)
{
  if (!islist_BM ((const value_tyBM) lis))
    return NULL;
  struct listlink_stBM *firstl = lis->list_first;
  if (!firstl)
    return NULL;
  for (unsigned ix = 0; ix < LINKSIZE_BM; ix++)
    {
      value_tyBM curmem = firstl->link_mems[ix];
      if (curmem)
        return curmem;
    };
  // should never happen
  FATAL_BM ("corrupted list@%p", lis);
}

value_tyBM
listlast_BM (const struct listtop_stBM * lis)
{
  if (!islist_BM ((const value_tyBM) lis))
    return NULL;
  struct listlink_stBM *lastl = lis->list_last;
  if (!lastl)
    return NULL;
  for (int ix = LINKSIZE_BM - 1; ix >= 0; ix--)
    {
      value_tyBM curmem = lastl->link_mems[ix];
      if (curmem)
        return curmem;
    };
  // should never happen
  FATAL_BM ("corrupted list@%p", lis);
}

unsigned
listlength_BM (const struct listtop_stBM *lis)
{
  if (!islist_BM ((value_tyBM) lis))
    return 0;
  return ((typedhead_tyBM *) lis)->rlen;
}                               /* end listlength_BM */


bool
istree_BM (const value_tyBM v)
{
  int ty = valtype_BM (v);
  return (ty == tyClosure_BM || ty == tyNode_BM || ty == typayl_quasinode_BM);
}                               /* end istree_BM */

bool
isclosure_BM (const value_tyBM v)
{
  return valtype_BM (v) == tyClosure_BM;
}                               /* end isclosure_BM */

const closure_tyBM *
closurecast_BM (const value_tyBM v)
{
  return isclosure_BM (v) ? (const closure_tyBM *) v : NULL;
}                               /* end closurecast_BM */

bool
isnode_BM (const value_tyBM v)
{
  return valtype_BM (v) == tyNode_BM;
}                               /* end isnode_BM */

const node_tyBM *
nodecast_BM (const value_tyBM v)
{
  return isnode_BM (v) ? (const node_tyBM *) v : NULL;
}                               /* end nodecast_BM */


objectval_tyBM *
treeconn_BM (const value_tyBM v)
{
  if (!istree_BM (v))
    return NULL;
  return ((const tree_tyBM *) v)->nodt_conn;
}                               /* end treeconn_BM */

objectval_tyBM *
closureconn_BM (const value_tyBM v)
{
  if (!isclosure_BM (v))
    return NULL;
  return ((const closure_tyBM *) v)->nodt_conn;
}                               /* end closureconn_BM */

objectval_tyBM *
nodeconn_BM (const value_tyBM v)
{
  if (!isnode_BM (v))
    return NULL;
  return ((const node_tyBM *) v)->nodt_conn;
}                               /* end nodeconn_BM */


unsigned
treewidth_BM (const value_tyBM v)
{
  if (!istree_BM (v))
    return 0;
  return ((const typedsize_tyBM *) v)->size;
}                               /* end treewidth_BM */

unsigned
closurewidth_BM (const value_tyBM v)
{
  if (!isclosure_BM (v))
    return 0;
  return ((const typedsize_tyBM *) v)->size;
}                               /* end closurewidth_BM */

unsigned
nodewidth_BM (const value_tyBM v)
{
  if (!isnode_BM (v))
    return 0;
  return ((const typedsize_tyBM *) v)->size;
}

value_tyBM
treenthson_BM (const value_tyBM tr, int rk)
{
  if (!istree_BM (tr))
    return NULL;
  unsigned w = treewidth_BM (tr);
  if (rk < 0)
    rk += (int) w;
  if (rk >= 0 && rk < (int) w)
    return ((const tree_tyBM *) tr)->nodt_sons[rk];
  return NULL;
}                               /* end treenthson_BM */

value_tyBM
closurenthson_BM (const value_tyBM clo, int rk)
{
  if (!isclosure_BM (clo))
    return NULL;
  unsigned w = closurewidth_BM (clo);
  if (rk < 0)
    rk += (int) w;
  if (rk >= 0 && rk < (int) w)
    return ((const closure_tyBM *) clo)->nodt_sons[rk];
  return NULL;
}                               /* end closurenthson_BM */

value_tyBM
nodenthson_BM (const value_tyBM nod, int rk)
{
  if (!isnode_BM (nod))
    return NULL;
  unsigned w = nodewidth_BM (nod);
  if (rk < 0)
    rk += (int) w;
  if (rk >= 0 && rk < (int) w)
    return ((const node_tyBM *) nod)->nodt_sons[rk];
  return NULL;
}                               /* end nodenthson_BM */


bool
isparser_BM (const extendedval_tyBM v)
{
  int ty = valtype_BM (v);
  return ty == typayl_parser_BM;
}                               /* end isparser_BM */

objectval_tyBM *
parserowner_BM (const extendedval_tyBM v)
{
  if (!isparser_BM (v))
    return NULL;
  return ((struct parser_stBM *) v)->pars_ownob;
}                               /* end parserowner_BM */

objectval_tyBM *
checkedparserowner_BM (const extendedval_tyBM v)
{
  objectval_tyBM *obown = parserowner_BM (v);
  if (!obown)
    FATAL_BM ("parser without owner");
  if (objpayload_BM (obown) != v)
    {
      char objidbuf[32];
      memset (objidbuf, 0, sizeof (objidbuf));
      idtocbuf32_BM (objid_BM (obown), objidbuf);
      FATAL_BM ("parser not owned by %s", objidbuf);
    };
  return obown;
}                               /* end checkedparserowner_BM */


struct parser_stBM *
parsercast_BM (const value_tyBM v)
{
  return isparser_BM (v) ? ((struct parser_stBM *) v) : NULL;
};

struct parser_stBM *
objparserpayload_BM (objectval_tyBM * obj)
{
  if (!isobject_BM (obj))
    return NULL;
  struct parser_stBM *pars = parsercast_BM (objpayload_BM (obj));
  if (!pars)
    return NULL;
  ASSERT_BM (checkedparserowner_BM (pars) == obj);
  return pars;
}                               /* end objparserpayload_BM */

unsigned
parserlineno_BM (const struct parser_stBM *pars)
{
  if (isparser_BM ((const value_tyBM) pars))
    return pars->pars_lineno;
  return 0;
}                               /* end parserlineno_BM */

unsigned
parsercolpos_BM (const struct parser_stBM *pars)
{
  if (isparser_BM ((const value_tyBM) pars))
    return pars->pars_colpos;
  return 0;
}                               /* end parsercolpos_BM */

const char *
parserrestline_BM (const struct parser_stBM *pars)
{
  if (!isparser_BM ((const value_tyBM) pars))
    return NULL;
  if (!pars->pars_linebuf)
    return NULL;
  if (pars->pars_linelen < 0)
    return NULL;
  if (pars->pars_curbyte == NULL)
    return NULL;
  ASSERT_BM (pars->pars_curbyte >= pars->pars_linebuf
             && pars->pars_curbyte <=
             pars->pars_linebuf + pars->pars_linesiz);
  return pars->pars_curbyte;
}                               /* end parserrestline_BM */

gunichar
parserunichar_BM (const struct parser_stBM * pars)
{
  if (!isparser_BM ((const value_tyBM) pars))
    return 0;
  if (!pars->pars_linebuf)
    return 0;
  ASSERT_BM (pars->pars_curbyte >= pars->pars_linebuf
             && pars->pars_curbyte <=
             pars->pars_linebuf + pars->pars_linesiz);

  return g_utf8_get_char (pars->pars_curbyte);
}                               /* end parserunichar_BM */

bool
parsereol_BM (const struct parser_stBM * pars)
{
  if (!isparser_BM ((const value_tyBM) pars))
    return false;
  if (!pars->pars_linebuf)
    return true;
  ASSERT_BM (pars->pars_curbyte >= pars->pars_linebuf
             && pars->pars_curbyte <=
             pars->pars_linebuf + pars->pars_linesiz);

  return pars->pars_curbyte >= pars->pars_linebuf + pars->pars_linesiz;
}                               /* end parsereol_BM */

bool
parserendoffile_BM (const struct parser_stBM * pars)
{
  if (!isparser_BM ((const value_tyBM) pars))
    return false;
  if (pars->pars_curbyte == NULL)
    return true;
  return parsereol_BM (pars) && feof (pars->pars_file);
}                               /* end parserendoffile_BM */

bool
parseradvanceutf8_BM (struct parser_stBM * pars, unsigned nbc)
{
  if (!isparser_BM ((const value_tyBM) pars))
    return false;
  if (!pars->pars_linebuf)
    return false;
  if (!pars->pars_curbyte)
    return false;
  while (nbc > 0 && !parsereol_BM (pars))
    {
      const char *pc = pars->pars_curbyte;
      if (!*pc)
        return false;
      pc = g_utf8_next_char (pc);
      pars->pars_curbyte = pc;
      pars->pars_colpos++;
      nbc--;
      if (nbc == 0)
        return true;
    }
  return false;
}                               /* end parseradvanceutf8_BM */

////
struct dumper_stBM *
obdumpgetdumper_BM (objectval_tyBM * dumpob)
{
  extendedval_tyBM payl = objpayload_BM (dumpob);
  if (!payl)
    return NULL;
  if (valtype_BM (payl) == typayl_dumper_BM)
    return (struct dumper_stBM *) (payl);
  return NULL;
}                               /* end obdumpgetdumper_BM */

////////////////////////////////////////////////////////////////
bool
isdict_BM (const value_tyBM v)
{
  int ty = valtype_BM (v);
  return ty == typayl_dict_BM;
}                               /* end isdict_BM */

bool
openmodule_BM (const rawid_tyBM id, struct stackframe_stBM * stkf)
{
  return openmoduleforloader_BM (id, NULL, stkf);
}                               /* end openmodule_BM */

////////////////////////////////////////////////////////////////
void
garbage_collect_if_wanted_BM (struct stackframe_stBM *stkf)
{
  if (atomic_load (&want_garbage_collection_BM))
    full_garbage_collection_BM (stkf);
}                               /* end garbage_collect_if_wanted_BM */

#endif /*INLINE_BM_INCLUDED */
