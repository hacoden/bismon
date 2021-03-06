// file main_BM.c

/***
    BISMON 
    Copyright © 2018 Basile Starynkevitch (working at CEA, LIST, France)
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

----
    Contact me (Basile Starynkevitch) by email
    basile@starynkevitch.net and/or basile.starynkevitch@cea.fr
***/
#include "bismon.h"


#ifdef BISMONGTK
#include <glib/giochannel.h>
#endif /* BISMONGTK */

struct timespec startrealtimespec_BM;
void *dlprog_BM;
const char *myprogname_BM;
bool gui_is_running_BM;
bool web_is_running_BM;
bool debugmsg_BM;
bool parsedebugmsg_BM;
int nbworkjobs_BM;
const char myhostname_BM[80];
const char *contributors_filepath_BM;
const char *passwords_filepath_BM;
const char *added_passwords_filepath_BM;
static const char *chdir_after_load_bm;
thread_local struct threadinfo_stBM *curthreadinfo_BM;
thread_local volatile struct failurehandler_stBM *curfailurehandle_BM;

volatile struct backstrace_state *backtracestate_BM;
static void backtracerrorcb_BM (void *data, const char *msg, int errnum);


static void add_passwords_from_file_BM (const char *addedpasspath);

bool run_gtk_BM = false;
bool run_onion_BM = false;

#ifdef BISMONGTK
GIOChannel *defer_gtk_readpipechan_BM;
int defer_gtk_readpipefd_BM = -1;
int defer_gtk_writepipefd_BM = -1;
static void rungui_BM (int nbjobs);
#endif /*BISMONGTK*/
////////////////
#ifdef BISMONION
const char *onion_ssl_certificate_BM;
const char *onion_web_base_BM;
const char *onion_anon_web_session_BM;
extern void run_onionweb_BM (int nbjobs);
#endif /*BISMONION*/
extern void weakfailure_BM (void);

// consider putting a gdb breakpoint here 
void
weakfailure_BM (void)
{
  fflush (NULL);
}                               /* end weakfailure_BM */

void
weakassertfailureat_BM (const char *condmsg, const char *fil, int lin)
{
  char thnambuf[16];
  memset (thnambuf, 0, sizeof (thnambuf));
  pthread_getname_np (pthread_self (), thnambuf, sizeof (thnambuf));
  fprintf (stderr, "**** weakassertfailureat_BM (%s:%d) %s (tid#%d/%s)\n",
           fil, lin, condmsg, (int) gettid_BM (), thnambuf);
  fflush (stderr);
  void *backbuf[50];
  memset (backbuf, 0, sizeof (backbuf));
  int nb = backtrace (backbuf, sizeof (backbuf) / sizeof (void *));
  backtrace_symbols_fd (backbuf, nb, STDERR_FILENO);
  if (backtracestate_BM)
    {
      fprintf (stderr, "\n\n\n** full backtrace **\n");
      fflush (stderr);
      backtrace_print_BM ((struct backtrace_state *) backtracestate_BM, 1,
                          stderr);
      fprintf (stderr, "\n----- end full backtrace ------\n\n");
      fflush (stderr);
    }
  weakfailure_BM ();
}                               /* end weakassertfailureat_BM */

value_tyBM
objrout_placeholder_BM (struct stackframe_stBM *stkf __attribute__ ((unused)),  //
                        const value_tyBM arg1 __attribute__ ((unused)), //
                        const value_tyBM arg2 __attribute__ ((unused)), //
                        const value_tyBM arg3 __attribute__ ((unused)), //
                        const value_tyBM arg4 __attribute__ ((unused)), //
                        const quasinode_tyBM * restargs
                        __attribute__ ((unused)))
{
  weakassertfailureat_BM ("objrout_placeholder_BM", __FILE__, __LINE__);
  return NULL;
}                               /* end objrout_placeholder_BM */


void
abort_BM (void)
{
  weakfailure_BM ();
  abort ();
}                               /* end abort_BM */


////////////////////////////////////////////////////////////////
char *load_dir_bm;
char *dump_dir_BM;
char *dump_after_load_dir_bm;
char *builder_file_bm = "bismon.ui";
char *css_file_bm = "bismon.css";
char *gui_log_name_bm = "_bismon.log";  /* default log file */
char *pid_filepath_bm = "_bismon.pid";  /* default pid file */
char *comment_bm;
char *module_to_emit_bm;
int count_emit_has_predef_bm;
int nb_added_predef_bm;
#define MAXADDEDPREDEF_BM 16
struct
{
  const char *pr_comment;
  const char *pr_name;
} added_predef_bm[MAXADDEDPREDEF_BM];


char **added_contributors_arr_bm;
int count_added_contributors_bm;
int size_added_contributors_bm;

char **removed_contributors_arr_bm;
int count_removed_contributors_bm;
int size_removed_contributors_bm;

#define MAXPARSED_VALUES_AFTER_LOAD_BM 10
char *parsed_values_after_loadarr_bm[MAXPARSED_VALUES_AFTER_LOAD_BM];
int nb_parsed_values_after_load_bm;


bool batch_bm;
bool give_version_bm;


void
failure_at_BM (int failcode, const char *fil, int lineno,
               const value_tyBM reasonv, const value_tyBM placev,
               struct stackframe_stBM *stkf)
{
  if (curfailurehandle_BM)
    {
      if (curfailurehandle_BM->failh_magic != FAILUREHANDLEMAGIC_BM)
        FATAL_AT_BM (fil, lineno,
                     "corrupted curfailurehandle_BM@%p for failcode %d",
                     curfailurehandle_BM, failcode);
      curfailurehandle_BM->failh_reason = reasonv;
      curfailurehandle_BM->failh_place = placev;
      if (!curfailurehandle_BM->failh_silent)
        {
          char thnambuf[16];
          memset (thnambuf, 0, sizeof (thnambuf));
          pthread_getname_np (pthread_self (), thnambuf, sizeof (thnambuf));
          fprintf (stderr, "\n\n*** failure code#%d %s:%d (tid#%d/%s) ***\n",
                   failcode, fil ? fil : "???", lineno, (int) gettid_BM (),
                   thnambuf);
          if (backtracestate_BM)
            {
              backtrace_print_BM ((struct backtrace_state *)
                                  backtracestate_BM, 1, stderr);
              fprintf (stderr, "\n----- end failure backtrace ------\n");
            }
          fflush (stderr);
          // we need that debug_outstr_value_BM should not fail...
          fprintf (stderr,
                   "*#* failure code#%d from %s:%d\n"
                   "fail reason : %s\n"
                   "fail place : %s\n",
                   failcode, fil ? fil : "???", lineno,
                   debug_outstr_value_BM (reasonv, stkf, 0),
                   debug_outstr_value_BM (placev, stkf, 0));
          fprintf (stderr, "#*#*#*#*#*#*#*#*#*#*\n\n");
          fflush (stderr);
        }
      longjmp (((struct failurehandler_stBM *)
                curfailurehandle_BM)->failh_jmpbuf, failcode);
    }
  else
    {
      FATAL_AT_BM (fil, lineno,
                   "unhandled failure code#%d reason %s",
                   failcode, debug_outstr_value_BM (reasonv, stkf, 0));
    }
}                               /* end failure_at_BM */

void
failure_BM (int failcode, const value_tyBM reasonv,
            struct stackframe_stBM *stkf)
{
  failure_at_BM (failcode, "??", 0, reasonv, taggedint_BM (failcode), stkf);
}                               /* end failure_BM */

void
fatal_stop_at_BM (const char *fil, int lineno)
{
  char thnambuf[16];
  memset (thnambuf, 0, sizeof (thnambuf));
  pthread_getname_np (pthread_self (), thnambuf, sizeof (thnambuf));
  fprintf (stderr, "** FATAL STOP %s:%d (tid#%d/%s)\n",
           fil ? fil : "???", lineno, (int) gettid_BM (), thnambuf);
  fflush (stderr);
  void *backarr[2 * TINYSIZE_BM];
  memset (backarr, 0, sizeof (backarr));
  int backdepth = backtrace (backarr, sizeof (backarr) / sizeof (void *));
  backtrace_symbols_fd (backarr, backdepth, STDERR_FILENO);
  if (backtracestate_BM)
    {
      fprintf (stderr, "\n\n\n** full fatal backtrace **\n");
      fflush (stderr);
      backtrace_print_BM ((struct backtrace_state *) backtracestate_BM, 1,
                          stderr);
      fprintf (stderr, "\n----- end full fatal backtrace ------\n\n");
    }
  fflush (stderr);
  abort_BM ();
}                               /* end fatal_stop_at_BM */


static void add_new_predefined_bm (void);

static bool
run_command_bm (const gchar * optname
                __attribute__ ((unused)), const gchar * val, gpointer data
                __attribute__ ((unused)), GError ** perr)
{
  ASSERT_BM (val != NULL);
  INFOPRINTF_BM ("running command: %s\n", val);
  int ok = system (val);
  if (ok == 0)
    return TRUE;
  g_set_error (perr, 0, ok, "command %s failed with status %d", val, ok);
  return FALSE;
}                               /* end run_command_bm */

static void
get_parse_value_after_load_bm (const gchar * optname __attribute__ ((unused)),
                               const gchar * val,
                               gpointer data __attribute__ ((unused)),
                               GError ** perr __attribute__ ((unused)))
{
  if (nb_parsed_values_after_load_bm >= MAXPARSED_VALUES_AFTER_LOAD_BM)
    FATAL_BM ("too many %d parsed values after load with --parse-value",
              nb_parsed_values_after_load_bm);
  NONPRINTF_BM ("get_parse_value_after_load #%d.. valen=%d:\n%s",
                nb_parsed_values_after_load_bm, (int) strlen (val), val);
  parsed_values_after_loadarr_bm[nb_parsed_values_after_load_bm++] =
    strdup (val);
}                               /* end get_parse_value_after_load_bm */


static bool
add_predef_bm (const gchar * optname __attribute__ ((unused)),
               const gchar * val,
               gpointer data __attribute__ ((unused)),
               GError ** perr __attribute__ ((unused)))
{
  ASSERT_BM (val != NULL);
  if (!validname_BM (val))
    FATAL_BM ("invalid predef name %s", val);
  if (nb_added_predef_bm >= MAXADDEDPREDEF_BM)
    FATAL_BM ("too many added predefined %i", nb_added_predef_bm);
  // in principle the strdup-s below should be checked, but in
  // practice this is so rarely used that we don't bother
  if (comment_bm)
    added_predef_bm[nb_added_predef_bm].pr_comment = strdup (comment_bm);
  added_predef_bm[nb_added_predef_bm].pr_name = strdup (val);
  nb_added_predef_bm++;
  comment_bm = NULL;
  return true;
}                               /* end add_predef_bm */

