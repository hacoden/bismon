// file parser_BM.c
#include "bismon.h"

#define RUNLEN_BM 16
#define RUNFMT_BM "%16[A-Za-z0-9]"


static const char *const lexkindnamearr_BM[] = {
  [plex__NONE] = "_NONE",
  [plex_LLONG] = "LLONG",
  [plex_DOUBLE] = "DOUBLE",
  [plex_DELIM] = "DELIM",
  [plex_ID] = "ID",
  [plex_CNAME] = "CNAME",
  [plex_WORD] = "WORD",
  [plex_STRING] = "STRING",
};

const char *
lexkindname_BM (enum parslexkind_enBM k)
{
  if ((unsigned) k <
      (unsigned) (sizeof (lexkindnamearr_BM) / sizeof (lexkindnamearr_BM[0])))
    return lexkindnamearr_BM[k];
  return "?";
}                               /* end of lexkindname_BM */

static const char *const delimnamearr_BM[] = {
#define HAS_DELIM_BM(Str,Delimname) [delim_##Delimname] = Str,
#include "_bm_delim.h"
};

const char *
delimstr_BM (enum lexdelim_enBM d)
{
  if ((unsigned) d <
      (unsigned) (sizeof (delimnamearr_BM) / sizeof (delimnamearr_BM[0])))
    return delimnamearr_BM[d];
  return "?*bad-delim*?";
}                               /* end of delimstr_BM */


struct parser_stBM *
makeparser_of_file_BM (FILE * f)
{
  if (!f)
    return NULL;
  struct parser_stBM *pars =    //
    allocgcty_BM (typayl_parser_BM, sizeof (struct parser_stBM));
  pars->pars_file = f;
  pars->pars_debug = false;
  pars->pars_filemem = NULL;
  pars->pars_filesize = 0;
  pars->pars_path = "";
  unsigned inilinsiz = 256;
  char *linebuf = calloc (1, inilinsiz);
  if (!linebuf)
    FATAL_BM ("linebuf calloc failed %d bytes (%m)", inilinsiz);
  pars->pars_linebuf = linebuf;
  pars->pars_linesiz = inilinsiz;
  pars->pars_linebuf[0] = (char) 0;
  pars->pars_linelen = getline (&pars->pars_linebuf, &pars->pars_linesiz, f);
  if (pars->pars_linelen < 0)
    {
      pars->pars_lineno = 0;
      pars->pars_curbyte = NULL;
    }
  else
    pars->pars_lineno = 1;
  if (!g_utf8_validate (pars->pars_linebuf, pars->pars_linelen, NULL))
    FATAL_BM ("invalid UTF8 line %s:%d", pars->pars_path, pars->pars_lineno);
  pars->pars_curbyte = pars->pars_linebuf;
  pars->pars_colpos = 0;
  unsigned inimemosiz = 32;
  pars->pars_memolines =
    calloc (inimemosiz, sizeof (struct memolineoffset_stBM));
  if (!pars->pars_memolines)
    FATAL_BM ("memolines calloc failed for size %d (%m)", inimemosiz);
  pars->pars_memolsize = inimemosiz;
  pars->pars_memolcount = 0;
  return pars;
}                               /* end makeparser_of_file_BM */

struct parser_stBM *
makeparser_memopen_BM (const char *filemem, long size)
{
  if (!filemem)
    return NULL;
  if (size < 0)
    size = strlen (filemem);
  FILE *fil = fmemopen ((void *) filemem, size, "r");
  if (!fil)
    FATAL_BM ("fmemopen failed (%m)");
  struct parser_stBM *pars = makeparser_of_file_BM (fil);
  pars->pars_filemem = filemem;
  pars->pars_filesize = size;
  return pars;
}                               /* end makeparser_memopen_BM */


void
parsergcmark_BM (struct garbcoll_stBM *gc, struct parser_stBM *pars)
{
  assert (gc && gc->gc_magic == GCMAGIC_BM);
  assert (isparser_BM (pars));
  uint8_t oldmark = ((typedhead_tyBM *) pars)->hgc;
  if (oldmark)
    return;
  ((typedhead_tyBM *) pars)->hgc = MARKGC_BM;
  gc->gc_nbmarks++;
  if (pars->pars_cvalue)
    VALUEGCPROC_BM (gc, pars->pars_cvalue, 0);
}                               /* end parsergcmark_BM */


void
parsergcdestroy_BM (struct garbcoll_stBM *gc, struct parser_stBM *pars)
{
  unsigned long parsiz = sizeof (*pars);
  assert (gc && gc->gc_magic == GCMAGIC_BM);
  if (pars->pars_filemem)
    {
      pars->pars_filemem = NULL;
      parsiz += pars->pars_filesize;
      pars->pars_filesize = 0;
    }
  if (pars->pars_linebuf)
    {
      parsiz += pars->pars_linesiz;
      free (pars->pars_linebuf), pars->pars_linebuf = NULL;
    }
  if (pars->pars_file)
    {
      parsiz += sizeof (FILE);
      fclose (pars->pars_file), pars->pars_file = NULL;
    }
  if (pars->pars_memolines)
    {
      parsiz += pars->pars_memolsize * sizeof (struct memolineoffset_stBM);
      free (pars->pars_memolines), pars->pars_memolines = NULL;
    }
  memset (pars, 0, sizeof (*pars));
  free (pars);
  gc->gc_freedbytes += parsiz;
}                               /* end parsergcdestroy_BM */


void
parsergckeep_BM (struct garbcoll_stBM *gc, struct parser_stBM *pars)
{
  assert (gc && gc->gc_magic == GCMAGIC_BM);
  unsigned long parsiz = sizeof (*pars);
  if (pars->pars_filemem)
    parsiz += pars->pars_filesize;
  if (pars->pars_linebuf)
    parsiz += pars->pars_linesiz;
  if (pars->pars_file)
    parsiz += sizeof (FILE);
  assert (parsiz < (4L * MAXSIZE_BM / 3 + 5L) * sizeof (void *));
  gc->gc_keptbytes += parsiz;
}                               /* end parsergckeep_BM */


bool
parsernextline_BM (struct parser_stBM *pars)
{
  if (!isparser_BM (pars))
    return false;
  long curoff = ftell (pars->pars_file);
  pars->pars_linelen =
    getline (&pars->pars_linebuf, &pars->pars_linesiz, pars->pars_file);
  if (pars->pars_linelen > 0)
    {
      pars->pars_curbyte = pars->pars_linebuf;
      pars->pars_colpos = 0;
      pars->pars_linebuf[pars->pars_linelen] = (char) 0;
      if ((pars->pars_lineno + 1) % PARSERMEMOLINERATIO_BM == 0)
        {
          unsigned curmemlcount = pars->pars_memolcount;
          if (curmemlcount + 1 >= pars->pars_memolsize)
            {
              unsigned newmemosize =
                prime_above_BM ((4 * curmemlcount) / 3 + 10);
              struct memolineoffset_stBM *newmemolines =        //
                calloc (newmemosize, sizeof (struct memolineoffset_stBM));
              if (!newmemolines)
                FATAL_BM ("calloc failed for newmemosize=%u", newmemosize);
              memcpy (newmemolines, pars->pars_memolines,       //
                      (pars->pars_memolcount) *
                      sizeof (struct memolineoffset_stBM));
              free (pars->pars_memolines);
              pars->pars_memolines = newmemolines;
              pars->pars_memolsize = newmemosize;
            };
          struct memolineoffset_stBM *curmemoline =
            pars->pars_memolines + curmemlcount;
          curmemoline->memli_off = curoff;
          curmemoline->memli_lineno = pars->pars_lineno;
          curmemoline->memli_linebylen = (unsigned) pars->pars_linelen;
          pars->pars_memolcount = curmemlcount + 1;
        }
      pars->pars_lineno++;
      if (!g_utf8_validate (pars->pars_linebuf, pars->pars_linelen, NULL))
        FATAL_BM ("invalid UTF8 line %s:%d", pars->pars_path,
                  pars->pars_lineno);
      return true;
    }
  else if (pars->pars_linelen < 0)
    {
      pars->pars_curbyte = NULL;
      pars->pars_colpos = 0;
    }
  return false;
}                               /* end parsernextline_BM */

void
parserseek_BM (struct parser_stBM *pars, unsigned lineno, unsigned colpos)
{
  if (!isparser_BM (pars))
    return;
  if (lineno != pars->pars_lineno)
    {
      unsigned memolcnt = pars->pars_memolcount;
      unsigned lo = 0, hi = memolcnt;
      struct memolineoffset_stBM *memolines = pars->pars_memolines;
      while (lo + 8 < hi)
        {
          unsigned md = (lo + hi) / 2;
          if (memolines[md].memli_lineno < (int) lineno)
            lo = md;
          else
            hi = md;
        }
      long startoff = 0;
      int startlineno = 0;
      int ix = 0;
      for (ix = (int) (hi - 1); ix >= (int) lo; ix--)
        {
          if (memolines[ix].memli_lineno <= (int) lineno)
            {
              startoff = memolines[ix].memli_off;
              startlineno = memolines[ix].memli_lineno;
              break;
            }
        }
      if (fseek (pars->pars_file, SEEK_SET, startoff) < 0)
        FATAL_BM ("fseek startoff=%ld failure", startoff);
      pars->pars_lineno = startlineno;
      do
        {
          pars->pars_linelen =
            getline (&pars->pars_linebuf, &pars->pars_linesiz,
                     pars->pars_file);
          if (pars->pars_linelen < 0)
            FATAL_BM ("getline failed line#%d", pars->pars_lineno);
          if (pars->pars_lineno == lineno)
            break;
          pars->pars_lineno++;
        }
      while (lineno < pars->pars_lineno);
    }
  else
    {                           //lineno == pars->pars_lineno
      pars->pars_curbyte = pars->pars_linebuf;
      pars->pars_colpos = 0;
    }
  if (!g_utf8_validate (pars->pars_linebuf, pars->pars_linelen, NULL))
    FATAL_BM ("invalid UTF8 line %s:%d", pars->pars_path, pars->pars_lineno);
  while (pars->pars_colpos < colpos)
    {
      if (!pars->pars_curbyte
          || *pars->pars_curbyte == (char) 0
          || pars->pars_curbyte >= pars->pars_linebuf + pars->pars_linelen)
        break;
      const char *pc = pars->pars_curbyte;
      if (!*pc)
        break;
      const char *nextpc = g_utf8_next_char (pc);
      pars->pars_curbyte = nextpc;
      pars->pars_colpos++;
    }
}                               /* end of parserseek_BM */

