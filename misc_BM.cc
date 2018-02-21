// file misc_BM.cc

#include <fnmatch.h>
#include <map>
#include <new>
#include <set>
#include <vector>
#include <deque>
#include <mutex>
#include <condition_variable>
#include <unordered_map>
#include <string>
#include <atomic>
#include <thread>
#include <ratio>
#include <chrono>
extern "C" {
#include <gtk/gtk.h>
#include <glib.h>
#include "bismon.h"
};

//// order with strcmp
struct StrcmpLess_BM
{
  inline bool operator() (const char*s1, const char*s2) const
  {
    ASSERT_BM (s1 != nullptr);
    ASSERT_BM (s2 != nullptr);
    return strcmp(s1,s2)<0;
  }
};				// end StrcmpLess_BM

//// order with
struct ValStringLess_BM
{
  inline bool operator() ( stringval_tyBM*vs1,  stringval_tyBM*vs2) const
  {
    ASSERT_BM (valtype_BM((const value_tyBM)vs1) == tyString_BM);
    ASSERT_BM (valtype_BM((const value_tyBM)vs2) == tyString_BM);
    return strcmp(vs1->strv_bytes,vs2->strv_bytes)<0;
  }
};				// end ValStringLess_BM

struct ObjectHash_BM
{
  inline size_t operator() (const objectval_tyBM*pob) const
  {
    return objecthash_BM(pob);
  };
};				// end Objecthash_BM

struct ObjectLess_BM
{
  inline bool operator() (const objectval_tyBM*pob1, const objectval_tyBM*pob2) const
  {
    return objectcmp_BM(pob1, pob2)<0;
  };
};				// end ObjectLess_BM

struct IdLess_BM
{
  inline bool operator() (const rawid_tyBM&id1, const rawid_tyBM&id2)
  {
    return cmpid_BM (id1, id2) < 0;
  };
};				// end IdLess_BM


struct threadinfo_stBM;

// keys are strdup-ed strings, values are objectval_tyBM*
static std::map<const char*,objectval_tyBM*,StrcmpLess_BM> namemap_BM;


// keys are objectval_tyBM*, values are strdup-ed strings
static std::unordered_map<objectval_tyBM*,const char*,ObjectHash_BM> objhashtable_BM;

struct failurelockset_stBM
{
  friend void register_failock_BM (struct failurelockset_stBM *,
                                   objectval_tyBM *);
  friend void unregister_failock_BM (struct failurelockset_stBM *,
                                     objectval_tyBM *);
  std::multiset<objectval_tyBM*,ObjectLess_BM> flhobjset;
  failurelockset_stBM () {};
  ~failurelockset_stBM ()
  {
    for (objectval_tyBM* ob : flhobjset)
      {
        ASSERT_BM (valtype_BM(ob) == tyObject_BM);
        pthread_mutex_unlock(&ob->ob_mutex);
      }
  }
};

struct ModuleData_BM
{
  rawid_tyBM mod_id;
  void* mod_dlh;		// the dlopen handle
  objectval_tyBM* mod_obj;	// the module object
};

static std::map<rawid_tyBM,ModuleData_BM,IdLess_BM> modulemap_BM;



////////////////
bool
validname_BM (const char *nam)
{
  if (!nam)
    return false;
  if (!isalpha (nam[0]))
    return false;
  const char *pc = nam;
  for (; *pc; pc++)
    {
      if (isalnum (*pc))
        continue;
      else if (*pc == '_')
        {
          if (!isalnum (pc[1]))
            return false;
          if (!isalnum (pc[-1]))
            return false;
          continue;
        }
      else
        return false;
    }
  return true;
}                               /* end validname_BM */


static void
add_predefined_name_BM (const char *name, objectval_tyBM * obj)
{
  ASSERT_BM (validname_BM (name));
  ASSERT_BM (isobject_BM (obj));
  char *dupname = strdup (name);
  if (!dupname)
    FATAL_BM ("strdup %s failed (%m)", name);
  ASSERT_BM (namemap_BM.find(name) == namemap_BM.end());
  ASSERT_BM (objhashtable_BM.find(obj) == objhashtable_BM.end());
  namemap_BM.insert({dupname,obj});
  objhashtable_BM.insert({obj,dupname});
}                               /* end add_predefined_name_BM */