static bool
add_contributor_bm (const gchar * optname __attribute__ ((unused)),
                    const gchar * contrib,
                    gpointer data __attribute__ ((unused)),
                    GError ** perr __attribute__ ((unused)))
{
  if (count_added_contributors_bm >= size_added_contributors_bm)
    {
      int newsiz = prime_above_BM (3 * count_added_contributors_bm / 2 + 16);
      char **newarr = calloc (newsiz, sizeof (char *));
      if (!newarr || newsiz > MAXSIZE_BM / 2)   /*very unlikely to happen in practice */
        FATAL_BM ("cannot grow added contributors array to %d for %s - %m",
                  newsiz, contrib);
      if (count_added_contributors_bm > 0)
        memcpy (newarr, added_contributors_arr_bm,
                count_added_contributors_bm * sizeof (char *));
      free (added_contributors_arr_bm);
      added_contributors_arr_bm = newarr;
      size_added_contributors_bm = newsiz;
    };
  char *newcontrib = strdup (contrib);
  if (!newcontrib)
    FATAL_BM ("failed to strdup added contributor %s - %m", contrib);
  added_contributors_arr_bm[count_added_contributors_bm++] = newcontrib;
  return true;
}                               /* end add_contributor_bm */



static bool
remove_contributor_bm (const gchar * optname __attribute__ ((unused)),
                       const gchar * contrib,
                       gpointer data __attribute__ ((unused)),
                       GError ** perr __attribute__ ((unused)))
{
  if (count_removed_contributors_bm >= size_removed_contributors_bm)
    {
      int newsiz =
        prime_above_BM (3 * count_removed_contributors_bm / 2 + 16);
      char **newarr = calloc (newsiz, sizeof (char *));
      if (!newarr || newsiz > MAXSIZE_BM / 2)   /*very unlikely to happen in practice */
        FATAL_BM ("cannot grow removed contributors array to %d for %s - %m",
                  newsiz, contrib);
      if (count_removed_contributors_bm > 0)
        memcpy (newarr, removed_contributors_arr_bm,
                count_removed_contributors_bm * sizeof (char *));
      free (removed_contributors_arr_bm);
      removed_contributors_arr_bm = newarr;
      size_removed_contributors_bm = newsiz;
    };
  char *newcontrib = strdup (contrib);
  if (!newcontrib)
    FATAL_BM ("failed to strdup removed contributor %s - %m", contrib);
  removed_contributors_arr_bm[count_removed_contributors_bm++] = newcontrib;
  return true;
}                               /* end remove_contributor_bm */


#if defined(BISMONION) && defined (BISMONGTK)
#pragma message "both BISMONION and BISMONGTK are defined"
#endif

////////////////////////////////////////////////////////////////
const GOptionEntry optionstab_bm[] = {
  //
  {.long_name = "load",.short_name = 'l',
   .flags = G_OPTION_FLAG_NONE,
   .arg = G_OPTION_ARG_FILENAME,
   .arg_data = &load_dir_bm,
   .description = "load directory DIR (default is .)",
   .arg_description = "DIR"},
  //
  {.long_name = "dump",.short_name = 'd',
   .flags = G_OPTION_FLAG_NONE,
   .arg = G_OPTION_ARG_FILENAME,
   .arg_data = &dump_dir_BM,
   .description = "dump directory DIR",
   .arg_description = "DIR"},
  //
  {.long_name = "dump-after-load",.short_name = (char) 0,
   .flags = G_OPTION_FLAG_NONE,
   .arg = G_OPTION_ARG_FILENAME,
   .arg_data = &dump_after_load_dir_bm,
   .description = "dump after load directory DIR",
   .arg_description = "DIR"},
  //
  {.long_name = "contributors-file",.short_name = (char) 0,
   .flags = G_OPTION_FLAG_NONE,
   .arg = G_OPTION_ARG_FILENAME,
   .arg_data = &contributors_filepath_BM,
   .description = "use PATH as the contributors file;\n"
   "\t .. default is contributors_BM or $HOME/contributors_BM",
   .arg_description = "PATH"},
  //
  {.long_name = "passwords-file",.short_name = (char) 0,
   .flags = G_OPTION_FLAG_NONE,
   .arg = G_OPTION_ARG_FILENAME,
   .arg_data = &passwords_filepath_BM,
   .description = "use PATH as the password file;\n"
   "\t .. default is passwords_BM or $HOME/passwords_BM",
   .arg_description = "PATH"},
  //
  {.long_name = "add-passwords",.short_name = (char) 0,
   .flags = G_OPTION_FLAG_NONE,
   .arg = G_OPTION_ARG_FILENAME,
   .arg_data = &added_passwords_filepath_BM,
   .description =
   "use the given PASSWORDENTRIES file (if it is -, stdin) containing lines like <username>:<password> to add passwords",
   .arg_description = "PASSWORDENTRIES"},
  //
  {.long_name = "emit-has-predef",.short_name = (char) 0,
   .flags = G_OPTION_FLAG_NONE,
   .arg = G_OPTION_ARG_INT,
   .arg_data = &count_emit_has_predef_bm,
   .description = "emit NB 'HAS_PREDEF_BM'",
   .arg_description = "NB"},
  //
  {.long_name = "job",.short_name = (char) 'j',
   .flags = G_OPTION_FLAG_NONE,
   .arg = G_OPTION_ARG_INT,
   .arg_data = &nbworkjobs_BM,
   .description = "number of worker threads NBJOBS (>=2, <16)",
   .arg_description = "NBJOBS"},
  //
  {.long_name = "pid-file",.short_name = (char) 0,
   .flags = G_OPTION_FLAG_NONE,
   .arg = G_OPTION_ARG_FILENAME,
   .arg_data = &pid_filepath_bm,
   .description = "use PATH as the pid file;\n"
   "\t .. default is _bismon.pid",
   .arg_description = "PATH"},
  //
  {.long_name = "run-command",.short_name = (char) 0,
   .flags = G_OPTION_FLAG_NONE,
   .arg = G_OPTION_ARG_CALLBACK,
   .arg_data = &run_command_bm,
   .description = "run the command CMD",
   .arg_description = "CMD"},
  //
  {.long_name = "chdir-after-load",.short_name = (char) 'c',
   .flags = G_OPTION_FLAG_NONE,
   .arg = G_OPTION_ARG_FILENAME,
   .arg_data = &chdir_after_load_bm,
   .description = "change directory after load to DIR (perhaps making it)",
   .arg_description = "DIR"},

  //
  {.long_name = "parse-value",.short_name = (char) 0,
   .flags = G_OPTION_FLAG_NONE,
   .arg = G_OPTION_ARG_CALLBACK,
   .arg_data = &get_parse_value_after_load_bm,
   .description = "parse (after loading) the value EXPR",
   .arg_description = "EXPR"},

  //
  {.long_name = "comment-predef",.short_name = (char) 0,
   .flags = G_OPTION_FLAG_NONE,
   .arg = G_OPTION_ARG_STRING,
   .arg_data = &comment_bm,
   .description = "set comment of next predefined to COMM",
   .arg_description = "COMM"},

  //
  {.long_name = "add-predef",.short_name = (char) 0,
   .flags = G_OPTION_FLAG_NONE,
   .arg = G_OPTION_ARG_CALLBACK,
   .arg_data = &add_predef_bm,
   .description = "add new predefined named PREDEFNAME",
   .arg_description = "PREDEFNAME"},

  //
  {.long_name = "contributor",.short_name = (char) 0,
   .flags = G_OPTION_FLAG_NONE,
   .arg = G_OPTION_ARG_CALLBACK,
   .arg_data = &add_contributor_bm,
   .description = "add or change contributor CONTRIBUTOR,\n"
   "\t like 'First Lastname <email@example.com>'\n"
   "\t or 'First Lastname;email@example.com;aliasmail@example.org'\n"
   "\t (this puts personal information relevant to European GDPR in file "
   CONTRIBUTORS_FILE_BM ")",
   .arg_description = "CONTRIBUTOR"},

  //
  {.long_name = "remove-contributor",.short_name = (char) 0,
   .flags = G_OPTION_FLAG_NONE,
   .arg = G_OPTION_ARG_CALLBACK,
   .arg_data = &remove_contributor_bm,
   .description = "remove existing contributor CONTRIBUTOR,\n"
   "\t like 'First Lastname'\n"
   "\t or email@example.com\n"
   "\t or some existing contributor oid similar to _2PFRochKb3N_3e8RFFAUi9K\n"
   "\t (this should remove personal information relevant to European GDPR in file "
   CONTRIBUTORS_FILE_BM ")",
   .arg_description = "CONTRIBUTOR"},
#if defined(BISMONION) && defined (BISMONGTK)
  //
  /* when both BISMONION and BISMONGTK */
  {.long_name = "gui",          //
   .short_name = (char) 0,
   .flags = G_OPTION_FLAG_NONE,
   .arg = G_OPTION_ARG_NONE,
   .arg_data = &run_gtk_BM,
   .description = "run GUI with GTK",
   .arg_description = NULL},
  //  #warning both gtk & onion options
  //
  {.long_name = "web",          //
   .short_name = (char) 0,
   .flags = G_OPTION_FLAG_NONE,
   .arg = G_OPTION_ARG_NONE,
   .arg_data = &run_onion_BM,
   .description = "run web interface with ONION",
   .arg_description = NULL},
  /* end when both BISMONION and BISMONGTK */
#endif /*both BISMONION and BISMONGTK */
  //
  {.long_name = "batch",.short_name = (char) 0,
   .flags = G_OPTION_FLAG_NONE,
   .arg = G_OPTION_ARG_NONE,
   .arg_data = &batch_bm,
   .description = "run in batch mode without GUI",
   .arg_description = NULL},
  //
  {.long_name = "debug",.short_name = (char) 'D',
   .flags = G_OPTION_FLAG_NONE,
   .arg = G_OPTION_ARG_NONE,
   .arg_data = &debugmsg_BM,
   .description = "gives lots of debug messages",
   .arg_description = NULL},
  //
  {.long_name = "emit-module",.short_name = (char) 0,
   .flags = G_OPTION_FLAG_NONE,
   .arg = G_OPTION_ARG_STRING,
   .arg_data = &module_to_emit_bm,
   .description = "emit module MODULEOBJ",
   .arg_description = "MODULEOBJ"},
  //
  //
  {.long_name = "version",.short_name = (char) 0,
   .flags = G_OPTION_FLAG_NONE,
   .arg = G_OPTION_ARG_NONE,
   .arg_data = &give_version_bm,
   .description = "gives version information",
   .arg_description = NULL},
  //
#ifdef BISMONGTK
  //////////////////
  {.long_name = "gui-builder",.short_name = (char) 0,
   .flags = G_OPTION_FLAG_NONE,
   .arg = G_OPTION_ARG_FILENAME,
   .arg_data = &builder_file_bm,
   .description = "with GTK builder file FILE (default: bismon.ui)",
   .arg_description = "FILE"},
  //
  {.long_name = "gui-style",.short_name = (char) 0,
   .flags = G_OPTION_FLAG_NONE,
   .arg = G_OPTION_ARG_FILENAME,
   .arg_data = &css_file_bm,
   .description = "with GTK style CSS file FILE (default: bismon.css)",
   .arg_description = "FILE"},
  //
  {.long_name = "gui-log",.short_name = (char) 0,
   .flags = G_OPTION_FLAG_NONE,
   .arg = G_OPTION_ARG_FILENAME,
   .arg_data = &gui_log_name_bm,
   .description =
   "GUI log file name (none if empty, - is stdout, default is _bismon.log)",
   .arg_description = "FILE"},
#endif /*BISMONGTK*/
    //////////////////
#ifdef BISMONION
    //
  {.long_name = "ssl-certificate",.short_name = (char) 0,
   .flags = G_OPTION_FLAG_NONE,
   .arg = G_OPTION_ARG_FILENAME,
   .arg_data = &onion_ssl_certificate_BM,
   .description =
   "Uses FILEPREFIX.pem & FILEPREFIX.key for SSL certificate to libonion",
   .arg_description = "FILEPREFIX"},
  //
  {.long_name = "web-base",.short_name = (char) 0,
   .flags = G_OPTION_FLAG_NONE,
   .arg = G_OPTION_ARG_STRING,
   .arg_data = &onion_web_base_BM,
   .description =
   "A string like <host>:<port>, default is localhost:8086, describing the base of web URLs served by bismon",
   .arg_description = "WEB_BASE"},
  {.long_name = "anon-web-session",.short_name = (char) 0,
   .flags = G_OPTION_FLAG_NONE,
   .arg = G_OPTION_ARG_FILENAME,
   .arg_data = &onion_anon_web_session_BM,
   .description =
   "Create an anonymous web session, and write its cookie in the given COOKIEFILE",
   .arg_description = "COOKIEFILE"},

#endif /*BISMONION*/
    /// end of options
  {}
};

