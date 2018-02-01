/* file newgui_BM.c */
#include "bismon.h"
#include "newgui_BM.const.h"

#define BROWSE_MAXDEPTH_NEWGUI_BM 48
// each named value has its own GtkTextBuffer, which is displayed in
//the value window, containing a GtkPane, in a GtkScrolledWindow
//containing one GtkFrame (containing a GtkHeaderBar & GtkTextView)
//per named value;

struct namedvaluethings_stBM
{
  GtkWidget *nvxt_frame;
  GtkWidget *nvxt_vbox;
  GtkWidget *nvxt_spindepth;
  GtkWidget *nvxt_headb;
  GtkWidget *nvxt_textview;
};
struct namedvaluenewguixtra_stBM
{
  int nvx_index;                /* corresponding index in browsedval_BM */
  GtkTextBuffer *nvx_tbuffer;
  // upper & lower things
  struct namedvaluethings_stBM nvx_upper, nvx_lower;
};

static const stringval_tyBM *astrval_bm;        // the "a" string value, name of default results, $a

static GtkCssProvider *cssprovider_newgui_bm;
static GtkWidget *windowvalues_newgui_bm;
static GtkWidget *valueslabel_newgui_bm;
static GtkWidget *upperscrollwvalues_newgui_bm;
static GtkWidget *lowerscrollwvalues_newgui_bm;
static GtkWidget *uppervboxvalues_newgui_bm;
static GtkWidget *lowervboxvalues_newgui_bm;


#warning objectview is incomplete
// each objectview appears twice, in a paned; the widgets inside upper
// & lower panes ...
struct objectviewthings_stBM
{
  GtkWidget *obvt_frame;
  GtkWidget *obvt_vbox;
  GtkWidget *obvt_headb;
  GtkWidget *obvt_textview;
};
struct objectwindow_newgui_stBM;
struct objectview_newgui_stBM
{
  int obv_rank;                 /* rank in window */
  int obv_depth;
  struct objectwindow_newgui_stBM *obv_obwindow;
  objectval_tyBM *obv_object;
  objectval_tyBM *obv_obsel;
  GtkTextBuffer *obv_tbuffer;
  struct objectviewthings_stBM obv_upper, obv_lower;
};                              /* end struct objectview_newgui_stBM */

// an  objectwindow
struct objectwindow_newgui_stBM
{
  struct objectwindow_newgui_stBM *obw_prev;
  struct objectwindow_newgui_stBM *obw_next;
  GtkWidget *obw_window;
  GtkWidget *obw_label;
  GtkWidget *obw_upperobjvbox;
  GtkWidget *obw_lowerobjvbox;
  int obw_rank;
  int obw_asiz;
  int obw_ulen;
  struct objectview_newgui_stBM **obw_arr;
};                              /* end struct objectwindow_newgui_stBM */

static struct objectwindow_newgui_stBM *obwin_first_newgui_BM;
static struct objectwindow_newgui_stBM *obwin_last_newgui_BM;
struct objectwindow_newgui_stBM *obwin_current_newgui_BM;

static struct objectwindow_newgui_stBM *make_obwin_newgui_BM (void);
static bool deleteobjectwin_newgui_BM (GtkWidget * widget, GdkEvent * ev,
                                       gpointer data);
/*****************************************************************/
// the function to handle keypresses of cmd, for Return & Tab
static gboolean handlekeypress_newgui_cmd_BM (GtkWidget *, GdkEventKey *,
                                              gpointer);

// the function to handle "end-user-action" on commandbuf_BM
static void enduseraction_newgui_cmd_BM (GtkTextBuffer *, gpointer);
// the function to handle "populate-popup" on commandview_BM
static void populatepopup_newgui_cmd_BM (GtkTextView *, GtkWidget *,
                                         gpointer);

static void runcommand_newgui_BM (bool erase);
static void run_then_erase_newgui_command_BM (void);
static void run_then_keep_newgui_command_BM (void);

static void markset_newgui_cmd_BM (GtkTextBuffer *, GtkTextIter *,
                                   GtkTextMark *, gpointer);

static void parsecommandbuf_newgui_BM (struct parser_stBM *pars,
                                       struct stackframe_stBM *stkf);

// for $:<var>, show in  dollar_cmdtag.
static parser_expand_dollarobj_sigBM parsdollarobj_newguicmd_BM;

// for $<var>, use show in dollar_cmdtag.
static parser_expand_dollarval_sigBM parsdollarval_newguicmd_BM;

// parse inside $(....)
static parser_expand_valexp_sigBM parsvalexp_newguicmd_BM;

// parse inside $[...]
static parser_expand_objexp_sigBM parsobjexp_newguicmd_BM;

// parse €<name> or $*<name>
static parser_expand_newname_sigBM parsmakenewname_newguicmd_BM;

// expand readmacro-s
static parser_expand_readmacro_sigBM parsreadmacroexp_newguicmd_BM;

static parser_error_sigBM parserror_newguicmd_BM;


// give the index of a name, or -1 if not found
static int index_named_value_newgui_BM (const char *vstr);

// return the value of a name, or null
static value_tyBM
find_named_value_newgui_BM (const char *vstr, struct stackframe_stBM *stkf);

static void
browse_named_value_newgui_BM (const stringval_tyBM * namev,
                              const value_tyBM val,
                              int browsdepth, struct stackframe_stBM *stkf);

static void
hide_named_value_newgui_BM (const char *namestr,
                            struct stackframe_stBM *stkf);
static void
hide_index_named_value_newgui_BM (int idx, struct stackframe_stBM *stkf);

const struct parserops_stBM parsop_command_build_newgui_BM = {
  .parsop_magic = PARSOPMAGIC_BM,
  .parsop_serial = 3,
  .parsop_nobuild = false,
  .parsop_error_rout = parserror_newguicmd_BM,
  ///
  .parsop_expand_dollarobj_rout = parsdollarobj_newguicmd_BM,
  .parsop_expand_dollarval_rout = parsdollarval_newguicmd_BM,
  .parsop_expand_newname_rout = parsmakenewname_newguicmd_BM,
  .parsop_expand_valexp_rout = parsvalexp_newguicmd_BM,
  .parsop_expand_objexp_rout = parsobjexp_newguicmd_BM,
  .parsop_expand_readmacro_rout = parsreadmacroexp_newguicmd_BM,
  ///
  .parsop_decorate_comment_sign_rout = parscommentsign_guicmd_BM,
  .parsop_decorate_comment_inside_rout = parscommentinside_guicmd_BM,
  .parsop_decorate_delimiter_rout = parsdelim_guicmd_BM,
  .parsop_decorate_id_rout = parsid_guicmd_BM,
  .parsop_decorate_known_name_rout = parsknowname_guicmd_BM,
  .parsop_decorate_new_name_rout = parsnewname_guicmd_BM,
  .parsop_decorate_number_rout = parsnumber_guicmd_BM,
  .parsop_decorate_string_sign_rout = parsstringsign_guicmd_BM,
  .parsop_decorate_string_inside_rout = parsstringinside_guicmd_BM,
  .parsop_decorate_nesting_rout = parsnesting_guicmd_BM,
  .parsop_decorate_start_nesting_rout = parsstartnesting_guicmd_BM,
};

const struct parserops_stBM parsop_command_nobuild_newgui_BM = {
  .parsop_magic = PARSOPMAGIC_BM,
  .parsop_serial = 4,
  .parsop_nobuild = true,
  .parsop_error_rout = parserror_newguicmd_BM,
  ///
  .parsop_expand_dollarobj_rout = parsdollarobj_newguicmd_BM,
  .parsop_expand_dollarval_rout = parsdollarval_newguicmd_BM,
  .parsop_expand_newname_rout = parsmakenewname_newguicmd_BM,
  .parsop_expand_valexp_rout = parsvalexp_newguicmd_BM,
  .parsop_expand_objexp_rout = parsobjexp_newguicmd_BM,
  .parsop_expand_readmacro_rout = parsreadmacroexp_newguicmd_BM,
  ///
  .parsop_decorate_comment_sign_rout = parscommentsign_guicmd_BM,
  .parsop_decorate_comment_inside_rout = parscommentinside_guicmd_BM,
  .parsop_decorate_delimiter_rout = parsdelim_guicmd_BM,
  .parsop_decorate_id_rout = parsid_guicmd_BM,
  .parsop_decorate_known_name_rout = parsknowname_guicmd_BM,
  .parsop_decorate_new_name_rout = parsnewname_guicmd_BM,
  .parsop_decorate_number_rout = parsnumber_guicmd_BM,
  .parsop_decorate_string_sign_rout = parsstringsign_guicmd_BM,
  .parsop_decorate_string_inside_rout = parsstringinside_guicmd_BM,
  .parsop_decorate_nesting_rout = parsnesting_guicmd_BM,
  .parsop_decorate_start_nesting_rout = parsstartnesting_guicmd_BM,
};



GtkWidget *
initialize_newgui_command_scrollview_BM (void)
{
  commandbuf_BM = gtk_text_buffer_new (commandtagtable_BM);
  assert (GTK_IS_TEXT_BUFFER (commandbuf_BM));
  for (int depth = 0; depth < CMD_MAXNEST_BM; depth++)
    {
      char opennamebuf[24];
      snprintf (opennamebuf, sizeof (opennamebuf), "open%d_cmdtag", depth);
      open_cmdtags_BM[depth] =  //
        gtk_text_buffer_create_tag (commandbuf_BM, opennamebuf, NULL);
      char closenamebuf[24];
      snprintf (closenamebuf, sizeof (closenamebuf), "close%d_cmdtag", depth);
      close_cmdtags_BM[depth] = //
        gtk_text_buffer_create_tag (commandbuf_BM, closenamebuf, NULL);
      char xtranamebuf[24];
      snprintf (xtranamebuf, sizeof (xtranamebuf), "xtra%d_cmdtag", depth);
      xtra_cmdtags_BM[depth] =  //
        gtk_text_buffer_create_tag (commandbuf_BM, xtranamebuf, NULL);
    };
  commandview_BM = gtk_text_view_new_with_buffer (commandbuf_BM);
  gtk_widget_set_name (commandview_BM, "commandview");
  gtk_text_view_set_editable (GTK_TEXT_VIEW (commandview_BM), true);
  gtk_text_view_set_accepts_tab (GTK_TEXT_VIEW (commandview_BM), FALSE);
  g_signal_connect (commandview_BM, "key-press-event",
                    G_CALLBACK (handlekeypress_newgui_cmd_BM), NULL);
  g_signal_connect (commandbuf_BM, "end-user-action",
                    G_CALLBACK (enduseraction_newgui_cmd_BM), NULL);
  g_signal_connect (commandview_BM, "populate-popup",
                    G_CALLBACK (populatepopup_newgui_cmd_BM), NULL);
  g_signal_connect (commandbuf_BM, "mark-set",
                    G_CALLBACK (markset_newgui_cmd_BM), NULL);
  GtkWidget *commandscrolw = gtk_scrolled_window_new (NULL, NULL);
  gtk_container_add (GTK_CONTAINER (commandscrolw), commandview_BM);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (commandscrolw),
                                  GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
  return commandscrolw;
}                               /* end initialize_newgui_command_scrollview_BM */