void
parsererrorprintf_BM (struct parser_stBM *pars, unsigned line, unsigned col,
                      const char *fmt, ...)
{
  if (!isparser_BM (pars))
    FATAL_BM ("non parser argument to parsererrorprintf_BM");
  va_list args;
  char *buf = NULL;
  va_start (args, fmt);
  vasprintf (&buf, fmt, args);
  if (!buf)
    FATAL_BM ("vasprintf failed");
  va_end (args);
  const struct parserops_stBM *parsops = pars->pars_ops;
  if (parsops)
    {
      assert (parsops->parsop_magic == PARSOPMAGIC_BM);
      if (parsops && parsops->parsop_error_rout)
        parsops->parsop_error_rout (pars, line, col, buf);
    };
  FATAL_BM ("%s:%d:%d: PARSER ERROR : %s", pars->pars_path, line, col, buf);
}                               /* end parsererrorprintf_BM */




void
parserskipspaces_BM (struct parser_stBM *pars)
{
  if (!isparser_BM (pars))
    return;
  const struct parserops_stBM *parsops = pars->pars_ops;
  assert (!parsops || parsops->parsop_magic == PARSOPMAGIC_BM);
  for (;;)
    {
      const char *restlines = parserrestline_BM (pars);
      if (!restlines)
        break;
      if (isspace (restlines[0]))
        {
          pars->pars_colpos++, pars->pars_curbyte++;
          continue;
        }
      if (restlines[0] == (char) 0)
        {
          if (!parsernextline_BM (pars))
            return;
          continue;
        }
      if (restlines[0] == '/' && restlines[1] == '/')
        {
          if (parsops && parsops->parsop_decorate_comment_sign_rout)
            parsops->parsop_decorate_comment_sign_rout  //
              (pars, pars->pars_lineno, pars->pars_colpos, 2);
          if (parsops && parsops->parsop_decorate_comment_inside_rout)
            parsops->parsop_decorate_comment_inside_rout        //
              (pars, pars->pars_lineno, pars->pars_colpos + 2,
               g_utf8_strlen (restlines + 2, -1));
          if (!parsernextline_BM (pars))
            return;
          continue;
        }
      else if (restlines[0] == '|')
        {
          unsigned curlineno = parserlineno_BM (pars);
          unsigned curcol = parsercolpos_BM (pars);
          if (parsops && parsops->parsop_decorate_comment_sign_rout)
            parsops->parsop_decorate_comment_sign_rout  //
              (pars, curlineno, curcol, 1);
          for (;;)
            {
              if (!parseradvanceutf8_BM (pars, 1) || parsereol_BM (pars)
                  || parserendoffile_BM (pars))
                {
                  char begcomment[40] = { };
                  const gchar *endcomm = NULL;
                  strncpy (begcomment, restlines, sizeof (begcomment) - 1);
                  g_utf8_validate (begcomment, -1, &endcomm);
                  *(char *) endcomm = 0;
                  parsererrorprintf_BM (pars, curlineno, curcol,
                                        "unterminated comment %s",
                                        begcomment);
                }
              if (parserunichar_BM (pars) == (gunichar) '|')
                {
                  if (parsops && parsops->parsop_decorate_comment_sign_rout)
                    parsops->parsop_decorate_comment_sign_rout  //
                      (pars, pars->pars_lineno, pars->pars_colpos, 1);
                  if (parsops && parsops->parsop_decorate_comment_inside_rout)
                    parsops->parsop_decorate_comment_inside_rout        //
                      (pars, pars->pars_lineno, curcol + 1,
                       pars->pars_colpos - curcol);
                  parseradvanceutf8_BM (pars, 1);
                  break;
                }
            }
        }
      else
        return;
    }
}                               /* end parserskipspaces_BM */



void
gctokenmark_BM (struct garbcoll_stBM *gc, struct parstoken_stBM *tok)
{
  assert (gc && gc->gc_magic == GCMAGIC_BM);
  assert (tok != NULL);
  switch (tok->tok_kind)
    {
    case plex__NONE:
      FATAL_BM ("empty token @%p", (void *) tok);
    case plex_LLONG:
    case plex_DOUBLE:
    case plex_DELIM:
    case plex_ID:
      break;
    case plex_CNAME:
      VALUEGCPROC_BM (gc, tok->tok_cname, 0);
      break;
    case plex_WORD:
      VALUEGCPROC_BM (gc, tok->tok_word, 0);
      break;
    case plex_STRING:
      VALUEGCPROC_BM (gc, tok->tok_string, 0);
      break;
    case plex_NAMEDOBJ:
      gcobjmark_BM (gc, tok->tok_namedobj);
      break;
    };
}                               /* end gctokenmark_BM */


// return the number of Unicode chars in the plain cord
static unsigned
parse_plain_cord_BM (struct parser_stBM *pars, FILE * memfil)
{
  assert (isparser_BM (pars));
  assert (memfil != NULL);
  const struct parserops_stBM *parsop = pars->pars_ops;
  const char *restlin = parserrestline_BM (pars);
  assert (restlin && *restlin == '"');
  if (pars->pars_debug)
    DBGPRINTF_BM ("parseplaincord start restlin@%p:%s", restlin, restlin);
  assert (!parsop || parsop->parsop_magic == PARSOPMAGIC_BM);
  if (parsop && parsop->parsop_decorate_string_sign_rout)
    parsop->parsop_decorate_string_sign_rout (pars, pars->pars_lineno,
                                              pars->pars_colpos, 1);
  const char *pc = restlin + 1;
  const char *startplain = restlin + 1;
  unsigned nbc = 0;
  while (*pc)
    {
      if (pars->pars_debug)
        DBGPRINTF_BM ("parseplaincord pc@%p:%s", pc, pc);
      if (*pc == '"')
        break;
      else if (*pc == '\n')
        break;
      else if (*pc == '\\')
        {
          if (parsop && parsop->parsop_decorate_string_inside_rout
              && startplain < pc)
            {
              if (pars->pars_debug)
                DBGPRINTF_BM ("parseplaincord stringinside L%dC%d w%d",
                              pars->pars_lineno,
                              pars->pars_colpos + g_utf8_strlen (restlin,
                                                                 pc -
                                                                 restlin),
                              g_utf8_strlen (startplain,
                                             pc - startplain - 1));
              parsop->parsop_decorate_string_inside_rout        //
                (pars,
                 pars->pars_lineno,
                 pars->pars_colpos + g_utf8_strlen (restlin, pc - restlin),
                 g_utf8_strlen (startplain, pc - startplain - 1));
            }
          const char *oldpc = pc;
          char nc = pc[1];
          int pos = -1;
          int b = 0;
          switch (nc)
            {
            case '\'':
            case '\"':
            case '\\':
              b = nc;
              fputc (nc, memfil);
              nbc++;
              pc += 2;
              startplain = pc;
              continue;
            case 'a':
              b = '\a';
              fputc (b, memfil);
              nbc++;
              pc += 2;
              startplain = pc;
              break;
            case 'b':
              b = '\b';
              fputc (b, memfil);
              nbc++;
              pc += 2;
              startplain = pc;
              break;
            case 'f':
              b = '\f';
              fputc (b, memfil);
              nbc++;
              pc += 2;
              startplain = pc;
              break;
            case 'n':
              b = '\n';
              fputc (b, memfil);
              nbc++;
              pc += 2;
              startplain = pc;
              break;
            case 'r':
              b = '\r';
              fputc (b, memfil);
              nbc++;
              pc += 2;
              startplain = pc;
              break;
            case 't':
              b = '\t';
              fputc (b, memfil);
              nbc++;
              pc += 2;
              startplain = pc;
              break;
            case 'v':
              b = '\v';
              fputc (b, memfil);
              nbc++;
              pc += 2;
              startplain = pc;
              break;
            case 'e':
              b = '\033' /*ESCAPE*/;
              fputc (b, memfil);;
              break;
            case 'x':
              if (sscanf (pc + 1, "%02x%n", &b, &pos) > 0 && pos == 2
                  && b > 0 && b <= 127)
                {
                  fputc (b, memfil);
                  pc += 3;
                  nbc++;
                  startplain = pc;
                  break;
                }
              else
                parsererrorprintf_BM (pars, pars->pars_lineno,
                                      pars->pars_colpos, "bad hex escape %s",
                                      pc);
            case 'o':
              if (sscanf (pc + 1, "%03o%n", &b, &pos) > 0 && pos == 3
                  && b > 0 && b <= 127)
                {
                  fputc (b, memfil);
                  pc += 3;
                  nbc++;
                  startplain = pc;
                  break;
                }
              else
                parsererrorprintf_BM (pars, pars->pars_lineno,
                                      pars->pars_colpos,
                                      "bad octal escape %s", pc);
            case 'u':
              if (sscanf (pc + 2, "%04x%n", &b, &pos) > 0 && pos == 4
                  && b > 0)
                {
                  char ebuf[8];
                  memset (ebuf, 0, sizeof (ebuf));
                  g_unichar_to_utf8 ((gunichar) b, ebuf);
                  fputs (ebuf, memfil);
                  nbc++;
                  pc += 6;
                  startplain = pc;
                  break;
                }
              else
                parsererrorprintf_BM (pars, pars->pars_lineno,
                                      pars->pars_colpos,
                                      "bad unicode4 escape %s", pc);
            case 'U':
              if (sscanf (pc + 2, "%08x%n", &b, &pos) > 0 && pos == 8
                  && b > 0)
                {
                  char ebuf[8];
                  memset (ebuf, 0, sizeof (ebuf));
                  g_unichar_to_utf8 ((gunichar) b, ebuf);
                  fputs (ebuf, memfil);
                  nbc++;
                  pc += 9;
                  startplain = pc;
                  break;
                }
              else
                parsererrorprintf_BM (pars, pars->pars_lineno,
                                      pars->pars_colpos,
                                      "bad unicode8 escape %s", pc);
            case '\0':
              parsererrorprintf_BM (pars, pars->pars_lineno,
                                    pars->pars_colpos, "bad null escape %s",
                                    pc);

            default:
              b = nc;
              fputc (b, memfil);
              nbc++;
              pc += 2;
              startplain = pc;
              break;
            }                   /* end switch nc */
          if (pars->pars_debug)
            DBGPRINTF_BM ("parseplaincord stringsign L%dC%d w%d",
                          pars->pars_lineno,
                          pars->pars_colpos + g_utf8_strlen (restlin,
                                                             oldpc - restlin),
                          g_utf8_strlen (oldpc, pc - oldpc));
          if (b && parsop && parsop->parsop_decorate_string_sign_rout)
            parsop->parsop_decorate_string_sign_rout    //
              (pars,
               pars->pars_lineno,
               pars->pars_colpos + g_utf8_strlen (restlin, oldpc - restlin),
               g_utf8_strlen (oldpc, pc - oldpc));
        }
      else if (*pc >= ' ' && *pc < 127)
        {                       // ASCII char
          fputc (*pc, memfil);
          nbc++;
          pc++;
          continue;
        }
      else
        {                       // probably some multibyte UTF8 char
          const char *npc = g_utf8_next_char (pc);
          fwrite (pc, npc - pc, 1, memfil);
          nbc++;
          pc = npc;
          continue;
        }
    };
  if (*pc == '"')
    {
      if (pars->pars_debug)
        DBGPRINTF_BM ("parseplaincord stringinside endquot L%dC%d w%d",
                      pars->pars_lineno,
                      pars->pars_colpos + g_utf8_strlen (restlin,
                                                         pc - restlin),
                      g_utf8_strlen (startplain, pc - startplain));
      if (parsop && parsop->parsop_decorate_string_inside_rout
          && startplain < pc)
        {
          parsop->parsop_decorate_string_inside_rout    //
            (pars,
             pars->pars_lineno,
             pars->pars_colpos + g_utf8_strlen (restlin, pc - restlin),
             g_utf8_strlen (startplain, pc - startplain));
        }
      if (pars->pars_debug)
        DBGPRINTF_BM ("parseplaincord stringsign endquot L%dC%d w%d",
                      pars->pars_lineno,
                      pars->pars_colpos + g_utf8_strlen (restlin,
                                                         pc - restlin), 1);
      if (parsop && parsop->parsop_decorate_string_sign_rout)
        parsop->parsop_decorate_string_sign_rout
          (pars,
           pars->pars_lineno,
           pars->pars_colpos + g_utf8_strlen (restlin, pc - restlin), 1);
      pc++;
    }
  else
    parsererrorprintf_BM (pars, pars->pars_lineno, pars->pars_colpos,
                          "bad plain cord ending %s", pc);
  pars->pars_colpos += g_utf8_strlen (restlin, pc - restlin);
  pars->pars_curbyte = pc;
  if (pars->pars_debug)
    DBGPRINTF_BM ("parsepaincord final L%dC%d", pars->pars_lineno,
                  pars->pars_colpos);
  return nbc;
}                               /* end parse_plain_cord_BM */