void
initialize_predefined_names_BM (void)
{
  objhashtable_BM.reserve(3*BM_NB_PREDEFINED+100);
#define HAS_NAMED_PREDEF_BM(Nam,Id) \
  add_predefined_name_BM(#Nam,PREDEF_BM(Id));
#include "_bm_predef.h"
}                               /* end initialize_predefined_names_BM */


const objectval_tyBM *
findnamedobj_BM (const char *nam)
{
  if (!nam || !nam[0] || !validname_BM (nam))
    return nullptr;
  auto it = namemap_BM.find(nam);
  if (it != namemap_BM.end())
    return it->second;
  return nullptr;
}                               /* end findnamedobj_BM */

const char *
findobjectname_BM (const objectval_tyBM * obj)
{
  if (!isobject_BM ((const value_tyBM) obj))
    return nullptr;
  auto it = objhashtable_BM.find(const_cast<objectval_tyBM*>(obj));
  if (it != objhashtable_BM.end())
    return it->second;
  return nullptr;
}                               /* end findobjectname_BM */


bool
registername_BM (const objectval_tyBM * obj, const char *nam)
{
  static long regcount;
  if (!isobject_BM ((const value_tyBM) obj))
    return false;
  if (!validname_BM (nam))
    return false;
  if (namemap_BM.find(nam) != namemap_BM.end())
    return false;
  if (objhashtable_BM.find(const_cast<objectval_tyBM*>(obj))
      != objhashtable_BM.end())
    return false;
  char *dupname = strdup (nam);
  if (!dupname)
    FATAL_BM ("strdup %s failed (%m)", nam);
  namemap_BM.insert({dupname,const_cast<objectval_tyBM*>(obj)});
  objhashtable_BM.emplace(const_cast<objectval_tyBM*>(obj),dupname);
  regcount++;
  if (regcount % 128)
    objhashtable_BM.rehash(0);
  return true;
}                               /* end registername_BM */


bool
forgetnamedobject_BM (const objectval_tyBM * obj)
{
  if (!isobject_BM ((const value_tyBM) obj))
    return false;
  auto itob = objhashtable_BM.find(const_cast<objectval_tyBM*>(obj));
  if (itob == objhashtable_BM.end())
    return false;
  const char* nam = itob->second;
  auto itn = namemap_BM.find(nam);
  ASSERT_BM (itn != namemap_BM.end() && itn->second == obj);
  objhashtable_BM.erase(itob);
  namemap_BM.erase(itn);
  free ((void*)nam);
  return true;
} /* end forgetnamedobject_BM */


bool
forgetnamestring_BM (const char *nam)
{
  if (!nam || !validname_BM(nam)) return false;
  auto itn = namemap_BM.find(nam);
  if (itn == namemap_BM.end())
    return false;
  objectval_tyBM* ob = itn->second;
  ASSERT_BM (isobject_BM(ob));
  auto itob = objhashtable_BM.find(ob);
  ASSERT_BM (itob != objhashtable_BM.end());
  const char*dupnam = itob->second;
  objhashtable_BM.erase(itob);
  namemap_BM.erase(itn);
  free ((void*)dupnam);
  return true;
} /* end forgetnamestring_BM */


const setval_tyBM *
setofnamedobjects_BM (void)
{
  std::vector<const objectval_tyBM *> vectobj;
  vectobj.reserve(namemap_BM.size());
  for (auto itn: namemap_BM)
    {
      const objectval_tyBM* ob = itn.second;
      ASSERT_BM (isobject_BM((const value_tyBM)ob));
      vectobj.push_back(ob);
    };
  return makeset_BM(vectobj.data(), vectobj.size());
} // end setofnamedobjects_BM


const setval_tyBM *
setofprefixednamedobjects_BM (const char*prefix)
{
  if (!prefix || !prefix[0]) return setofnamedobjects_BM();
  int prefixlen = strlen(prefix);
  std::vector<const objectval_tyBM *> vectobj;
  vectobj.reserve(2*TINYSIZE_BM);
  for (auto itn = namemap_BM.lower_bound(prefix);
       itn != namemap_BM.end(); itn++)
    {
      int cmp = strncmp(itn->first, prefix, prefixlen);
      if (cmp) break;
      const objectval_tyBM* ob = itn->second;
      ASSERT_BM (isobject_BM((const value_tyBM)ob));
      vectobj.push_back(ob);
    }
  return makeset_BM(vectobj.data(), vectobj.size());
} // end setofprefixednamedobjects_BM


const setval_tyBM*
setofmatchednamedobjects_BM(const char*fnmatcher)
{
  if (!fnmatcher || !fnmatcher[0]) return setofnamedobjects_BM();
  std::vector<const objectval_tyBM *> vectobj;
  vectobj.reserve(namemap_BM.size()/16+10);
  bool allcases = (fnmatcher[0] == '~');
  for (auto itn : namemap_BM)
    {
      const objectval_tyBM* ob = itn.second;
      ASSERT_BM (isobject_BM((const value_tyBM)ob));
      if (!fnmatch(allcases?(fnmatcher+1):fnmatcher, itn.first,
                   allcases ? (FNM_EXTMATCH | FNM_CASEFOLD)
                   : FNM_EXTMATCH))
        vectobj.push_back(ob);
    }
  return makeset_BM(vectobj.data(), vectobj.size());
} // end setofmatchednamedobjects_BM

const char*
findnameafter_BM(const char*prefix)
{
  if (!prefix || prefix[0]==(char)0)
    {
      if (namemap_BM.empty()) return nullptr;
      auto firstn = namemap_BM.begin();
      return firstn->first;
    }
  auto itn = namemap_BM.upper_bound(prefix);
  if (itn != namemap_BM.end())
    return itn->first;
  return nullptr;
} // end of findnameafter_BM


const char*
findnamesameorafter_BM(const char*prefix)
{
  if (!prefix || prefix[0]==(char)0)
    {
      if (namemap_BM.empty()) return nullptr;
      auto firstn = namemap_BM.begin();
      return firstn->first;
    }
  auto itn = namemap_BM.lower_bound(prefix);
  if (itn != namemap_BM.end())
    return itn->first;
  return nullptr;
} // end of findnamesameorafter_BM


const char*
findnamebefore_BM(const char*prefix)
{
  if (!prefix || prefix[0]==(char)0)
    {
      if (namemap_BM.empty())
        return nullptr;
      auto lastn = namemap_BM.end();
      lastn--;
      return lastn->first;
    }
  auto itn = namemap_BM.lower_bound(prefix);
  if (itn != namemap_BM.begin())
    itn--;
  else
    return nullptr;
  if (itn != namemap_BM.end())
    return itn->first;
  return nullptr;
} // end of findnamebefore_BM



////////////////////////////////////////////////////////////////

static std::map<std::string, objectval_tyBM**> mapglobals_BM;

void initialize_globals_BM(void)
{
#define HAS_GLOBAL_BM(Gnam) do {		\
  ASSERT_BM (mapglobals_BM.find(#Gnam)		\
	  == mapglobals_BM.end());		\
  mapglobals_BM[#Gnam] = &GLOBAL_BM(Gnam);	\
} while(0);
#include "_bm_global.h"
} // end of initialize_globals_BM

void
gcmarkglobals_BM(struct garbcoll_stBM*gc)
{
  ASSERT_BM (gc && gc->gc_magic == GCMAGIC_BM);
  for (auto it: mapglobals_BM)
    if (it.second && *it.second)
      gcobjmark_BM(gc, *it.second);
} // end gcmarkglobals_BM


const node_tyBM *
nodeglobalnames_BM (const objectval_tyBM * connob)
{
  if (!isobject_BM((const value_tyBM)connob))
    return nullptr;
  std::vector<value_tyBM> vecstr;
  vecstr.reserve(mapglobals_BM.size());
  for (auto it: mapglobals_BM)
    {
      const std::string& curname = it.first;
      vecstr.push_back((value_tyBM)makestring_BM(curname.c_str()));
    }
  return makenode_BM (connob, vecstr.size(), vecstr.data());
} // end nodeglobalnames_BM


const setval_tyBM *
setglobalobjects_BM(void)
{
  std::vector<objectval_tyBM*> vecobj;
  std::set<objectval_tyBM*,ObjectLess_BM> setobj;
  vecobj.reserve(mapglobals_BM.size());
  for (auto it: mapglobals_BM)
    {
      objectval_tyBM* curob = *(it.second);
      if (!curob || valtype_BM((const value_tyBM)curob) != tyObject_BM)
        continue;
      if (setobj.find (curob) != setobj.end()) continue;
      vecobj.push_back(curob);
    };
  return makeset_BM((const objectval_tyBM**)(vecobj.data()), vecobj.size());
} // end setglobalobjects_BM

bool
openmoduleforloader_BM(const rawid_tyBM modid,struct loader_stBM*ld, struct  stackframe_stBM *stkf)
{
  if (modid.id_hi == 0) return false;
  char modidbuf[32];
  memset (modidbuf, 0, sizeof(modidbuf));
  idtocbuf32_BM(modid, modidbuf);
  std::string commonprefixstr = //
    std::string{bismon_directory} + "/" + MODULEDIR_BM + "/" + MODULEPREFIX_BM + modidbuf;

  std::string srcmodpath = //
    commonprefixstr + ".c";
  std::string binmodpath =  //
    commonprefixstr + ".so";
  struct stat srcmodstat = {};
  struct stat binmodstat = {};
  if (::stat(srcmodpath.c_str(), &srcmodstat))
    {
      fprintf(stderr, "missing module source %s (%m)\n", srcmodpath.c_str());
      return false;
    }
  if (::stat(binmodpath.c_str(), &binmodstat))
    {
      fprintf(stderr, "missing module binary %s (%m)\n", binmodpath.c_str());
      return false;
    }
  if (srcmodstat.st_mtime > binmodstat.st_mtime)
    {
      struct tm srctm = {};
      struct tm bintm = {};
      char srcti[64] = "", binti[64] = "";
      strftime(srcti, sizeof(srcti), "%c %Z", localtime_r(&srcmodstat.st_mtime, &srctm));
      strftime(binti, sizeof(binti), "%c %Z", localtime_r(&binmodstat.st_mtime, &bintm));
      fprintf (stderr, "module source %s [%s] younger than binary %s [%s]\n",
               srcmodpath.c_str(), srcti, binmodpath.c_str(), binti);
      return false;
    }
  if (modulemap_BM.find(modid) != modulemap_BM.end())
    {
      // module already loaded
      return true;
    }
  void*dlh = dlopen(binmodpath.c_str(), RTLD_NOW | RTLD_GLOBAL);
  if (!dlh)
    {
      fprintf(stderr, "module dlopen failure %s\n", dlerror());
      return false;
    }
  const char*modidad = (const char*)dlsym(dlh,"module_id_BM");
  if (!modidad || strcmp(modidad,modidbuf))
    {
      fprintf(stderr, "bad module_id_BM in %s : %s\n",
              binmodpath.c_str(), (modidad?"modid mismatch":dlerror()));
      dlclose(dlh);
      return false;
    }
  objectval_tyBM* objmod = ld?makeobjofid_BM(modid):findobjofid_BM(modid);
  if (!objmod)
    {
      fprintf(stderr, "no object for module %s\n", modidbuf);
      dlclose(dlh);
      return false;
    }
  modulemap_BM.insert({modid,ModuleData_BM{.mod_id=modid, .mod_dlh=dlh, .mod_obj=objmod}});
  if (ld)
    {
      ASSERT_BM (ld->ld_magic == LOADERMAGIC_BM);
      ld->ld_modhset = hashsetobj_add_BM(ld->ld_modhset, objmod);
      value_tyBM closargs[2] = {NULL,NULL};
      closargs[0] = objmod;
      const closure_tyBM*closloadm = makeclosure_BM (BMP_load_module,1,closargs);
      load_addtodo_BM (closloadm);
    }
  else
    {
      value_tyBM v = send0_BM((const value_tyBM)objmod, BMP_load_module, stkf);
      if (!v)
        {
          fprintf(stderr, "failed to send load_module for %s\n", modidbuf);
          modulemap_BM.erase(modid);
          dlclose(dlh);
          return false;
        }
    }
  ld->ld_nbmodules++;
  return true;
} // end of openmoduleforloader_BM

////////////////////////////////////////////////////////////////

typedef std::map<stringval_tyBM*,value_tyBM,ValStringLess_BM> dictmap_claBM;

struct dict_stBM*
dictmake_BM(void)
{
  struct dict_stBM*dict = //
    (struct dict_stBM*)allocgcty_BM(typayl_dict_BM, sizeof(struct dict_stBM));
  static_assert (sizeof (dict->dict_data) >= sizeof(dictmap_claBM), "too small dictdata");
  static_assert (alignof (dict->dict_data) >= alignof(dictmap_claBM), "too small dictdata");
  new(dict->dict_data) dictmap_claBM();
  return dict;
} // end dictmake_BM


void
dictgcmark_BM(struct garbcoll_stBM *gc, struct dict_stBM*dict,
              int depth)
{
  ASSERT_BM (gc && gc->gc_magic == GCMAGIC_BM);
  ASSERT_BM (isdict_BM (dict));
  uint8_t oldmark = ((typedhead_tyBM *) dict)->hgc;
  if (oldmark)
    return;
  ((typedhead_tyBM *)dict)->hgc = MARKGC_BM;
  auto& dicm = *(dictmap_claBM*)dict->dict_data;
  for (auto& it : dicm)
    {
      VALUEGCPROC_BM(gc, *(void**) &it.first, depth+1);
      VALUEGCPROC_BM(gc, *(void**) &it.second, depth+1);
    }
} // end dictgcmark_BM


void dictgcdestroy_BM (struct garbcoll_stBM *gc, struct dict_stBM*dict)
{
  ASSERT_BM (gc && gc->gc_magic == GCMAGIC_BM);
  ASSERT_BM (isdict_BM (dict));
  auto& dicm = *(dictmap_claBM*)dict->dict_data;
  size_t siz = dicm.size();
  dicm.clear();
  dicm.~dictmap_claBM();
  gc->gc_freedbytes += sizeof(*dict) + siz*2*sizeof(void*);
  memset(dict, 0, sizeof(*dict));
  free (dict);
} // end dictgcdestroy_BM


void dictgckeep_BM (struct garbcoll_stBM *gc, struct dict_stBM*dict)
{
  ASSERT_BM (gc && gc->gc_magic == GCMAGIC_BM);
  ASSERT_BM (isdict_BM (dict));
  auto& dicm = *(dictmap_claBM*)dict->dict_data;
  size_t siz = dicm.size();
  gc->gc_keptbytes += sizeof(*dict) + siz*2*sizeof(void*);
} // end dictgckeep_BM


value_tyBM
dictget_BM(const struct dict_stBM* dict, const stringval_tyBM*str)
{
  if (!isdict_BM((const value_tyBM)dict))
    return nullptr;
  if (!isstring_BM((const value_tyBM)str))
    return nullptr;
  auto& dicm = *(dictmap_claBM*)dict->dict_data;
  auto it = dicm.find(const_cast<stringval_tyBM*>(str));
  if (it == dicm.end()) return nullptr;
  return it->second;
} // end dictget_BM


unsigned
dictsize_BM(const struct dict_stBM* dict)
{
  if (!isdict_BM((const value_tyBM)dict))
    return 0;
  auto& dicm = *(dictmap_claBM*)dict->dict_data;
  return dicm.size();
} // end dictsize_BM


void dictput_BM(struct dict_stBM* dict, const stringval_tyBM*str, const value_tyBM val)
{
  if (!isdict_BM((const value_tyBM)dict))
    return;
  if (!isstring_BM((const value_tyBM)str))
    return;
  auto& dicm = *(dictmap_claBM*)dict->dict_data;
  if (dicm.size() > MAXSIZE_BM)
    FATAL_BM("too big dict %lu", (long) dicm.size());
  if (val) dicm.insert({const_cast<stringval_tyBM*>(str),(void*)val});
  else dicm.erase(const_cast<stringval_tyBM*>(str));
} // end dictput_BM


void dictremove_BM(struct dict_stBM* dict, const stringval_tyBM*str)
{
  if (!isdict_BM((const value_tyBM)dict))
    return;
  if (!isstring_BM((const value_tyBM)str))
    return;
  auto& dicm = *(dictmap_claBM*)dict->dict_data;
  dicm.erase(const_cast<stringval_tyBM*>(str));
} // end dictremove_BM


const stringval_tyBM*
dictkeyafter_BM(struct dict_stBM* dict, const stringval_tyBM*str)
{
  if (!isdict_BM((const value_tyBM)dict))
    return nullptr;
  auto& dicm = *(dictmap_claBM*)dict->dict_data;
  if (!isstring_BM((const value_tyBM)str)
      || !bytstring_BM(str)[0])
    {
      if (dicm.empty()) return nullptr;
      auto firstn = dicm.begin();
      return firstn->first;
    }
  auto itn = dicm.upper_bound(const_cast<stringval_tyBM*>(str));
  if (itn != dicm.end())
    return itn->first;
  return nullptr;
} // end of dictkeyafter_BM


extern const stringval_tyBM*
dictkeysameorafter_BM(struct dict_stBM* dict, const stringval_tyBM*str)
{
  if (!isdict_BM((const value_tyBM)dict))
    return nullptr;
  auto& dicm = *(dictmap_claBM*)dict->dict_data;
  if (!isstring_BM((const value_tyBM)str)
      || !bytstring_BM(str)[0])
    {
      if (dicm.empty()) return nullptr;
      auto firstn = dicm.begin();
      return firstn->first;
    }
  auto itn = dicm.lower_bound(const_cast<stringval_tyBM*>(str));
  if (itn != dicm.end())
    return itn->first;
  return nullptr;
} // end of dictkeysameorafter_BM

const stringval_tyBM*
dictkeybefore_BM(struct dict_stBM* dict, const stringval_tyBM*str)
{
  if (!isdict_BM((const value_tyBM)dict))
    return nullptr;
  auto& dicm = *(dictmap_claBM*)dict->dict_data;
  if (!isstring_BM((const value_tyBM)str)
      || !bytstring_BM(str)[0])
    {
      if (dicm.empty())
        return nullptr;
      auto lastn = dicm.end();
      lastn--;
      return lastn->first;
    }
  auto itn = dicm.lower_bound(const_cast<stringval_tyBM*>(str));
  if (itn != dicm.begin())
    itn--;
  else
    return nullptr;
  if (itn != dicm.end())
    return itn->first;
  return nullptr;
} // end dictkeybefore_BM

const node_tyBM*
dictnodeofkeys_BM(struct dict_stBM* dict, const objectval_tyBM*obj)
{
  if (!isdict_BM((const value_tyBM)dict))
    return nullptr;
  if (!isobject_BM((const value_tyBM)obj))
    return nullptr;
  auto& dicm = *(dictmap_claBM*)dict->dict_data;
  value_tyBM*arr = (value_tyBM*) calloc(dicm.size(), sizeof(value_tyBM));
  if (!arr) FATAL_BM("calloc failure for %u keys", (unsigned) dicm.size());
  int cnt = 0;
  for (auto it : dicm)
    {
      arr[cnt++] = (value_tyBM)it.first;
    }
  const node_tyBM* nodv = makenode_BM(obj, cnt, arr);
  free (arr);
  return nodv;
} // end dictnodeofkeys_BM


////////////////////////////////////////////////////////////////
/****** support for command window and paren blinking ******/

static std::map<int,parenoffset_stBM> cmd_openmap_BM;
static std::map<int,parenoffset_stBM> cmd_closemap_BM;

void
cmd_clear_parens_BM(void)
{
  cmd_openmap_BM.clear();
  cmd_closemap_BM.clear();
} // end cmd_clear_parens_BM


void
cmd_add_parens_BM (struct parenoffset_stBM*par)
{
  ASSERT_BM (par != nullptr);
  cmd_openmap_BM.insert({par->paroff_open,*par});
  cmd_closemap_BM.insert({par->paroff_close,*par});
} // end cmd_add_parens_BM

bool
parens_surrounds_BM(struct parenoffset_stBM*par, int off)
{
  if (!par)
    return false;
  /// when ( ^ ) or [ ^ ] or { ^ }
  if (par->paroff_open <= off && off <= par->paroff_close)
    return true;
  /// when * conn ^ ( ... )
  if (par->paroff_xtralen > 0 && par->paroff_xtra >= 0 && par->paroff_xtra <= off
      && off <= par->paroff_open && par->paroff_xtra < par->paroff_open)
    return true;
  return false;
} // end cmd_parens_surrounds_BM

struct parenoffset_stBM*
cmd_find_enclosing_parens_BM(int off)
{
  // we probably might code something faster, but is it worth the hassle?
  if (cmd_closemap_BM.empty())
    return nullptr;
  auto it = cmd_closemap_BM.lower_bound (off);
  if (it == cmd_closemap_BM.end())
    return nullptr;
  do
    {
      struct parenoffset_stBM*po = &it->second;
      if (parens_surrounds_BM(po, off))
        return po;
      it--;
    }
  while (it != cmd_closemap_BM.begin());
  return nullptr;
} // end cmd_find_enclosing_parens_BM


////////////////////////////////////////////////////////////////

struct deferdoappl_stBM
{
  union
  {
    const closure_tyBM *defer_clos;
    objectval_tyBM* defer_obsel;
  };
  value_tyBM defer_recv;
  value_tyBM defer_arg1;
  value_tyBM defer_arg2;
  value_tyBM defer_arg3;
};
static std::deque<deferdoappl_stBM> deferdeque_BM;
static std::mutex deferqmtx_BM;

void
gcmarkdefergtk_BM(struct garbcoll_stBM*gc)
{
  ASSERT_BM (gc && gc->gc_magic == GCMAGIC_BM);
  std::lock_guard<std::mutex> _g(deferqmtx_BM);
  for (auto itd : deferdeque_BM)
    {
      if (itd.defer_recv)
        {
          VALUEGCPROC_BM (gc, itd.defer_clos, 0);
          gcobjmark_BM(gc, itd.defer_obsel);
        }
      else
        VALUEGCPROC_BM (gc, itd.defer_clos, 0);
      if (itd.defer_arg1)
        VALUEGCPROC_BM (gc, itd.defer_arg1, 0);
      if (itd.defer_arg2)
        VALUEGCPROC_BM (gc, itd.defer_arg2, 0);
      if (itd.defer_arg3)
        VALUEGCPROC_BM (gc, itd.defer_arg3, 0);
    }
} // end gcmarkdefergtk_BM

extern "C" void
do_internal_deferred_send3_gtk_BM(value_tyBM recv, objectval_tyBM*obsel, value_tyBM arg1, value_tyBM arg2, value_tyBM arg3);

extern "C" void
do_internal_deferred_apply3_gtk_BM(const closure_tyBM*clos, value_tyBM arg1, value_tyBM arg2, value_tyBM arg3);

extern "C" bool did_deferredgtk_BM (void);

bool
did_deferredgtk_BM (void)
{
  const closure_tyBM *dclosv = nullptr;
  objectval_tyBM* dobsel = nullptr;
  value_tyBM darg1v = nullptr;
  value_tyBM darg2v = nullptr;
  value_tyBM darg3v = nullptr;
  value_tyBM drecv = nullptr;
  {
    std::lock_guard<std::mutex> _g(deferqmtx_BM);
    if (deferdeque_BM.empty()) return false;
    auto f = deferdeque_BM.front();
    if (f.defer_recv)
      {
        drecv = f.defer_recv;
        dobsel = f.defer_obsel;
      }
    else
      {
        dclosv = f.defer_clos;
        drecv = nullptr;
      };
    darg1v = f.defer_arg1;
    darg2v = f.defer_arg2;
    darg3v = f.defer_arg3;
    deferdeque_BM.pop_front();
  }
  if (drecv)
    do_internal_deferred_send3_gtk_BM(drecv, dobsel, darg1v, darg2v, darg3v);
  else
    do_internal_deferred_apply3_gtk_BM(dclosv, darg1v, darg2v, darg3v);
  return true;
} // end did_deferredgtk_BM




extern "C" int defer_gtk_writepipefd_BM;
void
gtk_defer_apply3_BM (value_tyBM closv, value_tyBM arg1, value_tyBM arg2, value_tyBM arg3)
{
  if (!isclosure_BM(closv)) return;
  if (defer_gtk_writepipefd_BM<0)
    FATAL_BM("gtk_defer_apply3_BM without writepipe");
  char ch = "0123456789abcdefghijklmnopqrstuvwxyz" [valhash_BM (closv) % 36];
  {
    std::lock_guard<std::mutex> _g(deferqmtx_BM);
    struct deferdoappl_stBM dap = {};
    dap.defer_clos = closurecast_BM(closv);
    dap.defer_recv = nullptr;
    dap.defer_arg1 = arg1;
    dap.defer_arg2 = arg2;
    dap.defer_arg3 = arg3;
    deferdeque_BM.emplace_back(dap);
  }
  int nbtry = 0;
  int wrcnt = 0;
  for(;;)   // most of the time, this loop runs once
    {
      wrcnt = write(defer_gtk_writepipefd_BM, &ch, 1);
      if (wrcnt>0) return;
      usleep(1000);
      nbtry++;
      if (nbtry > 256)
        FATAL_BM("gtk_defer_apply3_BM failed to write to pipe");
    }
} // end gtk_defer_apply3_BM


void
gtk_defer_send3_BM(value_tyBM recv, objectval_tyBM*obsel,  value_tyBM arg1, value_tyBM arg2, value_tyBM arg3)
{
  if (!recv) return;
  if (!isobject_BM(obsel)) return;
  if (defer_gtk_writepipefd_BM<0)
    FATAL_BM("gtk_defer_send3_BM without writepipe");
  char ch = "0123456789abcdefghijklmnopqrstuvwxyz" [valhash_BM (obsel) % 36];
  {
    std::lock_guard<std::mutex> _g(deferqmtx_BM);
    struct deferdoappl_stBM dap = {};
    dap.defer_obsel = obsel;
    dap.defer_recv = recv;
    dap.defer_arg1 = arg1;
    dap.defer_arg2 = arg2;
    dap.defer_arg3 = arg3;
    deferdeque_BM.emplace_back(dap);
  }
  int nbtry = 0;
  int wrcnt = 0;
  for(;;)   // most of the time, this loop runs once
    {
      wrcnt = write(defer_gtk_writepipefd_BM, &ch, 1);
      if (wrcnt>0)
        return;
      usleep(1000);
      nbtry++;
      if (nbtry > 256)
        FATAL_BM("gtk_defer_send3_BM failed to write to pipe");
    }
} // end of gtk_defer_send3_BM



////////////////////////////////////////////////////////////////
/********** agenda support **********/
#ifndef AGENDA_SLOW_FACTOR_BM
#define AGENDA_SLOW_FACTOR_BM 3
#endif /*AGENDA_SLOW_FACTOR_BM*/

typedef void threadidle_sigtBM (int thrank);

#define TI_MAGICNUM_BM 0x67d1
struct threadinfo_stBM
{
  const unsigned ti_magic;
  short ti_rank;
  std::atomic_bool ti_stop;
  std::atomic_bool ti_gc;
  std::thread ti_thread;
  double ti_thstartelapsedtime;
  double ti_thstartcputime;
  std::atomic<threadidle_sigtBM*> ti_idlerout;
  static threadinfo_stBM ti_array[MAXNBWORKJOBS_BM+2];
  static std::atomic_bool ti_needgc;
  static std::mutex ti_agendamtx;
  static std::condition_variable  ti_agendacondv;
  static std::atomic_int ti_nbworkthreads;
  static std::atomic_int ti_countendedthreads;
  static std::deque<objectval_tyBM*> ti_taskque_veryhigh;
  static std::deque<objectval_tyBM*> ti_taskque_high;
  static std::deque<objectval_tyBM*> ti_taskque_low;
  static std::deque<objectval_tyBM*> ti_taskque_verylow;
  static std::unordered_map<objectval_tyBM*,std::deque<objectval_tyBM*>*,ObjectHash_BM> ti_task_hmap;
  static void thread_run(const int ix);
  friend void start_agenda_work_threads_BM(int nbjobs);
  friend void stop_agenda_work_threads_BM(void);
  friend void agenda_suspend_for_gc_BM (void);
  friend void agenda_continue_after_gc_BM(void);
  friend void gcmarkagenda_BM (struct garbcoll_stBM *gc);
  friend void agenda_add_very_high_priority_tasklet_front_BM (objectval_tyBM * obtk);
  friend void agenda_add_very_high_priority_tasklet_back_BM (objectval_tyBM * obtk);
  friend void agenda_add_high_priority_tasklet_front_BM (objectval_tyBM * obtk);
  friend void agenda_add_high_priority_tasklet_back_BM (objectval_tyBM * obtk);
  friend void agenda_add_very_low_priority_tasklet_front_BM (objectval_tyBM *obtk);
  friend void agenda_add_very_low_priority_tasklet_back_BM (objectval_tyBM * obtk);
  friend void agenda_add_low_priority_tasklet_front_BM (objectval_tyBM * obtk);
  friend void agenda_add_low_priority_tasklet_back_BM (objectval_tyBM * obtk);
  friend bool agenda_remove_tasklet_BM (objectval_tyBM * obtk);
  friend long agenda_get_counts_BM(long*pveryhigh, long*phigh, long*plow, long*pverylow);
  friend long agenda_get_tuples_BM(value_tyBM*pveryhightup, value_tyBM*phightup,  value_tyBM*plowtup, value_tyBM*pverylowtup);
  friend double taskletcputime_BM (void);
  friend double taskletelapsedtime_BM (void);
  ////
  threadinfo_stBM() : ti_magic(TI_MAGICNUM_BM)
  {
    ti_rank = 0;
    atomic_init(&ti_stop, false);
    atomic_init(&ti_gc, false);
    atomic_init(&ti_idlerout,(threadidle_sigtBM*) nullptr);
  }
  ///
private:
  static bool remove_from_taskque(objectval_tyBM*tob, std::deque<objectval_tyBM*>& taskque)
  {
    for (auto it = taskque.begin(); it != taskque.end(); it++)
      {
        if (*it == tob)
          {
            taskque.erase(it);
            return true;
          }
      }
    return false;
  }; // end remove_from_taskque
  static const tupleval_tyBM*tuple_from_taskque(std::deque<objectval_tyBM*>& taskque);
};				// end threadinfo_stBM


threadinfo_stBM threadinfo_stBM::ti_array[MAXNBWORKJOBS_BM+2];
std::mutex threadinfo_stBM::ti_agendamtx;
std::atomic_bool threadinfo_stBM::ti_needgc;
std::atomic_int threadinfo_stBM::ti_nbworkthreads;
std::atomic_int threadinfo_stBM::ti_countendedthreads;
std::condition_variable threadinfo_stBM::ti_agendacondv;
std::deque<objectval_tyBM*> threadinfo_stBM::ti_taskque_veryhigh;
std::deque<objectval_tyBM*> threadinfo_stBM::ti_taskque_high;
std::deque<objectval_tyBM*> threadinfo_stBM::ti_taskque_low;
std::deque<objectval_tyBM*> threadinfo_stBM::ti_taskque_verylow;
std::unordered_map<objectval_tyBM*,std::deque<objectval_tyBM*>*,ObjectHash_BM> threadinfo_stBM::ti_task_hmap;

void
start_agenda_work_threads_BM (int nbjobs)
{
  DBGPRINTF_BM("start_agenda_work_threads start nbjobs=%d", nbjobs);
  atomic_init(&threadinfo_stBM::ti_countendedthreads, 0);
  atomic_init(&threadinfo_stBM::ti_nbworkthreads, 0);
  ASSERT_BM (pthread_self() == mainthreadid_BM);
  ASSERT_BM (nbjobs >= MINNBWORKJOBS_BM && nbjobs <= MAXNBWORKJOBS_BM);
  atomic_store(&threadinfo_stBM::ti_nbworkthreads, nbjobs);
  for (int tix=1; tix<=nbjobs; tix++)
    {
      DBGPRINTF_BM("start_agenda_work_threads tix=%d", tix);
      auto& curth = threadinfo_stBM::ti_array[tix];
      curth.ti_rank = tix;
      curth.ti_thread  = std::thread(threadinfo_stBM::thread_run,tix);
      usleep (1000+40*(tix%32));
    }
  usleep (200);
  for (int tix=1; tix<=nbjobs; tix++)
    {
      threadinfo_stBM::ti_array[tix].ti_thread.detach();
      usleep (100+tix%32);
    }
  DBGPRINTF_BM("start_agenda_work_threads end nbjobs=%d", nbjobs);
} // end start_agenda_work_threads_BM


void
stop_agenda_work_threads_BM(void)
{
  int nbwth = atomic_load(&threadinfo_stBM::ti_nbworkthreads);
  DBGPRINTF_BM("stop_agenda_work_threads start nbwth=%d tid#%ld elapsed %.3f s",
               nbwth, (long)gettid_BM(), elapsedtime_BM());
  {
    std::lock_guard<std::mutex> _gu(threadinfo_stBM::ti_agendamtx);
    for (int tix=1; tix<=nbwth; tix++)
      {
        auto& curth = threadinfo_stBM::ti_array[tix];
        atomic_store(&curth.ti_stop, true);
      }
  }
  threadinfo_stBM::ti_agendacondv.notify_all();
  usleep (1000);
  DBGPRINTF_BM("stop_agenda_work_threads notified nbwth=%d tid#%ld elapsed %.3f s",
               nbwth, (long)gettid_BM(), elapsedtime_BM());
  do
    {
      std::unique_lock<std::mutex> lk_(threadinfo_stBM::ti_agendamtx);
      threadinfo_stBM::ti_agendacondv.wait_for(lk_,std::chrono::milliseconds{350});
    }
  while (atomic_load(&threadinfo_stBM::ti_countendedthreads) < nbwth);
  DBGPRINTF_BM("stop_agenda_work_threads end nbwth=%d tid#%ld elapsed %.3f s",
               nbwth, (long)gettid_BM(), elapsedtime_BM());
} // end stop_agenda_work_threads_BM

// not sure if this is needed...
void
agenda_notify_BM(void)
{
  threadinfo_stBM::ti_agendacondv.notify_all();
} // end agenda_notify_BM



void
agenda_suspend_for_gc_BM (void)
{
  ASSERT_BM (curthreadinfo_BM == nullptr);
  atomic_store(&threadinfo_stBM::ti_needgc, true);
  threadinfo_stBM::ti_agendacondv.notify_all();
  bool alldoinggc = false;
  int nbwth = atomic_load(&threadinfo_stBM::ti_nbworkthreads);
  long loopcnt = 0;
  DBGPRINTF_BM("agenda_suspend_for_gc_BM nwth=%d", nbwth);
  while (!alldoinggc)
    {
      alldoinggc = false;
      {
        std::lock_guard<std::mutex> gu_(threadinfo_stBM::ti_agendamtx);
        alldoinggc = true;
        for (int tix=1; tix <= nbwth && alldoinggc; tix++)
          alldoinggc = atomic_load(&threadinfo_stBM::ti_array[tix].ti_gc);
      }
      DBGPRINTF_BM("agenda_suspend_for_gc_BM alldoinggc %s loop#%ld",
                   alldoinggc?"yes":"no", loopcnt);
      loopcnt++;
      if (!alldoinggc)
        {
          std::unique_lock<std::mutex> lk_(threadinfo_stBM::ti_agendamtx);
          threadinfo_stBM::ti_agendacondv.wait_for(lk_,
              std::chrono::milliseconds{10+(AGENDA_SLOW_FACTOR_BM*650)});
        }
    };
  DBGPRINTF_BM("agenda_suspend_for_gc_BM done");
} // end agenda_suspend_for_gc_BM



void
agenda_continue_after_gc_BM(void)
{
  ASSERT_BM (curthreadinfo_BM == nullptr);
  int nbwth = atomic_load(&threadinfo_stBM::ti_nbworkthreads);
  atomic_store(&threadinfo_stBM::ti_needgc, false);
  {
    std::lock_guard<std::mutex> gu_(threadinfo_stBM::ti_agendamtx);
    for (int tix=1; tix<=nbwth; tix++)
      atomic_store (&threadinfo_stBM::ti_array[tix].ti_gc, false);
  }
  threadinfo_stBM::ti_agendacondv.notify_all();
  atomic_store(&threadinfo_stBM::ti_needgc, false);
} // end agenda_continue_after_gc_BM



void
threadinfo_stBM::thread_run(const int tix)
{
  curthreadinfo_BM = ti_array+tix;
  DBGPRINTF_BM("thread_run tix%d start tid#%ld", tix, (long) gettid_BM());
  usleep(2000+500*tix);
  // I need to be sure that clocktime_BM(CLOCK_THREAD_CPUTIME_ID) is positive, so warmup the CPU
  {
    volatile double x = tix*0.02 + 0.001;
    while (clocktime_BM (CLOCK_THREAD_CPUTIME_ID) <= 0.0  &&  x < 0.8)
      {
        x += 0.25*sin(x + tix*0.0001) + 1.0e-6 + 1.0e-7*(getpid()%16);
      };
  }
  long loopcnt=0;
  for (;;)
    {
      loopcnt++;
      DBGPRINTF_BM("thread_run tix%d loop#%ld begin tid#%ld elapsed %.3f s",
                   tix, loopcnt, (long) gettid_BM(), elapsedtime_BM());
      objectval_tyBM*taskob = nullptr;
      if (atomic_load(&curthreadinfo_BM->ti_stop))
        break;
      if (atomic_load (&ti_needgc))
        {
          atomic_store(&curthreadinfo_BM->ti_gc, true);
          do
            {
              DBGPRINTF_BM("thread_run tix%d needgc tid#%ld elapsed %.3f s", tix, (long) gettid_BM(), elapsedtime_BM());
              ti_agendacondv.notify_all();
              std::unique_lock<std::mutex> lk_(ti_agendamtx);
              ti_agendacondv.wait_for(lk_,
                                      std::chrono::milliseconds{40+(AGENDA_SLOW_FACTOR_BM*900)});
            }
          while(atomic_load(&curthreadinfo_BM->ti_gc));
        }
      gint rdi = g_random_int ();
      {
        std::lock_guard<std::mutex> _gu(ti_agendamtx);
        std::deque<objectval_tyBM*> *taqu = nullptr;
        if (!ti_taskque_veryhigh.empty())
          taqu = &ti_taskque_veryhigh;
        if ((!taqu || ((rdi % 16 == 0), (rdi = rdi/16), true)) && !ti_taskque_high.empty())
          taqu = &ti_taskque_high;
        if (!taqu)
          {
            if (!ti_taskque_low.empty())
              taqu = &ti_taskque_low;
            if ((!taqu || ((rdi % 16 == 0), (rdi = rdi/16), true)) && !ti_taskque_verylow.empty())
              taqu = &ti_taskque_verylow;
          };
        if (taqu)
          {
            ASSERT_BM (!taqu->empty());
            taskob = taqu->front();
            taqu->pop_front();
            ti_task_hmap.erase(taskob);
          }
      }
      char idbuf[32];
      memset (idbuf, 0, sizeof(idbuf));
      DBGPRINTF_BM("thread_run tix%d taskob %s tid#%ld threadcpu %.5f", tix,
                   taskob
                   ?(idtocbuf32_BM (objid_BM (taskob), idbuf),idbuf)
                   :"*none*",
                   (long) gettid_BM(), clocktime_BM(CLOCK_THREAD_CPUTIME_ID));
      if (taskob)
        {
          failurelockset_stBM fls;
          DBGPRINTF_BM("thread_run tix%d shouldrun tid#%ld elapsed %.3f s",
                       tix,  (long) gettid_BM(), elapsedtime_BM());
          curthreadinfo_BM->ti_thstartcputime = clocktime_BM(CLOCK_THREAD_CPUTIME_ID);
          curthreadinfo_BM->ti_thstartelapsedtime = clocktime_BM(CLOCK_MONOTONIC);
          run_agenda_tasklet_BM (taskob, &fls);
          curthreadinfo_BM->ti_thstartcputime = 0.0;
          curthreadinfo_BM->ti_thstartelapsedtime = 0.0;
          DBGPRINTF_BM("thread_run tix%d didrun tid#%ld elapsed %.3f s",
                       tix,  (long) gettid_BM(), elapsedtime_BM());
        }
      else   // no task to run
        {
          std::unique_lock<std::mutex> lk_(ti_agendamtx);
          int delayms = 50 + (AGENDA_SLOW_FACTOR_BM*900) + 13*tix +
                        (atomic_load (&want_garbage_collection_BM)?0:3*(g_random_int () % 256));
          ti_agendacondv.wait_for(lk_,std::chrono::milliseconds{delayms});
          DBGPRINTF_BM("thread_run tix%d waited notask tid#%ld elapsed %.3f s",
                       tix, (long) gettid_BM(), elapsedtime_BM());
        }
    } // end forever
  curthreadinfo_BM = nullptr;
  ti_agendacondv.notify_all();
  int endcnt = ti_countendedthreads.fetch_add(1);
  DBGPRINTF_BM("thread_run end endcnt=%d tix%d tid#%ld elapsed %.3f s",
               endcnt, tix, (long) gettid_BM(), elapsedtime_BM());
} // end thread_run

extern double taskletcputime_BM (void) __attribute__((optimize("-O3")));
extern double taskletelapsedtime_BM (void) __attribute__((optimize("-O3")));

double
taskletcputime_BM (void)
{
  double ths = 0.0;
  if (curthreadinfo_BM && (ths=curthreadinfo_BM->ti_thstartcputime) > 0.0)
    return clocktime_BM(CLOCK_THREAD_CPUTIME_ID) - ths;
  return NAN;
} // end taskletcputime_BM

double
taskletelapsedtime_BM (void)
{
  double ths = 0.0;
  if (curthreadinfo_BM && (ths=curthreadinfo_BM->ti_thstartelapsedtime) > 0.0)
    return clocktime_BM(CLOCK_MONOTONIC) - ths;
  return NAN;
} // end taskletelapsedtime_BM

int
agenda_nb_work_jobs_BM (void)
{
  return atomic_load(&threadinfo_stBM::ti_nbworkthreads);
} // end agenda_nb_work_jobs_BM


void
gcmarkagenda_BM (struct garbcoll_stBM *gc)
{
  ASSERT_BM (gc && gc->gc_magic == GCMAGIC_BM);
  std::lock_guard<std::mutex> _gu(threadinfo_stBM::ti_agendamtx);
  for (objectval_tyBM*tkob : threadinfo_stBM::ti_taskque_veryhigh)
    gcobjmark_BM (gc, tkob);
  for (objectval_tyBM*tkob : threadinfo_stBM::ti_taskque_high)
    gcobjmark_BM (gc, tkob);
  for (objectval_tyBM*tkob : threadinfo_stBM::ti_taskque_low)
    gcobjmark_BM (gc, tkob);
  for (objectval_tyBM*tkob : threadinfo_stBM::ti_taskque_verylow)
    gcobjmark_BM (gc, tkob);
} // end gcmarkagenda_BM



void
agenda_add_very_high_priority_tasklet_front_BM (objectval_tyBM * obtk)
{
  if (!isobject_BM(obtk)) return;
  std::lock_guard<std::mutex> _gu(threadinfo_stBM::ti_agendamtx);
  auto it = threadinfo_stBM::ti_task_hmap.find(obtk);
  if (it != threadinfo_stBM::ti_task_hmap.end())
    {
      std::deque<objectval_tyBM*>* tq = it->second;
      bool r = threadinfo_stBM::remove_from_taskque(obtk, *tq);
      ASSERT_BM (r);
    }
  threadinfo_stBM::ti_taskque_veryhigh.push_front(obtk);
  threadinfo_stBM::ti_task_hmap.insert({obtk, & threadinfo_stBM::ti_taskque_veryhigh});
} // end  agenda_add_very_high_priority_tasklet_front_BM


void
agenda_add_very_high_priority_tasklet_back_BM (objectval_tyBM *obtk)
{
  if (!isobject_BM(obtk)) return;
  std::lock_guard<std::mutex> _gu(threadinfo_stBM::ti_agendamtx);
  auto it = threadinfo_stBM::ti_task_hmap.find(obtk);
  if (it != threadinfo_stBM::ti_task_hmap.end())
    {
      std::deque<objectval_tyBM*>* tq = it->second;
      bool r = threadinfo_stBM::remove_from_taskque(obtk, *tq);
      ASSERT_BM (r);
    }
  threadinfo_stBM::ti_taskque_veryhigh.push_back(obtk);
  threadinfo_stBM::ti_task_hmap.insert({obtk, & threadinfo_stBM::ti_taskque_veryhigh});
} // end  agenda_add_very_high_priority_tasklet_back_BM




void
agenda_add_high_priority_tasklet_front_BM (objectval_tyBM * obtk)
{
  if (!isobject_BM(obtk)) return;
  std::lock_guard<std::mutex> _gu(threadinfo_stBM::ti_agendamtx);
  auto it = threadinfo_stBM::ti_task_hmap.find(obtk);
  if (it != threadinfo_stBM::ti_task_hmap.end())
    {
      std::deque<objectval_tyBM*>* tq = it->second;
      bool r = threadinfo_stBM::remove_from_taskque(obtk, *tq);
      ASSERT_BM (r);
    }
  threadinfo_stBM::ti_taskque_high.push_front(obtk);
  threadinfo_stBM::ti_task_hmap.insert({obtk, & threadinfo_stBM::ti_taskque_high});
} // end  agenda_add_high_priority_tasklet_front_BM


void
agenda_add_high_priority_tasklet_back_BM (objectval_tyBM *obtk)
{
  if (!isobject_BM(obtk)) return;
  std::lock_guard<std::mutex> _gu(threadinfo_stBM::ti_agendamtx);
  auto it = threadinfo_stBM::ti_task_hmap.find(obtk);
  if (it != threadinfo_stBM::ti_task_hmap.end())
    {
      std::deque<objectval_tyBM*>* tq = it->second;
      bool r = threadinfo_stBM::remove_from_taskque(obtk, *tq);
      ASSERT_BM (r);
    }
  threadinfo_stBM::ti_taskque_high.push_back(obtk);
  threadinfo_stBM::ti_task_hmap.insert({obtk, & threadinfo_stBM::ti_taskque_high});
} // end  agenda_add_high_priority_tasklet_back_BM



void
agenda_add_low_priority_tasklet_front_BM (objectval_tyBM * obtk)
{
  if (!isobject_BM(obtk)) return;
  std::lock_guard<std::mutex> _gu(threadinfo_stBM::ti_agendamtx);
  auto it = threadinfo_stBM::ti_task_hmap.find(obtk);
  if (it != threadinfo_stBM::ti_task_hmap.end())
    {
      std::deque<objectval_tyBM*>* tq = it->second;
      bool r = threadinfo_stBM::remove_from_taskque(obtk, *tq);
      ASSERT_BM (r);
    }
  threadinfo_stBM::ti_taskque_low.push_front(obtk);
  threadinfo_stBM::ti_task_hmap.insert({obtk, & threadinfo_stBM::ti_taskque_low});
} // end  agenda_add_low_priority_tasklet_front_BM


void
agenda_add_low_priority_tasklet_back_BM (objectval_tyBM *obtk)
{
  if (!isobject_BM(obtk)) return;
  std::lock_guard<std::mutex> _gu(threadinfo_stBM::ti_agendamtx);
  auto it = threadinfo_stBM::ti_task_hmap.find(obtk);
  if (it != threadinfo_stBM::ti_task_hmap.end())
    {
      std::deque<objectval_tyBM*>* tq = it->second;
      bool r = threadinfo_stBM::remove_from_taskque(obtk, *tq);
      ASSERT_BM (r);
    }
  threadinfo_stBM::ti_taskque_low.push_back(obtk);
  threadinfo_stBM::ti_task_hmap.insert({obtk, & threadinfo_stBM::ti_taskque_low});
} // end  agenda_add_low_priority_tasklet_back_BM



void
agenda_add_very_low_priority_tasklet_front_BM (objectval_tyBM * obtk)
{
  if (!isobject_BM(obtk)) return;
  std::lock_guard<std::mutex> _gu(threadinfo_stBM::ti_agendamtx);
  auto it = threadinfo_stBM::ti_task_hmap.find(obtk);
  if (it != threadinfo_stBM::ti_task_hmap.end())
    {
      std::deque<objectval_tyBM*>* tq = it->second;
      bool r = threadinfo_stBM::remove_from_taskque(obtk, *tq);
      ASSERT_BM (r);
    }
  threadinfo_stBM::ti_taskque_verylow.push_front(obtk);
  threadinfo_stBM::ti_task_hmap.insert({obtk, & threadinfo_stBM::ti_taskque_verylow});
} // end  agenda_add_very_low_priority_tasklet_front_BM


void
agenda_add_very_low_priority_tasklet_back_BM (objectval_tyBM *obtk)
{
  if (!isobject_BM(obtk)) return;
  std::lock_guard<std::mutex> _gu(threadinfo_stBM::ti_agendamtx);
  auto it = threadinfo_stBM::ti_task_hmap.find(obtk);
  if (it != threadinfo_stBM::ti_task_hmap.end())
    {
      std::deque<objectval_tyBM*>* tq = it->second;
      bool r = threadinfo_stBM::remove_from_taskque(obtk, *tq);
      ASSERT_BM (r);
    }
  threadinfo_stBM::ti_taskque_verylow.push_back(obtk);
  threadinfo_stBM::ti_task_hmap.insert({obtk, & threadinfo_stBM::ti_taskque_verylow});
} // end  agenda_add_very_low_priority_tasklet_back_BM


bool
agenda_remove_tasklet_BM (objectval_tyBM *obtk)
{
  if (!isobject_BM(obtk))
    return false;
  std::lock_guard<std::mutex> _gu(threadinfo_stBM::ti_agendamtx);
  auto it = threadinfo_stBM::ti_task_hmap.find(obtk);
  if (it != threadinfo_stBM::ti_task_hmap.end())
    {
      std::deque<objectval_tyBM*>* tq = it->second;
      bool r = threadinfo_stBM::remove_from_taskque(obtk, *tq);
      ASSERT_BM (r);
      return true;
    }
  return false;
} // end agenda_remove_tasklet_BM

bool
agenda_has_tasklet_BM (objectval_tyBM *obtk)
{
  if (!isobject_BM(obtk))
    return false;
  std::lock_guard<std::mutex> _gu(threadinfo_stBM::ti_agendamtx);
  auto it = threadinfo_stBM::ti_task_hmap.find(obtk);
  if (it != threadinfo_stBM::ti_task_hmap.end())
    return true;
  return false;
} // end agenda_has_tasklet_BM


const tupleval_tyBM*
threadinfo_stBM::tuple_from_taskque(std::deque<objectval_tyBM*>& taskque)
{
  const tupleval_tyBM* res = nullptr;
  long nbtasks = taskque.size();
  objectval_tyBM*tinyarr[TINYSIZE_BM] = {};
  objectval_tyBM**arr = (nbtasks<TINYSIZE_BM)?tinyarr:(objectval_tyBM**)calloc(nbtasks,sizeof(objectval_tyBM*));
  if (!arr)
    FATAL_BM("tuple_from_taskque failed for nbtasks %ld", nbtasks);
  long count=0;
  for (objectval_tyBM*tob : taskque)
    {
      ASSERT_BM (count<nbtasks);
      arr[count++] = tob;
    };
  res = maketuple_BM(arr, nbtasks);
  if (arr != tinyarr)
    free (arr);
  return res;
} // end threadinfo_stBM::tuple_from_taskque

long
agenda_get_counts_BM(long*pveryhigh, long*phigh, long*plow, long*pverylow)
{
  std::lock_guard<std::mutex> _gu(threadinfo_stBM::ti_agendamtx);
  long nbveryhigh = threadinfo_stBM::ti_taskque_veryhigh.size();
  long nbhigh = threadinfo_stBM::ti_taskque_high.size();
  long nblow = threadinfo_stBM::ti_taskque_low.size();
  long nbverylow = threadinfo_stBM::ti_taskque_verylow.size();
  if (pveryhigh)
    *pveryhigh = nbveryhigh;
  if (phigh)
    *phigh = nbhigh;
  if (plow)
    *plow = nblow;
  if (pverylow)
    *pverylow = nbverylow;
  return nbveryhigh+nbhigh+nblow+nbverylow;
} // end agenda_get_counts_BM



long
agenda_get_tuples_BM(value_tyBM*pveryhightup, value_tyBM*phightup,  value_tyBM*plowtup, value_tyBM*pverylowtup)
{
  std::lock_guard<std::mutex> _gu(threadinfo_stBM::ti_agendamtx);
  long nbveryhigh = 0;
  long nbhigh = 0;
  long nblow = 0;
  long nbverylow = 0;
  if (pveryhightup)
    {
      auto tup = threadinfo_stBM::tuple_from_taskque(threadinfo_stBM::ti_taskque_veryhigh);
      nbveryhigh = tuplesize_BM(tup);
      *pveryhightup = (value_tyBM)tup;
    }
  else
    nbveryhigh =  threadinfo_stBM::ti_taskque_veryhigh.size();
  if (phightup)
    {
      auto tup = threadinfo_stBM::tuple_from_taskque(threadinfo_stBM::ti_taskque_high);
      nbhigh = tuplesize_BM(tup);
      *phightup = (value_tyBM)tup;
    }
  else
    nbhigh =  threadinfo_stBM::ti_taskque_high.size();
  if (plowtup)
    {
      auto tup = threadinfo_stBM::tuple_from_taskque(threadinfo_stBM::ti_taskque_low);
      nblow = tuplesize_BM(tup);
      *plowtup = (value_tyBM)tup;
    }
  else
    nblow =  threadinfo_stBM::ti_taskque_low.size();
  if (pverylowtup)
    {
      auto tup = threadinfo_stBM::tuple_from_taskque(threadinfo_stBM::ti_taskque_verylow);
      nbverylow = tuplesize_BM(tup);
      *pverylowtup = (value_tyBM)tup;
    }
  else
    nbverylow =  threadinfo_stBM::ti_taskque_verylow.size();
  return nbveryhigh+nbhigh+nblow+nbverylow;
} // end agenda_get_tuples_BM


////////////////
void
register_failock_BM(struct failurelockset_stBM*flh, objectval_tyBM*ob)
{
  ASSERT_BM (flh != nullptr);
  ASSERT_BM (isobject_BM(ob));
  flh->flhobjset.insert(ob);
} // end register_failock_BM

void
unregister_failock_BM(struct failurelockset_stBM*flh, objectval_tyBM* ob)
{
  ASSERT_BM (flh != nullptr);
  ASSERT_BM (isobject_BM(ob));
  flh->flhobjset.erase(ob);
} // end unregister_failock_BM