void
gcmarknewgui_BM (struct garbcoll_stBM *gc)
{
  assert (gc && gc->gc_magic == GCMAGIC_BM);
  // mark the browsedobj_BM browsedval_stBM & complsetcmd_BM
  gcmarkoldgui_BM (gc);
  VALUEGCPROC_BM (gc, astrval_bm, 0);
  for (struct objectwindow_newgui_stBM * obw = obwin_first_newgui_BM;
       obw != NULL; obw = obw->obw_next)
    {
      struct objectview_newgui_stBM **varr = obw->obw_arr;
      if (varr == NULL)
        continue;
      assert (obw->obw_ulen >= 0 && obw->obw_ulen <= obw->obw_asiz);
      for (int obix = 0; obix < obw->obw_ulen; obix++)
        {
          struct objectview_newgui_stBM *oview = varr[obix];
          if (!oview)
            continue;
          assert (oview->obv_rank == obix);
          assert (oview->obv_obwindow == obw);
          gcobjmark_BM (gc, oview->obv_object);
          gcobjmark_BM (gc, oview->obv_obsel);
        }
    }
}                               /* end gcmarknewgui_BM */


// for "key-press-event" signal to commandview_BM
gboolean
handlekeypress_newgui_cmd_BM (GtkWidget * widg, GdkEventKey * evk,
                              gpointer data)
{
  assert (GTK_IS_TEXT_VIEW (widg));
  assert (evk != NULL);
  assert (data == NULL);
  // see <gdk/gdkkeysyms.h> for names of keysyms
  if (evk->keyval == GDK_KEY_Return)
    {
      GdkModifierType modmask = gtk_accelerator_get_default_mod_mask ();
      bool withctrl = (evk->state & modmask) == GDK_CONTROL_MASK;
      bool withshift = (evk->state & modmask) == GDK_SHIFT_MASK;
      if (withctrl)
        {
          run_then_erase_newgui_command_BM ();
        }
      else if (withshift)
        {
          run_then_keep_newgui_command_BM ();
        }
      else                      // plain RETURN key, propagate it
        return false;
      return true;              // dont propagate the return when withctrl or withshift
    }
  else if (evk->keyval == GDK_KEY_Tab)
    {
      tabautocomplete_gui_cmd_BM ();
      return true;              // dont propagate the tab
    }
  else if (evk->keyval >= GDK_KEY_F1 && evk->keyval <= GDK_KEY_F10)
    {
      GdkModifierType modmask = gtk_accelerator_get_default_mod_mask ();
      bool withctrl = (evk->state & modmask) == GDK_CONTROL_MASK;
      bool withshift = (evk->state & modmask) == GDK_SHIFT_MASK;
      DBGPRINTF_BM ("handlekeypress_newgui_cmd_BM keyval %#x KEY_F%d %s%s",
                    evk->keyval, evk->keyval - (GDK_KEY_F1 - 1),
                    withctrl ? " ctrl" : "", withshift ? " shift" : "");
#warning should handle the function key
      return false;
    }
  return false;                 // propagate the event
}                               /* end handlekeypresscmd_BM */


void
populatepopup_newgui_cmd_BM (GtkTextView * txview, GtkWidget * popup,
                             gpointer data)
{
  assert (txview == GTK_TEXT_VIEW (commandview_BM));
  assert (GTK_IS_MENU (popup));
  assert (data == NULL);
  char cursinfobuf[32];
  memset (cursinfobuf, 0, sizeof (cursinfobuf));
  GtkTextIter cursit = EMPTY_TEXT_ITER_BM;
  gtk_text_buffer_get_iter_at_mark      //
    (commandbuf_BM, &cursit, gtk_text_buffer_get_insert (commandbuf_BM));
  snprintf (cursinfobuf, sizeof (cursinfobuf), "* L%dC%d/%d",
            gtk_text_iter_get_line (&cursit) + 1,
            gtk_text_iter_get_line_offset (&cursit),
            gtk_text_iter_get_offset (&cursit));
  gtk_menu_shell_append (GTK_MENU_SHELL (popup),
                         gtk_separator_menu_item_new ());
  {
    GtkWidget *cursinfomenit =  //
      gtk_menu_item_new_with_label (cursinfobuf);
    gtk_widget_set_sensitive (cursinfomenit, false);
    gtk_menu_shell_append (GTK_MENU_SHELL (popup), cursinfomenit);
  }
  gtk_menu_shell_prepend (GTK_MENU_SHELL (popup),
                          gtk_separator_menu_item_new ());
  {
    GtkWidget *clearmenit =     //
      gtk_menu_item_new_with_label ("clear command");
    gtk_menu_shell_prepend (GTK_MENU_SHELL (popup), clearmenit);
    g_signal_connect (clearmenit, "activate",
                      G_CALLBACK (clear_command_BM), NULL);
  }
  {
    GtkWidget *runerasemenit =  //
      gtk_menu_item_new_with_label ("run & erase");
    gtk_menu_shell_prepend (GTK_MENU_SHELL (popup), runerasemenit);
    g_signal_connect (runerasemenit, "activate",
                      G_CALLBACK (run_then_erase_newgui_command_BM), NULL);
  }
  {
    GtkWidget *runkeepmenit =   //
      gtk_menu_item_new_with_label ("run & keep");
    gtk_menu_shell_prepend (GTK_MENU_SHELL (popup), runkeepmenit);
    g_signal_connect (runkeepmenit, "activate",
                      G_CALLBACK (run_then_keep_newgui_command_BM), NULL);
  }
  gtk_widget_show_all (popup);
}                               /* end populatepopup_newgui_cmd_BM */



void
run_then_erase_newgui_command_BM (void)
{
  runcommand_newgui_BM (true);
}                               /* end run_then_erase_newgui_command_BM */

void
run_then_keep_newgui_command_BM (void)
{
  runcommand_newgui_BM (false);
}                               /* end run_then_keep_newgui_command_BM */

void
initialize_newgui_BM (const char *builderfile, const char *cssfile)
{
  if (!builderfile)
    builderfile = "bismon.ui";
  if (!cssfile)
    cssfile = "bismon.css";
  browserdepth_BM = 6;
  GtkBuilder *bld = gtk_builder_new_from_file (builderfile);
  cssprovider_newgui_bm = gtk_css_provider_get_default ();
  g_signal_connect (cssprovider_newgui_bm, "parsing-error",
                    G_CALLBACK (cssparsingerror_BM), NULL);
  gtk_css_provider_load_from_path (cssprovider_newgui_bm, cssfile, NULL);
  initialize_gui_tags_BM (bld);
  //gtk_builder_add_callback_symbols (bld, "quitaction_BM", quit_BM, NULL);
  mainwin_BM = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_style_context_add_provider_for_screen
    (gtk_window_get_screen (GTK_WINDOW (mainwin_BM)),
     GTK_STYLE_PROVIDER (cssprovider_newgui_bm),
     GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
  ////////////////
  GtkWidget *mainvbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 5);
  gtk_container_add (GTK_CONTAINER (mainwin_BM), mainvbox);
  ///////////////
  initialize_gui_menu_BM (mainvbox, bld);
  GtkWidget *paned = gtk_paned_new (GTK_ORIENTATION_VERTICAL);
  gtk_paned_set_wide_handle (GTK_PANED (paned), true);
  gtk_paned_set_position (GTK_PANED (paned), 250);
  gtk_box_pack_start (GTK_BOX (mainvbox), paned, BOXEXPAND_BM, BOXFILL_BM, 2);
  //
  GtkWidget *commandscrolw = initialize_newgui_command_scrollview_BM ();
  GtkWidget *logscrolw = initialize_log_scrollview_BM ();
  gtk_paned_add1 (GTK_PANED (paned), commandscrolw);
  gtk_paned_add2 (GTK_PANED (paned), logscrolw);
  ///
  {
    windowvalues_newgui_bm = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_style_context_add_provider_for_screen
      (gtk_window_get_screen (GTK_WINDOW (windowvalues_newgui_bm)),
       GTK_STYLE_PROVIDER (cssprovider_newgui_bm),
       GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    GtkWidget *valuesvbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 3);
    gtk_container_add (GTK_CONTAINER (windowvalues_newgui_bm), valuesvbox);
    valueslabel_newgui_bm = gtk_label_new ("values");
    gtk_box_pack_start (GTK_BOX (valuesvbox), valueslabel_newgui_bm,
                        BOXNOEXPAND_BM, BOXFILL_BM, 2);
    GtkWidget *sep1 = gtk_separator_new (GTK_ORIENTATION_HORIZONTAL);
    gtk_box_pack_start (GTK_BOX (valuesvbox), sep1,
                        BOXNOEXPAND_BM, BOXNOFILL_BM, 2);
    GtkWidget *paned = gtk_paned_new (GTK_ORIENTATION_VERTICAL);
    gtk_box_pack_start (GTK_BOX (valuesvbox), paned, BOXEXPAND_BM, BOXFILL_BM,
                        2);
    upperscrollwvalues_newgui_bm = gtk_scrolled_window_new (NULL, NULL);
    lowerscrollwvalues_newgui_bm = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW
                                    (upperscrollwvalues_newgui_bm),
                                    GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW
                                    (lowerscrollwvalues_newgui_bm),
                                    GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
    gtk_paned_add1 (GTK_PANED (paned), upperscrollwvalues_newgui_bm);
    gtk_paned_add2 (GTK_PANED (paned), lowerscrollwvalues_newgui_bm);
    gtk_paned_set_wide_handle (GTK_PANED (paned), true);
    uppervboxvalues_newgui_bm = gtk_box_new (GTK_ORIENTATION_VERTICAL, 3);
    lowervboxvalues_newgui_bm = gtk_box_new (GTK_ORIENTATION_VERTICAL, 3);
    gtk_container_add (GTK_CONTAINER (upperscrollwvalues_newgui_bm),
                       uppervboxvalues_newgui_bm);
    GtkWidget *upperlab = gtk_label_new ("upper");
    gtk_box_pack_start (GTK_BOX (uppervboxvalues_newgui_bm), upperlab,
                        BOXNOEXPAND_BM, BOXNOFILL_BM, 2);
    gtk_container_add (GTK_CONTAINER (lowerscrollwvalues_newgui_bm),
                       lowervboxvalues_newgui_bm);
    gtk_box_pack_start (GTK_BOX (lowervboxvalues_newgui_bm),
                        gtk_separator_new (GTK_ORIENTATION_HORIZONTAL),
                        BOXNOEXPAND_BM, BOXNOFILL_BM, 1);
    GtkWidget *lowerlab = gtk_label_new ("lower");
    gtk_box_pack_start (GTK_BOX (lowervboxvalues_newgui_bm), lowerlab,
                        BOXNOEXPAND_BM, BOXNOFILL_BM, 2);
    gtk_paned_set_position (GTK_PANED (paned), 220);
    gtk_window_set_title (GTK_WINDOW (windowvalues_newgui_bm),
                          "bismon values");
    gtk_window_set_default_size (GTK_WINDOW (windowvalues_newgui_bm), 550,
                                 620);
    g_signal_connect (windowvalues_newgui_bm, "delete-event",
                      (GCallback) deletemainwin_BM, NULL);
    gtk_widget_show_all (GTK_WIDGET (windowvalues_newgui_bm));
  }
  ///
  gtk_window_set_title (GTK_WINDOW (mainwin_BM), "new-bismon");
  gtk_window_set_default_size (GTK_WINDOW (mainwin_BM), 650, 720);
  // perhaps run the GC twice a second
  g_timeout_add (500, guiperiodicgarbagecollection_BM, NULL);
  gtk_widget_show_all (GTK_WIDGET (mainwin_BM));
  obwin_current_newgui_BM = make_obwin_newgui_BM ();
}                               /* end initialize_newgui_BM */


