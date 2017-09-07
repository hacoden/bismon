// file misc_BM.cc
extern "C" {
#include "bismon.h"
};

#include <fnmatch.h>
#include <map>
#include <new>
#include <set>
#include <vector>
#include <unordered_map>
#include <string>

//// order with strcmp
struct StrcmpLess_BM
{
  inline bool operator() (const char*s1, const char*s2) const
  {
    assert (s1 != nullptr);
    assert (s2 != nullptr);
    return strcmp(s1,s2)<0;
  }
};				// end StrcmpLess_BM

//// order with
struct ValStringLess_BM
{
  inline bool operator() (const stringval_tyBM*vs1, const stringval_tyBM*vs2) const
  {
    assert (valtype_BM((const value_tyBM)vs1) == tyString_BM);
    assert (valtype_BM((const value_tyBM)vs2) == tyString_BM);
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

// keys are strdup-ed strings, values are objectval_tyBM*
static std::map<const char*,objectval_tyBM*,StrcmpLess_BM> namemap_BM;


// keys are objectval_tyBM*, values are strdup-ed strings
static std::unordered_map<objectval_tyBM*,const char*,ObjectHash_BM> objhashtable_BM;

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
  assert (validname_BM (name));
  assert (isobject_BM (obj));
  char *dupname = strdup (name);
  if (!dupname)
    FATAL_BM ("strdup %s failed (%m)", name);
  assert (namemap_BM.find(name) == namemap_BM.end());
  assert (objhashtable_BM.find(obj) == objhashtable_BM.end());
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
  assert (itn != namemap_BM.end() && itn->second == obj);
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
  assert (isobject_BM(ob));
  auto itob = objhashtable_BM.find(ob);
  assert (itob != objhashtable_BM.end());
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
      assert (isobject_BM((const value_tyBM)ob));
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
      assert (isobject_BM((const value_tyBM)ob));
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
      assert (isobject_BM((const value_tyBM)ob));
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
  assert (mapglobals_BM.find(#Gnam)		\
	  == mapglobals_BM.end());		\
  mapglobals_BM[#Gnam] = &GLOBAL_BM(Gnam);	\
} while(0);
#include "_bm_global.h"
} // end of initialize_globals_BM

void
gcmarkglobals_BM(struct garbcoll_stBM*gc)
{
  for (auto it: mapglobals_BM)
    if (it.second && *it.second)
      gcmark_BM(gc, *it.second, 0);
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
      assert (ld->ld_magic == LOADERMAGIC_BM);
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
  return true;
} // end of openmoduleforloader_BM

////////////////////////////////////////////////////////////////

typedef std::map<const stringval_tyBM*,const value_tyBM,ValStringLess_BM> dictmap_claBM;

struct dict_stBM*
dictmake_BM(void)
{
  struct dict_stBM*dict = //
    (struct dict_stBM*)allocgcty_BM(tydata_dict_BM, sizeof(struct dict_stBM));
  static_assert (sizeof (dict->dict_data) >= sizeof(dictmap_claBM), "too small dictdata");
  static_assert (alignof (dict->dict_data) >= alignof(dictmap_claBM), "too small dictdata");
  new(dict->dict_data) dictmap_claBM();
  return dict;
} // end dictmake_BM


void
dictgcmark_BM(struct garbcoll_stBM *gc, struct dict_stBM*dict,
              int depth)
{
  assert (gc && gc->gc_magic == GCMAGIC_BM);
  assert (isdict_BM (dict));
  uint8_t oldmark = ((typedhead_tyBM *) dict)->hgc;
  if (oldmark)
    return;
  ((typedhead_tyBM *)dict)->hgc = MARKGC_BM;
  auto& dicm = *(dictmap_claBM*)dict->dict_data;
  for (auto it : dicm)
    {
      gcmark_BM(gc, (const value_tyBM)it.first, depth+1);
      gcmark_BM(gc, (const value_tyBM)it.second, depth+1);
    }
} // end dictgcmark_BM


void dictgcdestroy_BM (struct garbcoll_stBM *gc, struct dict_stBM*dict)
{
  assert (gc && gc->gc_magic == GCMAGIC_BM);
  assert (isdict_BM (dict));
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
  assert (gc && gc->gc_magic == GCMAGIC_BM);
  assert (isdict_BM (dict));
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
  auto it = dicm.find(str);
  if (it == dicm.end()) return nullptr;
  return it->second;
} // end dictget_BM

void dictput_BM(struct dict_stBM* dict, const stringval_tyBM*str, const value_tyBM val)
{
  if (!isdict_BM((const value_tyBM)dict))
    return;
  if (!isstring_BM((const value_tyBM)str))
    return;
  auto& dicm = *(dictmap_claBM*)dict->dict_data;
  if (dicm.size() > MAXSIZE_BM)
    FATAL_BM("too big dict %lu", (long) dicm.size());
  if (val) dicm.insert({str,val});
  else dicm.erase(str);
} // end dictput_BM


void dictremove_BM(struct dict_stBM* dict, const stringval_tyBM*str)
{
  if (!isdict_BM((const value_tyBM)dict))
    return;
  if (!isstring_BM((const value_tyBM)str))
    return;
  auto& dicm = *(dictmap_claBM*)dict->dict_data;
  dicm.erase(str);
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
  auto itn = dicm.upper_bound(str);
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
  auto itn = dicm.lower_bound(str);
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
  auto itn = dicm.lower_bound(str);
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
  assert (par != nullptr);
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