// return the number of Unicode chars in the raw cord
static unsigned
parse_raw_cord_BM (struct parser_stBM *pars, const char *run, FILE * memfil)
{
  assert (isparser_BM (pars));
  assert (memfil != NULL);
  char endrunbuf[RUNLEN_BM + 8] = "";
  unsigned runlen = strlen (run);
  assert (runlen <= RUNLEN_BM);
  unsigned nbc = 0;
  const char *restlin = parserrestline_BM (pars);
  assert (restlin && restlin[0] == '/' && restlin[1] == '"'
          && !strncmp (restlin + 2, run, runlen)
          && restlin[runlen + 3] == '(');
  const char *curstart = restlin + runlen + 4;
  snprintf (endrunbuf, sizeof (endrunbuf), ")%s\"/", run);
  for (;;)
    {
      char *ends = strstr (curstart, endrunbuf);
      if (ends)
        {
          fwrite (curstart, ends - curstart, 1, memfil);
          unsigned plen = g_utf8_strlen (curstart, ends - curstart);
          nbc += plen;
          pars->pars_curbyte += (ends - curstart) + strlen (endrunbuf);
          pars->pars_colpos += plen + strlen (endrunbuf);
          break;
        }
      else
        {
          fputs (curstart, memfil);
          unsigned plen = g_utf8_strlen (curstart, -1);
          pars->pars_curbyte += strlen (curstart);
          pars->pars_colpos += plen;
          if (!parsernextline_BM (pars))
            parsererrorprintf_BM (pars, pars->pars_lineno, pars->pars_colpos,
                                  "unterminated raw cord");
          restlin = parserrestline_BM (pars);
          curstart = restlin;
          assert (restlin != NULL);
          continue;
        }
    };
  return nbc;
}                               /* end parse_raw_cord_BM */


static const stringval_tyBM *
parse_cords_BM (struct parser_stBM *pars)
{
  if (!isparser_BM ((const value_tyBM) pars))
    return NULL;
  const struct parserops_stBM *parsops = pars->pars_ops;
  assert (!parsops || parsops->parsop_magic == PARSOPMAGIC_BM);
  bool nobuild = parsops && parsops->parsop_nobuild;
  parserskipspaces_BM (pars);
  const char *restlin = parserrestline_BM (pars);
  if (!restlin)
    return NULL;
  if (restlin[0] != '"' && (restlin[0] != '/' && restlin[1] != '"'))
    return NULL;
  char runbuf[RUNLEN_BM + 8] = "";
  int runlen = -1;
  size_t bufsiz = 256;
  char *cbuf = malloc (bufsiz);
  if (!cbuf)
    FATAL_BM ("malloc for cord of %ld failed (%m)", (long) bufsiz);
  memset (cbuf, 0, bufsiz);
  FILE *filmem = open_memstream (&cbuf, &bufsiz);
  if (!filmem)
    FATAL_BM ("open_memstream failed (%m)");
  bool againcord = false;
  unsigned long cumulchars = 0;
  do
    {
      againcord = false;
      restlin = parserrestline_BM (pars);
      if (!restlin)
        break;
      if (restlin[0] == '"')
        cumulchars += parse_plain_cord_BM (pars, filmem);
      else if (restlin[0] == '/' && restlin[1] == '"'
               && ((runlen = -1), (memset (runbuf, 0, sizeof (runbuf))),
                   sscanf (restlin, "/\"" RUNFMT_BM "(%n", runbuf,
                           &runlen) > 0) && runlen > 0)
        cumulchars += parse_raw_cord_BM (pars, runbuf, filmem);
      else
        {
          fclose (filmem);
          free (cbuf), cbuf = NULL;
          parsererrorprintf_BM (pars, pars->pars_lineno, pars->pars_colpos,
                                "bad cord %s", restlin);
        }

      restlin = parserrestline_BM (pars);
      if (!restlin)
        break;
      if (restlin[0] == '&')
        {
          pars->pars_colpos++;
          pars->pars_curbyte++;
          againcord = true;
        }
      else if (restlin[0] == '+')
        {
          pars->pars_colpos++;
          pars->pars_curbyte++;
          fputc ('\n', filmem);
          cumulchars++;
          againcord = true;
        }
      else
        break;
      parserskipspaces_BM (pars);
      if (cumulchars >= MAXSIZE_BM)
        {
          fclose (filmem);
          free (cbuf);
          parsererrorprintf_BM (pars, pars->pars_lineno, pars->pars_colpos,
                                "too long string (%lu)", cumulchars);
        }
    }
  while (againcord);
  fputc ((char) 0, filmem);
  fflush (filmem);
  const stringval_tyBM *str = nobuild ? NULL : (makestring_BM (cbuf));
  fclose (filmem);
  free (cbuf), cbuf = NULL;
  return str;
}                               /* end parse_cords_BM */