static void
check_delims_BM (void)
{
  int delimcnt = 0;
  char *prevdelim = "";
#define HAS_DELIM_BM(Str,Name) do {			\
    delimcnt++;						\
    if (strcmp(Str,prevdelim)<=0)			\
      FATAL_BM("unsorted delimiter#%d '%s' and '%s'",	\
	       delimcnt, Str, prevdelim);		\
    prevdelim = Str;					\
  } while(0);
#include "_bm_delim.h"
  if (delimcnt != BM_NB_DELIM)
    FATAL_BM ("expected %d delimiters, got %d", BM_NB_DELIM, delimcnt);
}                               /* end check_delims_BM */

void
add_new_predefined_bm (void)
{
  for (int pix = 0; pix < nb_added_predef_bm; pix++)
    {
      const char *predname = added_predef_bm[pix].pr_name;
      const char *predcomm = added_predef_bm[pix].pr_comment;
      if (!validname_BM (predname))
        FATAL_BM ("predefined name '%s' invalid", predname);
      const objectval_tyBM *predobj = findnamedobj_BM (predname);
      if (!predobj)
        {
          predobj = makeobj_BM ();
          registername_BM (predobj, predname);
        }
      else
        {
          char idpred[32];
          memset (idpred, 0, sizeof (idpred));
          idtocbuf32_BM (objid_BM (predobj), idpred);
          INFOPRINTF_BM ("existing %s becomes predefined %s\n", idpred,
                         predname);
        };
      objtouchnow_BM ((objectval_tyBM *) predobj);
      if (predcomm)
        objputattr_BM ((objectval_tyBM *) predobj, BMP_comment,
                       (value_tyBM) makestring_BM (predcomm));
      objputspacenum_BM ((objectval_tyBM *) predobj, PredefSp_BM);
      char idpred[32];
      memset (idpred, 0, sizeof (idpred));
      idtocbuf32_BM (objid_BM (predobj), idpred);
      if (predcomm)
        INFOPRINTF_BM ("made predefined %s (%s) - %s\n", predname, idpred,
                       predcomm);
      else
        INFOPRINTF_BM ("made predefined %s (%s)\n", predname, idpred);
    }
}                               /* end add_new_predefined_bm */

static int
idqcmp_BM (const void *p1, const void *p2)
{
  return cmpid_BM (*(rawid_tyBM *) p1, *(rawid_tyBM *) p2);
}                               /* end idqcmp_BM */




static void give_prog_version_BM (const char *progname);

static void do_emit_module_from_main_BM (void);

void
do_emit_module_from_main_BM (void)
{
  LOCALFRAME_BM (NULL, /*descr: */ BMP_emit_module,
                 objectval_tyBM * modulob;      //
                 objectval_tyBM * parsob;       //
                 value_tyBM resultv;    //
                 value_tyBM failres;    //
                 value_tyBM failplace;  //
    );
  WEAKASSERTRET_BM (module_to_emit_bm != NULL);
  _.failres = NULL;
  _.failplace = NULL;
  int failcod = 0;
  struct failurelockset_stBM flockset = { };
  struct failurehandler_stBM *prevfailurehandle =
    (struct failurehandler_stBM *) curfailurehandle_BM;
  initialize_failurelockset_BM (&flockset, sizeof (flockset));
  LOCAL_FAILURE_HANDLE_BM (&flockset, lab_failureemit, failcod, _.failres,
                           _.failplace);
  if (failcod > 0)
  lab_failureemit:{
      destroy_failurelockset_BM (&flockset);
      curfailurehandle_BM = prevfailurehandle;
      {
        WARNPRINTF_BM
          ("Failed to emit module from main %s, with failcode#%d, failres %s\n"
           "failplace %s",
           objectdbg_BM (_.modulob), failcod,
           OUTSTRVALUE_BM (_.failres), OUTSTRVALUE_BM (_.failplace));
        return;
      };
    };
  INFOPRINTF_BM ("begin emit module from main: %s\n", module_to_emit_bm);
  _.parsob = makeobj_BM ();
  bool gotobj = false;
  struct parser_stBM *pars =
    makeparser_memopen_BM (module_to_emit_bm, strlen (module_to_emit_bm),
                           _.parsob);
  _.modulob = parsergetobject_BM (pars, CURFRAME_BM, 0, &gotobj);
  DBGPRINTF_BM ("do_emit_module_from_main_BM modulob=%s parsob=%s",
                objectdbg_BM (_.modulob), objectdbg1_BM (_.parsob));
  objlock_BM (_.modulob);
  _.resultv = send0_BM (_.modulob, BMP_emit_module, CURFRAME_BM);
  objunlock_BM (_.modulob);
  objclearpayload_BM (_.parsob);
  destroy_failurelockset_BM (&flockset);
  curfailurehandle_BM = prevfailurehandle;
  DBGPRINTF_BM ("do_emit_module_from_main_bm end modulob=%s result %s", //
                objectdbg_BM (_.modulob),       //
                debug_outstr_value_BM (_.resultv, CURFRAME_BM, 0));
  if (_.resultv)
    INFOPRINTF_BM ("successful emit module from main: %s\n",
                   module_to_emit_bm);
  else
    FATAL_BM ("failed emit module from main: %s", module_to_emit_bm);
  char modulidbuf[32];
  memset (modulidbuf, 0, sizeof (modulidbuf));
  idtocbuf32_BM (objid_BM (_.modulob), modulidbuf);
  char makemodulecmd[128];
  memset (makemodulecmd, 0, sizeof (makemodulecmd));
  snprintf (makemodulecmd, sizeof (makemodulecmd),
            "%s/build-bismon-module.sh %s", bismon_directory, modulidbuf);
  DBGPRINTF_BM ("do_emit_module_from_main_bm makemodulecmd=%s",
                makemodulecmd);
  fflush (NULL);
  int cmdcod = system (makemodulecmd);
  if (cmdcod)
    FATAL_BM ("failed module making %s (%d)", makemodulecmd, cmdcod);
  INFOPRINTF_BM ("successfully compiled emitted module %s (%s) from main\n",
                 module_to_emit_bm, modulidbuf);
  fflush (NULL);
  return;
}                               /* end do_emit_module_from_main_BM */


static void parse_values_after_load_BM (void);
static void add_contributors_after_load_BM (void);
static void remove_contributors_after_load_BM (void);
static void initialize_contributors_path_BM (void);
static void initialize_passwords_path_BM (void);
static void emit_has_predef_BM (void);
static void do_dump_after_load_BM (void);