GtkTextBuffer *
newgui_get_browsebuf_BM (void)
{
  fprintf (stderr, "newgui_get_browsebuf_BM unimplemented\n");
#warning newgui_get_browsebuf_BM unimplemented
  return NULL;
}                               /* end newgui_get_browsebuf_BM */


void
runcommand_newgui_BM (bool erase)
{
  LOCALFRAME_BM ( /*prev: */ NULL, /*descr: */ NULL,
                 objectval_tyBM * cmdparsob;);
  DBGPRINTF_BM ("runcommand_newgui_BM start erase=%s", erase ? "yes" : "no");
  GtkTextIter startit = EMPTY_TEXT_ITER_BM;
  GtkTextIter endit = EMPTY_TEXT_ITER_BM;
  cmd_clear_parens_BM ();
  if (errormessagedialog_BM)
    {
      gtk_widget_destroy (errormessagedialog_BM), errormessagedialog_BM =
        NULL;
    };
  gtk_text_buffer_get_bounds (commandbuf_BM, &startit, &endit);
  gtk_text_buffer_remove_all_tags (commandbuf_BM, &startit, &endit);
  char *cmdstr = gtk_text_buffer_get_text (commandbuf_BM, &startit, &endit,
                                           false);
  bool gotffortab = false;
  // replace tabs and formfeeds
  for (char *pc = cmdstr; *pc; pc++)
    {
      if (*pc == '\t')
        {
          gotffortab = true;
          *pc = ' ';
        }
      else if (*pc == '\f')
        {
          gotffortab = true;
          *pc = '\n';
        };
    };
  if (gotffortab)
    {
      gtk_text_buffer_set_text (commandbuf_BM, cmdstr, -1);
    }
  _.cmdparsob = makeobj_BM ();
  struct parser_stBM *cmdpars =
    makeparser_memopen_BM (cmdstr, -1, _.cmdparsob);
  int cmdlen = strlen (cmdstr);
  DBGPRINTF_BM
    ("runcommand_newgui cmdparsob=%s cmdlen=%d, @@@cmdstr=\n%s\n\n",
     objectdbg_BM (_.cmdparsob), cmdlen, cmdstr);
  cmdpars->pars_ops = &parsop_command_build_newgui_BM;
  volatile int errpars = setjmp (jmperrorcmd_BM);
  if (!errpars)
    {
      // should parse the command buffer, this could longjmp to jmperrorcmd_BM
      parsecommandbuf_newgui_BM (cmdpars, (struct stackframe_stBM *) &_);
      commandnumber_BM++;
      serial63_tyBM sercmd = randomserial63_BM ();
      char serbuf[16];
      memset (serbuf, 0, sizeof (serbuf));
      serial63tocbuf16_BM (sercmd, serbuf);
      if (gui_command_log_file_BM)
        {
          fprintf (gui_command_log_file_BM, "///++%s command #%d,l%d:\n",
                   serbuf, commandnumber_BM, cmdlen);
          fputs (cmdstr, gui_command_log_file_BM);
          if (cmdlen > 0 && cmdstr[cmdlen - 1] != '\n')
            putc ('\n', gui_command_log_file_BM);
          fprintf (gui_command_log_file_BM, "///--%s end command #%d\n\f\n",
                   serbuf, commandnumber_BM);
          fflush (gui_command_log_file_BM);
        }
      log_begin_message_BM ();
      log_printf_message_BM
        ("run %s command #%d successfully:\n",
         erase ? "erased" : "kept", commandnumber_BM);
      char commbuf[80];
      memset (commbuf, 0, sizeof (commbuf));
      snprintf (commbuf, sizeof (commbuf), "///++%s command #%d,l%d:",
                serbuf, commandnumber_BM, cmdlen);
      GtkTextIter it = EMPTY_TEXT_ITER_BM;
      gtk_text_buffer_get_end_iter (logbuf_BM, &it);
      gtk_text_buffer_insert_with_tags
        (logbuf_BM, &it, commbuf, -1, comment_logtag_BM, NULL);
      gtk_text_buffer_insert (logbuf_BM, &it, "\n", -1);
      gtk_text_buffer_insert_with_tags
        (logbuf_BM, &it, cmdstr, -1, command_logtag_BM, NULL);
      if (cmdlen > 0 && cmdstr[cmdlen - 1] != '\n')
        gtk_text_buffer_insert (logbuf_BM, &it, "\n", -1);
      snprintf (commbuf, sizeof (commbuf), "///--%s end command #%d",
                serbuf, commandnumber_BM);
      gtk_text_buffer_insert_with_tags
        (logbuf_BM, &it, commbuf, -1, comment_logtag_BM, NULL);
      gtk_text_buffer_insert (logbuf_BM, &it, "\n", -1);
      log_end_message_BM ();
    }
  else                          /* error */
    {
      // the errormessagedialog_BM was created in parserrorcmd_BM
      DBGPRINTF_BM ("runcommand_newgui errpars %d", errpars);
      if (errormessagedialog_BM)
        {
          gtk_dialog_run (GTK_DIALOG (errormessagedialog_BM));
          gtk_widget_destroy (errormessagedialog_BM), errormessagedialog_BM =
            NULL;
        }
      free (cmdstr), cmdstr = NULL;
      return;
    }
  free (cmdstr), cmdstr = NULL;
  if (erase)
    gtk_text_buffer_set_text (commandbuf_BM, "", 0);
}                               /* end runcommand_newgui_BM */


void
markset_newgui_cmd_BM (GtkTextBuffer * tbuf, GtkTextIter * titer,
                       GtkTextMark * tmark, gpointer cdata)
{
  DBGPRINTF_BM ("markset_newgui titer=%s", textiterstrdbg_BM (titer));
}                               /* end markset_newgui_cmd_BM */


void
enduseraction_newgui_cmd_BM (GtkTextBuffer * txbuf, gpointer data)
{
  LOCALFRAME_BM ( /*prev: */ NULL, /*descr: */ NULL,
                 objectval_tyBM * cmdparsob;);
  assert (txbuf == commandbuf_BM);
  assert (data == NULL);
  GtkTextIter startit = EMPTY_TEXT_ITER_BM;
  GtkTextIter endit = EMPTY_TEXT_ITER_BM;
  gtk_text_buffer_get_bounds (commandbuf_BM, &startit, &endit);
  gtk_text_buffer_remove_all_tags (commandbuf_BM, &startit, &endit);
  cmd_clear_parens_BM ();
  char *cmdstr = gtk_text_buffer_get_text (commandbuf_BM, &startit, &endit,
                                           false);
  _.cmdparsob = makeobj_BM ();
  struct parser_stBM *cmdpars =
    makeparser_memopen_BM (cmdstr, -1, _.cmdparsob);
  cmdpars->pars_ops = &parsop_command_nobuild_newgui_BM;
  volatile int errpars = setjmp (jmperrorcmd_BM);
  if (!errpars)
    {
      // should parse the command buffer
      parsecommandbuf_newgui_BM (cmdpars, (struct stackframe_stBM *) &_);
      DBGPRINTF_BM ("newgui command parsed ok");
    }
  else
    {
      // got an error while parsing
      DBGPRINTF_BM ("newgui command parserror %d", errpars);
    }
  free (cmdstr), cmdstr = NULL;
  // for parenthesis blinking
  GtkTextMark *insmk = gtk_text_buffer_get_insert (commandbuf_BM);
  GtkTextIter insit = EMPTY_TEXT_ITER_BM;
  gtk_text_buffer_get_iter_at_mark (commandbuf_BM, &insit, insmk);
  markset_newgui_cmd_BM (commandbuf_BM, &insit, insmk, NULL);
}                               /* end enduseraction_newgui_cmd_BM */



