// file load_BM.c
#include "bismon.h"

struct loader_stBM *firstloader_BM;

void
loadergcmark_BM (struct garbcoll_stBM *gc, struct loader_stBM *ld)
{
  assert (gc && gc->gc_magic == GCMAGIC_BM);
  assert (valtype_BM ((const value_tyBM) ld) == tydata_loader_BM);
  assert (ld->ld_magic == LOADERMAGIC_BM);
  gcmark_BM (gc, ld->ld_hset, 0);
  gcmark_BM (gc, ld->ld_todolist, 0);
}                               /* end loadergcmark_BM */

static void doload_BM (struct stackframe_stBM *fr, struct loader_stBM *ld);

void
load_initial_BM (const char *ldirpath)
{
  if (!ldirpath)
    ldirpath = ".";
  DIR *ldir = opendir (ldirpath);
  if (!ldir)
    FATAL_BM ("load_initial failed to opendir %s : %m", ldirpath);
  struct dirent *de = NULL;
  char *todopath = NULL;
  int maxnum = 0;
  int nbfiles = 0;
  char *patharr[MAXLOADEDFILES_BM] = { };
  while ((de = readdir (ldir)) != NULL)
    {
      if (!isalpha (de->d_name[0]))
        continue;
      int num = 0;
      int pos = 0;
      if ((sscanf (de->d_name, "store%d.bismon%n", &num, &pos) > 0
           && pos > 0) && num > 0 && de->d_name[pos] == 0)
        {
          if (num >= MAXLOADEDFILES_BM)
            FATAL_BM ("too high file number for %s in %s", de->d_name,
                      ldirpath);
          char *buf = NULL;
          if (asprintf (&buf, "%s/%s", ldirpath, de->d_name) > 0
              && buf != NULL)
            {
              struct stat fst = { };
              if (stat (buf, &fst))
                FATAL_BM ("failed to stat %s : %m", buf);
              if ((fst.st_mode & S_IFMT) != S_IFREG)
                FATAL_BM ("%s is not a plain file", buf);
              // detect store12.bismon and store+12.bismon or store0012.bismon
              if (patharr[num])
                FATAL_BM ("duplicate file %s for #%d in %s", buf, num,
                          ldirpath);
              if (num > maxnum)
                maxnum = num;
              nbfiles++;
            }
          else
            FATAL_BM ("asprintf failure (%m)");
          patharr[num] = buf;
        }
      else if (!strcmp (de->d_name, "store_todo.bismon"))
        {
          char *buf = NULL;
          if (asprintf (&buf, "%s/%s", ldirpath, de->d_name) > 0
              && buf != NULL)
            {
              struct stat fst = { };
              if (stat (buf, &fst))
                FATAL_BM ("failed to stat %s : %m", buf);
              if ((fst.st_mode & S_IFMT) != S_IFREG)
                FATAL_BM ("%s is not a plain file", buf);
            }
          else
            FATAL_BM ("asprintf failure (%m)");
          assert (patharr[0] == NULL);
          patharr[0] = buf;
        }
    }
  closedir (ldir);
  if (maxnum > MAXLOADEDFILES_BM)
    FATAL_BM ("too many store files %d to load", maxnum);
  printf ("got %d store files in loaded directory %s\n", nbfiles, ldirpath);
  struct loader_stBM *ld =      //
    allocgcty_BM (tydata_loader_BM, sizeof (struct loader_stBM));
  // ((typedhead_tyBM *) ld)->htyp = tydata_loader_BM;
  // ((typedhead_tyBM *) ld)->hgc = 0;
  ((typedhead_tyBM *) ld)->rlen = 0;
  ld->ld_magic = LOADERMAGIC_BM;
  ld->ld_maxnum = maxnum;
  ld->ld_hset =
    hashsetobj_grow_BM (NULL, 2 * BM_NB_PREDEFINED + maxnum * 100);
  ld->ld_todolist = makelist_BM ();
  ld->ld_storepatharr = calloc (maxnum + 2, sizeof (void *));
  if (!ld->ld_storepatharr)
    FATAL_BM ("cannot calloc for %d store files (%m)", maxnum);
  ld->ld_startoffarr = calloc (maxnum + 2, sizeof (long));
  if (!ld->ld_startoffarr)
    FATAL_BM ("cannot call for %d start offsets (%m)", maxnum);
  for (int ix = 1; ix <= maxnum; ix++)
    {
      char *pa = patharr[ix];
      if (pa)
        ld->ld_storepatharr[ix] = pa;
    }
  ld->ld_storepatharr[0] = todopath;
  ld->ld_dir = strdup (ldirpath);
  if (!ld->ld_dir)
    FATAL_BM ("cannot strdup dir %s (%m)", ldirpath);
  firstloader_BM = ld;
  {
    LOCALFRAME_BM ( /*prev: */ NULL, /*descr: */ NULL,
                   struct loader_stBM *curld);
    _.curld = ld;
    doload_BM ((struct stackframe_stBM *) &_, ld);
  }
  firstloader_BM = NULL;
  free (ld->ld_dir), ld->ld_dir = NULL;
  for (int ix = 0; ix <= maxnum; ix++)
    free (ld->ld_storepatharr[ix]), ld->ld_storepatharr[ix] = NULL;
  ld->ld_hset = NULL;
  memset (ld, 0, sizeof (*ld)), ld = NULL;
}                               /* end load_initial_BM */