//// see also https://github.com/dtrebbien/GNOME.supp and
//// https://stackoverflow.com/q/16659781/841108 to use valgrind with
//// GTK appplications
int
main (int argc, char **argv)
{
  clock_gettime (CLOCK_MONOTONIC, &startrealtimespec_BM);
  if (argc <= 0)
    {                           // this should never happen in practice, but see
      // https://stackoverflow.com/q/49817316/841108
      fprintf (stderr, "[bismon] requires at least one argument\n");
      exit (EXIT_FAILURE);
    }
  myprogname_BM = argv[0];
  if (argc > 1 && (!strcmp (argv[1], "-D") || !strcmp (argv[1], "--debug")))
    debugmsg_BM = true;
#if defined(BISMONION) && defined (BISMONGTK)
  /* when both BISMONION and BISMONGTK */
  if (strstr (basename (myprogname_BM), "gtk")
      || (argc > 1 && !strcmp (argv[1], "--gui"))
      || (argc > 2 && argv[1][0] == '-' && !strcmp (argv[1], "--gui")))
    run_gtk_BM = true;
  if (strstr (basename (myprogname_BM), "onion")
      || (argc > 1 && !strcmp (argv[1], "--web"))
      || (argc > 2 && argv[1][0] == '-' && !strcmp (argv[1], "--web")))
    run_onion_BM = true;
  for (int ix = 1; ix < argc; ix++)
    {
      if (!strcmp (argv[ix], "--gui"))
        run_gtk_BM = true;
      else if (!strcmp (argv[ix], "--web"))
        run_onion_BM = true;
    }
  if (run_gtk_BM && run_onion_BM)
    INFOPRINTF_BM ("running both GUI (GTK) & Web (Onion) interfaces");
#endif /*both BISMONION and BISMONGTK */
  DBGPRINTF_BM ("run_gtk is %s & run_onion is %s",
                run_gtk_BM ? "true" : "false",
                run_onion_BM ? "true" : "false");
  dlprog_BM = dlopen (NULL, RTLD_NOW | RTLD_GLOBAL);
  if (!dlprog_BM)
    {
      fprintf (stderr, "%s: dlopen for whole program fails %s\n",
               argv[0], dlerror ());
      exit (EXIT_FAILURE);
    }
  memset ((char *) myhostname_BM, 0, sizeof (myhostname_BM));
  if (gethostname ((char *) myhostname_BM, sizeof (myhostname_BM) - 1))
    FATAL_BM ("gethostname failure %m");
  {
    char *oldloc = setlocale (LC_ALL, "POSIX");
    DBGPRINTF_BM ("oldlocale %s", oldloc);
    if (oldloc && strcmp (oldloc, "en_US.UTF-8")
        && strcmp (oldloc, "en_GB.UTF-8")
        && strcmp (oldloc, "C") && strcmp (oldloc, "POSIX"))
      WARNPRINTF_BM
        ("your locale '%s' is strange but should be in English, preferably encoded in UTF-8.\n"
         "Please use C or en_US.UTF-8 or en_GB.UTF-8 or POSIX", oldloc);
  }
  {
    double nwt = clocktime_BM (CLOCK_REALTIME);
    intptr_t y2kwt = timetoY2Kmillisec_BM (nwt);
    NONPRINTF_BM ("nwt=%.4f y2kwt=%lld=%#llx as time=%.4f", nwt,
                  (long long) y2kwt, (long long) y2kwt,
                  Y2Kmillisectotime_BM (y2kwt));
    ASSERT_BM (abs (Y2Kmillisectotime_BM (y2kwt) - nwt) < 0.5);
  }
  backtracestate_BM             //
    = (volatile struct backstrace_state *)
    backtrace_create_state ( /*filename: */ NULL,
                            /*threaded: */ true,
                            /*errorcb: */
                            backtracerrorcb_BM,
                            /*data: */ NULL);
  initialize_garbage_collector_BM ();
  check_delims_BM ();
  initialize_globals_BM ();
  initialize_predefined_objects_BM ();
  initialize_predefined_names_BM ();
  initialize_agenda_BM ();
  GError *opterr = NULL;
  bool shouldfreedumpdir = false;
  bool guiok = false;
  DBGPRINTF_BM ("run_gtk is %s & run_onion is %s (argc=%d)",
                run_gtk_BM ? "true" : "false",
                run_onion_BM ? "true" : "false", argc);
#ifdef BISMONGTK
  if (run_gtk_BM)
    /// should actually use gtk_init_with_args so define some
    /// GOptionEntry array
    guiok = gtk_init_with_args (&argc, &argv,
                                " - The bismon[gtk] program (with GTK GUI)",
                                optionstab_bm, NULL, &opterr);
  if (onion_web_base_BM)
    {
      DBGPRINTF_BM ("force run_onion with onion_web_base_BM '%s'",
                    onion_web_base_BM);
      run_onion_BM = true;
    };
  if (guiok)
    {
      DBGPRINTF_BM ("force run_gtk with guiok");
      run_gtk_BM = true;
    }
#endif /*BISMONGTK*/
    DBGPRINTF_BM ("run_gtk is %s & run_onion is %s (argc=%d); guiok=%s",
                  run_gtk_BM ? "true" : "false",
                  run_onion_BM ? "true" : "false",
                  argc, guiok ? "true" : "false");
  ////
#ifdef BISMONION
  if (!guiok)
    {
      GOptionContext *weboptctx =
        g_option_context_new
        ("- The bismon[ion] program (with Web interface)");
      if (!weboptctx)
        FATAL_BM ("no option context");
      g_option_context_add_main_entries (weboptctx, optionstab_bm, NULL);
      if (!g_option_context_parse (weboptctx, &argc, &argv, &opterr))
        FATAL_BM ("bismonion failed to parse options - %s",
                  opterr ? opterr->message : "??");
      g_option_context_free (weboptctx);
    }
#endif /*BISMONION*/
    ///
    if (debugmsg_BM)
    fprintf (stderr,
             "debug messages enabled %s pid %d timestamp %s commit %s\n",
             myprogname_BM, (int) getpid (), bismon_timestamp,
             bismon_lastgitcommit);
  DBGPRINTF_BM ("run_gtk is %s & run_onion is %s",
                run_gtk_BM ? "true" : "false",
                run_onion_BM ? "true" : "false");
  if (give_version_bm)
    give_prog_version_BM (myprogname_BM);
  if (nbworkjobs_BM < MINNBWORKJOBS_BM)
    nbworkjobs_BM = MINNBWORKJOBS_BM + 1;
  else if (nbworkjobs_BM > MAXNBWORKJOBS_BM)
    nbworkjobs_BM = MAXNBWORKJOBS_BM;
  if (!batch_bm && !run_gtk_BM && !run_onion_BM)
    FATAL_BM ("no batch or gtk or onion option");
  if (run_gtk_BM && run_onion_BM)
    INFOPRINTF_BM ("both GUI and Web interfaces requested");
  else if (run_onion_BM)
    INFOPRINTF_BM ("Web interface requested alone");
  else if (run_gtk_BM)
    INFOPRINTF_BM ("GUI interface requested alone");
  else if (batch_bm)
    INFOPRINTF_BM ("batch mode requested without Web or GUI");
  //
  initialize_contributors_path_BM ();
  initialize_passwords_path_BM ();
  //
  if (count_emit_has_predef_bm > 0)
    emit_has_predef_BM ();
  DBGPRINTF_BM ("run_gtk is %s & run_onion is %s",
                run_gtk_BM ? "true" : "false",
                run_onion_BM ? "true" : "false");
#ifdef BISMONGTK
  if (!guiok && !batch_bm && run_gtk_BM)
    FATAL_BM ("gtk_init_with_args failed : %s",
              opterr ? opterr->message : "???");
  if (!batch_bm)
    {
      if (run_gtk_BM)
        {
          INFOPRINTF_BM ("initializing GTK with builder %s & css %s",
                         builder_file_bm, css_file_bm);
          initialize_newgui_BM (builder_file_bm, css_file_bm);
        }
    }
#endif /*BISMONGTK*/
    DBGPRINTF_BM ("run_gtk is %s & run_onion is %s",
                  run_gtk_BM ? "true" : "false",
                  run_onion_BM ? "true" : "false");
#ifdef BISMONION
  if (!batch_bm)
    {
      if (run_onion_BM)
        {
          if (!run_gtk_BM)
            INFOPRINTF_BM ("initializing ONION");
          else
            INFOPRINTF_BM ("initializing ONION, but GTK also requested");
          initialize_webonion_BM ();
        }
    }
#endif /*BISMONION*/
    DBGPRINTF_BM ("run_gtk is %s & run_onion is %s",
                  run_gtk_BM ? "true" : "false",
                  run_onion_BM ? "true" : "false");
  if (!load_dir_bm)
    load_dir_bm = ".";
  if (!dump_dir_BM)
    {
      dump_dir_BM = realpath (load_dir_bm, NULL);
      shouldfreedumpdir = dump_dir_BM != NULL;
    }
  if (run_gtk_BM && run_onion_BM)
    WARNPRINTF_BM ("bismon trying to run both GTK and ONION");
  load_initial_BM (load_dir_bm);
  if (added_passwords_filepath_BM && added_passwords_filepath_BM[0])
    add_passwords_from_file_BM (added_passwords_filepath_BM);
  if (chdir_after_load_bm)
    {
      if (g_mkdir_with_parents (chdir_after_load_bm, 0750))
        FATAL_BM ("failed to mkdir with parents %s for chdir-after-load",
                  chdir_after_load_bm);
      char cwdbuf[128];
      memset (cwdbuf, 0, sizeof (cwdbuf));
      if (chdir (chdir_after_load_bm))
        FATAL_BM ("failed to change directory after load to %s - %m",
                  chdir_after_load_bm);
      char *newd = getcwd (cwdbuf, sizeof (cwdbuf));
      if (newd)
        INFOPRINTF_BM ("changed directory after load to %s given as %s\n",
                       newd, chdir_after_load_bm);
      else
        INFOPRINTF_BM ("changed directory, given as %s, after load\n",
                       chdir_after_load_bm);
      fflush (NULL);
    }
  DBGPRINTF_BM ("run_gtk is %s & run_onion is %s",
                run_gtk_BM ? "true" : "false",
                run_onion_BM ? "true" : "false");
  if (nb_added_predef_bm > 0)
    add_new_predefined_bm ();
  if (nb_parsed_values_after_load_bm > 0)
    parse_values_after_load_BM ();
  if (count_added_contributors_bm > 0)
    add_contributors_after_load_BM ();
  if (count_removed_contributors_bm > 0)
    remove_contributors_after_load_BM ();
  if (module_to_emit_bm != NULL)
    do_emit_module_from_main_BM ();
  if (dump_after_load_dir_bm)
    do_dump_after_load_BM ();
  DBGPRINTF_BM ("run_gtk is %s & run_onion is %s",
                run_gtk_BM ? "true" : "false",
                run_onion_BM ? "true" : "false");
#ifdef BISMONGTK
  if (batch_bm)
    {
      nbworkjobs_BM = 0;
      INFOPRINTF_BM ("no GUI in batch mode\n");
    }
  else if (!run_onion_BM)
    {
      DBGPRINTF_BM ("no onion, with BISMONGTK");
      if (pid_filepath_bm && pid_filepath_bm[0]
          && strcmp (pid_filepath_bm, "-"))
        {
          FILE *pidfile = fopen (pid_filepath_bm, "w");
          if (!pidfile)
            FATAL_BM ("failed to open pid file %s - %m", pid_filepath_bm);
          fprintf (pidfile, "%d\n", (int) getpid ());
          fclose (pidfile);
          INFOPRINTF_BM ("wrote pid %d (BISMONGTK) in pid-file %s",
                         (int) getpid (), pid_filepath_bm);
        }
      if (run_gtk_BM)
        {
          INFOPRINTF_BM ("running GUI interface with GTK for %d jobs",
                         nbworkjobs_BM);
          rungui_BM (nbworkjobs_BM);
        }
    }
#ifdef BISMONION
  else if (run_onion_BM && run_gtk_BM)
    {
      DBGPRINTF_BM ("both onion & gtk");
      if (pid_filepath_bm && pid_filepath_bm[0]
          && strcmp (pid_filepath_bm, "-"))
        {
          FILE *pidfile = fopen (pid_filepath_bm, "w");
          if (!pidfile)
            FATAL_BM ("failed to open pid file %s - %m", pid_filepath_bm);
          fprintf (pidfile, "%d\n", (int) getpid ());
          fclose (pidfile);
          INFOPRINTF_BM
            ("wrote pid %d (BISMONGTK & BISMONION) in pid-file %s",
             (int) getpid (), pid_filepath_bm);
        }
      INFOPRINTF_BM ("running Web interface with ONION (& Gui!) for %d jobs",
                     nbworkjobs_BM);
      run_onionweb_BM (nbworkjobs_BM);
      INFOPRINTF_BM ("running GUI interface with GTK (& Web!) for %d jobs",
                     nbworkjobs_BM);
      rungui_BM (nbworkjobs_BM);
    }
#endif /*BISMONION with BISMONGTK */
#endif /*BISMONGTK*/
    //
    DBGPRINTF_BM ("run_gtk is %s & run_onion is %s",
                  run_gtk_BM ? "true" : "false",
                  run_onion_BM ? "true" : "false");
#ifdef BISMONION
  if (run_onion_BM && !run_gtk_BM)
    {
      DBGPRINTF_BM ("BISMONION with run_onion_BM without run_gtk_BM");
      if (batch_bm)
        {
          nbworkjobs_BM = 0;
          INFOPRINTF_BM ("no web in batch mode\n");
        }
      else
        {
          if (pid_filepath_bm && pid_filepath_bm[0]
              && strcmp (pid_filepath_bm, "-"))
            {
              FILE *pidfile = fopen (pid_filepath_bm, "w");
              if (!pidfile)
                FATAL_BM ("failed to open pid file %s - %m", pid_filepath_bm);
              fprintf (pidfile, "%d\n", (int) getpid ());
              fclose (pidfile);
              INFOPRINTF_BM ("wrote pid %d (BISMONION) in pid-file %s",
                             (int) getpid (), pid_filepath_bm);
            }
          INFOPRINTF_BM ("running ONION web interface for %d jobs %s",
                         nbworkjobs_BM,
                         bismon_has_gui_BM ()? "also with a GUI" :
                         "alone (no GUI)");
          run_onionweb_BM (nbworkjobs_BM);
        }
    }
#endif /*BISMONION*/
    ///
    if (run_onion_BM)
    {
      stop_onion_event_loop_BM ();
    }
  DBGPRINTF_BM ("ending BISMON run_gtk_BM %s run_onion_BM %s batch_bm %s",
                run_gtk_BM ? "true" : "false",
                run_onion_BM ? "true" : "false", batch_bm ? "true" : "false");
  free ((void *) contributors_filepath_BM), contributors_filepath_BM = NULL;
  free ((void *) passwords_filepath_BM), passwords_filepath_BM = NULL;
  if (shouldfreedumpdir)
    free ((void *) dump_dir_BM), dump_dir_BM = NULL;
  INFOPRINTF_BM ("end of %s, on %s, pid %d, %.3f elapsed, %.3f cpu time\n"
                 "... timestamp %s\n"
                 "... lastgitcommit %s\n"
                 "... checksum %s\n",
                 myprogname_BM, myhostname_BM, (int) getpid (),
                 elapsedtime_BM (), cputime_BM (), bismon_timestamp,
                 bismon_lastgitcommit, bismon_checksum);
  fflush (NULL);
}                               /* end main */