parstoken_tyBM
parsertokenget_BM (struct parser_stBM * pars)
{
  if (!isparser_BM ((const value_tyBM) pars))
    return EMPTY_TOKEN_BM;
  const struct parserops_stBM *parsop = pars->pars_ops;
  assert (!parsop || parsop->parsop_magic == PARSOPMAGIC_BM);
  bool nobuild = parsop && parsop->parsop_nobuild;
  const char *restlin = NULL;
again:
  parserskipspaces_BM (pars);
  restlin = parserrestline_BM (pars);
  if (restlin && restlin[0] == (char) 0)
    {
      if (parserendoffile_BM (pars))
        return EMPTY_TOKEN_BM;
      goto again;
    }
  if (!restlin)
    return EMPTY_TOKEN_BM;
  int curlin = parserlineno_BM (pars);
  int curcol = parsercolpos_BM (pars);
  if (isdigit (restlin[0])
      || ((restlin[0] == '+' || restlin[0] == '-') && isdigit (restlin[1])))
    {
      char *endint = NULL;
      char *endflo = NULL;
      long long ll = strtoll (restlin, &endint, 0);
      if (strchr (restlin, '.'))
        {
          double x = strtod (restlin, &endflo);
          if (endflo > endint)
            {
              unsigned coldbl = g_utf8_strlen (restlin, endflo - restlin);
              if (parsop && parsop->parsop_decorate_number_rout)
                parsop->parsop_decorate_number_rout     //
                  (pars, pars->pars_lineno, pars->pars_colpos, coldbl);
              pars->pars_curbyte = endflo;
              pars->pars_colpos += coldbl;
              return (parstoken_tyBM)
              {
              .tok_kind = plex_DOUBLE,.tok_line = curlin,.tok_col =
                  curcol,.tok_dbl = x};
            }
        }
      if (endint > restlin)
        {
          unsigned colint = g_utf8_strlen (restlin, endint - restlin);
          if (parsop && parsop->parsop_decorate_number_rout)
            parsop->parsop_decorate_number_rout //
              (pars, pars->pars_lineno, pars->pars_colpos, colint);
          pars->pars_curbyte = endint;
          pars->pars_colpos += colint;
          return (parstoken_tyBM)
          {
          .tok_kind = plex_LLONG,.tok_line = curlin,.tok_col =
              curcol,.tok_llong = ll};
        }
    }

  // parse ids
  else if (restlin[0] == '_' && isdigit (restlin[1]))
    {
      const char *endid = NULL;
      rawid_tyBM id = parse_rawid_BM (restlin, &endid);
      if (id.id_hi != 0 && endid != NULL && endid > restlin)
        {
          assert (endid == restlin + IDLEN_BM);
          if (parsop && parsop->parsop_decorate_id_rout)
            parsop->parsop_decorate_id_rout     //
              (pars, pars->pars_lineno, pars->pars_colpos, IDLEN_BM);
          pars->pars_curbyte += IDLEN_BM;
          pars->pars_colpos += IDLEN_BM;
          return (parstoken_tyBM)
          {
          .tok_kind = plex_ID,.tok_line = curlin,.tok_col = curcol,.tok_id =
              id};
        }
      else
        parsererrorprintf_BM (pars, pars->pars_lineno, pars->pars_colpos,
                              "bad ident %s", restlin);
    }

  // parse nil id
  else if (restlin[0] == '_' && restlin[1] == '_'
           && !isalnum (restlin[2]) && restlin[2] != '_')
    {
      if (parsop && parsop->parsop_decorate_id_rout)
        parsop->parsop_decorate_id_rout //
          (pars, pars->pars_lineno, pars->pars_colpos, 2);
      pars->pars_curbyte += 2;
      pars->pars_colpos += 2;
      return (parstoken_tyBM)
      {
        .tok_kind = plex_ID,.tok_line = curlin,.tok_col = curcol,.tok_id =
        {
        0, 0}
      };
    }

  // parse names, either as named objects or as strings
  else if (isalpha (restlin[0]))
    {
      const char *endnam = restlin;
      while (isalnum (*endnam) || *endnam == '_')
        endnam++;
      unsigned namlen = endnam - restlin;
      char tinynambuf[TINYSIZE_BM] = "";
      char *nambuf =            //
        (namlen < TINYSIZE_BM)  //
        ? tinynambuf            //
        : malloc (prime_above_BM ((namlen) / 4 + 2) * 4);
      if (!namlen)
        FATAL_BM ("failed to malloc name for %d bytes", namlen);
      memcpy (nambuf, restlin, namlen);
      memcpy (tinynambuf, restlin,
              (namlen < TINYSIZE_BM) ? namlen : TINYSIZE_BM);
      nambuf[namlen] = (char) 0;
      if (!validname_BM (nambuf))
        {
          if (nambuf != tinynambuf)     // to avoid memory leaks
            free (nambuf), nambuf = NULL;
          parsererrorprintf_BM (pars, pars->pars_lineno, pars->pars_colpos,
                                "bad ident %s%s", tinynambuf,
                                (namlen > TINYSIZE_BM) ? "..." : "");
        }
      const objectval_tyBM *namedobj =  //
        findnamedobj_BM (nambuf);
      if (namedobj)
        {
          if (nambuf != tinynambuf)
            free (nambuf), nambuf = NULL;
          if (parsop && parsop->parsop_decorate_known_name_rout)
            parsop->parsop_decorate_known_name_rout     //
              (pars, curlin, pars->pars_colpos, namlen);
          pars->pars_curbyte += namlen;
          pars->pars_colpos += namlen;
          return (parstoken_tyBM)
          {
          .tok_kind = plex_NAMEDOBJ,.tok_line = curlin,.tok_col =
              curcol,.tok_namedobj = (objectval_tyBM *) namedobj};
        }
      else
        {                       // new name
          const stringval_tyBM *newnam =        //
            nobuild ? NULL : makestring_BM (nambuf);
          if (nambuf != tinynambuf)
            free (nambuf), nambuf = NULL;
          if (parsop && parsop->parsop_decorate_new_name_rout)
            parsop->parsop_decorate_new_name_rout       //
              (pars, curlin, pars->pars_colpos, namlen);
          pars->pars_curbyte += namlen;
          pars->pars_colpos += namlen;
          return (parstoken_tyBM)
          {
          .tok_kind = plex_CNAME,.tok_line = curlin,.tok_col =
              curcol,.tok_cname = newnam};
        }
    }

  // special case for +NAN +INF -INF, in uppercases
  else if (((restlin[0] == '+') || (restlin[0] == '-'))
           && (restlin[1] == 'N' || restlin[1] == 'I')
           && (!strncmp (restlin, "+NAN", 4)
               || !strncmp (restlin, "+INF", 4)
               || !strncmp (restlin, "-INF", 4)) && !isalnum (restlin[4]))
    {
      char specbuf[8] = { };
      memcpy (specbuf, restlin, 4);
      double x = strtod (specbuf, NULL);
      if (parsop && parsop->parsop_decorate_number_rout)
        parsop->parsop_decorate_number_rout (pars, curlin, pars->pars_colpos,
                                             4);
      pars->pars_curbyte += 4;
      pars->pars_colpos += 4;
      return (parstoken_tyBM)
      {
      .tok_kind = plex_DOUBLE,.tok_line = curlin,.tok_col =
          curcol,.tok_dbl = x};
    }

  else if (restlin[0] == '"')
    {
      const stringval_tyBM *str = parse_cords_BM (pars);
      return (parstoken_tyBM)
      {
      .tok_kind = plex_STRING,.tok_line = curlin,.tok_col =
          curcol,.tok_string = str};
    }

  else if (restlin[0] == '/' && restlin[1] == '"')
    {
      const stringval_tyBM *str = parse_cords_BM (pars);
      return (parstoken_tyBM)
      {
      .tok_kind = plex_STRING,.tok_line = curlin,.tok_col =
          curcol,.tok_string = str};
    }
  // special case for $<var> the dollar should be immediately followed
  // by a letter
  else if (restlin[0] == '$' && isalpha (restlin[1]))
    {

      pars->pars_curbyte += 1;
      pars->pars_colpos += 1;
      return (parstoken_tyBM)
      {
      .tok_kind = plex_DELIM,.tok_line = curlin,.tok_col =
          curcol,.tok_delim = delim_dollar};
    }
  // special case for $:<var> the colon should be immediately followed
  // by a letter
  else if (restlin[0] == '$' && restlin[1] == ':' && isalpha (restlin[2]))
    {
      pars->pars_curbyte += 2;
      pars->pars_colpos += 2;
      return (parstoken_tyBM)
      {
      .tok_kind = plex_DELIM,.tok_line = curlin,.tok_col =
          curcol,.tok_delim = delim_dollarcolon};
    }
  char delimstr[16];
  memset (delimstr, 0, sizeof (delimstr));
  enum lexdelim_enBM curdelim = delim__NONE;
#define HAS_DELIM_BM(Str,Nam) do {		\
    if (restlin[0] == (Str)[0]			\
	&& !strncmp(restlin, Str, strlen(Str))	\
	&& strlen(delimstr) < strlen(Str)) {	\
    curdelim = delim_##Nam;			\
    strcpy(delimstr, Str);			\
    }}  while(0);
  //
#include "_bm_delim.h"
  //
  if (curdelim == delim__NONE)
    parsererrorprintf_BM (pars, pars->pars_lineno, pars->pars_colpos,
                          "unexpected token %s", restlin);
  else if (curdelim == delim_dollar)
    parsererrorprintf_BM (pars, pars->pars_lineno, pars->pars_colpos,
                          "dollar not immediately followed by letter %s",
                          restlin);
  else if (curdelim == delim_dollarcolon)
    parsererrorprintf_BM (pars, pars->pars_lineno, pars->pars_colpos,
                          "dollar-colon not immediately followed by letter %s",
                          restlin);

  pars->pars_curbyte += strlen (delimstr);
  pars->pars_colpos += g_utf8_strlen (delimstr, -1);
  return (parstoken_tyBM)
  {
  .tok_kind = plex_DELIM,.tok_line = curlin,.tok_col = curcol,.tok_delim =
      curdelim};
}                               /* end parsertokenget_BM */