void
load_addtodo_BM (const closure_tyBM * clos)
{
  if (!firstloader_BM)
    FATAL_BM ("load_addtodo_BM called after loading");
  if (!isclosure_BM ((const value_tyBM) clos))
    FATAL_BM ("load_addtodo_BM called with non-closure");
  if (!islist_BM (firstloader_BM->ld_todolist))
    FATAL_BM ("missing todolist in loader for load_addtodo_BM");
  listappend_BM (firstloader_BM->ld_todolist, (value_tyBM) clos);
}                               /* end load_addtodo_BM */


static void
load_first_pass_BM (struct loader_stBM *ld, int ix)
{
  assert (ld && ld->ld_magic == LOADERMAGIC_BM);
  assert (ix >= 0 && ix <= (int) ld->ld_maxnum);
  char *curldpath = ld->ld_storepatharr[ix];
  assert (curldpath != NULL);
  size_t linsiz = 256;
  char *linbuf = malloc (linsiz);
  if (!linbuf)
    FATAL_BM ("malloc failed linsiz=%zd (%m)", linsiz);
  memset (linbuf, 0, linsiz);
  FILE *fil = fopen (curldpath, "r");
  if (!fil)
    FATAL_BM ("fopen %s failed (%m)", curldpath);
  int lincnt = 0;
  int nbobjdef = 0;
  long afterstartlineoff = 0;
  int startlineno = 0;
  do
    {
      ssize_t linlen = getline (&linbuf, &linsiz, fil);
      if (linlen < 0)
        {
          if (!feof (fil))
            FATAL_BM ("getline %s:%d failed (%m)", curldpath, lincnt);
          break;
        }
      linbuf[linlen] = (char) 0;
      lincnt++;
      if (!strncmp (linbuf, "//!!STARTBISMON", strlen ("//!!STARTBISMON")))
        {
          if (afterstartlineoff > 0)
            FATAL_BM
              ("multiple //!!STARTBISMON lines near %s:%d, previous at line#%d",
               curldpath, lincnt, startlineno);
          afterstartlineoff = ftell (fil);
          assert (afterstartlineoff > 0);
          startlineno = lincnt;
        }
      /* object definition lines are !*<id> e.g. !*_7D8xcWnEiys_8oqOVSkCxkA */
      else if (linbuf[0] == '!' && startlineno > 0 && linbuf[1] == '*'
               && linbuf[2] == '_' && isdigit (linbuf[3]))
        {
          const char *endid = NULL;
          rawid_tyBM id = parse_rawid_BM (linbuf + 2, &endid);
          if (hashid_BM (id) && endid >= linbuf + 2 * SERIALDIGITS_BM
              && (*endid == (char) 0 || isspace (*endid)))
            {
              objectval_tyBM *newobj = makeobjofid_BM (id);
              if (hashsetobj_contains_BM (ld->ld_hset, newobj))
                {
                  char idbuf32[32] = "";
                  idtocbuf32_BM (id, idbuf32);
                  FATAL_BM ("duplicate id %s near %s:%d", idbuf32, curldpath,
                            lincnt);
                };
              ld->ld_hset = hashsetobj_add_BM (ld->ld_hset, newobj);
              nbobjdef++;
            }
        }
    }
  while (!feof (fil));
  if (!startlineno)
    FATAL_BM ("load file %s without any !!STARTBISMON line", curldpath);
  ld->ld_startoffarr[ix] = afterstartlineoff;
  if (nbobjdef == 0 && ix > 0)
    FATAL_BM ("no object definition in %s\n", curldpath);
  free (linbuf), linbuf = 0;
  fclose (fil);
}                               /* end load_first_pass_BM */