bool
bismon_has_gui_BM (void)
{
  return gui_is_running_BM || run_gtk_BM;
}                               /* end bismon_has_gui_BM */


bool
bismon_has_web_BM (void)
{
  return web_is_running_BM || run_onion_BM;
}                               /* end bismon_has_web_BM */

void
do_dump_after_load_BM (void)
{
  {
    char *rd = realpath (dump_after_load_dir_bm, NULL);
    if (rd)
      {
        INFOPRINTF_BM ("dump after load into %s directory (%s)",
                       dump_after_load_dir_bm, rd);
        free (rd), rd = NULL;
      }
    else
      INFOPRINTF_BM ("dump after load into %s directory",
                     dump_after_load_dir_bm);
  }
  struct dumpinfo_stBM di = dump_BM (dump_after_load_dir_bm, NULL);
  INFOPRINTF_BM ("dump after load: scanned %ld, emitted %ld objects\n"
                 "... did %ld todos, wrote %ld files\n"
                 "... in %.3f elapsed, %.4f cpu seconds\n (%.1f elapsed, %.1f cpu µs/obj)\n",
                 di.dumpinfo_scanedobjectcount,
                 di.dumpinfo_emittedobjectcount, di.dumpinfo_todocount,
                 di.dumpinfo_wrotefilecount, di.dumpinfo_elapsedtime,
                 di.dumpinfo_cputime,
                 di.dumpinfo_elapsedtime * 1.0e6 /
                 di.dumpinfo_emittedobjectcount,
                 di.dumpinfo_cputime * 1.0e6 /
                 di.dumpinfo_emittedobjectcount);
}                               /*  end do_dump_after_load_BM */



void
emit_has_predef_BM (void)
{
  rawid_tyBM *idarr = calloc (count_emit_has_predef_bm, sizeof (rawid_tyBM));
  if (!idarr)
    FATAL_BM ("failed to calloc idarr for %d", count_emit_has_predef_bm);
  for (int ix = 0; ix < count_emit_has_predef_bm; ix++)
    idarr[ix] = randomid_BM ();
  qsort (idarr, count_emit_has_predef_bm, sizeof (rawid_tyBM), idqcmp_BM);
  INFOPRINTF_BM ("/// %d extra predefs\n", count_emit_has_predef_bm);
  INFOPRINTF_BM ("// !@ %.3f\n", clocktime_BM (CLOCK_REALTIME));
  // we won't use INFOPRINTF_BM below
  for (int ix = 0; ix < count_emit_has_predef_bm; ix++)
    {
      rawid_tyBM id = idarr[ix];
      char idbuf[32];
      memset (idbuf, 0, sizeof (idbuf));
      idtocbuf32_BM (id, idbuf);
      printf ("HAS_PREDEF_BM(%s,%lld,%lld,%u)\n",
              idbuf, (long long) id.id_hi, (long long) id.id_lo,
              hashid_BM (id));
    }
  printf ("\n\n/***\n");
  for (int ix = 0; ix < count_emit_has_predef_bm; ix++)
    {
      rawid_tyBM id = idarr[ix];
      char idbuf[32];
      memset (idbuf, 0, sizeof (idbuf));
      idtocbuf32_BM (id, idbuf);
      printf (" ROUTINEOBJNAME_BM (%s)\n", idbuf);
    }
  printf ("***/\n\n\n");
  INFOPRINTF_BM ("/// emitted %d extra predefs\n", count_emit_has_predef_bm);
}                               /* end emit_has_predef_BM */


void
initialize_contributors_path_BM (void)
{
  if (!contributors_filepath_BM)
    {
      char *path = NULL;
      char *homepath = NULL;
      if (!access (CONTRIBUTORS_FILE_BM, R_OK))
        contributors_filepath_BM = CONTRIBUTORS_FILE_BM;
      else if ((homepath = getenv ("HOME")) != NULL
               && asprintf (&path, "%s/" CONTRIBUTORS_FILE_BM, homepath) > 0
               && path != NULL && (!access (path, R_OK)
                                   || (free (path), (path = NULL))))
        contributors_filepath_BM = path;
      if (!contributors_filepath_BM)
        {
          char cwdbuf[128];
          memset (cwdbuf, 0, sizeof (cwdbuf));
          if (!getcwd (cwdbuf, sizeof (cwdbuf)))
            strcpy (cwdbuf, "./");
          FATAL_BM
            ("no %s file found here in %s or in $HOME %s, so pass --contributors-file=... option to give one",
             CONTRIBUTORS_FILE_BM, cwdbuf, homepath);
        }
      if (access (contributors_filepath_BM, R_OK))
        FATAL_BM ("cannot read contributors file %s - %m",
                  contributors_filepath_BM);
      char *rcpath = realpath (contributors_filepath_BM, NULL);
      if (!rcpath)
        FATAL_BM ("cannot get real path of contributors file %s - %m",
                  contributors_filepath_BM);
      if (path && rcpath != path)
        free (path), path = NULL;
      contributors_filepath_BM = rcpath;
    }
  else
    {                           // some given contributors_filepath_bm with --contributors-path= ... argument
      if (access (contributors_filepath_BM, R_OK))
        FATAL_BM ("cannot read contributors file %s - %m",
                  contributors_filepath_BM);
      char *rcpath = realpath (contributors_filepath_BM, NULL);
      if (!rcpath)
        FATAL_BM ("cannot get real path of contributors file %s - %m",
                  contributors_filepath_BM);
      contributors_filepath_BM = rcpath;
    }
  if (access (contributors_filepath_BM, R_OK))
    FATAL_BM ("cannot read real contributors file %s - %m",
              contributors_filepath_BM);
  INFOPRINTF_BM ("using %s as the contributors file\n",
                 contributors_filepath_BM);
}                               /* end initialize_contributors_path_BM */



////////////////

