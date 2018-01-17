/* file newgui_BM.c */
#include "bismon.h"

// each named value has its own GtkTextBuffer, which is displayed in
//the value window, containing a GtkPane, in a GtkScrolledWindow
//containing one GtkFrame (containing a GtkHeaderBar & GtkTextView)
//per named value;
struct namedvaluenewguixtra_stBM
{
  int nvx_index;                /* corresponding index in browsedval_BM */
  GtkTextBuffer *nvx_tbuffer;
  // upper 
  GtkWidget *nvx_upframe;
  GtkWidget *nvx_upvbox;
  GtkWidget *nvx_upheadb;
  GtkWidget *nvx_uptextview;
  // lower
  GtkWidget *nvx_loframe;
  GtkWidget *nvx_lovbox;
  GtkWidget *nvx_loheadb;
  GtkWidget *nvx_lotextview;
};

static GtkWidget *windowvalues_newgui_bm;
static GtkWidget *valueslabel_newgui_bm;
static GtkWidget *upperscrollwvalues_newgui_bm;
static GtkWidget *lowerscrollwvalues_newgui_bm;
static GtkWidget *uppervboxvalues_newgui_bm;
static GtkWidget *lowervboxvalues_newgui_bm;

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

static value_tyBM
find_named_value_newgui_BM (const char *vstr, struct stackframe_stBM *stkf);

static void
browse_named_value_newgui_BM (const stringval_tyBM * namev,
                              const value_tyBM val,
                              int browsdepth, struct stackframe_stBM *stkf);

static void
hide_named_value_newgui_BM (const char *namestr,
                            struct stackframe_stBM *stkf);

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
  GtkBuilder *bld = gtk_builder_new_from_file (builderfile);
  GtkCssProvider *cssprovider = gtk_css_provider_get_default ();
  g_signal_connect (cssprovider, "parsing-error",
                    G_CALLBACK (cssparsingerror_BM), NULL);
  gtk_css_provider_load_from_path (cssprovider, cssfile, NULL);
  initialize_gui_tags_BM (bld);
  //gtk_builder_add_callback_symbols (bld, "quitaction_BM", quit_BM, NULL);
  mainwin_BM = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_style_context_add_provider_for_screen
    (gtk_window_get_screen (GTK_WINDOW (mainwin_BM)),
     GTK_STYLE_PROVIDER (cssprovider),
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
       GTK_STYLE_PROVIDER (cssprovider),
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
    GtkWidget *lowerlab = gtk_label_new ("lower");
    gtk_box_pack_start (GTK_BOX (lowervboxvalues_newgui_bm), lowerlab,
                        BOXNOEXPAND_BM, BOXNOFILL_BM, 2);
    gtk_paned_set_position (GTK_PANED (paned), 220);
    gtk_window_set_title (GTK_WINDOW (windowvalues_newgui_bm),
                          "bismon values");
    gtk_window_set_default_size (GTK_WINDOW (windowvalues_newgui_bm), 450,
                                 620);
    g_signal_connect (windowvalues_newgui_bm, "delete-event",
                      (GCallback) deletemainwin_BM, NULL);
    gtk_widget_show_all (GTK_WIDGET (windowvalues_newgui_bm));
  }