void
parsecommandbuf_newgui_BM (struct parser_stBM *pars,
                           struct stackframe_stBM *stkf)
{
  if (!isparser_BM (pars))
    return;
  LOCALFRAME_BM ( /*prev: */ stkf, /*descr: */ NULL,
                 value_tyBM val;
                 const stringval_tyBM * astrv; objectval_tyBM * obj;
                 const stringval_tyBM * name; const stringval_tyBM * result;
                 objectval_tyBM * parsob;
    );
  if (browserdepth_BM < 2)
    browserdepth_BM = 2;
  else if (browserdepth_BM > BROWSE_MAXDEPTH_NEWGUI_BM)
    browserdepth_BM = BROWSE_MAXDEPTH_NEWGUI_BM;
  if (!astrval_bm || !isstring_BM ((value_tyBM) astrval_bm)
      || strcmp (bytstring_BM (astrval_bm), "a"))
    {
      _.astrv = makestring_BM ("a");
      astrval_bm = _.astrv;
    }
  else
    _.astrv = astrval_bm;
  objectval_tyBM *k_nval = BMK_5xGpTXhdqX1_1aVTq1TZOXX;
  objectval_tyBM *k_depth = BMK_17YdW6dWrBA_2mn4QmBjMNs;
  objectval_tyBM *k_nhide = BMK_5mgZTJ64WH9_4r2XC8eZmW7;
  _.parsob = checkedparserowner_BM (pars);
  const struct parserops_stBM *parsops = pars->pars_ops;
  assert (parsops && parsops->parsop_magic == PARSOPMAGIC_BM);
  bool nobuild = parsops->parsop_nobuild;
  int nbloop = 0;
  for (;;)
    {
      _.name = NULL;
      _.obj = NULL;
      _.result = NULL;
      _.val = NULL;
      parserskipspaces_BM (pars, (struct stackframe_stBM *) &_);
      if (nbloop++ > MAXSIZE_BM / 32)
        parsererrorprintf_BM (pars, (struct stackframe_stBM *) &_,
                              pars->pars_lineno, pars->pars_colpos,
                              "too many %d loops", nbloop);
      if (parserendoffile_BM (pars))
        break;
      unsigned curlineno = parserlineno_BM (pars);
      unsigned curcolpos = parsercolpos_BM (pars);
      parstoken_tyBM tok =
        parsertokenget_BM (pars, (struct stackframe_stBM *) &_);
      DBGPRINTF_BM ("parsecommandbuf_newgui_BM nbloop#%d tok~%s L%dC%d",
                    nbloop, lexkindname_BM (tok.tok_kind), curlineno,
                    curcolpos);
      //commands starting with comma?
      if (tok.tok_kind == plex_DELIM && tok.tok_delim == delim_comma)
        {
          parstoken_tyBM cmdtok =
            parsertokenget_BM (pars, (struct stackframe_stBM *) &_);
          // ,depth <number>
          if (cmdtok.tok_kind == plex_NAMEDOBJ
              && cmdtok.tok_namedobj == k_depth)
            {
              parstoken_tyBM depthtok =
                parsertokenget_BM (pars, (struct stackframe_stBM *) &_);
              if (depthtok.tok_kind != plex_LLONG)
                parsererrorprintf_BM (pars, (struct stackframe_stBM *) &_,
                                      cmdtok.tok_line, cmdtok.tok_col,
                                      ",depth not followed by number");
              if (depthtok.tok_llong < 2
                  || depthtok.tok_llong > BROWSE_MAXDEPTH_NEWGUI_BM)
                parsererrorprintf_BM (pars, (struct stackframe_stBM *) &_,
                                      cmdtok.tok_line, cmdtok.tok_col,
                                      "bad ,depth %lld, should be between 2 and %d",
                                      depthtok.tok_llong,
                                      BROWSE_MAXDEPTH_NEWGUI_BM);
              browserdepth_BM = (int) depthtok.tok_llong;
            }
          // ,nval <name> <value>
          else if (cmdtok.tok_kind == plex_NAMEDOBJ
                   && cmdtok.tok_namedobj == k_nval)
            {
              parstoken_tyBM vartok =
                parsertokenget_BM (pars, (struct stackframe_stBM *) &_);
              if (vartok.tok_kind == plex_NAMEDOBJ)
                _.name =
                  makestring_BM (findobjectname_BM (vartok.tok_namedobj));
              else if (vartok.tok_kind == plex_CNAME)
                _.name = vartok.tok_cname;
              else
                parsererrorprintf_BM (pars, (struct stackframe_stBM *) &_,
                                      cmdtok.tok_line, cmdtok.tok_col,
                                      "name expected after ,nval");
              bool gotval = false;
              _.val = parsergetvalue_BM (pars, (struct stackframe_stBM *) &_,
                                         0, &gotval);
              if (!gotval)
                parsererrorprintf_BM (pars, (struct stackframe_stBM *) &_,
                                      cmdtok.tok_line, cmdtok.tok_col,
                                      "value expected after ,nval %s",
                                      bytstring_BM (_.name));
              if (!nobuild)
                {
                  if (_.val)
                    {
                      browse_named_value_newgui_BM (_.name, _.val,
                                                    browserdepth_BM,
                                                    (struct stackframe_stBM *)
                                                    &_);
                      log_begin_message_BM ();
                      log_printf_message_BM
                        ("show value named %s at depth %d",
                         bytstring_BM (_.name), browserdepth_BM);
                      log_end_message_BM ();
                    }
                  else
                    {
                      hide_named_value_newgui_BM (bytstring_BM (_.name),
                                                  (struct stackframe_stBM *)
                                                  &_);
                      log_begin_message_BM ();
                      log_printf_message_BM ("forgot value named %s",
                                             bytstring_BM (_.name));
                      log_end_message_BM ();
                    }

                }
            }
          /// ,nhide <name>
          else if (cmdtok.tok_kind == plex_NAMEDOBJ
                   && cmdtok.tok_namedobj == k_nhide)
            {
              parstoken_tyBM vartok =
                parsertokenget_BM (pars, (struct stackframe_stBM *) &_);
              if (vartok.tok_kind == plex_NAMEDOBJ)
                _.name =
                  makestring_BM (findobjectname_BM (vartok.tok_namedobj));
              else if (vartok.tok_kind == plex_CNAME)
                _.name = vartok.tok_cname;
              else
                parsererrorprintf_BM (pars, (struct stackframe_stBM *) &_,
                                      cmdtok.tok_line, cmdtok.tok_col,
                                      "name expected after ,nhide");
              if (!nobuild)
                {
                  if (index_named_value_newgui_BM (bytstring_BM (_.name)) < 0)
                    parsererrorprintf_BM (pars, (struct stackframe_stBM *) &_,
                                          cmdtok.tok_line, cmdtok.tok_col,
                                          ",nhide %s : unknown name",
                                          bytstring_BM (_.name));
                  hide_named_value_newgui_BM (bytstring_BM (_.name),
                                              (struct stackframe_stBM *) &_);
                  log_begin_message_BM ();
                  log_printf_message_BM ("forgot hidden value named %s",
                                         bytstring_BM (_.name));
                  log_end_message_BM ();
                }
            }
          else
            parsererrorprintf_BM (pars, (struct stackframe_stBM *) &_,
                                  cmdtok.tok_line, cmdtok.tok_col,
                                  "unimplemented command");

        }
      //start of object?
      else if (parsertokenstartobject_BM (pars, tok))
        {
          parserseek_BM (pars, curlineno, curcolpos);
          bool gotobj = false;
          _.obj =
            parsergetobject_BM (pars, (struct stackframe_stBM *) &_, 0,
                                &gotobj);
          if (!gotobj)
            parsererrorprintf_BM (pars, (struct stackframe_stBM *) &_,
                                  curlineno, curcolpos, "invalid object");
#warning parsecommandbuf_newgui should show the _.obj
        }
      // start of value?
      else if (parsertokenstartvalue_BM (pars, tok))
        {
          parserseek_BM (pars, curlineno, curcolpos);
          bool gotval = false;
          _.val =
            parsergetvalue_BM (pars, (struct stackframe_stBM *) &_, 0,
                               &gotval);
          if (!gotval)
            parsererrorprintf_BM (pars, (struct stackframe_stBM *) &_,
                                  curlineno, curcolpos, "invalid value");
          if (!nobuild)
            {
              DBGPRINTF_BM ("parsecommandbuf_newgui val=%s",
                            debug_outstr_value_BM (_.val,
                                                   (struct stackframe_stBM *)
                                                   &_, 5));
              if (_.val)
                browse_named_value_newgui_BM (_.astrv, _.val, browserdepth_BM,
                                              (struct stackframe_stBM *) &_);
              else
                {
                  log_begin_message_BM ();
                  log_printf_message_BM ("no value");
                  log_end_message_BM ();
                }
            }
        }
      else if (tok.tok_kind == plex__NONE)
        parsererrorprintf_BM (pars, (struct stackframe_stBM *) &_,
                              curlineno, curcolpos, "invalid token");


    }
}                               /* end parsecommandbuf_newgui_BM */



// for $:<var>
const objectval_tyBM *
parsdollarobj_newguicmd_BM (struct parser_stBM *pars,
                            unsigned lineno, unsigned colpos,
                            const value_tyBM varname,
                            struct stackframe_stBM *stkf)
{

  LOCALFRAME_BM ( /*prev: */ stkf, /*descr: */ NULL,
                 value_tyBM val;);
  const struct parserops_stBM *parsops = pars->pars_ops;
  bool nobuild = parsops && parsops->parsop_nobuild;
  const char *varstr = NULL;
  if (isstring_BM (varname))
    varstr = bytstring_BM (varname);
  else if (isobject_BM (varname))
    varstr = findobjectname_BM (varname);
  if (!varstr)
    parsererrorprintf_BM (pars, (struct stackframe_stBM *) &_, lineno, colpos,
                          "invalid $:<var>");
  _.val = find_named_value_newgui_BM (varstr, (struct stackframe_stBM *) &_);
  if (!_.val && !nobuild)
    parsererrorprintf_BM (pars, (struct stackframe_stBM *) &_, lineno, colpos,
                          "not found $:%s", varstr);
  if (!isobject_BM (_.val) && !nobuild)
    parsererrorprintf_BM (pars, (struct stackframe_stBM *) &_, lineno, colpos,
                          "non-object $:%s", varstr);
  GtkTextIter it = EMPTY_TEXT_ITER_BM, endit = EMPTY_TEXT_ITER_BM;
  gtk_text_buffer_get_iter_at_line (commandbuf_BM, &it, lineno - 1);
  gtk_text_iter_forward_chars (&it, colpos);
  endit = it;
  gtk_text_iter_forward_chars (&endit, 2 + strlen (varstr));
  gtk_text_buffer_apply_tag (commandbuf_BM, dollar_cmdtag_BM, &it, &endit);
  return (const objectval_tyBM *) _.val;
}                               /* end parsdollarobj_newguicmd_BM */