void
initialize_passwords_path_BM (void)
{
  if (!passwords_filepath_BM)   // no --passwords-file program option
    {
      char *path = NULL;
      char *homepath = NULL;
      if (!access (PASSWORDS_FILE_BM, R_OK))
        passwords_filepath_BM = PASSWORDS_FILE_BM;
      else if ((homepath = getenv ("HOME")) != NULL
               && asprintf (&path, "%s/" PASSWORDS_FILE_BM, homepath) > 0
               && path != NULL && (!access (path, R_OK)
                                   || (free (path), (path = NULL))))
        passwords_filepath_BM = path;
      if (!passwords_filepath_BM)
        {
          char cwdbuf[128];
          memset (cwdbuf, 0, sizeof (cwdbuf));
          if (!getcwd (cwdbuf, sizeof (cwdbuf)))
            strcpy (cwdbuf, "./");
          FATAL_BM
            ("no %s file found here in %s or in $HOME %s, pass --passwords-file=... option to give one, or make an empty one e.g. with touch(1)",
             PASSWORDS_FILE_BM, cwdbuf, homepath);
        }
      if (access (passwords_filepath_BM, R_OK))
        FATAL_BM ("cannot read passwords file %s - %m",
                  passwords_filepath_BM);
      char *rcpath = realpath (passwords_filepath_BM, NULL);
      if (!rcpath)
        FATAL_BM ("cannot get real path of passwords file %s - %m",
                  passwords_filepath_BM);
      if (path && rcpath != path)
        free (path), path = NULL;
      passwords_filepath_BM = rcpath;
    }
  else
    {                           // some given passwords_filepath_bm with --passwords-path= ... argument
      if (access (passwords_filepath_BM, R_OK))
        FATAL_BM ("cannot read passwords file %s - %m",
                  passwords_filepath_BM);
      char *rcpath = realpath (passwords_filepath_BM, NULL);
      if (!rcpath)
        FATAL_BM ("cannot get real path of passwords file %s - %m",
                  passwords_filepath_BM);
      passwords_filepath_BM = rcpath;
    }
  if (access (passwords_filepath_BM, R_OK))
    FATAL_BM ("cannot read real passwords file %s - %m",
              passwords_filepath_BM);
  struct stat mystat = { };
  if (stat (passwords_filepath_BM, &mystat))
    FATAL_BM ("cannot stat real passwords file %s - %m",
              passwords_filepath_BM);
  if ((mystat.st_mode & S_IFMT) != S_IFREG)
    FATAL_BM ("real passwords file %s is not a regular file",
              passwords_filepath_BM);
  if ((mystat.st_mode & (S_IRWXG | S_IRWXO)) != 0)
    FATAL_BM
      ("real passwords file %s should not be group or others readable/writable but only by owner; run chmod go-rwx on it",
       passwords_filepath_BM);
  INFOPRINTF_BM ("using %s as the password file", passwords_filepath_BM);
}                               /* end initialize_passwords_path_BM */



////////////////
void
parse_values_after_load_BM (void)
{
  LOCALFRAME_BM ( /*prev stackf: */ NULL, /*descr: */ NULL,
                 objectval_tyBM * parsob;
                 value_tyBM parsedval;
    );
  _.parsob = makeobj_BM ();
  INFOPRINTF_BM ("parsing %d values after load %s using parsob %s\n",
                 nb_parsed_values_after_load_bm, load_dir_bm,
                 objectdbg_BM (_.parsob));
  for (int ix = 0; ix < nb_parsed_values_after_load_bm; ix++)
    {
      const char *curvalstr = parsed_values_after_loadarr_bm[ix];
      INFOPRINTF_BM ("parsing value#%d:::\n%s\n///----\n", ix, curvalstr);
      struct parser_stBM *pars =
        makeparser_memopen_BM (curvalstr, strlen (curvalstr), _.parsob);
      ASSERT_BM (pars != NULL);
      bool gotval = false;
      _.parsedval = parsergetvalue_BM (pars, CURFRAME_BM, 0, &gotval);
      if (!gotval)
        FATAL_BM ("failed to parse value#%d", ix);
      INFOPRINTF_BM ("parsed value#%d is:\n%s\n", ix,
                     debug_outstr_value_BM (_.parsedval, CURFRAME_BM, 0));
      fflush (NULL);
      objclearpayload_BM (_.parsob);
    }
  for (int ix = 0; ix < nb_parsed_values_after_load_bm; ix++)
    {
      free (parsed_values_after_loadarr_bm[ix]),
        parsed_values_after_loadarr_bm[ix] = NULL;
    }
  INFOPRINTF_BM ("done parsing %d values after load\n",
                 nb_parsed_values_after_load_bm);
}                               /* end parse_values_after_load_BM */


void
add_contributors_after_load_BM (void)
{
  LOCALFRAME_BM ( /*prev stackf: */ NULL, /*descr: */ NULL,
                 objectval_tyBM * userob;
    );
  ASSERT_BM (count_added_contributors_bm > 0);
  ASSERT_BM (added_contributors_arr_bm != NULL);
  INFOPRINTF_BM ("adding %d contributors after load",
                 count_added_contributors_bm);
  for (int cix = 0; cix < count_added_contributors_bm; cix++)
    {
      char *errmsg = NULL;
      _.userob =
        add_contributor_user_BM (added_contributors_arr_bm[cix], &errmsg,
                                 CURFRAME_BM);
      if (!_.userob)
        FATAL_BM ("failed to add contributor user#%d %s - %s", cix,
                  added_contributors_arr_bm[cix], errmsg);
      ASSERT_BM (errmsg == NULL);
      INFOPRINTF_BM ("added contributor %s with object %s",
                     added_contributors_arr_bm[cix], objectdbg_BM (_.userob));
    }
  for (int cix = 0; cix < count_added_contributors_bm; cix++)
    free (added_contributors_arr_bm[cix]), added_contributors_arr_bm[cix] =
      NULL;
  INFOPRINTF_BM ("added %d contributors after load",
                 count_added_contributors_bm);
  free (added_contributors_arr_bm);
  count_added_contributors_bm = 0;
  size_added_contributors_bm = 0;
}                               /* end add_contributors_after_load_BM */




void
remove_contributors_after_load_BM (void)
{
  LOCALFRAME_BM ( /*prev stackf: */ NULL, /*descr: */ NULL,
                 objectval_tyBM * oldcontribob;
    );
  ASSERT_BM (count_removed_contributors_bm > 0);
  INFOPRINTF_BM ("removing %d contributors after load",
                 count_removed_contributors_bm);
  ASSERT_BM (removed_contributors_arr_bm != NULL);
  for (int cix = 0; cix < count_removed_contributors_bm; cix++)
    {
      _.oldcontribob =
        remove_contributor_by_name_BM (removed_contributors_arr_bm[cix],
                                       CURFRAME_BM);
      if (!_.oldcontribob)
        FATAL_BM ("failed to remove contributor user#%d %ss",
                  cix, removed_contributors_arr_bm[cix]);
      objputspacenum_BM (_.oldcontribob, TransientSp_BM);
      INFOPRINTF_BM ("removed contributor %s of object %s",
                     removed_contributors_arr_bm[cix],
                     objectdbg_BM (_.oldcontribob));
    }
  for (int cix = 0; cix < count_removed_contributors_bm; cix++)
    free (removed_contributors_arr_bm[cix]),
      removed_contributors_arr_bm[cix] = NULL;
  INFOPRINTF_BM ("removed %d contributors after load",
                 count_removed_contributors_bm);
  free (removed_contributors_arr_bm);
  count_removed_contributors_bm = 0;
  size_removed_contributors_bm = 0;
}                               /* end remove_contributors_after_load_BM */




void
add_passwords_from_file_BM (const char *addedpasspath)
{
  LOCALFRAME_BM ( /*prev stackf: */ NULL, /*descr: */ NULL,
                 objectval_tyBM * contribob;
    );
  ASSERT_BM (addedpasspath != NULL);
  DBGPRINTF_BM ("add_passwords_from_file start addedpasspath %s",
                addedpasspath);
  bool withstdin = (!addedpasspath[0] || !strcmp (addedpasspath, "-"));
  FILE *pasfil = withstdin ? stdin : fopen (addedpasspath, "r");
  if (!pasfil)
    FATAL_BM ("cannot open added passwords file %s : %m", addedpasspath);
  size_t sizpas = 128;
  char *linpas = calloc (sizpas, 0);
  int lincnt = 0;
  int nbch = 0;
  if (!linpas)
    FATAL_BM ("calloc failure in add_passwords_from_file - %m");
  do
    {
      ssize_t linlenpas = getline (&linpas, &sizpas, pasfil);
      if (linlenpas < 0)
        break;
      if (linlenpas > 0 && linpas[linlenpas - 1] == '\n')
        linpas[linlenpas - 1] = (char) 0;
      lincnt++;
      DBGPRINTF_BM ("add_passwords_from_file lincnt=%d linpas=%s",
                    lincnt, linpas);
      if (linpas[0] == (char) 0 || linpas[0] == '#')
        continue;
      char *pcol = strchr (linpas, ':');
      if (linlenpas < 10 || !pcol || strlen (pcol + 1) < 8)
        FATAL_BM ("bad or too short line#%d when adding passwords from %s",
                  lincnt, addedpasspath);
      *pcol = (char) 0;
      const char *contributorstr = linpas;
      const char *passwdstr = pcol + 1;
      _.contribob = find_contributor_BM (contributorstr, CURFRAME_BM);
      DBGPRINTF_BM ("add_passwords_from_file  line#%d contributorstr '%s' "
                    " passwdstr '%s' contribob %s",
                    lincnt, contributorstr, passwdstr,
                    objectdbg_BM (_.contribob));
      if (!_.contribob)
        FATAL_BM
          ("add_passwords_from_file line#%d cannot find contributor for '%s'",
           lincnt, contributorstr);
      {
        char *errmsg = NULL;
        if (!valid_password_BM (passwdstr, &errmsg))
          FATAL_BM ("add_passwords_from_file line#%d password is invalid: %s",
                    lincnt, errmsg);
      }
      DBGPRINTF_BM ("add_passwords_from_file  line#%d contributorstr '%s'"
                    "  passwdstr '%s' contribob %s",
                    lincnt, contributorstr, passwdstr,
                    objectdbg_BM (_.contribob));
      if (!put_contributor_password_BM (_.contribob, passwdstr, CURFRAME_BM))
        FATAL_BM
          ("add_passwords_from_file line#%d cannot set password for '%s'",
           lincnt, contributorstr);
      DBGPRINTF_BM ("add_passwords_from_file "
                    "after put_contributor_password line#%d contributorstr '%s'"
                    "  passwdstr '%s' contribob %s",
                    lincnt, contributorstr, passwdstr,
                    objectdbg_BM (_.contribob));
      {
        objlock_BM (_.contribob);
        INFOPRINTF_BM ("changed password for contributor %s named %s",
                       objectdbg_BM (_.contribob),
                       bytstring_BM (objcontributornamepayl_BM (_.contribob))
                       ? : "**??**");
        objunlock_BM (_.contribob);
      }
      nbch++;
    }
  while (!feof (pasfil));
  if (!withstdin)
    fclose (pasfil), pasfil = NULL;
  if (withstdin)
    INFOPRINTF_BM ("Added or changed %d passwords using standard input",
                   nbch);
  else
    INFOPRINTF_BM ("Added or changed %d passwords using file %s", nbch,
                   addedpasspath);
}                               /* end of add_passwords_from_file_BM */




////////////////////////////////////////////////////////////////

extern void do_internal_deferred_apply3_BM (value_tyBM funv,
                                            value_tyBM arg1,
                                            value_tyBM arg2, value_tyBM arg3);
extern void do_internal_deferred_send3_BM (value_tyBM recv,
                                           objectval_tyBM * obsel,
                                           value_tyBM arg1,
                                           value_tyBM arg2, value_tyBM arg3);