objectval_tyBM *
parsergetobject_BM (struct parser_stBM * pars,
                    struct stackframe_stBM * prevstkf, int depth,
                    bool * pgotobj)
{
  if (!isparser_BM ((const value_tyBM) pars))
    FATAL_BM ("bad parser");
  if (!pgotobj)
    FATAL_BM ("missing pgotobj");
  LOCALFRAME_BM (prevstkf, NULL, const objectval_tyBM * resobj);
  const struct parserops_stBM *parsops = pars->pars_ops;
  assert (!parsops || parsops->parsop_magic == PARSOPMAGIC_BM);
  bool nobuild = parsops && parsops->parsop_nobuild;
  parserskipspaces_BM (pars);
  unsigned lineno = parserlineno_BM (pars);
  unsigned colpos = parsercolpos_BM (pars);
  if (depth > MAXDEPTHPARSE_BM)
    parsererrorprintf_BM (pars, lineno, colpos, //
                          "too deep (%d) object", depth);
  parstoken_tyBM tok = parsertokenget_BM (pars);
  if (tok.tok_kind == plex__NONE)
    goto failure;
  else if (tok.tok_kind == plex_NAMEDOBJ)
    {
      assert (tok.tok_namedobj != NULL);
      *pgotobj = true;
      return tok.tok_namedobj;
    }
  else if (tok.tok_kind == plex_ID)
    {
      if (tok.tok_id.id_hi == 0 && tok.tok_id.id_lo == 0)
        {
          *pgotobj = true;
          return NULL;
        };
      const objectval_tyBM *obid = _.resobj     //
        = nobuild ? NULL : (const objectval_tyBM
                            *) (findobjofid_BM (tok.tok_id));
      if (!obid && !nobuild)
        {
          char idbuf[32];
          memset (idbuf, 0, sizeof (idbuf));
          idtocbuf32_BM (tok.tok_id, idbuf);
          parsererrorprintf_BM (pars, lineno, colpos,   //
                                "unknown id %s", idbuf);
        };
      *pgotobj = true;
      return (objectval_tyBM *) obid;
    }
  // parse $:<var> object
  else if (tok.tok_kind == plex_DELIM && tok.tok_delim == delim_dollarcolon)
    {
      if (!parsops || !parsops->parsop_expand_dollarobj_rout)
        parsererrorprintf_BM (pars, lineno, colpos,
                              "no expansion for $:<var> object");
      const char *reslin = parserrestline_BM (pars);
      unsigned varlineno = parserlineno_BM (pars);
      unsigned varcolpos = parsercolpos_BM (pars);
      parstoken_tyBM vartok = parsertokenget_BM (pars);
      if (vartok.tok_kind == plex_NAMEDOBJ)
        {
          assert (varlineno == lineno);
          _.resobj =            //
            parsops->parsop_expand_dollarobj_rout
            (pars, varlineno, tok.tok_col,
             (const value_tyBM) vartok.tok_namedobj,
             (struct stackframe_stBM *) &_);
        }
      else if (vartok.tok_kind == plex_CNAME)
        {
          _.resobj =            //
            parsops->parsop_expand_dollarobj_rout
            (pars, varlineno, varcolpos, (const value_tyBM) vartok.tok_cname,
             (struct stackframe_stBM *) &_);
        }
      else                      // could happen if $: is followed by word
        parsererrorprintf_BM (pars, lineno, colpos,
                              "bad $:<var> expansion for %s", reslin);
      if (!nobuild && !isobject_BM ((const value_tyBM) _.resobj))
        parsererrorprintf_BM (pars, lineno, colpos,
                              "did not found $:<var> expansion for %s",
                              reslin);
      *pgotobj = true;
      return (objectval_tyBM *) _.resobj;
    }
  // parse and expand $[ ... ]
  else if (tok.tok_kind == plex_DELIM
           && tok.tok_delim == delim_dollarleftbracket)
    {
      const char *reslin = parserrestline_BM (pars);
      if (!parsops || !parsops->parsop_expand_objexp_rout)
        parsererrorprintf_BM (pars, lineno, colpos,
                              "no $[...] expansion for %s", reslin);
      char resbuf[40];
      memset (resbuf, 0, sizeof (resbuf));
      strncpy (resbuf, reslin, sizeof (resbuf) - 1);
      const char *end = NULL;
      g_utf8_validate (resbuf, -1, &end);
      *(char *) end = (char) 0;
      _.resobj = parsops->parsop_expand_objexp_rout
        (pars, lineno, colpos, depth, (struct stackframe_stBM *) &_);
      if (!nobuild && !isobject_BM ((const value_tyBM) _.resobj))
        parsererrorprintf_BM (pars, lineno, colpos,
                              "failed $[...] expansion of %s", resbuf);
      *pgotobj = true;
      return (objectval_tyBM *) _.resobj;
    }
failure:
  parserseek_BM (pars, lineno, colpos);
  *pgotobj = false;
  return NULL;
}                               /* end parsergetobject_BM */