// for $<var>
value_tyBM
parsdollarval_newguicmd_BM (struct parser_stBM *pars,
                            unsigned lineno,
                            unsigned colpos,
                            const value_tyBM varname,
                            struct stackframe_stBM *stkf)
{
  const char *varstr = NULL;
  LOCALFRAME_BM ( /*prev: */ stkf, /*descr: */ NULL,
                 value_tyBM val;);
  assert (isparser_BM (pars));
  const struct parserops_stBM *parsops = pars->pars_ops;
  bool nobuild = parsops && parsops->parsop_nobuild;
  if (isstring_BM (varname))
    varstr = bytstring_BM (varname);
  else if (isobject_BM (varname))
    varstr = findobjectname_BM (varname);
  if (!varstr)
    parsererrorprintf_BM (pars, (struct stackframe_stBM *) &_, lineno, colpos,
                          "invalid $<var>");
  _.val = find_named_value_newgui_BM (varstr, (struct stackframe_stBM *) &_);
  if (!_.val && !nobuild)
    parsererrorprintf_BM (pars, (struct stackframe_stBM *) &_, lineno, colpos,
                          "not found $%s", varstr);
  GtkTextIter it = EMPTY_TEXT_ITER_BM, endit = EMPTY_TEXT_ITER_BM;
  gtk_text_buffer_get_iter_at_line (commandbuf_BM, &it, lineno - 1);
  gtk_text_iter_forward_chars (&it, colpos);
  endit = it;
  gtk_text_iter_forward_chars (&endit, 1 + strlen (varstr));
  gtk_text_buffer_apply_tag (commandbuf_BM, dollar_cmdtag_BM, &it, &endit);
  return _.val;
}                               /* end parsdollarval_newguicmd_BM */


// parse inside $(....)
value_tyBM
parsvalexp_newguicmd_BM (struct parser_stBM * pars, unsigned lineno,
                         unsigned colpos, int depth,
                         struct stackframe_stBM * stkf)
{
  assert (isparser_BM (pars));
  const struct parserops_stBM *parsops = pars->pars_ops;
  bool nobuild = parsops && parsops->parsop_nobuild;
  LOCALFRAME_BM ( /*prev: */ stkf, /*descr: */ NULL,
                 value_tyBM resval;
                 value_tyBM srcval; objectval_tyBM * parsob;
                 objectval_tyBM * obj; objectval_tyBM * obsel;
                 objectval_tyBM * obattr; closure_tyBM * clos;
                 value_tyBM otherval; const stringval_tyBM * name;);
  _.parsob = checkedparserowner_BM (pars);
  unsigned srclineno = parserlineno_BM (pars);
  unsigned srccolpos = parsercolpos_BM (pars);
  parsererrorprintf_BM (pars, (struct stackframe_stBM *) &_, srclineno,
                        srccolpos,
                        "unimplemented parsvalexp_newguicmd for $(...)");
#warning parsvalexp_newguicmd_BM unimplemented
  return NULL;
}                               /* end parsvalexp_newguicmd_BM */


// parse inside $[...]
const objectval_tyBM *
parsobjexp_newguicmd_BM (struct parser_stBM *pars,
                         unsigned lineno, unsigned colpos,
                         int depth, struct stackframe_stBM *stkf)
{
  const struct parserops_stBM *parsops = pars->pars_ops;
  bool nobuild = parsops && parsops->parsop_nobuild;
  LOCALFRAME_BM ( /*prev: */ stkf, /*descr: */ NULL,
                 objectval_tyBM * obj;
                 const stringval_tyBM * namev; objectval_tyBM * oldnamedob;
                 value_tyBM val;);
  assert (isparser_BM (pars));
  parserskipspaces_BM (pars, (struct stackframe_stBM *) &_);
  unsigned oblineno = parserlineno_BM (pars);
  unsigned obcolpos = parsercolpos_BM (pars);
  parsererrorprintf_BM (pars, (struct stackframe_stBM *) &_, oblineno,
                        obcolpos,
                        "unimplemented parsobjexp_newguicmd for $(...)");
#warning parsobjexp_newguicmd_BM unimplemented
  return NULL;
}                               /* end parsobjexp_newguicmd_BM */


static int
index_named_value_newgui_BM (const char *vstr)
{
  if (!vstr || !vstr[0])
    return -1;
  if (browsednvulen_BM == 0)
    return -1;
  assert (browsednvulen_BM <= browsednvsize_BM);
  assert (browsednvulen_BM < MAXSIZE_BM);
  assert (browsedval_BM != NULL);
  int lo = 0, hi = (int) browsednvulen_BM, md = 0;
  while (lo + 4 < hi)
    {
      md = (lo + hi) / 2;
      struct browsedval_stBM *mdbv = browsedval_BM + md;
      assert (isstring_BM ((const value_tyBM) (mdbv->brow_name)));
      int cmp = strcmp (vstr, bytstring_BM (mdbv->brow_name));
      if (cmp == 0)
        return md;
      else if (cmp < 0)
        hi = md;
      else
        lo = md;
    }
  for (md = lo; md < hi; md++)
    {
      struct browsedval_stBM *mdbv = browsedval_BM + md;
      assert (isstring_BM ((value_tyBM) mdbv->brow_name));
      if (!strcmp (vstr, bytstring_BM (mdbv->brow_name)))
        return md;
    }
  return -1;
}                               /* end index_named_value_newgui_BM */

value_tyBM
find_named_value_newgui_BM (const char *vstr, struct stackframe_stBM * stkf)
{
  LOCALFRAME_BM ( /*prev: */ stkf, /*descr: */ NULL,
                 value_tyBM val;);
  if (!validname_BM (vstr))
    return NULL;
  if (browsednvulen_BM == 0)
    return NULL;
  int ix = index_named_value_newgui_BM (vstr);
  if (ix < 0)
    return NULL;
  _.val = browsedval_BM[ix].brow_val;
  return _.val;
}                               /* end find_named_value_newgui_BM */

static void
add_indexed_named_value_newgui_BM (const stringval_tyBM * namev,
                                   const value_tyBM val,
                                   int browsdepth,
                                   unsigned index,
                                   struct stackframe_stBM *stkf);

static void
replace_indexed_named_value_newgui_BM (const value_tyBM val,
                                       int browsdepth,
                                       unsigned index,
                                       struct stackframe_stBM *stkf);

static void
browse_indexed_named_value_newgui_BM (const value_tyBM val,
                                      int browsdepth,
                                      unsigned index,
                                      struct stackframe_stBM *stkf);

void
browse_named_value_newgui_BM (const stringval_tyBM * namev,
                              const value_tyBM val,
                              int browsdepth, struct stackframe_stBM *stkf)
{
  LOCALFRAME_BM ( /*prev: */ stkf, /*descr: */ NULL,
                 const stringval_tyBM * namev;
                 value_tyBM val;);
  if (browsdepth < 2)
    browsdepth = 2;
  else if (browsdepth > BROWSE_MAXDEPTH_NEWGUI_BM)
    browsdepth = BROWSE_MAXDEPTH_NEWGUI_BM;
  _.namev = namev;
  _.val = val;
  if (!isstring_BM ((value_tyBM) namev))
    return;
  if (!validname_BM (bytstring_BM ((value_tyBM) namev)))
    return;
  if (!val)
    return;
  DBGPRINTF_BM ("browse_named_value_newgui start name: %s depth %d ulen %u",
                bytstring_BM ((value_tyBM) _.namev), browsdepth,
                browsednvulen_BM);
  if (browsednvulen_BM == 0)
    {
      if (!browsedval_BM)
        {
          const unsigned inisiz = 11;
          browsedval_BM = calloc (inisiz, sizeof (struct browsedval_stBM));
          if (!browsedval_BM)
            FATAL_BM ("failed to allocate browsedval_BM for %u elements",
                      inisiz);
          browsednvsize_BM = inisiz;
        }
      browsednvulen_BM = 1;
      memset (browsedval_BM + 0, 0, sizeof (struct browsedval_stBM));
      add_indexed_named_value_newgui_BM //
        (_.namev, _.val, browsdepth, 0, (struct stackframe_stBM *) &_);
      DBGPRINTF_BM ("browse_named_value_newgui (empty) end name: %s",
                    bytstring_BM (_.namev));
      return;
    };
  // grow array if needed
  if (browsednvulen_BM >= browsednvsize_BM)
    {
      unsigned newsiz = prime_above_BM (4 * browsednvsize_BM / 3 + 8);
      struct browsedval_stBM *newarr =
        calloc (newsiz, sizeof (struct browsedval_stBM));
      if (!newarr)
        FATAL_BM ("failed to grow browsedval_BM for %u elements", newsiz);
      memcpy (newarr, browsedval_BM,
              browsednvulen_BM * sizeof (struct browsedval_stBM));
      free (browsedval_BM), (browsedval_BM = newarr);
      browsednvsize_BM = newsiz;
      DBGPRINTF_BM
        ("browse_named_value_newgui grow name: %s nvsize %u ulen %u",
         bytstring_BM (_.namev), browsednvsize_BM, browsednvulen_BM);
    }
  int lo = 0, hi = (int) browsednvulen_BM, md = 0;
  while (lo + 4 < hi)
    {
      md = (lo + hi) / 2;
      struct browsedval_stBM *mdbv = browsedval_BM + md;
      assert (isstring_BM ((value_tyBM) mdbv->brow_name));
      int cmp = strcmp (bytstring_BM ((value_tyBM) _.namev),
                        bytstring_BM ((value_tyBM) mdbv->brow_name));
      if (cmp == 0)
        {
          replace_indexed_named_value_newgui_BM //
            (_.val, browsdepth, (unsigned) md, (struct stackframe_stBM *) &_);
          DBGPRINTF_BM
            ("browse_named_value_newgui end replaced name: %s md %d ulen %u",
             bytstring_BM (_.namev), md, browsednvulen_BM);
          return;
        }
      else if (cmp < 0)
        hi = md;
      else
        lo = md;
    }
  for (md = lo; md < hi; md++)
    {
      struct browsedval_stBM *mdbv = browsedval_BM + md;
      assert (isstring_BM ((value_tyBM) mdbv->brow_name));
      int cmp = strcmp (bytstring_BM ((value_tyBM) _.namev),
                        bytstring_BM ((value_tyBM) mdbv->brow_name));
      if (!cmp)
        {
          replace_indexed_named_value_newgui_BM //
            (_.val, browsdepth, (unsigned) md, (struct stackframe_stBM *) &_);
          DBGPRINTF_BM
            ("browse_named_value_newgui end replaced name: %s md %d ulen %u",
             bytstring_BM ((value_tyBM) _.namev), md, browsednvulen_BM);
          return;
        }
      else if (cmp > 0)
        break;
    }
  // insert before md
  assert (md >= 0);
  DBGPRINTF_BM
    ("browse_named_value_newgui lo=%d, hi=%d, md=%d, ulen=%d, name=%s", lo,
     hi, md, browsednvulen_BM, bytstring_BM (_.namev));
  for (int ix = browsednvulen_BM; ix > md; ix--)
    {
      browsedval_BM[ix] = browsedval_BM[ix - 1];
      struct namedvaluenewguixtra_stBM *xp =
        (struct namedvaluenewguixtra_stBM *) (browsedval_BM[ix].brow_vdata);
      if (xp)
        {
          assert (xp->nvx_index == ix - 1);
          xp->nvx_index = ix;
        }
    }
  memset (browsedval_BM + md, 0, sizeof (struct browsedval_stBM));
  browsednvulen_BM++;
  add_indexed_named_value_newgui_BM     //
    (_.namev, _.val, browsdepth, md, (struct stackframe_stBM *) &_);
  DBGPRINTF_BM ("browse_named_value_newgui end added name: %s md %d ulen %u",
                bytstring_BM (_.namev), md, browsednvulen_BM);
  return;
}                               /* end browse_named_value_newgui_BM */