// called from did_deferred_BM
void
do_internal_deferred_apply3_BM (value_tyBM fun,
                                value_tyBM arg1, value_tyBM arg2,
                                value_tyBM arg3)
{
  LOCALFRAME_BM ( /*prev stackf: */ NULL, /*descr: */ NULL,
                 value_tyBM funv; value_tyBM arg1v, arg2v, arg3v;
                 value_tyBM failres;    //
                 value_tyBM failplace;  //
    );
  _.funv = fun;
  _.arg1v = arg1;
  _.arg2v = arg2;
  _.arg3v = arg3;
  int failcod = 0;
  _.failres = NULL;
  _.failplace = NULL;
  struct failurelockset_stBM flockset = { };
  initialize_failurelockset_BM (&flockset, sizeof (flockset));
  LOCAL_FAILURE_HANDLE_BM (&flockset, lab_failureapply, failcod, _.failres,
                           _.failplace);
  if (failcod)
  lab_failureapply:
    {
      destroy_failurelockset_BM (&flockset);
      curfailurehandle_BM = NULL;
      WARNPRINTF_BM ("deffered_apply3_gtk failure, failcod#%d failreason: %s\n", failcod,       //
                     debug_outstr_value_BM (_.failres,  //
                                            CURFRAME_BM, 0));
      return;
    }
  NONPRINTF_BM ("internaldeferapply funv %s arg1 %s arg2 %s arg3 %s",   //
                debug_outstr_value_BM (_.funv,  //
                                       CURFRAME_BM, 0), //
                debug_outstr_value_BM (_.arg1v, //
                                       CURFRAME_BM, 0), //
                debug_outstr_value_BM (_.arg2v, //
                                       CURFRAME_BM, 0), //
                debug_outstr_value_BM (_.arg3v, //
                                       CURFRAME_BM, 0));
  (void) apply3_BM (_.funv, CURFRAME_BM, _.arg1v, _.arg2v, _.arg3v);
  NONPRINTF_BM ("internaldeferapply applied funv %s",   //
                debug_outstr_value_BM (_.funv,  //
                                       CURFRAME_BM, 0));
  destroy_failurelockset_BM (&flockset);
  curfailurehandle_BM = NULL;
}                               /* end do_internal_defer_apply3_BM */


// called from did_deferred_BM
void
do_internal_deferred_send3_BM (value_tyBM recv, objectval_tyBM * obsel,
                               value_tyBM arg1, value_tyBM arg2,
                               value_tyBM arg3)
{
  LOCALFRAME_BM ( /*prev stackf: */ NULL, /*descr: */ NULL,
                 objectval_tyBM * obsel;
                 value_tyBM recva, arg1v, arg2v, arg3v;
                 value_tyBM failres;    //
                 value_tyBM failplace;  //
    );
  _.recva = recv;
  _.obsel = obsel;
  _.arg1v = arg1;
  _.arg2v = arg2;
  _.arg3v = arg3;
  int failcod = 0;
  _.failres = NULL;
  _.failplace = NULL;
  struct failurelockset_stBM flockset = { };
  initialize_failurelockset_BM (&flockset, sizeof (flockset));
  LOCAL_FAILURE_HANDLE_BM (&flockset, lab_failuresend, failcod, _.failres,
                           _.failplace);
  if (failcod)
  lab_failuresend:
    {
      destroy_failurelockset_BM (&flockset);
      curfailurehandle_BM = NULL;
      WARNPRINTF_BM ("deffered_send3_gtk failure, failcod#%d\n" "failreason: %s\n" "failplace: %s\n",   //
                     failcod,
                     OUTSTRVALUE_BM (_.failres),
                     OUTSTRVALUE_BM (_.failplace));
      return;
    }
  DBGPRINTF_BM ("internaldefersend recv %s obsel %s arg1 %s arg2 %s arg3 %s",   //
                debug_outstr_value_BM (_.recva, //
                                       CURFRAME_BM, 0), //
                objectdbg_BM (_.obsel), //
                debug_outstr_value_BM (_.arg1v, //
                                       CURFRAME_BM, 0), //
                debug_outstr_value_BM (_.arg2v, //
                                       CURFRAME_BM, 0), //
                debug_outstr_value_BM (_.arg3v, //
                                       CURFRAME_BM, 0));
  (void) send3_BM (recv, obsel, CURFRAME_BM, arg1, arg2, arg3);
  DBGPRINTF_BM ("internaldefersend did send recv %s obsel %s",  //
                debug_outstr_value_BM (_.recva, //
                                       CURFRAME_BM, 0), //
                objectdbg_BM (_.obsel));
  destroy_failurelockset_BM (&flockset);
  curfailurehandle_BM = NULL;
}                               /* end do_internal_defer_send3_BM */



////////////////////////////////////////////////////////////////
#ifdef BISMONGTK
extern bool did_deferred_BM (void);

static gboolean
deferpipereadhandler_BM (GIOChannel * source,
                         GIOCondition condition __attribute__ ((unused)),
                         gpointer data __attribute__ ((unused)))
{
  NONPRINTF_BM ("deferpipereadhandler_BM start tid#%ld", (long) gettid_BM ());
  if (!source)
    return false;
  gchar buf[8] = "";
  for (;;)
    {
      memset (buf, 0, sizeof (buf));
      gsize nbrd = 0;
      // reading more than one byte each time can block the program
      GIOStatus st = g_io_channel_read_chars (source, buf, 1,
                                              &nbrd, NULL);
      NONPRINTF_BM ("deferpipereadhandler_BM nbrd=%d buf '%s' st#%d tid#%ld",
                    (int) nbrd, buf, (int) st, (long) gettid_BM ());
      if (st == G_IO_STATUS_EOF)
        return FALSE;
      if (!did_deferred_BM ())
        return TRUE;
      if (st == G_IO_STATUS_NORMAL && nbrd > 0)
        return TRUE;
    }
  return TRUE;                  // to keep watching
}                               /* end deferpipereadhandler_BM */


extern void add_defer_command_gtk_BM (void);

static void startguilog_BM (void);
static void endguilog_BM (void);


////////////////////////////////////////////////////////////////
void
rungui_BM (int nbjobs)
{
  NONPRINTF_BM ("rungui nbjobs %d start tid#%ld",
                nbjobs, (long) gettid_BM ());
  int deferpipes[2] = { -1, -1 };
  if (pipe (deferpipes) < 0)
    FATAL_BM ("failed to pipe GTK deferpipe");
  defer_gtk_readpipefd_BM = deferpipes[0];
  defer_gtk_writepipefd_BM = deferpipes[1];
  NONPRINTF_BM ("rungui defer_gtk_readpipefd=%d defer_gtk_writepipefd_BM=%d",
                defer_gtk_readpipefd_BM, defer_gtk_writepipefd_BM);
  defer_gtk_readpipechan_BM = g_io_channel_unix_new (defer_gtk_readpipefd_BM);
  g_io_add_watch (defer_gtk_readpipechan_BM, G_IO_IN, deferpipereadhandler_BM,
                  NULL);
  gui_is_running_BM = true;
  startguilog_BM ();
  start_agenda_work_threads_BM (nbjobs);
  NONPRINTF_BM ("rungui nbjobs %d before gtkmain", nbjobs);
  gtk_main ();
  NONPRINTF_BM
    ("rungui nbjobs %d after gtkmain before stopagendawork tid#%ld elapsed %.3f s",
     nbjobs, (long) gettid_BM (), elapsedtime_BM ());
  stop_agenda_work_threads_BM ();
  NONPRINTF_BM ("rungui nbjobs %d after stopagendawork elapsed %.3f s",
                nbjobs, elapsedtime_BM ());
  g_io_channel_shutdown (defer_gtk_readpipechan_BM, false, NULL);
  g_io_channel_unref (defer_gtk_readpipechan_BM), defer_gtk_readpipechan_BM =
    NULL;
  close (defer_gtk_readpipefd_BM), defer_gtk_readpipefd_BM = -1;
  close (defer_gtk_writepipefd_BM), defer_gtk_writepipefd_BM = -1;
  gui_is_running_BM = false;
  if (gui_command_log_file_BM)
    endguilog_BM ();
  NONPRINTF_BM ("rungui nbjobs %d ending tid#%ld elapsed %.3f s",
                nbjobs, (long) gettid_BM (), elapsedtime_BM ());
}                               /* end rungui_BM */


void
startguilog_BM (void)
{
  if (!gui_log_name_bm || !gui_log_name_bm[0])
    {
      gui_command_log_file_BM = NULL;
      printf ("no GUI log\n");
    }
  else if (!strcmp (gui_log_name_bm, "-"))
    {
      gui_command_log_file_BM = stdout;
      printf ("GUI log to stdout\n");
    }
  else
    {
      if (!access (gui_log_name_bm, R_OK))
        {
          char *backupath = NULL;
          asprintf (&backupath, "%s~", gui_log_name_bm);
          if (!backupath)
            FATAL_BM ("asprintf fail for backupath %s (%m)", gui_log_name_bm);
          if (!access (backupath, R_OK))
            {
              char *backup2path = NULL;
              asprintf (&backup2path, "%s~%%", gui_log_name_bm);
              if (backup2path)
                {
                  if (!rename (backupath, backup2path))
                    INFOPRINTF_BM ("logfile backup^2: %s -> %s", backupath,
                                   backup2path);
                }
              free (backup2path);
            }
          if (!rename (gui_log_name_bm, backupath))
            INFOPRINTF_BM ("logfile backup: %s -> %s", gui_log_name_bm,
                           backupath);
          free (backupath), backupath = NULL;
        };
      gui_command_log_file_BM = fopen (gui_log_name_bm, "w");
      if (!gui_command_log_file_BM)
        FATAL_BM ("fopen GUI log %s failure (%m)", gui_log_name_bm);
      INFOPRINTF_BM ("GUI log to %s\n", gui_log_name_bm);
      fprintf (gui_command_log_file_BM, "// GUI command log file %s\n",
               basename (gui_log_name_bm));
    }
  if (gui_command_log_file_BM)
    {
      {
        time_t nowtim = time (NULL);
        struct tm nowtm = { };
        localtime_r (&nowtim, &nowtm);
        char nowbuf[64];
        memset (nowbuf, 0, sizeof (nowbuf));
        strftime (nowbuf, sizeof (nowbuf), "%c", &nowtm);
        fprintf (gui_command_log_file_BM,
                 "// bismon GUI log at %s on %s pid %d\n",
                 nowbuf, myhostname_BM, (int) getpid ());
      }
      fprintf (gui_command_log_file_BM,
               "// bismon checksum %s lastgitcommit %s\n", bismon_checksum,
               bismon_lastgitcommit);
      fprintf (gui_command_log_file_BM,
               "// bismon timestamp %s directory %s\n", bismon_timestamp,
               bismon_directory);
      {
        char curdirpath[128];
        memset (curdirpath, 0, sizeof (curdirpath));
        if (getcwd (curdirpath, sizeof (curdirpath) - 1))
          fprintf (gui_command_log_file_BM,
                   "// bismon current directory %s\n", curdirpath);
        else
          FATAL_BM ("getcwd failure %m");
      }
      fflush (gui_command_log_file_BM);
    }
}                               /* end startguilog_BM */