////////////////
value_tyBM
parsergetvalue_BM (struct parser_stBM * pars,
                   struct stackframe_stBM * prevstkf, int depth,
                   bool * pgotval)
{
  if (!isparser_BM ((const value_tyBM) pars))
    FATAL_BM ("bad parser");
  if (!pgotval)
    FATAL_BM ("missing pgotval");
  const struct parserops_stBM *parsops = pars->pars_ops;
  assert (!parsops || parsops->parsop_magic == PARSOPMAGIC_BM);
  bool nobuild = parsops && parsops->parsop_nobuild;
  LOCALFRAME_BM                 //
    (prevstkf, NULL,            //
     value_tyBM resval;
     value_tyBM macroval;
     objectval_tyBM * connobj;
     union
     {
     objectval_tyBM * elemobj; objectval_tyBM * compobj; value_tyBM sonval;
     };
     struct datavectval_stBM *contdvec);
  parserskipspaces_BM (pars);
  unsigned lineno = parserlineno_BM (pars);
  unsigned colpos = parsercolpos_BM (pars);
  if (depth > MAXDEPTHPARSE_BM)
    parsererrorprintf_BM (pars, lineno, colpos, //
                          "too deep (%d) value", depth);
  parstoken_tyBM tok = parsertokenget_BM (pars);
  if (tok.tok_kind == plex__NONE)
    goto failure;
  //
  // parse numbers
  else if (tok.tok_kind == plex_LLONG)
    {
      *pgotval = true;
      return taggedint_BM (tok.tok_llong);
    }
  //
  // parse strings
  else if (tok.tok_kind == plex_STRING)
    {
      *pgotval = true;
      return nobuild ? NULL : (value_tyBM) tok.tok_string;
    }
  // parse objects given by id
  else if (tok.tok_kind == plex_ID)
    {
      if (tok.tok_id.id_hi == 0 && tok.tok_id.id_lo == 0)
        {
          *pgotval = true;
          return NULL;
        };
      objectval_tyBM *obid = _.compobj  //
        = nobuild ? NULL : (findobjofid_BM (tok.tok_id));
      if (!obid && !nobuild)
        {
          char idbuf[32];
          memset (idbuf, 0, sizeof (idbuf));
          idtocbuf32_BM (tok.tok_id, idbuf);
          parsererrorprintf_BM (pars, lineno, colpos,   //
                                "unknown id %s", idbuf);
        };
      *pgotval = true;
      return obid;
    }
  // parse objects given by name
  else if (tok.tok_kind == plex_NAMEDOBJ)
    {
      parserseek_BM (pars, lineno, colpos);
      bool gotobj = false;
      _.compobj =
        parsergetobject_BM (pars, (struct stackframe_stBM *) &_, depth + 1,
                            &gotobj);
      *pgotval = gotobj;
      return nobuild ? NULL : ((value_tyBM) _.compobj);
    }
  // parse code chunks
  else if (tok.tok_kind == plex_DELIM && tok.tok_delim == delim_hashleftbrace)
    {
      bool gotchunk = false;
      _.compobj =
        parsergetchunk_BM (pars, (struct stackframe_stBM *) &_, depth + 1,
                           &gotchunk);
      *pgotval = gotchunk;
      return nobuild ? NULL : ((value_tyBM) _.compobj);
    }
  //
  // parse tuples
  else if (tok.tok_kind == plex_DELIM && tok.tok_delim == delim_leftbracket)
    {
      int tuplin = tok.tok_line;
      int tupcol = tok.tok_col;
      // a tuple : [ obj1 .... objn ]
      bool gotcompobj = false;
      _.contdvec = nobuild ? NULL : datavect_grow_BM (NULL, 5);
      while ((gotcompobj = false),      //
             (_.compobj =       //
              parsergetobject_BM        //
              (pars,            //
               (struct stackframe_stBM *) &_,   //
               depth + 1, &gotcompobj)),        //
             gotcompobj)
        {
          if (!nobuild)
            _.contdvec = datavect_append_BM (_.contdvec, _.compobj);
        }
      parserskipspaces_BM (pars);
      parstoken_tyBM endtok = parsertokenget_BM (pars);
      if (endtok.tok_kind != plex_DELIM
          || endtok.tok_delim != delim_rightbracket)
        parsererrorprintf_BM (pars, lineno, colpos,     //
                              "missing closing bracket for tuple");
      int endlin = endtok.tok_line;
      int endcol = endtok.tok_col;
      if (parsops && parsops->parsop_decorate_nesting_rout)
        parsops->parsop_decorate_nesting_rout
          (pars, depth,
           delim_leftbracket, tuplin, tupcol,
           delim_rightbracket, endlin, endcol);
      if (!nobuild)
        _.resval = (value_tyBM)
          maketuple_BM ((objectval_tyBM **) (_.contdvec->vec_data),
                        datavectlen_BM (_.contdvec));
      else
        _.resval = NULL;
      *pgotval = true;
      return _.resval;
    }
  //
  // parse sets
  else if (tok.tok_kind == plex_DELIM && tok.tok_delim == delim_leftbrace)
    {
      // a set : { obj1 .... objn }
      int setlin = tok.tok_line;
      int setcol = tok.tok_col;
      bool gotelemobj = false;
      _.contdvec = nobuild ? NULL : datavect_grow_BM (NULL, 5);
      while ((gotelemobj = false),      //
             (_.elemobj =       //
              parsergetobject_BM        //
              (pars,            //
               (struct stackframe_stBM *) &_,   //
               depth + 1, &gotelemobj)),        //
             gotelemobj)
        {
          if (!nobuild)
            _.contdvec = datavect_append_BM (_.contdvec, _.elemobj);
        }
      parserskipspaces_BM (pars);
      parstoken_tyBM endtok = parsertokenget_BM (pars);
      if (endtok.tok_kind != plex_DELIM
          || endtok.tok_delim != delim_rightbrace)
        parsererrorprintf_BM (pars, lineno, colpos,     //
                              "missing closing brace for set");
      int endlin = endtok.tok_line;
      int endcol = endtok.tok_col;
      if (parsops && parsops->parsop_decorate_nesting_rout)
        parsops->parsop_decorate_nesting_rout
          (pars, depth,
           delim_leftbrace, setlin, setcol, delim_rightbrace, endlin, endcol);
      if (!nobuild)
        _.resval = (value_tyBM)
          makeset_BM ((const objectval_tyBM **) (_.contdvec->vec_data),
                      datavectlen_BM (_.contdvec));
      else
        _.resval = NULL;
      *pgotval = true;
      return _.resval;
    }
  //
  // parse named object sets
  // ~: * # set of all named objects
  // ~: foo # set of named objects starting with foo
  // ~: "foo*bar*" # set of named objects fnmatching "foo*bar*"
  // ~: "~?aa*" # set of named objects case-insensitively fnmatching "?aa*"
  else if (tok.tok_kind == plex_DELIM && tok.tok_delim == delim_tildecolon)
    {
      parserskipspaces_BM (pars);
      parstoken_tyBM patok = parsertokenget_BM (pars);
      if (patok.tok_kind == plex_NAMEDOBJ)
        {
          if (!nobuild)
            _.resval = (value_tyBM)
              setofprefixednamedobjects_BM (findobjectname_BM
                                            (patok.tok_namedobj));
        }
      else if (patok.tok_kind == plex_CNAME)
        {
          if (!nobuild)
            _.resval = (value_tyBM)
              setofprefixednamedobjects_BM (bytstring_BM (patok.tok_cname));
        }
      else if (patok.tok_kind == plex_STRING)
        {
          if (!nobuild)
            _.resval = (value_tyBM)
              setofmatchednamedobjects_BM (bytstring_BM (patok.tok_string));
        }
      else if (patok.tok_kind == plex_DELIM && patok.tok_delim == delim_star)
        {
          if (!nobuild)
            _.resval = (value_tyBM) setofnamedobjects_BM ();
        }
      else
        parsererrorprintf_BM (pars, lineno, colpos,     //
                              "~: not followed by name or string or * for matching set of named");
      *pgotval = true;
      return _.resval;
    }
  //
  // parse nodes
  else if (tok.tok_kind == plex_DELIM && tok.tok_delim == delim_star)
    {
      // a node: * connobj ( son1 ... sonn )
      int nodlin = tok.tok_line;
      int nodcol = tok.tok_col;
      bool gotconnobj = false;
      _.connobj =               //
        parsergetobject_BM      //
        (pars,                  //
         (struct stackframe_stBM *) &_, //
         depth + 1, &gotconnobj);
      if (!gotconnobj)
        parsererrorprintf_BM (pars, lineno, colpos,     //
                              "missing connective object of node after *");
      parstoken_tyBM lefttok = parsertokenget_BM (pars);
      if (lefttok.tok_kind != plex_DELIM
          || lefttok.tok_delim != delim_leftparen)
        parsererrorprintf_BM (pars, lineno, colpos,     //
                              "missing left parenthesis for node");
      int leftlin = lefttok.tok_line;
      int leftcol = lefttok.tok_col;
      _.contdvec = nobuild ? NULL : datavect_grow_BM (NULL, 3);
      bool gotson = false;
      while ((gotson = false),  //
             (_.sonval =        //
              parsergetvalue_BM //
              (pars,            //
               (struct stackframe_stBM *) &_,   //
               depth + 1, &gotson)),    //
             gotson)
        {
          if (!nobuild)
            _.contdvec = datavect_append_BM (_.contdvec, _.sonval);
        }
      parserskipspaces_BM (pars);
      parstoken_tyBM endtok = parsertokenget_BM (pars);
      if (endtok.tok_kind != plex_DELIM
          || endtok.tok_delim != delim_rightparen)
        parsererrorprintf_BM (pars, lineno, colpos,     //
                              "missing right parenthesis for node");
      int endlin = endtok.tok_line;
      int endcol = endtok.tok_col;
      if (parsops && parsops->parsop_decorate_start_nesting_rout)
        parsops->parsop_decorate_start_nesting_rout
          (pars, depth,
           delim_star, nodlin, nodcol,
           delim_leftparen, leftlin, leftcol,
           delim_rightparen, endlin, endcol);
      if (!nobuild)
        _.resval = (value_tyBM)
          makenode_BM (_.connobj,
                       datavectlen_BM (_.contdvec),
                       (const value_tyBM *) (_.contdvec->vec_data));
      else
        _.resval = NULL;
      *pgotval = true;
      return _.resval;
    }
  //
  // parse closures
  else if (tok.tok_kind == plex_DELIM && tok.tok_delim == delim_percent)
    {
      // a closure: % connobj ( son1 ... sonn )
      int clolin = tok.tok_line;
      int clocol = tok.tok_col;
      bool gotconnobj = false;
      _.connobj =               //
        parsergetobject_BM      //
        (pars,                  //
         (struct stackframe_stBM *) &_, //
         depth + 1, &gotconnobj);
      if (!gotconnobj)
        parsererrorprintf_BM    //
          (pars, lineno, colpos,        //
           "missing connective object of closure after %%");
      parserskipspaces_BM (pars);
      parstoken_tyBM lefttok = parsertokenget_BM (pars);
      if (lefttok.tok_kind != plex_DELIM
          || lefttok.tok_delim != delim_leftparen)
        parsererrorprintf_BM (pars, lineno, colpos,     //
                              "missing left parenthesis for closure");
      int leftlin = lefttok.tok_line;
      int leftcol = lefttok.tok_col;
      _.contdvec = nobuild ? NULL : datavect_grow_BM (NULL, 3);
      bool gotson = false;
      while ((gotson = false),  //
             (_.sonval =        //
              parsergetvalue_BM //
              (pars,            //
               (struct stackframe_stBM *) &_,   //
               depth + 1, &gotson)),    //
             gotson)
        {
          if (!nobuild)
            _.contdvec = datavect_append_BM (_.contdvec, _.sonval);
        }
      parserskipspaces_BM (pars);
      parstoken_tyBM endtok = parsertokenget_BM (pars);
      if (endtok.tok_kind != plex_DELIM
          || endtok.tok_delim != delim_rightparen)
        parsererrorprintf_BM (pars, lineno, colpos,     //
                              "missing right parenthesis for closure");
      int endlin = endtok.tok_line;
      int endcol = endtok.tok_col;
      if (parsops && parsops->parsop_decorate_start_nesting_rout)
        parsops->parsop_decorate_start_nesting_rout
          (pars, depth,
           delim_percent, clolin, clocol,
           delim_leftparen, leftlin, leftcol,
           delim_rightparen, endlin, endcol);
      if (!nobuild)
        _.resval = (value_tyBM)
          makeclosure_BM (_.connobj,
                          datavectlen_BM (_.contdvec),
                          (const value_tyBM *) (_.contdvec->vec_data));
      else
        _.resval = NULL;
      *pgotval = true;
      return _.resval;
    }
  //
  // parse read-macro expansion: ^ object ( arguments ... )
  else if (tok.tok_kind == plex_DELIM && tok.tok_delim == delim_caret)
    {
      if (!parsops || !parsops->parsop_expand_readmacro_rout)
        parsererrorprintf_BM (pars, lineno, colpos, "no readmacro expansion");

      int nodlin = tok.tok_line;
      int nodcol = tok.tok_col;
      bool gotconnobj = false;
      _.connobj =               //
        parsergetobject_BM      //
        (pars,                  //
         (struct stackframe_stBM *) &_, //
         depth + 1, &gotconnobj);
      if (!gotconnobj)
        parsererrorprintf_BM (pars, lineno, colpos,     //
                              "missing connective object of readmacro after *");
      parstoken_tyBM lefttok = parsertokenget_BM (pars);
      if (lefttok.tok_kind != plex_DELIM
          || lefttok.tok_delim != delim_leftparen)
        parsererrorprintf_BM (pars, lineno, colpos,     //
                              "missing left parenthesis for readmacro");
      int leftlin = lefttok.tok_line;
      int leftcol = lefttok.tok_col;
      _.contdvec = nobuild ? NULL : datavect_grow_BM (NULL, 3);
      bool gotson = false;
      while ((gotson = false),  //
             (_.sonval =        //
              parsergetvalue_BM //
              (pars,            //
               (struct stackframe_stBM *) &_,   //
               depth + 1, &gotson)),    //
             gotson)
        {
          if (!nobuild)
            _.contdvec = datavect_append_BM (_.contdvec, _.sonval);
        }
      parserskipspaces_BM (pars);
      parstoken_tyBM endtok = parsertokenget_BM (pars);
      if (endtok.tok_kind != plex_DELIM
          || endtok.tok_delim != delim_rightparen)
        parsererrorprintf_BM (pars, lineno, colpos,     //
                              "missing right parenthesis for readmacro");
      int endlin = endtok.tok_line;
      int endcol = endtok.tok_col;
      if (parsops && parsops->parsop_decorate_start_nesting_rout)
        parsops->parsop_decorate_start_nesting_rout
          (pars, depth,
           delim_caret, nodlin, nodcol,
           delim_leftparen, leftlin, leftcol,
           delim_rightparen, endlin, endcol);
      if (!nobuild)
        {
          _.macroval = (value_tyBM)
            makenode_BM (_.connobj,
                         datavectlen_BM (_.contdvec),
                         (const value_tyBM *) (_.contdvec->vec_data));
          _.resval =            //
            parsops->parsop_expand_readmacro_rout
            (pars, nodlin, nodcol, depth, _.macroval,
             (struct stackframe_stBM *) &_);
        }
      else
        _.resval = NULL;
      *pgotval = true;
      return _.resval;
    }
  //
  // parse $<var> value
  else if (tok.tok_kind == plex_DELIM && tok.tok_delim == delim_dollar)
    {
      if (!parsops || !parsops->parsop_expand_dollarval_rout)
        parsererrorprintf_BM (pars, lineno, colpos,
                              "no expansion for $<var> object");
      const char *reslin = parserrestline_BM (pars);
      parstoken_tyBM vartok = parsertokenget_BM (pars);
      if (vartok.tok_kind == plex_NAMEDOBJ)
        {
          assert (tok.tok_line == vartok.tok_line);
          _.resval = (value_tyBM) parsops->parsop_expand_dollarval_rout //
            (pars, tok.tok_line, tok.tok_col,
             (const value_tyBM)
             vartok.tok_namedobj, (struct stackframe_stBM *) &_);
        }
      else if (vartok.tok_kind == plex_CNAME)
        {
          assert (tok.tok_line == vartok.tok_line);
          _.resval = (value_tyBM) parsops->parsop_expand_dollarval_rout //
            (pars, tok.tok_line, vartok.tok_col,
             (const value_tyBM)
             vartok.tok_cname, (struct stackframe_stBM *) &_);
        }
      else                      // could happen if $ is followed by word
        parsererrorprintf_BM (pars, lineno, colpos,
                              "bad $<var> expansion for %s", reslin);
      if (!nobuild && !isobject_BM ((const value_tyBM) _.resval))
        parsererrorprintf_BM (pars, lineno, colpos,
                              "did not found $<var> expansion for %s",
                              reslin);
      *pgotval = true;
      return _.resval;

    }
  // parse $:<var> object
  else if (tok.tok_kind == plex_DELIM && tok.tok_delim == delim_dollarcolon)
    {
      if (!parsops || !parsops->parsop_expand_dollarobj_rout)
        parsererrorprintf_BM (pars, lineno, colpos,
                              "no expansion for $:<var> object");
      const char *reslin = parserrestline_BM (pars);
      unsigned varlineno = parserlineno_BM (pars);
      //unsigned varcolpos = parsercolpos_BM (pars);
      parstoken_tyBM vartok = parsertokenget_BM (pars);
      if (vartok.tok_kind == plex_NAMEDOBJ)
        {
          assert (varlineno == lineno);
          _.resval = (value_tyBM) parsops->parsop_expand_dollarobj_rout //
            (pars, tok.tok_line, tok.tok_col,
             (const value_tyBM)
             vartok.tok_namedobj, (struct stackframe_stBM *) &_);
        }
      else if (vartok.tok_kind == plex_CNAME)
        {
          _.resval = (value_tyBM) parsops->parsop_expand_dollarobj_rout //
            (pars, vartok.tok_line, tok.tok_col,
             (const value_tyBM)
             vartok.tok_cname, (struct stackframe_stBM *) &_);
        }
      else                      // could happen if $: is followed by word
        parsererrorprintf_BM (pars, lineno, colpos,
                              "bad $:<var> expansion for %s", reslin);
      if (!nobuild && !isobject_BM ((const value_tyBM) _.resval))
        parsererrorprintf_BM (pars, lineno, colpos,
                              "did not found $:<var> expansion for %s",
                              reslin);
      *pgotval = true;
      return _.resval;
    }

  // parse and expand $[ ... ]
  else if (tok.tok_kind == plex_DELIM
           && tok.tok_delim == delim_dollarleftbracket)
    {
      const char *reslin = parserrestline_BM (pars);
      if (!parsops || !parsops->parsop_expand_objexp_rout)
        parsererrorprintf_BM (pars, lineno, colpos,
                              "no $[...] expansion for %s", reslin);
      char resbuf[40];
      memset (resbuf, 0, sizeof (resbuf));
      strncpy (resbuf, reslin, sizeof (resbuf) - 1);
      const char *end = NULL;
      g_utf8_validate (resbuf, -1, &end);
      *(char *) end = (char) 0;
      _.resval =
        (const value_tyBM) parsops->parsop_expand_objexp_rout
        (pars, lineno, colpos, depth, (struct stackframe_stBM *) &_);
      if (!nobuild && !isobject_BM ((const value_tyBM) _.resval))
        parsererrorprintf_BM (pars, lineno, colpos,
                              "failed $[...] expansion of %s", resbuf);
      *pgotval = true;
      return (objectval_tyBM *) _.resval;
    }
  // parse and expand $( ... )
  else if (tok.tok_kind == plex_DELIM
           && tok.tok_delim == delim_dollarleftparen)
    {
      const char *reslin = parserrestline_BM (pars);
      if (!parsops || !parsops->parsop_expand_valexp_rout)
        parsererrorprintf_BM (pars, lineno, colpos,
                              "no $(...) expansion for %s", reslin);
      char resbuf[40];
      memset (resbuf, 0, sizeof (resbuf));
      strncpy (resbuf, reslin, sizeof (resbuf) - 1);
      const char *end = NULL;
      g_utf8_validate (resbuf, -1, &end);
      *(char *) end = (char) 0;
      _.resval =
        parsops->parsop_expand_valexp_rout (pars, lineno, colpos, depth,
                                            (struct stackframe_stBM *) &_);
      if (!nobuild && ((const value_tyBM) _.resval) == NULL)
        parsererrorprintf_BM (pars, lineno, colpos,
                              "failed $(...) expansion of %s", resbuf);
      *pgotval = true;
      return (objectval_tyBM *) _.resval;
    }
  //////
failure:
  parserseek_BM (pars, lineno, colpos);
  *pgotval = false;
  return NULL;
}                               /* end of parsergetvalue_BM */