static void closebut_namedval_newgui_cbBM (GtkWidget * wbut, gpointer data);
static void spindepth_namedval_newgui_cbBM (GtkSpinButton * spbut,
                                            gpointer data);
void
closebut_namedval_newgui_cbBM (GtkWidget * wbut, gpointer data)
{
  assert (GTK_IS_BUTTON (wbut));
  assert (data != NULL);
  struct namedvaluenewguixtra_stBM *nvx = data;
  int idx = nvx->nvx_index;
  DBGPRINTF_BM ("closebut_namedval_newgui idx=%d", idx);
  assert (idx >= 0 && idx <= (int) browsednvulen_BM
          && idx < (int) browsednvsize_BM);
  assert (browsedval_BM[idx].brow_vdata == (void *) nvx);
  hide_index_named_value_newgui_BM (idx, NULL);
}                               /* end closebut_namedval_newgui_cbBM */

void
spindepth_namedval_newgui_cbBM (GtkSpinButton * spbut, gpointer data)
{
  assert (GTK_IS_SPIN_BUTTON (spbut));
  assert (data != NULL);
  struct namedvaluenewguixtra_stBM *nvx = data;
  int idx = nvx->nvx_index;
  assert (idx >= 0 && idx <= (int) browsednvulen_BM
          && idx < (int) browsednvsize_BM);
  assert (browsedval_BM[idx].brow_vdata == (void *) nvx);
  double d = gtk_spin_button_get_value (spbut);
  int newdepth = (int) d;
  DBGPRINTF_BM ("spindepth_namedval_newgui_cbBM idx=%d d=%.3f = %d", idx, d,
                newdepth);
  browsedval_BM[idx].brow_vdepth = newdepth;
  browse_indexed_named_value_newgui_BM (browsedval_BM[idx].brow_val, newdepth,
                                        idx, NULL);
  if (spbut != nvx->nvx_upper.nvxt_spindepth)
    gtk_spin_button_set_value (nvx->nvx_upper.nvxt_spindepth,
                               (double) newdepth);
  if (spbut != nvx->nvx_lower.nvxt_spindepth)
    gtk_spin_button_set_value (nvx->nvx_lower.nvxt_spindepth,
                               (double) newdepth);
  gtk_widget_show_all (nvx->nvx_upper.nvxt_frame);
  gtk_widget_show_all (nvx->nvx_lower.nvxt_frame);
#warning spindepth_namedval_newgui_cbBM unimplemented
}                               /* end spindepth_namedval_newgui_cbBM */

static void
fill_nvx_thing_newgui_BM (struct namedvaluenewguixtra_stBM *nvx, bool upper,
                          const char *title, const char *subtitle)
{
  assert (nvx != NULL);
  assert (title != NULL);
  assert (subtitle != NULL);
  struct namedvaluethings_stBM *nt =
    upper ? (&nvx->nvx_upper) : (&nvx->nvx_lower);
  int idx = nvx->nvx_index;
  DBGPRINTF_BM
    ("fill_nvx_thing_newgui %s idx=%d ulen:%u title'%s' subtitle'%s'",
     upper ? "upper" : "lower", idx, browsednvulen_BM, title, subtitle);
  assert (idx >= 0 && idx <= (int) browsednvulen_BM
          && idx < (int) browsednvsize_BM);
  assert (browsedval_BM[idx].brow_vdata == (void *) nvx);
  nt->nvxt_frame = gtk_frame_new (NULL);
  GtkBox *inbox = GTK_BOX (upper ? uppervboxvalues_newgui_bm :
                           lowervboxvalues_newgui_bm);
  gtk_box_pack_end (inbox, nt->nvxt_frame, BOXEXPAND_BM, BOXFILL_BM, 2);
  // the lower box starts with a separator, so...
  gtk_box_reorder_child         //
    (inbox, nt->nvxt_frame, upper ? idx : (idx + 1));
  nt->nvxt_vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 2);
  gtk_container_add (GTK_CONTAINER (nt->nvxt_frame), nt->nvxt_vbox);
  nt->nvxt_headb = gtk_header_bar_new ();
  GtkWidget *clobut =           //
    gtk_button_new_from_icon_name ("window-close",
                                   GTK_ICON_SIZE_MENU);
  g_signal_connect (clobut, "activate", closebut_namedval_newgui_cbBM, nvx);
  gtk_header_bar_pack_end (GTK_HEADER_BAR (nt->nvxt_headb), clobut);
  nt->nvxt_spindepth =          //
    gtk_spin_button_new_with_range (2.0,
                                    (double) BROWSE_MAXDEPTH_NEWGUI_BM, 1.0);
  g_signal_connect (nt->nvxt_spindepth, "value-changed",
                    spindepth_namedval_newgui_cbBM, nvx);
  gtk_header_bar_pack_end (GTK_HEADER_BAR (nt->nvxt_headb),
                           nt->nvxt_spindepth);
  gtk_header_bar_set_title (GTK_HEADER_BAR (nt->nvxt_headb), title);
  gtk_header_bar_set_subtitle (GTK_HEADER_BAR (nt->nvxt_headb), subtitle);
  gtk_box_pack_start (GTK_BOX (nt->nvxt_vbox), nt->nvxt_headb,
                      BOXNOEXPAND_BM, BOXNOFILL_BM, 1);
  nt->nvxt_textview = gtk_text_view_new_with_buffer (nvx->nvx_tbuffer);
  gtk_text_view_set_editable (GTK_TEXT_VIEW (nt->nvxt_textview), false);
  gtk_text_view_set_cursor_visible (GTK_TEXT_VIEW (nt->nvxt_textview), true);
  gtk_box_pack_start (GTK_BOX (nt->nvxt_vbox), nt->nvxt_textview,
                      BOXEXPAND_BM, BOXFILL_BM, 1);
}                               /* end fill_nvx_thing_newgui_BM */




void
add_indexed_named_value_newgui_BM (const stringval_tyBM * namev,
                                   const value_tyBM val,
                                   int browsdepth,
                                   unsigned idx, struct stackframe_stBM *stkf)
{
  LOCALFRAME_BM ( /*prev: */ stkf, /*descr: */ NULL,
                 const stringval_tyBM * namev;
                 value_tyBM val;
    );
  _.namev = namev;
  _.val = val;
  assert (isstring_BM (namev));
  assert (val != NULL);
  assert (idx <= browsednvulen_BM);
  assert (idx < browsednvsize_BM);
  assert (pthread_self () == mainthreadid_BM);
  assert (browsdepth > 0 && browsdepth <= BROWSE_MAXDEPTH_NEWGUI_BM);
  DBGPRINTF_BM
    ("add_indexed_named_value_newgui namev: %s browsdepth=%d idx#%d ulen:%u",
     bytstring_BM (_.namev), browsdepth, idx, browsednvulen_BM);
  struct browsedval_stBM *curbv = browsedval_BM + idx;
  curbv->brow_name = _.namev;
  curbv->brow_val = _.val;
  curbv->brow_vstartmk = NULL;
  curbv->brow_vendmk = NULL;
  curbv->brow_vdepth = browsdepth;
  curbv->brow_vdata = NULL;
  curbv->brow_vparensize = 0;
  curbv->brow_vparenulen = 0;
  curbv->brow_vparenarr = NULL;
  struct namedvaluenewguixtra_stBM *nvx =
    malloc (sizeof (struct namedvaluenewguixtra_stBM));
  if (!nvx)
    FATAL_BM ("failed to malloc namedvaluenewguixtra_stBM");
  memset (nvx, 0, sizeof (*nvx));
  curbv->brow_vdata = nvx;
  nvx->nvx_index = (int) idx;
  nvx->nvx_tbuffer = gtk_text_buffer_new (browsertagtable_BM);
  DBGPRINTF_BM
    ("add_indexed_named_value_newgui_BM idx=%u ulen=%u nvx_tbuffer@%p", idx,
     browsednvulen_BM, nvx->nvx_tbuffer);
  char *title = g_strdup_printf ("$%s", bytstring_BM (_.namev));
  char subtitle[16];
  memset (subtitle, 0, sizeof (subtitle));
  snprintf (subtitle, sizeof (subtitle), "∇ %d" /*Unicode U+2207 NABLA */ ,
            browsdepth);
  fill_nvx_thing_newgui_BM (nvx, true /*upper */ , title, subtitle);
  fill_nvx_thing_newgui_BM (nvx, false /*lower */ , title, subtitle);
  browse_indexed_named_value_newgui_BM (_.val, browsdepth, idx,
                                        (struct stackframe_stBM *) &_);
  //
  gtk_widget_show_all (nvx->nvx_upper.nvxt_frame);
  gtk_widget_show_all (nvx->nvx_lower.nvxt_frame);
  g_free (title), (title = NULL);
}                               /* end add_indexed_named_value_newgui_BM */