void
endguilog_BM (void)
{
  time_t nowtim = time (NULL);
  struct tm nowtm = { };
  localtime_r (&nowtim, &nowtm);
  char nowbuf[64];
  memset (nowbuf, 0, sizeof (nowbuf));
  strftime (nowbuf, sizeof (nowbuf), "%c", &nowtm);
  fprintf (gui_command_log_file_BM,
           "\n\f/// end of bismon GUI command log file %s at %s\n",
           gui_log_name_bm, nowbuf);
  if (gui_command_log_file_BM != stdout && gui_command_log_file_BM != stderr)
    fclose (gui_command_log_file_BM);
  gui_command_log_file_BM = NULL;
  fflush (NULL);
}                               /* end endguilog_BM */

void
add_defer_command_gtk_BM (void)
{
  DBGPRINTF_BM ("add_defer_command_gtk_BM start");
  ASSERT_BM (defer_gtk_writepipefd_BM > 0);
  int count = 0;
  do
    {                           /* most of the time, that loop runs once */
      int nbw = write (defer_gtk_writepipefd_BM, "X", 1);
      if (nbw < 0)
        {
          if (errno == EINTR)
            {
              count++;
              continue;
            }
          if (errno == EWOULDBLOCK)
            {
              usleep (6500);
              count++;
              continue;
            }
          else
            FATAL_BM ("add_defer_command_gtk_BM failed write, count %d - %m",
                      count);
        }
      if (nbw == 1)
        return;
      usleep (1000);
      count++;
    }
  while (count < 256);
  FATAL_BM ("add_defer_command_gtk_BM failure count#%d", count);
}                               /* end add_defer_command_gtk_BM */

#endif /*BISMONGTK*/
////////////////////////////////////////////////////////////////
  void
give_prog_version_BM (const char *progname)
{
  printf ("%s: version information\n", progname);
  printf ("\t timestamp: %s\n", bismon_timestamp);
  printf ("\t last git commit: %s\n", bismon_lastgitcommit);
  printf ("\t last git tag: %s\n", bismon_lastgittag);
  printf ("\t source checksum: %s\n", bismon_checksum);
  printf ("\t source dir: %s\n", bismon_directory);
  printf ("\t makefile: %s\n", bismon_makefile);
  printf ("########\n");
  printf ("run\n" "\t   %s --help\n" "to get help.\n", progname);
  exit (EXIT_SUCCESS);
}                               /* end give_prog_version_BM */

void
backtracerrorcb_BM (void *data __attribute__ ((unused)),
                    const char *msg, int errnum)
{
  backtracestate_BM = NULL;
  FATAL_BM ("backtrace error: %s #%d", msg, errnum);
}                               /* end backtracerrorcb_BM */


////////////////////////////////////////////////////////////////

/// nearly copied from Ian Lance Taylor's libbacktrace/print.c
/// see https://github.com/ianlancetaylor/libbacktrace
struct print_data_BM
{
  struct backtrace_state *state;
  FILE *f;
};
/* Print one level of a backtrace.  */

static int
printbt_callback_BM (void *data, uintptr_t pc, const char *filename,
                     int lineno, const char *function)
{
  struct print_data_BM *pdata = (struct print_data_BM *) data;

  const char *funame = function;
  char nambuf[80];
  memset (nambuf, 0, sizeof (nambuf));
  if (!funame)
    {
      Dl_info di;
      memset (&di, 0, sizeof (di));
      if (dladdr ((void *) pc, &di))
        {
          if (di.dli_sname)
            {
              fprintf (pdata->f, "0x%lx @%s+%#lx\n",
                       (unsigned long) pc, di.dli_sname,
                       (char *) pc - (char *) di.dli_saddr);
            }
          else if (di.dli_fname)
            {
              fprintf (pdata->f, "0x%lx @@%s+%#lx\n",
                       (unsigned long) pc, basename (di.dli_fname),
                       (char *) pc - (char *) di.dli_fbase);
            }
          else
            fprintf (pdata->f, "0x%lx ?-?\n", (unsigned long) pc);
        }
      else
        fprintf (pdata->f, "0x%lx ???\n", (unsigned long) pc);
    }
  else
    fprintf (pdata->f, "0x%lx %s\n", (unsigned long) pc, funame);
  if (filename)
    fprintf (pdata->f, "\t%s:%d\n", basename (filename), lineno);
  return 0;
}                               /* end printbt_callback_BM */

/* Print errors to stderr.  */

static void
errorbt_callback_BM (void *data_
                     __attribute__ ((unused)), const char *msg, int errnum)
{
  //  struct print_data_BM *pdata = (struct print_data_BM *) data;

  //if (pdata->state->filename != NULL)
  //  fprintf (stderr, "%s: ", pdata->state->filename);
  if (errnum > 0)
    WARNPRINTF_BM ("libbacktrace:: %s (%s)", msg, strerror (errnum));
  else
    WARNPRINTF_BM ("libbacktrace: %s", msg);
}

/* Print a backtrace.  */

void
backtrace_print_BM (struct backtrace_state *state, int skip, FILE * f)
{
  struct print_data_BM data;

  data.state = state;
  data.f = f;
  backtrace_full (state, skip + 1, printbt_callback_BM, errorbt_callback_BM,
                  (void *) &data);
}                               /* end backtrace_print_BM */

////////////////////////////////////////////////////////////////
void
queue_process_BM (const stringval_tyBM * dirstrarg,
                  const node_tyBM * cmdnodarg,
                  const closure_tyBM * endclosarg,
                  struct stackframe_stBM *stkf)
{
#ifdef BISMONGTK
  extern void
    gtk_queue_process_BM (const stringval_tyBM * dirstrarg,
                          const node_tyBM * cmdnodarg,
                          const closure_tyBM * endclosarg,
                          struct stackframe_stBM *stkf);
#endif
#ifdef BISMONION
  extern void
    onion_queue_process_BM (const stringval_tyBM * dirstrarg,
                            const node_tyBM * cmdnodarg,
                            const closure_tyBM * endclosarg,
                            struct stackframe_stBM *stkf);
#endif
#ifdef BISMONGTK
  if (gui_is_running_BM)
    {
      gtk_queue_process_BM (dirstrarg, cmdnodarg, endclosarg, stkf);
      return;
    }
#endif
#ifdef BISMONION
  if (web_is_running_BM)
    {
      onion_queue_process_BM (dirstrarg, cmdnodarg, endclosarg, stkf);
      return;
    }
#endif
  FATAL_BM ("queue_process_BM without web or GUI");
}                               /* end queue_process_BM */

void
log_begin_message_BM (void)
{
#ifdef BISMONGTK
  extern void gtk_log_begin_message_BM (void);
#endif
#ifdef BISMONION
  extern void onion_log_begin_message_BM (void);
#endif
#ifdef BISMONGTK
  if (gui_is_running_BM)
    {
      gtk_log_begin_message_BM ();
      return;
    };
#endif /*BISMONGTK*/
#ifdef BISMONION
    if (web_is_running_BM)
    {
      onion_log_begin_message_BM ();
      return;
    }
#endif
  FATAL_BM ("log_begin_message_BM without web or GUI");
}                               /* end log_begin_message_BM */



void
log_end_message_BM (void)
{
#ifdef BISMONGTK
  extern void gtk_log_end_message_BM (void);
#endif
#ifdef BISMONION
  extern void onion_log_end_message_BM (void);
#endif
#ifdef BISMONGTK
  if (gui_is_running_BM)
    {
      gtk_log_end_message_BM ();
      return;
    };
#endif /*BISMONGTK*/
#ifdef BISMONION
    if (web_is_running_BM)
    {
      onion_log_end_message_BM ();
      return;
    }
#endif
  FATAL_BM ("log_end_message_BM without web or GUI");
}                               /* end log_end_message_BM */



void
log_puts_message_BM (const char *str)
{
#ifdef BISMONGTK
  extern void gtk_log_puts_message_BM (const char *);
#endif
#ifdef BISMONION
  extern void onion_log_puts_message_BM (const char *);
#endif
#ifdef BISMONGTK
  if (gui_is_running_BM)
    {
      gtk_log_puts_message_BM (str);
      return;
    };
#endif /*BISMONGTK*/
#ifdef BISMONION
    if (web_is_running_BM)
    {
      onion_log_puts_message_BM (str);
      return;
    }
#endif
  FATAL_BM ("log_puts_message_BM without web or GUI for: %s", str);
}                               /* end log_puts_message_BM */

void
log_object_message_BM (const objectval_tyBM * obj)
{
#ifdef BISMONGTK
  extern void gtk_log_object_message_BM (const objectval_tyBM *);
#endif
#ifdef BISMONION
  extern void onion_log_object_message_BM (const objectval_tyBM *);
#endif
#ifdef BISMONGTK
  if (gui_is_running_BM)
    {
      gtk_log_object_message_BM (obj);
      return;
    };
#endif /*BISMONGTK*/
#ifdef BISMONION
    if (web_is_running_BM)
    {
      onion_log_object_message_BM (obj);
      return;
    }
#endif
  FATAL_BM ("log_object_message_BM without web or GUI for %s",
            objectdbg_BM (obj));
}                               /* end log_object_message_BM */

void
log_printf_message_BM (const char *fmt, ...)
{
  char smallbuf[64];
  memset (smallbuf, 0, sizeof (smallbuf));
  va_list args;
  char *buf = smallbuf;
  va_start (args, fmt);
  int ln = vsnprintf (smallbuf, sizeof (smallbuf), fmt, args);
  va_end (args);
  if (ln >= (int) sizeof (smallbuf) - 1)
    {
      buf = calloc ((prime_above_BM (ln + 2) | 7) + 1, 1);
      if (!buf)
        FATAL_BM ("failed to calloc for %d bytes (%m)", ln);
      va_start (args, fmt);
      vsnprintf (buf, ln + 1, fmt, args);
      va_end (args);
    }
  log_puts_message_BM (buf);
  if (buf != smallbuf)
    free (buf);
}                               /* end log_printf_message_BM */

/*** end of file main_BM.c ***/