static void
load_second_pass_BM (struct loader_stBM *ld, int ix)
{
  assert (ld && ld->ld_magic == LOADERMAGIC_BM);
  assert (ix >= 0 && ix <= (int) ld->ld_maxnum);
  char *curldpath = ld->ld_storepatharr[ix];
  assert (curldpath != NULL);
  fprintf (stderr, "load_second_pass_BM ix=%d path=%s unimplemented\n",
           ix, curldpath);
  FILE *fil = fopen (curldpath, "r");
  if (!fil)
    FATAL_BM ("failed to fopen %s (%m)", curldpath);
  if (fseek (fil, ld->ld_startoffarr[ix], SEEK_SET))
    FATAL_BM ("failed to fseek %s to offset %ld (%m)", curldpath,
              ld->ld_startoffarr[ix]);
  struct parser_stBM *ldpars = makeparser_of_file_BM (fil);
  assert (ldpars != NULL);
  ldpars->pars_path = ld->ld_storepatharr[ix];
#warning should parse for loading
}                               /* end load_second_pass_BM */

void
doload_BM (struct stackframe_stBM *_parentframe, struct loader_stBM *ld)
{
  assert (ld && ld->ld_magic == LOADERMAGIC_BM);
  assert (_parentframe != NULL);
  LOCALFRAME_BM (_parentframe, NULL, value_tyBM firsttodo;
    );
  /// run the first pass to create every object
  for (int ix = 1; ix <= (int) ld->ld_maxnum; ix++)
    if (ld->ld_storepatharr[ix])
      load_first_pass_BM (ld, ix);
  if (ld->ld_storepatharr[0])
    load_first_pass_BM (ld, 0);
  /// run the second pass to fill objects
  for (int ix = 1; ix <= (int) ld->ld_maxnum; ix++)
    if (ld->ld_storepatharr[ix])
      load_second_pass_BM (ld, ix);
  if (ld->ld_storepatharr[0])
    load_second_pass_BM (ld, 0);
  /// run the todo list
  long todocnt = 0;
  while (islist_BM (ld->ld_todolist) && listlength_BM (ld->ld_todolist) > 0)
    {
      _.firsttodo = listfirst_BM (ld->ld_todolist);
      assert (isclosure_BM (_.firsttodo));
      listpopfirst_BM (ld->ld_todolist);
      apply0_BM ((const closure_tyBM *) (_.firsttodo),
                 (struct stackframe_stBM *) &_);
      todocnt++;
      if (todocnt % 128 == 0)
        fullgarbagecollection_BM ((struct stackframe_stBM *) &_);
    }
}                               /* end doload_BM */