void
replace_indexed_named_value_newgui_BM (const value_tyBM val,
                                       int browsdepth,
                                       unsigned idx,
                                       struct stackframe_stBM *stkf)
{
  LOCALFRAME_BM ( /*prev: */ stkf, /*descr: */ NULL,
                 value_tyBM val;
    );
  _.val = val;
  assert (val != NULL);
  assert (idx <= browsednvulen_BM);
  assert (idx < browsednvsize_BM);
  assert (browsdepth > 0 && browsdepth <= BROWSE_MAXDEPTH_NEWGUI_BM);
  assert (pthread_self () == mainthreadid_BM);
  struct browsedval_stBM *curbv = browsedval_BM + idx;
  struct namedvaluenewguixtra_stBM *nvx =
    (struct namedvaluenewguixtra_stBM *) (curbv->brow_vdata);
  assert (nvx != NULL && nvx->nvx_index == idx);
  free (curbv->brow_vparenarr), (curbv->brow_vparenarr = NULL);
  curbv->brow_vparensize = 0;
  curbv->brow_vparenulen = 0;
  GtkTextBuffer *txbuf = nvx->nvx_tbuffer;
  assert (txbuf != NULL);
  gtk_text_buffer_delete_mark (txbuf, curbv->brow_vstartmk),
    (curbv->brow_vstartmk = NULL);
  gtk_text_buffer_delete_mark (txbuf, curbv->brow_vendmk),
    (curbv->brow_vendmk = NULL);
  char subtitle[16];
  memset (subtitle, 0, sizeof (subtitle));
  snprintf (subtitle, sizeof (subtitle), "∇ %d" /*Unicode U+2207 NABLA */ ,
            browsdepth);
  gtk_header_bar_set_subtitle (GTK_HEADER_BAR (nvx->nvx_upper.nvxt_headb),
                               subtitle);
  gtk_header_bar_set_subtitle (GTK_HEADER_BAR (nvx->nvx_lower.nvxt_headb),
                               subtitle);
  browse_indexed_named_value_newgui_BM (_.val, browsdepth, idx,
                                        (struct stackframe_stBM *) &_);
  gtk_widget_show_all (nvx->nvx_upper.nvxt_frame);
  gtk_widget_show_all (nvx->nvx_lower.nvxt_frame);
}                               /* end replace_indexed_named_value_newgui_BM */

static void
browse_indexed_named_value_newgui_BM (const value_tyBM val,
                                      int browsdepth,
                                      unsigned idx,
                                      struct stackframe_stBM *stkf)
{
  LOCALFRAME_BM ( /*prev: */ stkf, /*descr: */ NULL,
                 value_tyBM val;
    );
  DBGPRINTF_BM ("browse_indexed_named_value_newgui_BM idx=%u ulen=%u", idx,
                browsednvulen_BM);
  assert (val != NULL);
  assert (idx < browsednvulen_BM);
  assert (idx < browsednvsize_BM);
  assert (browsdepth > 0 && browsdepth <= BROWSE_MAXDEPTH_NEWGUI_BM);
  assert (pthread_self () == mainthreadid_BM);
  struct browsedval_stBM *curbv = browsedval_BM + idx;
  struct namedvaluenewguixtra_stBM *nvx =
    (struct namedvaluenewguixtra_stBM *) (curbv->brow_vdata);
  assert (nvx != NULL && nvx->nvx_index == idx);
  GtkTextBuffer *txbuf = nvx->nvx_tbuffer;
  assert (GTK_IS_TEXT_BUFFER (txbuf));
  _.val = val;
  gtk_text_buffer_set_text (txbuf, "", 0);
  browserobcurix_BM = -1;
  browsednvcurix_BM = idx;
  int prevbrowdepth = browserdepth_BM;
  browserdepth_BM = browsdepth;
  char subtitle[16];
  memset (subtitle, 0, sizeof (subtitle));
  snprintf (subtitle, sizeof (subtitle), "∇ %d" /*Unicode U+2207 NABLA */ ,
            browsdepth);
  gtk_header_bar_set_subtitle (GTK_HEADER_BAR (nvx->nvx_upper.nvxt_headb),
                               subtitle);
  gtk_header_bar_set_subtitle (GTK_HEADER_BAR (nvx->nvx_lower.nvxt_headb),
                               subtitle);
  gtk_text_buffer_get_start_iter (txbuf, &browserit_BM);
  browserbuf_BM = txbuf;
  curbv->brow_vstartmk = gtk_text_buffer_create_mark
    (txbuf, NULL, &browserit_BM, LEFT_GRAVITY_BM);
  send2_BM ((const value_tyBM) _.val, BMP_browse_value,
            (struct stackframe_stBM *) &_,
            taggedint_BM (browsdepth), taggedint_BM (0));
  gtk_text_buffer_get_end_iter (txbuf, &browserit_BM);
  curbv->brow_vendmk = gtk_text_buffer_create_mark
    (txbuf, NULL, &browserit_BM, RIGHT_GRAVITY_BM);
  gtk_text_buffer_insert (txbuf, &browserit_BM, "\n", 1);
  browserbuf_BM = NULL;
  browsednvcurix_BM = -1;
  browserobcurix_BM = -1;
  browserdepth_BM = prevbrowdepth;
  memset (&browserit_BM, 0, sizeof (browserit_BM));
}                               /* end browse_indexed_named_value_newgui_BM */




static void
hide_named_value_newgui_BM (const char *namestr, struct stackframe_stBM *stkf)
{
  if (!namestr)
    return;
  int idx = find_named_value_newgui_BM (namestr, stkf);
  if (idx < 0)
    return;
  hide_index_named_value_newgui_BM (idx, stkf);
}                               /* end hide_named_value_newgui_BM */


void
hide_index_named_value_newgui_BM (int idx, struct stackframe_stBM *stkf)
{
  LOCALFRAME_BM ( /*prev: */ stkf, /*descr: */ NULL,
                 value_tyBM val;
    );
  if (idx < 0 || idx >= (int) browsednvulen_BM)
    return;
  struct browsedval_stBM *curbv = browsedval_BM + idx;
  struct namedvaluenewguixtra_stBM *nvx =
    (struct namedvaluenewguixtra_stBM *) (curbv->brow_vdata);
  assert (nvx != NULL && nvx->nvx_index == idx);
  GtkTextBuffer *txbuf = nvx->nvx_tbuffer;
  assert (txbuf != NULL);
  gtk_text_buffer_set_text (txbuf, "", 0);
  gtk_text_buffer_delete_mark (txbuf, curbv->brow_vstartmk),
    (curbv->brow_vstartmk = NULL);
  gtk_text_buffer_delete_mark (txbuf, curbv->brow_vendmk),
    (curbv->brow_vendmk = NULL);
  gtk_container_remove (GTK_CONTAINER (uppervboxvalues_newgui_bm),
                        nvx->nvx_upper.nvxt_frame),
    (nvx->nvx_upper.nvxt_frame = NULL);
  gtk_container_remove (GTK_CONTAINER (lowervboxvalues_newgui_bm),
                        nvx->nvx_lower.nvxt_frame),
    (nvx->nvx_lower.nvxt_frame = NULL);
  memset (nvx, 0, sizeof (*nvx));
  free (nvx), (curbv->brow_vdata = nvx = NULL);
  for (int ix = idx; ix < (int) browsednvulen_BM; ix++)
    {
      browsedval_BM[ix] = browsedval_BM[ix + 1];
      struct namedvaluenewguixtra_stBM *uvx =
        (struct namedvaluenewguixtra_stBM *) (browsedval_BM[ix].brow_vdata);
      assert (uvx != NULL && uvx->nvx_index == ix + 1);
      uvx->nvx_index = ix;
    }
  browsednvulen_BM--;
  if (browsednvsize_BM > 20 && 2 * browsednvulen_BM < browsednvsize_BM)
    {
      unsigned newsiz = prime_above_BM (4 * browsednvulen_BM / 3 + 3);
      if (newsiz < browsednvsize_BM)
        {
          struct browsedval_stBM *newarr =
            calloc (newsiz, sizeof (struct browsedval_stBM));
          if (!newarr)
            FATAL_BM ("failed to shrink browsedval_BM for %u elements",
                      newsiz);
          memcpy (newarr, browsedval_BM,
                  browsednvulen_BM * sizeof (struct browsedval_stBM));
          free (browsedval_BM), (browsedval_BM = newarr);
          browsednvsize_BM = newsiz;
        }
    }
}                               /* end hide_index_named_value_newgui_BM  */



// for €<newname> or $*<newname>
const objectval_tyBM *parsmakenewname_newguicmd_BM
  (struct parser_stBM *pars, unsigned lineno, unsigned colpos,
   const value_tyBM varname, struct stackframe_stBM *stkf)
{
  LOCALFRAME_BM ( /*prev: */ stkf, /*descr: */ NULL,
                 objectval_tyBM * namedobj;
                 const stringval_tyBM * strnam; objectval_tyBM * parsob;
    );
  if (!isparser_BM (pars))
    return NULL;
  _.parsob = checkedparserowner_BM (pars);
  assert (isstring_BM (varname));
  _.strnam = varname;
  if (!validname_BM (bytstring_BM (varname)))
    parsererrorprintf_BM (pars, (struct stackframe_stBM *) &_, lineno, colpos,
                          "invalid new name %s", bytstring_BM (varname));
  _.namedobj = findnamedobj_BM (bytstring_BM (varname));
  if (_.namedobj)
    return _.namedobj;
  _.namedobj = makeobj_BM ();
  objtouchnow_BM (_.namedobj);
  objputspacenum_BM (_.namedobj, GlobalSp_BM);
  registername_BM (_.namedobj, bytstring_BM (_.strnam));
  log_begin_message_BM ();
  log_puts_message_BM ("created global new named object ");
  log_object_message_BM (_.namedobj);
  log_end_message_BM ();
  return _.namedobj;
}                               /* end parsmakenewname_newguicmd_BM */