value_tyBM
  parsergetchunk_BM
  (struct parser_stBM * pars,
   struct stackframe_stBM * prevstkf, int depth, bool * pgotchunk)
{
  if (!isparser_BM ((const value_tyBM) pars))
    FATAL_BM ("bad parser");
  if (!pgotchunk)
    FATAL_BM ("missing pgotchunk");
  const struct parserops_stBM *parsops = pars->pars_ops;
  assert (!parsops || parsops->parsop_magic == PARSOPMAGIC_BM);
  unsigned startlineno = parserlineno_BM (pars);
  unsigned startcolpos = parsercolpos_BM (pars);
  bool nobuild = parsops && parsops->parsop_nobuild;
  LOCALFRAME_BM                 //
    (prevstkf, NULL,            //
     value_tyBM resval;
     struct datavectval_stBM *chunkvec; const objectval_tyBM * obj;
     value_tyBM compv;
    );
  _.chunkvec = nobuild ? NULL : datavect_grow_BM (NULL, 5);
  const char *end = NULL;
  bool gotend = false;
  int loopcnt = 0;
  do
    {
      unsigned curlineno = parserlineno_BM (pars);
      unsigned curcolpos = parsercolpos_BM (pars);
      char *curpc = (char *) parserrestline_BM (pars);
      if (pars->pars_debug)
        DBGPRINTF_BM ("parsergetchunk_BM L%dC%d loop#%d curpc(l%d):%s",
                      curlineno, curcolpos, loopcnt,
                      curpc ? ((int) strlen (curpc)) : -1, curpc);
      if (loopcnt++ > MAXSIZE_BM / 8)
        parsererrorprintf_BM (pars, curlineno, curcolpos,
                              "too many loops %d in chunk (started line %d, col %d) : %s",
                              loopcnt, startlineno, startcolpos, curpc);
      if (parserendoffile_BM (pars))
        parsererrorprintf_BM (pars, curlineno, curcolpos,       //
                              "end of file in chunk (started line %d, col %d)",
                              startlineno, startcolpos);
      if (parsereol_BM (pars) || !curpc || *curpc == (char) 0)
        {
          parsernextline_BM (pars);
          continue;
        }
      gunichar uc = g_utf8_get_char (curpc);
      if (!uc)
        break;
      /// process word-like characters, i.e. alnum & underscore & marks
      if (g_unichar_isalnum (uc) || uc == '_' || g_unichar_ismark (uc))
        {
          char *npc = curpc;
          gunichar nc = 0;
          while (*npc && (nc = g_utf8_get_char (npc)) > 0
                 && (g_unichar_isalnum (nc) || g_unichar_ismark (nc)
                     || nc == '_'))
            npc = g_utf8_next_char (npc);
          bool allascii = uc < 127;
          for (const char *p = curpc; p < npc && allascii; p++)
            allascii = (*p) < 127 && (isalnum (*p) || *p == '_');
          if (allascii)
            {
              char oldn = *npc;
              *npc = 0;
              rawid_tyBM id = { 0, 0 };
              // check for ids
              if (curpc[0] == '_' && isdigit (curpc[1])
                  && npc <= curpc + IDLEN_BM
                  && (id =
                      parse_rawid_BM (curpc, (const char **) &end)).id_hi > 0
                  && end <= npc)
                {
                  *npc = oldn;
                  _.obj = findobjofid_BM (id);
                  if (_.obj)
                    {
                      if (!nobuild)
                        _.chunkvec =    //
                          datavect_append_BM (_.chunkvec,
                                              (const value_tyBM) _.obj);
                      parseradvanceutf8_BM (pars,
                                            g_utf8_strlen (curpc,
                                                           npc - curpc));
                      continue;
                    }
                }
              // check for numbers in decimal
              if (*curpc < 127 && isdigit (*curpc))
                {
                  char numbuf[32];
                  memset (numbuf, 0, sizeof (numbuf));
                  char *end = NULL;
                  long long ll = strtoll (curpc, (char **) &end, 10);
                  if (end == npc && ll < LLONG_MAX / 2 && ll < INTPTR_MAX / 2
                      && snprintf (numbuf, sizeof (numbuf), "%lld", ll) > 0
                      && !strncmp (numbuf, curpc, npc - curpc))
                    {
                      *npc = oldn;
                      _.compv = taggedint_BM (ll);
                      if (!nobuild)
                        _.chunkvec =    //
                          datavect_append_BM (_.chunkvec,
                                              (const value_tyBM) _.compv);
                      if (parsops && parsops->parsop_decorate_number_rout)
                        parsops->parsop_decorate_number_rout    //
                          (pars, curlineno, curcolpos,
                           g_utf8_strlen (curpc, npc - curpc));
                      parseradvanceutf8_BM (pars,
                                            g_utf8_strlen (curpc,
                                                           npc - curpc));
                      continue;
                    }
                  *npc = oldn;
                }
              // check for name of existing objects
              if (isalpha (*curpc) && validname_BM (curpc))
                {
                  _.obj = findnamedobj_BM (curpc);
                  *npc = oldn;
                  if (_.obj)
                    {
                      if (parsops && parsops->parsop_decorate_known_name_rout)
                        parsops->parsop_decorate_known_name_rout
                          (pars, curlineno, curcolpos,
                           g_utf8_strlen (curpc, npc - curpc));
                      if (!nobuild)
                        _.chunkvec =    //
                          datavect_append_BM (_.chunkvec,
                                              (const value_tyBM) _.obj);
                      parseradvanceutf8_BM (pars,
                                            g_utf8_strlen (curpc,
                                                           npc - curpc));
                      continue;
                    }
                  else
                    {
                      if (parsops && parsops->parsop_decorate_new_name_rout)
                        parsops->parsop_decorate_new_name_rout
                          (pars, curlineno, curcolpos,
                           g_utf8_strlen (curpc, npc - curpc));
                    }
                }
              *npc = oldn;
            }                   /* end if allascii */
          // plain word, make a string of it
          if (!nobuild)
            {
              _.compv =
                (const value_tyBM) makestringlen_BM (curpc, npc - curpc);
              _.chunkvec =      //
                datavect_append_BM (_.chunkvec, (const value_tyBM) _.compv);
            }
          parseradvanceutf8_BM (pars, g_utf8_strlen (curpc, npc - curpc));
          continue;
        }                       /* end if word */
      // process contiguous space characters
      if (g_unichar_isspace (uc))
        {
          char *npc = curpc;
          gunichar nc = 0;
          while (*npc && (nc = g_utf8_get_char (npc)) > 0
                 && g_unichar_isspace (nc))
            npc = g_utf8_next_char (npc);
          if (!nobuild)
            {
              _.compv =
                (const value_tyBM) makestringlen_BM (curpc, npc - curpc);
              _.chunkvec =      //
                datavect_append_BM (_.chunkvec, (const value_tyBM) _.compv);
            }
          parseradvanceutf8_BM (pars, g_utf8_strlen (curpc, npc - curpc));
          continue;
        }                       /* end if spaces */
      // handle end of chunk
      if (curpc[0] == '}' && curpc[1] == '#')
        {
          if (parsops && parsops->parsop_decorate_nesting_rout)
            parsops->parsop_decorate_nesting_rout
              (pars, depth,
               delim_hashleftbrace, startlineno, startcolpos,
               delim_rightbracehash, curlineno, curcolpos);
          parseradvanceutf8_BM (pars, 2);
          gotend = true;
          break;
        }
      // process contiguous punctuation characters non $, perhaps
      // ended by $$
      if (uc != '$' && g_unichar_ispunct (uc))
        {
          char *npc = curpc;
          gunichar nc = 0;
          while (*npc && (nc = g_utf8_get_char (npc)) > 0
                 && nc != '$' && g_unichar_ispunct (nc))
            {
              if (npc[0] == '}' && npc[1] == '#')
                break;
              npc = g_utf8_next_char (npc);
            };
          bool twodollars = npc[0] == '$' && npc[1] == '$';
          if (twodollars)
            npc++;
          if (!nobuild)
            {
              _.compv =
                (const value_tyBM) makestringlen_BM (curpc, npc - curpc);
              _.chunkvec =      //
                datavect_append_BM (_.chunkvec, (const value_tyBM) _.compv);
            }
          parseradvanceutf8_BM (pars,
                                g_utf8_strlen (curpc,
                                               npc - curpc) +
                                (twodollars ? 1 : 0));
          continue;
        }
      // special processing for $
      if (uc == '$')
        {
          const char *npc = curpc + 1;
          gunichar nc = g_utf8_get_char (npc);
          if (!nc)
            parsererrorprintf_BM (pars, curlineno, curcolpos,   //
                                  "end of file after dollar");
          if (nc == '}')
            parsererrorprintf_BM (pars, curlineno, curcolpos,
                                  "$} is forbidden");
          npc = g_utf8_next_char (npc);
          // $, and $& and $. are skipped
          if (nc == ',' || nc == '&' || nc == '.')
            {
              parseradvanceutf8_BM (pars, 2);
              continue;
            }
          // $( starts a nested value, which should be followed by a )
          else if (nc == '(')
            {
              parseradvanceutf8_BM (pars, 2);
              bool gotval = false;
              _.compv =
                parsergetvalue_BM (pars, (struct stackframe_stBM *) &_,
                                   depth + 1, &gotval);
              if (!gotval)
                parsererrorprintf_BM (pars, curlineno, curcolpos,
                                      "$( not followed by value");
              parserskipspaces_BM (pars);
              parstoken_tyBM tok = parsertokenget_BM (pars);
              unsigned clolineno = parserlineno_BM (pars);
              unsigned clocolpos = parsercolpos_BM (pars);
              if (tok.tok_kind != plex_DELIM
                  || tok.tok_delim != delim_rightparen)
                parsererrorprintf_BM (pars, clolineno, clocolpos,
                                      "closing paren expected after nested expression $(...) in chunk line %d, col %d",
                                      curlineno, curcolpos);
              if (parsops && parsops->parsop_decorate_nesting_rout)
                parsops->parsop_decorate_nesting_rout
                  (pars, depth,
                   delim_dollarleftparen, curlineno, curcolpos,
                   delim_rightparen, clolineno, clocolpos);
              if (!nobuild)
                {
                  _.chunkvec =  //
                    datavect_append_BM (_.chunkvec,
                                        (const value_tyBM)
                                        makenode_BM (BMP_embed, 1, &_.compv));
                }
            }                   /* end nested values $(...) */
          /// $name ...
          else if (nc < 127 && isalpha (nc))
            {
              while (isalnum (*npc) || *npc == '_')
                npc++;
              char oldc = *npc;
              *(char *) npc = 0;
              _.obj = findnamedobj_BM (curpc + 1);
              *(char *) npc = oldc;
              if (!_.obj)
                parsererrorprintf_BM (pars, curlineno, curcolpos,
                                      "invalid dollarvar %s in chunk", curpc);
              if (!nobuild)
                {
                  _.chunkvec =  //
                    datavect_append_BM (_.chunkvec,
                                        (const value_tyBM)
                                        makenode_BM (BMP_variable, 1,
                                                     (value_tyBM *) & _.obj));
                }
              parseradvanceutf8_BM (pars, g_utf8_strlen (curpc, npc - curpc));
              continue;
            }                   /* end $name */
          /// $$ taken as a single dollar in isolation
          else if (nc == '$')
            {
              if (!nobuild)
                {
                  _.compv = (const value_tyBM) makestring_BM ("$");
                  _.chunkvec =  //
                    datavect_append_BM (_.chunkvec,
                                        (const value_tyBM) _.compv);
                }
              parseradvanceutf8_BM (pars, 2);
              continue;
            }
          else
            parsererrorprintf_BM (pars, curlineno, curcolpos,
                                  "invalid dollar seqence %s", curpc);
        }                       /* end dollar */
      else
        {                       // any other character, including control characters, is taken in isolation       
          char *npc = g_utf8_next_char (curpc);
          if (!nobuild)
            {
              _.compv =
                (const value_tyBM) makestringlen_BM (curpc, npc - curpc);
              _.chunkvec =      //
                datavect_append_BM (_.chunkvec, (const value_tyBM) _.compv);
            }
          parseradvanceutf8_BM (pars, 1);

          continue;
        }
    }
  while (!gotend);
  if (pars->pars_debug)
    DBGPRINTF_BM ("parsergetchunk_BM gotend %s veclen %u",
                  gotend ? "true" : "false", datavectlen_BM (_.chunkvec));
  if (gotend)
    {
      _.resval = nobuild ? NULL //
        : (const value_tyBM) makenode_BM (BMP_chunk,
                                          datavectlen_BM (_.chunkvec),
                                          datavectdata_BM (_.chunkvec));
      if (pgotchunk)
        *pgotchunk = true;
      return _.resval;
    }
  if (pgotchunk)
    *pgotchunk = false;
  return NULL;
}                               /* end parsergetchunk_BM */

// eof parser_BM.c