#warning initialize_newgui_BM unimplemented
  fprintf (stderr, "initialize_newgui_BM builder %s css %s unimplemented\n",
           builderfile, cssfile);
  ///
  gtk_window_set_title (GTK_WINDOW (mainwin_BM), "new-bismon");
  gtk_window_set_default_size (GTK_WINDOW (mainwin_BM), 650, 720);
  // perhaps run the GC twice a second
  g_timeout_add (500, guiperiodicgarbagecollection_BM, NULL);
  gtk_widget_show_all (GTK_WIDGET (mainwin_BM));
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
                 objectval_tyBM * obj; objectval_tyBM * oldfocusobj;
                 const stringval_tyBM * name; const stringval_tyBM * result;
                 objectval_tyBM * parsob;
    );
  _.parsob = checkedparserowner_BM (pars);
  const struct parserops_stBM *parsops = pars->pars_ops;
  assert (!parsops || parsops->parsop_magic == PARSOPMAGIC_BM);
  int nbloop = 0;
  for (;;)
    {
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
      //start of object?
      if (parsertokenstartobject_BM (pars, tok))
        {
          parserseek_BM (pars, curlineno, curcolpos);
          bool gotobj = false;
          _.obj =
            parsergetobject_BM (pars, (struct stackframe_stBM *) &_, 0,
                                &gotobj);
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
#warning parsecommandbuf_newgui should show the _.val
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



value_tyBM
find_named_value_newgui_BM (const char *vstr, struct stackframe_stBM * stkf)
{
  LOCALFRAME_BM ( /*prev: */ stkf, /*descr: */ NULL,
                 value_tyBM val;);
  if (!validname_BM (vstr))
    return NULL;
  if (browsednvulen_BM == 0)
    return NULL;
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
        return mdbv->brow_val;
      else if (cmp < 0)
        hi = md;
      else
        lo = md;
    }
  for (md = lo; md < hi; md++)
    {
      struct browsedval_stBM *mdbv = browsedval_BM + md;
      assert (isstring_BM (mdbv->brow_name));
      if (!strcmp (vstr, bytstring_BM (mdbv->brow_name)))
        return mdbv->brow_val;
    }
  return NULL;
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

void
browse_named_value_newgui_BM (const stringval_tyBM * namev,
                              const value_tyBM val,
                              int browsdepth, struct stackframe_stBM *stkf)
{
  LOCALFRAME_BM ( /*prev: */ stkf, /*descr: */ NULL,
                 const stringval_tyBM * namev;
                 value_tyBM val;);
  _.namev = namev;
  _.val = val;
  if (!isstring_BM (namev))
    return;
  if (!validname_BM (bytstring_BM (namev)))
    return;
  if (!val)
    return;
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
    }
  int lo = 0, hi = (int) browsednvulen_BM, md = 0;
  while (lo + 4 < hi)
    {
      md = (lo + hi) / 2;
      struct browsedval_stBM *mdbv = browsedval_BM + md;
      assert (isstring_BM (mdbv->brow_name));
      int cmp =
        strcmp (bytstring_BM (_.namev), bytstring_BM (mdbv->brow_name));
      if (cmp == 0)
        {
          replace_indexed_named_value_newgui_BM //
            (_.val, browsdepth, (unsigned) md, (struct stackframe_stBM *) &_);
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
      assert (isstring_BM (mdbv->brow_name));
      int cmp =
        strcmp (bytstring_BM (_.namev), bytstring_BM (mdbv->brow_name));
      if (!cmp)
        {
          replace_indexed_named_value_newgui_BM //
            (_.val, browsdepth, (unsigned) md, (struct stackframe_stBM *) &_);
          return;
        }
      else if (cmp > 0)
        break;
    }
  // insert before md
  assert (md > 0);
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
  add_indexed_named_value_newgui_BM     //
    (_.namev, _.val, browsdepth, md, (struct stackframe_stBM *) &_);
  return;
}                               /* end browse_named_value_newgui_BM */



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
  struct browsedval_stBM *curbv = browsedval_BM + idx;
  curbv->brow_name = _.namev;
  curbv->brow_val = _.val;
  curbv->brow_vstartmk = NULL;
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
  nvx->nvx_upframe = gtk_frame_new (NULL);
  gtk_box_pack_start (GTK_BOX (uppervboxvalues_newgui_bm), nvx->nvx_upframe,
                      BOXEXPAND_BM, BOXFILL_BM, 2);
  if (idx > 0)
    gtk_box_reorder_child (GTK_BOX (uppervboxvalues_newgui_bm),
                           nvx->nvx_upframe, idx);
  nvx->nvx_upvbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 2);
  gtk_container_add (GTK_CONTAINER (nvx->nvx_upframe), nvx->nvx_upvbox);
  nvx->nvx_upheadb = gtk_header_bar_new ();
  char *title = g_strdup_printf ("$%s", bytstring_BM (_.namev));
  char subtitle[16];
  memset (subtitle, 0, sizeof (subtitle));
  snprintf (subtitle, sizeof (subtitle), "∇ %d" /*Unicode U+2207 NABLA */ ,
            browsdepth);
  gtk_header_bar_set_title (GTK_HEADER_BAR (nvx->nvx_upheadb), title);
  gtk_header_bar_set_subtitle (GTK_HEADER_BAR (nvx->nvx_upheadb), subtitle);
  gtk_box_pack_start (GTK_BOX (nvx->nvx_upvbox), nvx->nvx_upheadb,
                      BOXNOEXPAND_BM, BOXNOFILL_BM, 1);
  nvx->nvx_uptextview = gtk_text_view_new_with_buffer (nvx->nvx_tbuffer);
  gtk_box_pack_start (GTK_BOX (nvx->nvx_upvbox), nvx->nvx_uptextview,
                      BOXEXPAND_BM, BOXFILL_BM, 1);
  //
  nvx->nvx_loframe = gtk_frame_new (NULL);
  gtk_box_pack_start (GTK_BOX (lowervboxvalues_newgui_bm), nvx->nvx_loframe,
                      BOXEXPAND_BM, BOXFILL_BM, 2);
  if (idx > 0)
    gtk_box_reorder_child (GTK_BOX (lowervboxvalues_newgui_bm),
                           nvx->nvx_loframe, idx);
  nvx->nvx_lovbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 2);
  gtk_container_add (GTK_CONTAINER (nvx->nvx_loframe), nvx->nvx_lovbox);
  nvx->nvx_loheadb = gtk_header_bar_new ();
  gtk_header_bar_set_title (GTK_HEADER_BAR (nvx->nvx_loheadb), title);
  gtk_header_bar_set_subtitle (GTK_HEADER_BAR (nvx->nvx_loheadb), subtitle);
  gtk_box_pack_start (GTK_BOX (nvx->nvx_lovbox), nvx->nvx_loheadb,
                      BOXNOEXPAND_BM, BOXNOFILL_BM, 1);
  nvx->nvx_lotextview = gtk_text_view_new_with_buffer (nvx->nvx_tbuffer);
  gtk_box_pack_start (GTK_BOX (nvx->nvx_lovbox), nvx->nvx_lotextview,
                      BOXEXPAND_BM, BOXFILL_BM, 1);
  //
  gtk_text_buffer_get_start_iter (&browserit_BM, nvx->nvx_tbuffer);
  browserbuf_BM = nvx->nvx_tbuffer;
  curbv->brow_vstartmk = gtk_text_buffer_create_mark
    (nvx->nvx_tbuffer, NULL, &browserit_BM, LEFT_GRAVITY_BM);
  send2_BM ((const value_tyBM) _.val, BMP_browse_value,
            (struct stackframe_stBM *) &_,
            taggedint_BM (browsdepth), taggedint_BM (0));
  curbv->brow_vendmk = gtk_text_buffer_create_mark
    (nvx->nvx_tbuffer, NULL, &browserit_BM, RIGHT_GRAVITY_BM);
  gtk_text_buffer_insert (nvx->nvx_tbuffer, &browserit_BM, "\n", 1);
  browserbuf_BM = NULL;
  memset (&browserit_BM, 0, sizeof (browserit_BM));
  gtk_widget_show_all (nvx->nvx_upframe);
  gtk_widget_show_all (nvx->nvx_loframe);
  g_free (title), (title = NULL);
}                               /* end add_indexed_named_value_newgui_BM */

void
replace_indexed_named_value_newgui_BM (const value_tyBM val,
                                       int browsdepth,
                                       unsigned index,
                                       struct stackframe_stBM *stkf)
{
#warning unimplemented replace_indexed_named_value_newgui_BM
  FATAL_BM ("unimplemented replace_indexed_named_value_newgui_BM index %u",
            index);
}                               /* end replace_indexed_named_value_newgui_BM */


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