// expand readmacro-s
value_tyBM parsreadmacroexp_newguicmd_BM
  (struct parser_stBM * pars, unsigned lineno, unsigned colpos, int depth,
   const node_tyBM * nod, struct stackframe_stBM * stkf)
{
  LOCALFRAME_BM ( /*prev: */ stkf, /*descr: */ NULL,
                 value_tyBM resval;
                 const node_tyBM * nod;
                 value_tyBM crm; const objectval_tyBM * conn;
                 objectval_tyBM * parsob;
    );
  _.parsob = checkedparserowner_BM (pars);
  _.nod = nod;
  if (depth > MAXDEPTHPARSE_BM)
    parsererrorprintf_BM (pars, (struct stackframe_stBM *) &_, lineno, colpos,
                          "too deep %d readmacro", depth);
  assert (isnode_BM ((const value_tyBM) nod));
  _.conn = nodeconn_BM ((const value_tyBM) _.nod);
  assert (isobject_BM ((const value_tyBM) _.conn));
  _.crm = objgetattr_BM (_.conn, BMP_command_readmacro);
  if (!isclosure_BM ((const value_tyBM) _.crm))
    {
      char crmidbuf[32];
      memset (crmidbuf, 0, sizeof (crmidbuf));
      idtocbuf32_BM (objid_BM (_.crm), crmidbuf);
      const char *crmname = findobjectname_BM (_.crm);
      if (crmname)
        parsererrorprintf_BM (pars, (struct stackframe_stBM *) &_, lineno,
                              colpos,
                              "readmacro ^ %s |=%s| has bad `command_readmacro` attribute",
                              crmidbuf, crmname);
      else
        parsererrorprintf_BM (pars, (struct stackframe_stBM *) &_, lineno,
                              colpos,
                              "readmacro ^ %s has bad `command_readmacro` attribute",
                              crmidbuf);
    }
  _.resval =                    //
    apply4_BM (_.crm, (struct stackframe_stBM *) &_,
               (value_tyBM) _.nod,
               taggedint_BM (lineno), taggedint_BM (colpos), _.parsob);
  return _.resval;
}                               /* end parsreadmacroexp_newguicmd_BM */

void
parserror_newguicmd_BM (struct parser_stBM *pars,
                        struct stackframe_stBM *stkf, unsigned lineno,
                        unsigned colpos, char *msg)
{
  LOCALFRAME_BM ( /*prev: */ stkf, /*descr: */ NULL,
                 objectval_tyBM * parsob; value_tyBM errstrv;
                 value_tyBM errnodv;
    );
  assert (isparser_BM (pars));
  const struct parserops_stBM *parsops = pars->pars_ops;
  assert (parsops && parsops->parsop_magic == PARSOPMAGIC_BM);
  bool nobuild = parsops && parsops->parsop_nobuild;
  GtkTextIter it = EMPTY_TEXT_ITER_BM;
  gtk_text_buffer_get_iter_at_line (commandbuf_BM, &it, lineno - 1);
  gtk_text_iter_forward_chars (&it, colpos);
  GtkTextIter endit = EMPTY_TEXT_ITER_BM;
  gtk_text_buffer_get_end_iter (commandbuf_BM, &endit);
  gtk_text_buffer_apply_tag (commandbuf_BM, errored_cmdtag_BM, &it, &endit);
  if (!nobuild)
    {
      log_begin_message_BM ();
      char errbuf[64];
      snprintf (errbuf, sizeof (errbuf), "command error L%dC%d:",
                lineno, colpos);
      GtkTextIter logit = EMPTY_TEXT_ITER_BM;
      gtk_text_buffer_get_end_iter (logbuf_BM, &logit);
      gtk_text_buffer_insert_with_tags
        (logbuf_BM, &logit, errbuf, -1, error_logtag_BM, NULL);
      log_puts_message_BM (msg);
      log_end_message_BM ();
      gtk_text_view_scroll_to_iter (GTK_TEXT_VIEW (commandview_BM),
                                    &it, 0.1, false, 0.5, 0.2);
      errormessagedialog_BM = gtk_message_dialog_new_with_markup        //
        (GTK_WINDOW (mainwin_BM),
         GTK_DIALOG_DESTROY_WITH_PARENT,
         GTK_MESSAGE_ERROR,
         GTK_BUTTONS_CLOSE,
         "<b>command error</b> L%dC%d:\n" "%s", lineno, colpos, msg);
      gtk_widget_show_all (errormessagedialog_BM);
      fflush (NULL);
      /// errormessagedialog_BM is run in runcommand_BM 
    }
  free (msg);
#warning should use failure in parserror_newguicmd_BM
  longjmp (jmperrorcmd_BM, 1);
}                               /* end parserror_newguicmd_BM */



struct objectwindow_newgui_stBM *
make_obwin_newgui_BM (void)
{
  static int wincount;
  assert (pthread_self () == mainthreadid_BM);
  struct objectwindow_newgui_stBM *newobw =
    calloc (1, sizeof (struct objectwindow_newgui_stBM));
  if (!newobw)
    FATAL_BM
      ("failed to allocate objectwindow_newgui in make_obwin_newgui_BM");
  newobw->obw_next = obwin_first_newgui_BM;
  assert (obwin_first_newgui_BM == NULL
          || obwin_first_newgui_BM->obw_prev == NULL);
  if (obwin_first_newgui_BM)
    obwin_first_newgui_BM->obw_prev = newobw;
  obwin_first_newgui_BM = newobw;
  if (!obwin_last_newgui_BM)
    obwin_last_newgui_BM = newobw;
  int inisiz = 11;
  newobw->obw_arr = calloc (inisiz, sizeof (void *));
  if (!newobw->obw_arr)
    FATAL_BM ("failed to calloc %d slots for objects in make_obwin_newgui_BM",
              inisiz);
  newobw->obw_asiz = inisiz;
  newobw->obw_ulen = 0;
  GtkWidget *obwin = newobw->obw_window =
    gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_style_context_add_provider_for_screen (gtk_window_get_screen
                                             (GTK_WINDOW (obwin)),
                                             GTK_STYLE_PROVIDER
                                             (cssprovider_newgui_bm),
                                             GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
  GtkWidget *mainvbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 3);
  gtk_container_add (GTK_CONTAINER (obwin), mainvbox);
  char labelbuf[32];
  wincount++;
  newobw->obw_rank = wincount;
  memset (labelbuf, 0, sizeof (labelbuf));
  snprintf (labelbuf, sizeof (labelbuf), "bismonob#%d", newobw->obw_rank);
  gtk_window_set_title (GTK_WINDOW (obwin), labelbuf);
  newobw->obw_label = gtk_label_new (labelbuf);
  gtk_box_pack_start (GTK_BOX (mainvbox), newobw->obw_label, BOXNOEXPAND_BM,
                      BOXNOFILL_BM, 2);
  GtkWidget *paned = gtk_paned_new (GTK_ORIENTATION_VERTICAL);
  gtk_paned_set_wide_handle (GTK_PANED (paned), true);
  gtk_paned_set_position (GTK_PANED (paned), 250);
  gtk_box_pack_start (GTK_BOX (mainvbox), paned, BOXEXPAND_BM, BOXFILL_BM, 2);
  GtkWidget *uppervbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 1);
  GtkWidget *lowervbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 1);
  gtk_paned_add1 (GTK_PANED (paned), uppervbox);
  gtk_paned_add2 (GTK_PANED (paned), lowervbox);
  gtk_box_pack_start (GTK_BOX (uppervbox),
                      gtk_separator_new (GTK_ORIENTATION_HORIZONTAL),
                      BOXNOEXPAND_BM, BOXNOFILL_BM, 2);
  gtk_box_pack_start (GTK_BOX (lowervbox),
                      gtk_separator_new (GTK_ORIENTATION_HORIZONTAL),
                      BOXNOEXPAND_BM, BOXNOFILL_BM, 2);
  GtkWidget *upperscrowin = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW
                                  (upperscrowin),
                                  GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
  gtk_box_pack_start (GTK_BOX (uppervbox), upperscrowin, BOXEXPAND_BM,
                      BOXFILL_BM, 2);
  GtkWidget *lowerscrowin = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW
                                  (lowerscrowin),
                                  GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
  gtk_box_pack_start (GTK_BOX (lowervbox), lowerscrowin, BOXEXPAND_BM,
                      BOXFILL_BM, 2);
  GtkWidget *upperobvbox = newobw->obw_upperobjvbox =
    gtk_box_new (GTK_ORIENTATION_VERTICAL, 3);
  GtkWidget *lowerobvbox = newobw->obw_lowerobjvbox =
    gtk_box_new (GTK_ORIENTATION_VERTICAL, 3);
  gtk_container_add (GTK_CONTAINER (upperscrowin), upperobvbox);
  gtk_container_add (GTK_CONTAINER (lowerscrowin), lowerobvbox);
  gtk_window_set_default_size (GTK_WINDOW (obwin), 350, 420);
  gtk_widget_show_all (obwin);
  DBGPRINTF_BM ("make_obwin_newgui_BM incomplete obwin@%p rank#%d", obwin,
                newobw->obw_rank);
  g_signal_connect (obwin, "delete-event",
                    (GCallback) deleteobjectwin_newgui_BM, newobw);
#warning make_obwin_newgui_BM incomplete
  // should connect destructor on that window, etc...
  return newobw;
}                               /* end make_obwin_newgui_BM */

bool
deleteobjectwin_newgui_BM (GtkWidget * widget,
                           GdkEvent * ev __attribute__ ((unused)),
                           gpointer data)
{
  struct objectwindow_newgui_stBM *oldobw =
    (struct objectwindow_newgui_stBM *) data;
  assert (oldobw != NULL && oldobw->obw_window == widget);
  struct objectwindow_newgui_stBM *prevobw = oldobw->obw_prev;
  struct objectwindow_newgui_stBM *nextobw = oldobw->obw_next;
  if (prevobw)
    {
      assert (oldobw == prevobw->obw_next);
      prevobw->obw_next = nextobw;
    }
  else
    {
      assert (oldobw == obwin_first_newgui_BM);
      obwin_first_newgui_BM = nextobw;
    }
  if (nextobw)
    {
      assert (oldobw == nextobw->obw_prev);
      nextobw->obw_prev = prevobw;
    }
  else
    {
      assert (oldobw == obwin_last_newgui_BM);
      obwin_last_newgui_BM = prevobw;
    }
  if (oldobw->obw_arr)
    {
      free (oldobw->obw_arr);
    }
  oldobw->obw_arr = NULL;
  oldobw->obw_asiz = oldobw->obw_ulen = 0;
  if (obwin_current_newgui_BM == oldobw)
    obwin_current_newgui_BM = NULL;
#warning deleteobjectwin_newgui_BM unimplemented
  return false;                 // to let the window be destroyed
}                               /* end deleteobjectwin_newgui_BM */
