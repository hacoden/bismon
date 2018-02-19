/* file newgui_BM.c */
#include "bismon.h"
#include "newgui_BM.const.h"

#define BROWSE_MAXDEPTH_NEWGUI_BM 48
#define BROWSE_MAXREFRESHDELAY_NEWGUI_BM 10
#define BROWSE_BLINKMILLISECOND_NEWGUI_BM 250
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


#warning objectview shows a + the_system in wrong order
// each objectview appears twice, in a paned; the widgets inside upper
// & lower panes ...
struct objectviewthings_stBM
{
  GtkWidget *obvt_frame;
  GtkWidget *obvt_vbox;
  GtkWidget *obvt_headb;
  GtkWidget *obvt_spindepth;
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
  unsigned obv_parenulen, obv_parenasiz;
  struct parenoffset_stBM *obv_parenarr;
  struct objectviewthings_stBM obv_upper, obv_lower;
  int obv_blinkix;              /* index of blinking paren */
  int obv_blinkcount;           /* counting */
  guint obv_blinkid;            /* timeout id */

};                              /* end struct objectview_newgui_stBM */

static struct objectview_newgui_stBM *curobjview_newgui_BM;

// an objectwindow
struct objectwindow_newgui_stBM
{
  struct objectwindow_newgui_stBM *obw_prev;
  struct objectwindow_newgui_stBM *obw_next;
  GtkWidget *obw_window;
  GtkWidget *obw_label;
  GtkWidget *obw_refreshspinbox;
  GtkWidget *obw_upperobjvbox;
  GtkWidget *obw_lowerobjvbox;
  int obw_rank;
  int obw_refreshperiod;
  guint obw_refreshid;
  int obw_asiz;
  int obw_ulen;
  // the array below is sorted in order of its obv_object using objectnamedcmp_BM
  // and each obv_object appears at most once
  struct objectview_newgui_stBM **obw_arr;
};                              /* end struct objectwindow_newgui_stBM */

static struct objectwindow_newgui_stBM *obwin_first_newgui_BM;
static struct objectwindow_newgui_stBM *obwin_last_newgui_BM;
struct objectwindow_newgui_stBM *obwin_current_newgui_BM;

static struct objectwindow_newgui_stBM *make_obwin_newgui_BM (void);
static void spinrefresh_obwin_newgui_cbBM (GtkSpinButton * spbut,
                                           gpointer data);
static bool deleteobjectwin_newgui_cbBM (GtkWidget * widget, GdkEvent * ev,
                                         gpointer data);
static void fill_objectviewthing_BM (struct objectview_newgui_stBM *obv,
                                     const char *labstr,
                                     bool upper,
                                     struct stackframe_stBM *stkf);
static void fill_objectviewbuffer_BM (struct objectview_newgui_stBM *obv,
                                      struct stackframe_stBM *stkf);
static void enable_blink_objectview_BM (struct objectview_newgui_stBM *obv,
                                        int ix);
static void disable_blink_objectview_BM (struct objectview_newgui_stBM *obv);
static gboolean blink_objectview_cbBM (gpointer data);
static void destroy_objectviewbuffer_BM (struct objectview_newgui_stBM *obv,
                                         struct stackframe_stBM *stkf);
static gboolean refresh_obwin_newgui_cbBM (gpointer data);

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
static void markset_newgui_objview_BM (GtkTextBuffer *, GtkTextIter *,
                                       GtkTextMark *, gpointer);
static void beginuact_newgui_objview_BM (GtkTextBuffer *, gpointer);
static void enduact_newgui_objview_BM (GtkTextBuffer *, gpointer);
// the function to handle "populate-popup" on objectviews
static void populatepopup_objview_newgui_BM (GtkTextView *, GtkWidget *,
                                             gpointer);
static struct parenoffset_stBM *paren_objview_at_offset_newgui_BM (struct
                                                                   objectview_newgui_stBM
                                                                   *obv,
                                                                   unsigned
                                                                   off);

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


////////////////
// give the index of a shown object in an obwin, or -1 if not found
static int index_shown_object_in_obwin_newgui_BM
  (struct objectwindow_newgui_stBM *obw, objectval_tyBM * obj);

// show an object, with a showing selector and depth, in a given obwin
static void show_object_in_obwin_newgui_BM
  (struct objectwindow_newgui_stBM *obw, objectval_tyBM * obj,
   objectval_tyBM * shobsel, int depth, struct stackframe_stBM *stkf);

static void obwin_start_refresh_newgui_BM (struct objectwindow_newgui_stBM
                                           *obw);
static void obwin_stop_refresh_newgui_BM (struct objectwindow_newgui_stBM
                                          *obw);
static void destroy_obwin_newgui_BM (struct objectwindow_newgui_stBM *oldobw,
                                     bool destroywin);
static void remove_objectview_newgui_BM (struct objectview_newgui_stBM *obv,
                                         struct stackframe_stBM *stkf);
////////////////
const struct parserops_stBM parsop_command_build_newgui_BM = {
  .parsop_magic =
    PARSOPMAGIC_BM,.parsop_serial =
    3,.parsop_nobuild = false,.parsop_error_rout = parserror_newguicmd_BM,
  ///
  .parsop_expand_dollarobj_rout =
    parsdollarobj_newguicmd_BM,.parsop_expand_dollarval_rout
    =
    parsdollarval_newguicmd_BM,.parsop_expand_newname_rout
    =
    parsmakenewname_newguicmd_BM,.parsop_expand_valexp_rout
    =
    parsvalexp_newguicmd_BM,.parsop_expand_objexp_rout
    =
    parsobjexp_newguicmd_BM,.parsop_expand_readmacro_rout
    = parsreadmacroexp_newguicmd_BM,
  ///
  .parsop_decorate_comment_sign_rout =
    parscommentsign_guicmd_BM,.parsop_decorate_comment_inside_rout
    =
    parscommentinside_guicmd_BM,.parsop_decorate_delimiter_rout
    =
    parsdelim_guicmd_BM,.parsop_decorate_id_rout
    =
    parsid_guicmd_BM,.parsop_decorate_known_name_rout
    =
    parsknowname_guicmd_BM,.parsop_decorate_new_name_rout
    =
    parsnewname_guicmd_BM,.parsop_decorate_number_rout
    =
    parsnumber_guicmd_BM,.parsop_decorate_string_sign_rout
    =
    parsstringsign_guicmd_BM,.parsop_decorate_string_inside_rout
    =
    parsstringinside_guicmd_BM,.parsop_decorate_nesting_rout
    =
    parsnesting_guicmd_BM,.parsop_decorate_start_nesting_rout
    = parsstartnesting_guicmd_BM,
};

const struct parserops_stBM parsop_command_nobuild_newgui_BM = {
  .parsop_magic =
    PARSOPMAGIC_BM,.parsop_serial =
    4,.parsop_nobuild = true,.parsop_error_rout = parserror_newguicmd_BM,
  ///
  .parsop_expand_dollarobj_rout =
    parsdollarobj_newguicmd_BM,.parsop_expand_dollarval_rout
    =
    parsdollarval_newguicmd_BM,.parsop_expand_newname_rout
    =
    parsmakenewname_newguicmd_BM,.parsop_expand_valexp_rout
    =
    parsvalexp_newguicmd_BM,.parsop_expand_objexp_rout
    =
    parsobjexp_newguicmd_BM,.parsop_expand_readmacro_rout
    = parsreadmacroexp_newguicmd_BM,
  ///
  .parsop_decorate_comment_sign_rout =
    parscommentsign_guicmd_BM,.parsop_decorate_comment_inside_rout
    =
    parscommentinside_guicmd_BM,.parsop_decorate_delimiter_rout
    =
    parsdelim_guicmd_BM,.parsop_decorate_id_rout
    =
    parsid_guicmd_BM,.parsop_decorate_known_name_rout
    =
    parsknowname_guicmd_BM,.parsop_decorate_new_name_rout
    =
    parsnewname_guicmd_BM,.parsop_decorate_number_rout
    =
    parsnumber_guicmd_BM,.parsop_decorate_string_sign_rout
    =
    parsstringsign_guicmd_BM,.parsop_decorate_string_inside_rout
    =
    parsstringinside_guicmd_BM,.parsop_decorate_nesting_rout
    =
    parsnesting_guicmd_BM,.parsop_decorate_start_nesting_rout
    = parsstartnesting_guicmd_BM,
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
  for (struct objectwindow_newgui_stBM *
       obw = obwin_first_newgui_BM; obw != NULL; obw = obw->obw_next)
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
handlekeypress_newgui_cmd_BM (GtkWidget *
                              widg, GdkEventKey * evk, gpointer data)
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
      DBGPRINTF_BM
        ("handlekeypress_newgui_cmd_BM keyval %#x KEY_F%d %s%s",
         evk->keyval,
         evk->keyval - (GDK_KEY_F1 - 1),
         withctrl ? " ctrl" : "", withshift ? " shift" : "");
#warning should handle the function key
      return false;
    }
  return false;                 // propagate the event
}                               /* end handlekeypresscmd_BM */


void
populatepopup_newgui_cmd_BM (GtkTextView *
                             txview, GtkWidget * popup, gpointer data)
{
  assert (txview == GTK_TEXT_VIEW (commandview_BM));
  assert (GTK_IS_MENU (popup));
  assert (data == NULL);
  char cursinfobuf[32];
  memset (cursinfobuf, 0, sizeof (cursinfobuf));
  GtkTextIter cursit = EMPTY_TEXT_ITER_BM;
  gtk_text_buffer_get_iter_at_mark      //
    (commandbuf_BM, &cursit, gtk_text_buffer_get_insert (commandbuf_BM));
  snprintf (cursinfobuf,
            sizeof (cursinfobuf),
            "* L%dC%d/%d",
            gtk_text_iter_get_line (&cursit)
            + 1,
            gtk_text_iter_get_line_offset
            (&cursit), gtk_text_iter_get_offset (&cursit));
  gtk_menu_shell_append (GTK_MENU_SHELL
                         (popup), gtk_separator_menu_item_new ());
  {
    GtkWidget *cursinfomenit =  //
      gtk_menu_item_new_with_label (cursinfobuf);
    gtk_widget_set_sensitive (cursinfomenit, false);
    gtk_menu_shell_append (GTK_MENU_SHELL (popup), cursinfomenit);
  }
  gtk_menu_shell_prepend (GTK_MENU_SHELL
                          (popup), gtk_separator_menu_item_new ());
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
    g_signal_connect (runerasemenit,
                      "activate",
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
  browserobcurix_BM = -1;
  browsednvcurix_BM = -1;
  GtkBuilder *bld = gtk_builder_new_from_file (builderfile);
  cssprovider_newgui_bm = gtk_css_provider_get_default ();
  g_signal_connect (cssprovider_newgui_bm,
                    "parsing-error", G_CALLBACK (cssparsingerror_BM), NULL);
  gtk_css_provider_load_from_path (cssprovider_newgui_bm, cssfile, NULL);
  initialize_gui_tags_BM (bld);
  //gtk_builder_add_callback_symbols (bld, "quitaction_BM", quit_BM, NULL);
  mainwin_BM = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_style_context_add_provider_for_screen
    (gtk_window_get_screen
     (GTK_WINDOW (mainwin_BM)),
     GTK_STYLE_PROVIDER
     (cssprovider_newgui_bm), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
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
      (gtk_window_get_screen
       (GTK_WINDOW (windowvalues_newgui_bm)),
       GTK_STYLE_PROVIDER
       (cssprovider_newgui_bm), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    GtkWidget *valuesvbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 3);
    gtk_container_add (GTK_CONTAINER (windowvalues_newgui_bm), valuesvbox);
    valueslabel_newgui_bm = gtk_label_new ("values");
    gtk_box_pack_start (GTK_BOX (valuesvbox),
                        valueslabel_newgui_bm, BOXNOEXPAND_BM, BOXFILL_BM, 2);
    GtkWidget *sep1 = gtk_separator_new (GTK_ORIENTATION_HORIZONTAL);
    gtk_box_pack_start (GTK_BOX (valuesvbox),
                        sep1, BOXNOEXPAND_BM, BOXNOFILL_BM, 2);
    GtkWidget *paned = gtk_paned_new (GTK_ORIENTATION_VERTICAL);
    gtk_box_pack_start (GTK_BOX (valuesvbox),
                        paned, BOXEXPAND_BM, BOXFILL_BM, 2);
    upperscrollwvalues_newgui_bm = gtk_scrolled_window_new (NULL, NULL);
    lowerscrollwvalues_newgui_bm = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy
      (GTK_SCROLLED_WINDOW
       (upperscrollwvalues_newgui_bm),
       GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
    gtk_scrolled_window_set_policy
      (GTK_SCROLLED_WINDOW
       (lowerscrollwvalues_newgui_bm),
       GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
    gtk_paned_add1 (GTK_PANED (paned), upperscrollwvalues_newgui_bm);
    gtk_paned_add2 (GTK_PANED (paned), lowerscrollwvalues_newgui_bm);
    gtk_paned_set_wide_handle (GTK_PANED (paned), true);
    uppervboxvalues_newgui_bm = gtk_box_new (GTK_ORIENTATION_VERTICAL, 3);
    lowervboxvalues_newgui_bm = gtk_box_new (GTK_ORIENTATION_VERTICAL, 3);
    gtk_container_add (GTK_CONTAINER
                       (upperscrollwvalues_newgui_bm),
                       uppervboxvalues_newgui_bm);
    GtkWidget *upperlab = gtk_label_new ("upper");
    gtk_box_pack_start (GTK_BOX
                        (uppervboxvalues_newgui_bm),
                        upperlab, BOXNOEXPAND_BM, BOXNOFILL_BM, 2);
    gtk_container_add (GTK_CONTAINER
                       (lowerscrollwvalues_newgui_bm),
                       lowervboxvalues_newgui_bm);
    gtk_box_pack_start (GTK_BOX
                        (lowervboxvalues_newgui_bm),
                        gtk_separator_new
                        (GTK_ORIENTATION_HORIZONTAL),
                        BOXNOEXPAND_BM, BOXNOFILL_BM, 1);
    GtkWidget *lowerlab = gtk_label_new ("lower");
    gtk_box_pack_start (GTK_BOX
                        (lowervboxvalues_newgui_bm),
                        lowerlab, BOXNOEXPAND_BM, BOXNOFILL_BM, 2);
    gtk_paned_set_position (GTK_PANED (paned), 220);
    gtk_window_set_title (GTK_WINDOW
                          (windowvalues_newgui_bm), "bismon values");
    gtk_window_set_default_size (GTK_WINDOW
                                 (windowvalues_newgui_bm), 550, 620);
    g_signal_connect (windowvalues_newgui_bm,
                      "delete-event", (GCallback) deletemainwin_BM, NULL);
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
  LOCALFRAME_BM ( /*prev: */ NULL,
                 /*descr: */ NULL,
                 objectval_tyBM * cmdparsob;);
  DBGPRINTF_BM ("runcommand_newgui_BM start erase=%s", erase ? "yes" : "no");
  GtkTextIter startit = EMPTY_TEXT_ITER_BM;
  GtkTextIter endit = EMPTY_TEXT_ITER_BM;
  cmd_clear_parens_BM ();
  if (errormessagedialog_BM)
    {
      gtk_widget_destroy (errormessagedialog_BM),
        errormessagedialog_BM = NULL;
    };
  gtk_text_buffer_get_bounds (commandbuf_BM, &startit, &endit);
  gtk_text_buffer_remove_all_tags (commandbuf_BM, &startit, &endit);
  char *cmdstr = gtk_text_buffer_get_text (commandbuf_BM,
                                           &startit, &endit,
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
  struct parser_stBM *cmdpars = makeparser_memopen_BM (cmdstr, -1,
                                                       _.cmdparsob);
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
          fprintf (gui_command_log_file_BM,
                   "///++%s command #%d,l%d:\n",
                   serbuf, commandnumber_BM, cmdlen);
          fputs (cmdstr, gui_command_log_file_BM);
          if (cmdlen > 0 && cmdstr[cmdlen - 1] != '\n')
            putc ('\n', gui_command_log_file_BM);
          fprintf (gui_command_log_file_BM,
                   "///--%s end command #%d\n\f\n", serbuf, commandnumber_BM);
          fflush (gui_command_log_file_BM);
        }
      log_begin_message_BM ();
      log_printf_message_BM
        ("run %s command #%d successfully:\n",
         erase ? "erased" : "kept", commandnumber_BM);
      char commbuf[80];
      memset (commbuf, 0, sizeof (commbuf));
      snprintf (commbuf, sizeof (commbuf),
                "///++%s command #%d,l%d:", serbuf, commandnumber_BM, cmdlen);
      GtkTextIter it = EMPTY_TEXT_ITER_BM;
      gtk_text_buffer_get_end_iter (logbuf_BM, &it);
      gtk_text_buffer_insert_with_tags
        (logbuf_BM, &it, commbuf, -1, comment_logtag_BM, NULL);
      gtk_text_buffer_insert (logbuf_BM, &it, "\n", -1);
      gtk_text_buffer_insert_with_tags
        (logbuf_BM, &it, cmdstr, -1, command_logtag_BM, NULL);
      if (cmdlen > 0 && cmdstr[cmdlen - 1] != '\n')
        gtk_text_buffer_insert (logbuf_BM, &it, "\n", -1);
      snprintf (commbuf, sizeof (commbuf),
                "///--%s end command #%d", serbuf, commandnumber_BM);
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
          gtk_widget_destroy (errormessagedialog_BM),
            errormessagedialog_BM = NULL;
        }
      free (cmdstr), cmdstr = NULL;
      return;
    }
  free (cmdstr), cmdstr = NULL;
  if (erase)
    gtk_text_buffer_set_text (commandbuf_BM, "", 0);
}                               /* end runcommand_newgui_BM */


void
markset_newgui_cmd_BM (GtkTextBuffer *
                       tbuf,
                       GtkTextIter * titer,
                       GtkTextMark * tmark, gpointer cdata)
{
  NONPRINTF_BM ("markset_newgui_cmd titer=%s tmark %s",
                textiterstrdbg_BM (titer),
                tmark ? (gtk_text_mark_get_name (tmark) ? : "*anon*") :
                "*none*");
}                               /* end markset_newgui_cmd_BM */


void
enduseraction_newgui_cmd_BM (GtkTextBuffer * txbuf, gpointer data)
{
  LOCALFRAME_BM ( /*prev: */ NULL,
                 /*descr: */ NULL,
                 objectval_tyBM * cmdparsob;);
  assert (txbuf == commandbuf_BM);
  assert (data == NULL);
  GtkTextIter startit = EMPTY_TEXT_ITER_BM;
  GtkTextIter endit = EMPTY_TEXT_ITER_BM;
  gtk_text_buffer_get_bounds (commandbuf_BM, &startit, &endit);
  gtk_text_buffer_remove_all_tags (commandbuf_BM, &startit, &endit);
  cmd_clear_parens_BM ();
  char *cmdstr = gtk_text_buffer_get_text (commandbuf_BM,
                                           &startit, &endit,
                                           false);
  _.cmdparsob = makeobj_BM ();
  struct parser_stBM *cmdpars = makeparser_memopen_BM (cmdstr, -1,
                                                       _.cmdparsob);
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
parsecommandbuf_newgui_BM (struct
                           parser_stBM *pars, struct stackframe_stBM *stkf)
{
  if (!isparser_BM (pars))
    return;
  LOCALFRAME_BM ( /*prev: */ stkf,
                 /*descr: */ NULL,
                 value_tyBM val;
                 const stringval_tyBM * astrv; objectval_tyBM * obj;
                 const stringval_tyBM * name; const stringval_tyBM * result;
                 objectval_tyBM * parsob;);
  if (browserdepth_BM < 2)
    browserdepth_BM = 2;
  else if (browserdepth_BM > BROWSE_MAXDEPTH_NEWGUI_BM)
    browserdepth_BM = BROWSE_MAXDEPTH_NEWGUI_BM;
  if (!astrval_bm
      || !isstring_BM ((value_tyBM)
                       astrval_bm) || strcmp (bytstring_BM (astrval_bm), "a"))
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
        parsererrorprintf_BM (pars,
                              (struct
                               stackframe_stBM
                               *) &_,
                              pars->pars_lineno,
                              pars->pars_colpos, "too many %d loops", nbloop);
      if (parserendoffile_BM (pars))
        break;
      unsigned curlineno = parserlineno_BM (pars);
      unsigned curcolpos = parsercolpos_BM (pars);
      parstoken_tyBM tok = parsertokenget_BM (pars,
                                              (struct stackframe_stBM *) &_);
      DBGPRINTF_BM
        ("parsecommandbuf_newgui_BM nbloop#%d tok~%s L%dC%d",
         nbloop, lexkindname_BM (tok.tok_kind), curlineno, curcolpos);
      //commands starting with comma?
      if (tok.tok_kind == plex_DELIM && tok.tok_delim == delim_comma)
        {
          parstoken_tyBM cmdtok = parsertokenget_BM (pars,
                                                     (struct stackframe_stBM
                                                      *) &_);
          // ,depth <number>
          if (cmdtok.tok_kind == plex_NAMEDOBJ
              && cmdtok.tok_namedobj == k_depth)
            {
              parstoken_tyBM depthtok = parsertokenget_BM (pars,
                                                           (struct
                                                            stackframe_stBM *)
                                                           &_);
              if (depthtok.tok_kind != plex_LLONG)
                parsererrorprintf_BM (pars,
                                      (struct
                                       stackframe_stBM
                                       *)
                                      &_,
                                      cmdtok.tok_line,
                                      cmdtok.tok_col,
                                      ",depth not followed by number");
              if (depthtok.tok_llong < 2
                  || depthtok.tok_llong > BROWSE_MAXDEPTH_NEWGUI_BM)
                parsererrorprintf_BM (pars,
                                      (struct
                                       stackframe_stBM *)
                                      &_, cmdtok.tok_line,
                                      cmdtok.tok_col,
                                      "bad ,depth %lld, should be between 2 and %d",
                                      depthtok.tok_llong,
                                      BROWSE_MAXDEPTH_NEWGUI_BM);
              browserdepth_BM = (int) depthtok.tok_llong;
            }
          // ,nval <name> <value>
          else
            if (cmdtok.tok_kind == plex_NAMEDOBJ
                && cmdtok.tok_namedobj == k_nval)
            {
              parstoken_tyBM vartok = parsertokenget_BM (pars,
                                                         (struct
                                                          stackframe_stBM *)
                                                         &_);
              if (vartok.tok_kind == plex_NAMEDOBJ)
                _.name =
                  makestring_BM (findobjectname_BM (vartok.tok_namedobj));
              else if (vartok.tok_kind == plex_CNAME)
                _.name = vartok.tok_cname;
              else
                parsererrorprintf_BM (pars,
                                      (struct
                                       stackframe_stBM *)
                                      &_, cmdtok.tok_line,
                                      cmdtok.tok_col,
                                      "name expected after ,nval");
              bool gotval = false;
              _.val =
                parsergetvalue_BM (pars,
                                   (struct stackframe_stBM *) &_, 0, &gotval);
              if (!gotval)
                parsererrorprintf_BM (pars,
                                      (struct
                                       stackframe_stBM
                                       *) &_,
                                      cmdtok.tok_line,
                                      cmdtok.tok_col,
                                      "value expected after ,nval %s",
                                      bytstring_BM (_.name));
              if (!nobuild)
                {
                  if (_.val)
                    {
                      browse_named_value_newgui_BM (_.name,
                                                    _.val,
                                                    browserdepth_BM,
                                                    (struct
                                                     stackframe_stBM *) &_);
                      log_begin_message_BM ();
                      log_printf_message_BM
                        ("show value named %s at depth %d",
                         bytstring_BM (_.name), browserdepth_BM);
                      log_end_message_BM ();
                    }
                  else
                    {
                      hide_named_value_newgui_BM (bytstring_BM
                                                  (_.name),
                                                  (struct
                                                   stackframe_stBM *) &_);
                      log_begin_message_BM ();
                      log_printf_message_BM
                        ("forgot value named %s", bytstring_BM (_.name));
                      log_end_message_BM ();
                    }

                }
            }
          /// ,nhide <name>
          else
            if (cmdtok.tok_kind == plex_NAMEDOBJ
                && cmdtok.tok_namedobj == k_nhide)
            {
              parstoken_tyBM vartok = parsertokenget_BM (pars,
                                                         (struct
                                                          stackframe_stBM *)
                                                         &_);
              if (vartok.tok_kind == plex_NAMEDOBJ)
                _.name =
                  makestring_BM (findobjectname_BM (vartok.tok_namedobj));
              else if (vartok.tok_kind == plex_CNAME)
                _.name = vartok.tok_cname;
              else
                parsererrorprintf_BM (pars,
                                      (struct
                                       stackframe_stBM *)
                                      &_, cmdtok.tok_line,
                                      cmdtok.tok_col,
                                      "name expected after ,nhide");
              if (!nobuild)
                {
                  if (index_named_value_newgui_BM (bytstring_BM (_.name)) < 0)
                    parsererrorprintf_BM (pars,
                                          (struct
                                           stackframe_stBM
                                           *) &_,
                                          cmdtok.tok_line,
                                          cmdtok.tok_col,
                                          ",nhide %s : unknown name",
                                          bytstring_BM (_.name));
                  hide_named_value_newgui_BM (bytstring_BM
                                              (_.name),
                                              (struct stackframe_stBM *) &_);
                  log_begin_message_BM ();
                  log_printf_message_BM
                    ("forgot hidden value named %s", bytstring_BM (_.name));
                  log_end_message_BM ();
                }
            }
          else
            parsererrorprintf_BM (pars,
                                  (struct
                                   stackframe_stBM *)
                                  &_, cmdtok.tok_line,
                                  cmdtok.tok_col, "unimplemented command");
        }
      //start of object?
      else if (parsertokenstartobject_BM (pars, tok))
        {
          parserseek_BM (pars, curlineno, curcolpos);
          bool gotobj = false;
          _.obj =
            parsergetobject_BM (pars,
                                (struct stackframe_stBM *) &_, 0, &gotobj);
          if (!gotobj)
            parsererrorprintf_BM (pars,
                                  (struct
                                   stackframe_stBM
                                   *) &_,
                                  curlineno, curcolpos, "invalid object");
          if (!obwin_current_newgui_BM)
            parsererrorprintf_BM (pars,
                                  (struct
                                   stackframe_stBM *)
                                  &_, curlineno,
                                  curcolpos,
                                  "no current object window to show object");
          if (!nobuild)
            {
              if (!_.obj)
                parsererrorprintf_BM (pars,
                                      (struct
                                       stackframe_stBM *)
                                      &_, curlineno, curcolpos, "no object");
              int depth = browserdepth_BM;
              DBGPRINTF_BM
                ("should browse obj %s depth %d in obwin#%d",
                 objectdbg_BM (_.obj), depth,
                 obwin_current_newgui_BM->obw_rank);
              show_object_in_obwin_newgui_BM
                (obwin_current_newgui_BM, _.obj,
                 BMP_browse_in_object, depth, (struct stackframe_stBM *) &_);
              browserdepth_BM = depth;
              log_begin_message_BM ();
              log_puts_message_BM ("browsing object ");
              log_object_message_BM (_.obj);
              log_printf_message_BM
                (" at depth %d in obwin#%d.", depth,
                 obwin_current_newgui_BM->obw_rank);
              log_end_message_BM ();
            }
        }
      // start of value?
      else if (parsertokenstartvalue_BM (pars, tok))
        {
          parserseek_BM (pars, curlineno, curcolpos);
          bool gotval = false;
          _.val =
            parsergetvalue_BM (pars,
                               (struct stackframe_stBM *) &_, 0, &gotval);
          if (!gotval)
            parsererrorprintf_BM (pars,
                                  (struct
                                   stackframe_stBM
                                   *) &_,
                                  curlineno, curcolpos, "invalid value");
          if (!nobuild)
            {
              DBGPRINTF_BM
                ("parsecommandbuf_newgui val=%s",
                 debug_outstr_value_BM (_.val,
                                        (struct stackframe_stBM *) &_, 5));
              if (_.val)
                browse_named_value_newgui_BM (_.astrv,
                                              _.val,
                                              browserdepth_BM,
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
        parsererrorprintf_BM (pars,
                              (struct
                               stackframe_stBM *)
                              &_, curlineno, curcolpos, "invalid token");
    }
}                               /* end parsecommandbuf_newgui_BM */



// for $:<var>
const objectval_tyBM *
parsdollarobj_newguicmd_BM (struct
                            parser_stBM
                            *pars,
                            unsigned
                            lineno,
                            unsigned
                            colpos,
                            const
                            value_tyBM varname, struct stackframe_stBM *stkf)
{

  LOCALFRAME_BM ( /*prev: */ stkf,
                 /*descr: */ NULL,
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
parsdollarval_newguicmd_BM (struct
                            parser_stBM
                            *pars,
                            unsigned
                            lineno,
                            unsigned
                            colpos,
                            const
                            value_tyBM varname, struct stackframe_stBM *stkf)
{
  const char *varstr = NULL;
  LOCALFRAME_BM ( /*prev: */ stkf,
                 /*descr: */ NULL,
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
parsvalexp_newguicmd_BM (struct parser_stBM
                         * pars,
                         unsigned lineno,
                         unsigned colpos,
                         int depth, struct stackframe_stBM * stkf)
{
  assert (isparser_BM (pars));
  const struct parserops_stBM *parsops = pars->pars_ops;
  bool nobuild = parsops && parsops->parsop_nobuild;
  LOCALFRAME_BM ( /*prev: */ stkf,
                 /*descr: */ NULL,
                 value_tyBM resval; value_tyBM srcval;
                 objectval_tyBM * parsob; objectval_tyBM * obj;
                 objectval_tyBM * obsel; objectval_tyBM * obattr;
                 closure_tyBM * clos; value_tyBM otherval;
                 const stringval_tyBM * name;);
  _.parsob = checkedparserowner_BM (pars);
  unsigned srclineno = parserlineno_BM (pars);
  unsigned srccolpos = parsercolpos_BM (pars);
  parsererrorprintf_BM (pars,
                        (struct
                         stackframe_stBM *)
                        &_, srclineno,
                        srccolpos,
                        "unimplemented parsvalexp_newguicmd for $(...)");
#warning parsvalexp_newguicmd_BM unimplemented
  return NULL;
}                               /* end parsvalexp_newguicmd_BM */


// parse inside $[...]
const objectval_tyBM *
parsobjexp_newguicmd_BM (struct parser_stBM
                         *pars,
                         unsigned lineno,
                         unsigned colpos,
                         int depth, struct stackframe_stBM *stkf)
{
  const struct parserops_stBM *parsops = pars->pars_ops;
  bool nobuild = parsops && parsops->parsop_nobuild;
  LOCALFRAME_BM ( /*prev: */ stkf,
                 /*descr: */ NULL,
                 objectval_tyBM * obj;
                 const stringval_tyBM * namev; objectval_tyBM * oldnamedob;
                 value_tyBM val;);
  assert (isparser_BM (pars));
  parserskipspaces_BM (pars, (struct stackframe_stBM *) &_);
  unsigned oblineno = parserlineno_BM (pars);
  unsigned obcolpos = parsercolpos_BM (pars);
  parsererrorprintf_BM (pars,
                        (struct
                         stackframe_stBM *)
                        &_, oblineno,
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
      int cmp = strcmp (vstr,
                        bytstring_BM (mdbv->brow_name));
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
  LOCALFRAME_BM ( /*prev: */ stkf,
                 /*descr: */ NULL,
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
add_indexed_named_value_newgui_BM (const
                                   stringval_tyBM
                                   * namev,
                                   const
                                   value_tyBM
                                   val,
                                   int
                                   browsdepth,
                                   unsigned
                                   index, struct stackframe_stBM *stkf);
static void
  replace_indexed_named_value_newgui_BM
  (const value_tyBM val, int browsdepth,
   unsigned index, struct stackframe_stBM *stkf);
static void
browse_indexed_named_value_newgui_BM (const
                                      value_tyBM
                                      val,
                                      int
                                      browsdepth,
                                      unsigned
                                      index, struct stackframe_stBM *stkf);
void
browse_named_value_newgui_BM (const
                              stringval_tyBM
                              * namev,
                              const
                              value_tyBM
                              val,
                              int browsdepth, struct stackframe_stBM *stkf)
{
  LOCALFRAME_BM ( /*prev: */ stkf,
                 /*descr: */ NULL,
                 const stringval_tyBM * namev; value_tyBM val;);
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
  DBGPRINTF_BM
    ("browse_named_value_newgui start name: %s depth %d ulen %u",
     bytstring_BM ((value_tyBM) _.namev), browsdepth, browsednvulen_BM);
  if (browsednvulen_BM == 0)
    {
      if (!browsedval_BM)
        {
          const unsigned inisiz = 11;
          browsedval_BM = calloc (inisiz, sizeof (struct browsedval_stBM));
          if (!browsedval_BM)
            FATAL_BM
              ("failed to allocate browsedval_BM for %u elements", inisiz);
          browsednvsize_BM = inisiz;
        }
      browsednvulen_BM = 1;
      memset (browsedval_BM + 0, 0, sizeof (struct browsedval_stBM));
      add_indexed_named_value_newgui_BM //
        (_.namev, _.val, browsdepth, 0, (struct stackframe_stBM *) &_);
      DBGPRINTF_BM
        ("browse_named_value_newgui (empty) end name: %s",
         bytstring_BM (_.namev));
      return;
    };
  // grow array if needed
  if (browsednvulen_BM >= browsednvsize_BM)
    {
      unsigned newsiz = prime_above_BM (4 * browsednvsize_BM / 3 + 8);
      struct browsedval_stBM *newarr = calloc (newsiz,
                                               sizeof (struct
                                                       browsedval_stBM));
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
    ("browse_named_value_newgui lo=%d, hi=%d, md=%d, ulen=%d, name=%s",
     lo, hi, md, browsednvulen_BM, bytstring_BM (_.namev));
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
static void
spindepth_namedval_newgui_cbBM (GtkSpinButton * spbut, gpointer data);
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
  assert (idx >= 0
          && idx <= (int) browsednvulen_BM && idx < (int) browsednvsize_BM);
  assert (browsedval_BM[idx].brow_vdata == (void *) nvx);
  int newdepth = gtk_spin_button_get_value_as_int (spbut);
  DBGPRINTF_BM
    ("spindepth_namedval_newgui_cbBM idx=%d newdepth %d", idx, newdepth);
  browsedval_BM[idx].brow_vdepth = newdepth;
  browse_indexed_named_value_newgui_BM
    (browsedval_BM[idx].brow_val, newdepth, idx, NULL);
  if (newdepth != nvx->nvx_upper.nvxt_spindepth)
    gtk_spin_button_set_value (GTK_SPIN_BUTTON
                               (nvx->nvx_upper.nvxt_spindepth),
                               (double) newdepth);
  if (newdepth != nvx->nvx_lower.nvxt_spindepth)
    gtk_spin_button_set_value (GTK_SPIN_BUTTON
                               (nvx->nvx_lower.nvxt_spindepth),
                               (double) newdepth);
  gtk_widget_show_all (nvx->nvx_upper.nvxt_frame);
  gtk_widget_show_all (nvx->nvx_lower.nvxt_frame);
#warning spindepth_namedval_newgui_cbBM unimplemented
}                               /* end spindepth_namedval_newgui_cbBM */

static void
fill_nvx_thing_newgui_BM (struct
                          namedvaluenewguixtra_stBM
                          *nvx, bool upper,
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
  assert (idx >= 0
          && idx <= (int) browsednvulen_BM && idx < (int) browsednvsize_BM);
  assert (browsedval_BM[idx].brow_vdata == (void *) nvx);
  nt->nvxt_frame = gtk_frame_new (NULL);
  GtkBox *inbox =
    GTK_BOX (upper ? uppervboxvalues_newgui_bm : lowervboxvalues_newgui_bm);
  gtk_box_pack_end (inbox, nt->nvxt_frame, BOXEXPAND_BM, BOXFILL_BM, 2);
  // the lower box starts with a separator, so...
  gtk_box_reorder_child         //
    (inbox, nt->nvxt_frame, upper ? idx : (idx + 1));
  nt->nvxt_vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 2);
  gtk_container_add (GTK_CONTAINER (nt->nvxt_frame), nt->nvxt_vbox);
  nt->nvxt_headb = gtk_header_bar_new ();
  GtkWidget *clobut =           //
    gtk_button_new_from_icon_name ("window-close", GTK_ICON_SIZE_MENU);
  g_signal_connect (clobut, "activate",
                    (GCallback) closebut_namedval_newgui_cbBM, nvx);
  gtk_header_bar_pack_end (GTK_HEADER_BAR (nt->nvxt_headb), clobut);
  nt->nvxt_spindepth =          //
    gtk_spin_button_new_with_range (2.0, (double) BROWSE_MAXDEPTH_NEWGUI_BM,
                                    1.0);
  g_signal_connect (nt->nvxt_spindepth, "value-changed",
                    (GCallback) spindepth_namedval_newgui_cbBM, nvx);
  gtk_header_bar_pack_end (GTK_HEADER_BAR (nt->nvxt_headb),
                           nt->nvxt_spindepth);
  gtk_header_bar_set_title (GTK_HEADER_BAR (nt->nvxt_headb), title);
  gtk_header_bar_set_subtitle (GTK_HEADER_BAR (nt->nvxt_headb), subtitle);
  gtk_box_pack_start (GTK_BOX (nt->nvxt_vbox), nt->nvxt_headb, BOXNOEXPAND_BM,
                      BOXNOFILL_BM, 1);
  nt->nvxt_textview = gtk_text_view_new_with_buffer (nvx->nvx_tbuffer);
  gtk_text_view_set_editable (GTK_TEXT_VIEW (nt->nvxt_textview), false);
  gtk_text_view_set_cursor_visible (GTK_TEXT_VIEW (nt->nvxt_textview), true);
  gtk_box_pack_start (GTK_BOX (nt->nvxt_vbox), nt->nvxt_textview,
                      BOXEXPAND_BM, BOXFILL_BM, 1);
}                               /* end fill_nvx_thing_newgui_BM */




void
add_indexed_named_value_newgui_BM (const
                                   stringval_tyBM
                                   * namev,
                                   const
                                   value_tyBM
                                   val,
                                   int
                                   browsdepth,
                                   unsigned idx, struct stackframe_stBM *stkf)
{
  LOCALFRAME_BM ( /*prev: */ stkf,
                 /*descr: */ NULL,
                 const stringval_tyBM * namev; value_tyBM val;);
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
    ("add_indexed_named_value_newgui_BM idx=%u ulen=%u nvx_tbuffer@%p",
     idx, browsednvulen_BM, nvx->nvx_tbuffer);
  char *title = g_strdup_printf ("$%s",
                                 bytstring_BM (_.namev));
  char subtitle[16];
  memset (subtitle, 0, sizeof (subtitle));
  snprintf (subtitle, sizeof (subtitle), "∇ %d"
            /*Unicode U+2207 NABLA */ ,
            browsdepth);
  fill_nvx_thing_newgui_BM (nvx, true /*upper */ ,
                            title, subtitle);
  fill_nvx_thing_newgui_BM (nvx, false  /*lower */
                            , title, subtitle);
  browse_indexed_named_value_newgui_BM
    (_.val, browsdepth, idx, (struct stackframe_stBM *) &_);
  //
  gtk_widget_show_all (nvx->nvx_upper.nvxt_frame);
  gtk_widget_show_all (nvx->nvx_lower.nvxt_frame);
  g_free (title), (title = NULL);
}                               /* end add_indexed_named_value_newgui_BM */



void
  replace_indexed_named_value_newgui_BM
  (const value_tyBM val, int browsdepth,
   unsigned idx, struct stackframe_stBM *stkf)
{
  LOCALFRAME_BM ( /*prev: */ stkf,
                 /*descr: */ NULL,
                 value_tyBM val;);
  _.val = val;
  assert (val != NULL);
  assert (idx <= browsednvulen_BM);
  assert (idx < browsednvsize_BM);
  assert (browsdepth > 0 && browsdepth <= BROWSE_MAXDEPTH_NEWGUI_BM);
  assert (pthread_self () == mainthreadid_BM);
  struct browsedval_stBM *curbv = browsedval_BM + idx;
  struct namedvaluenewguixtra_stBM *nvx =
    (struct namedvaluenewguixtra_stBM *) (curbv->brow_vdata);
  assert (nvx != NULL && nvx->nvx_index == (int) idx);
  free (curbv->brow_vparenarr), (curbv->brow_vparenarr = NULL);
  curbv->brow_vparensize = 0;
  curbv->brow_vparenulen = 0;
  GtkTextBuffer *txbuf = nvx->nvx_tbuffer;
  assert (txbuf != NULL);
  gtk_text_buffer_delete_mark (txbuf,
                               curbv->brow_vstartmk),
    (curbv->brow_vstartmk = NULL);
  gtk_text_buffer_delete_mark (txbuf,
                               curbv->brow_vendmk),
    (curbv->brow_vendmk = NULL);
  char subtitle[16];
  memset (subtitle, 0, sizeof (subtitle));
  snprintf (subtitle, sizeof (subtitle), "∇ %d"
            /*Unicode U+2207 NABLA */ ,
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
browse_indexed_named_value_newgui_BM (const
                                      value_tyBM
                                      val,
                                      int
                                      browsdepth,
                                      unsigned
                                      idx, struct stackframe_stBM *stkf)
{
  LOCALFRAME_BM ( /*prev: */ stkf,
                 /*descr: */ NULL,
                 value_tyBM val;);
  DBGPRINTF_BM
    ("browse_indexed_named_value_newgui_BM idx=%u ulen=%u",
     idx, browsednvulen_BM);
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
  snprintf (subtitle, sizeof (subtitle), "∇ %d"
            /*Unicode U+2207 NABLA */ ,
            browsdepth);
  gtk_header_bar_set_subtitle (GTK_HEADER_BAR (nvx->nvx_upper.nvxt_headb),
                               subtitle);
  gtk_header_bar_set_subtitle (GTK_HEADER_BAR (nvx->nvx_lower.nvxt_headb),
                               subtitle);
  gtk_text_buffer_get_start_iter (txbuf, &browserit_BM);
  browserbuf_BM = txbuf;
  curbv->brow_vstartmk =
    gtk_text_buffer_create_mark (txbuf, NULL, &browserit_BM, LEFT_GRAVITY_BM);
  send2_BM ((const value_tyBM) _.val, BMP_browse_value,
            (struct stackframe_stBM *) &_, taggedint_BM (browsdepth),
            taggedint_BM (0));
  gtk_text_buffer_get_end_iter (txbuf, &browserit_BM);
  curbv->brow_vendmk =
    gtk_text_buffer_create_mark (txbuf, NULL, &browserit_BM,
                                 RIGHT_GRAVITY_BM);
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
  LOCALFRAME_BM ( /*prev: */ stkf,
                 /*descr: */ NULL,
                 value_tyBM val;);
  if (idx < 0 || idx >= (int) browsednvulen_BM)
    return;
  struct browsedval_stBM *curbv = browsedval_BM + idx;
  struct namedvaluenewguixtra_stBM *nvx =
    (struct namedvaluenewguixtra_stBM *) (curbv->brow_vdata);
  assert (nvx != NULL && nvx->nvx_index == idx);
  GtkTextBuffer *txbuf = nvx->nvx_tbuffer;
  assert (txbuf != NULL);
  gtk_text_buffer_set_text (txbuf, "", 0);
  gtk_text_buffer_delete_mark (txbuf,
                               curbv->brow_vstartmk),
    (curbv->brow_vstartmk = NULL);
  gtk_text_buffer_delete_mark (txbuf,
                               curbv->brow_vendmk),
    (curbv->brow_vendmk = NULL);
  gtk_container_remove (GTK_CONTAINER
                        (uppervboxvalues_newgui_bm),
                        nvx->nvx_upper.nvxt_frame),
    (nvx->nvx_upper.nvxt_frame = NULL);
  gtk_container_remove (GTK_CONTAINER
                        (lowervboxvalues_newgui_bm),
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
          struct browsedval_stBM *newarr = calloc (newsiz,
                                                   sizeof (struct
                                                           browsedval_stBM));
          if (!newarr)
            FATAL_BM
              ("failed to shrink browsedval_BM for %u elements", newsiz);
          memcpy (newarr, browsedval_BM,
                  browsednvulen_BM * sizeof (struct browsedval_stBM));
          free (browsedval_BM), (browsedval_BM = newarr);
          browsednvsize_BM = newsiz;
        }
    }
}                               /* end hide_index_named_value_newgui_BM  */



// for €<newname> or $*<newname>
const objectval_tyBM *
parsmakenewname_newguicmd_BM (struct
                              parser_stBM
                              *pars,
                              unsigned
                              lineno,
                              unsigned
                              colpos,
                              const
                              value_tyBM
                              varname, struct stackframe_stBM *stkf)
{
  LOCALFRAME_BM ( /*prev: */ stkf,
                 /*descr: */ NULL,
                 objectval_tyBM * namedobj;
                 const stringval_tyBM * strnam; objectval_tyBM * parsob;);
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
  (struct parser_stBM * pars,
   unsigned lineno, unsigned colpos,
   int depth, const node_tyBM * nod, struct stackframe_stBM * stkf)
{
  LOCALFRAME_BM ( /*prev: */ stkf,
                 /*descr: */ NULL,
                 value_tyBM resval;
                 const node_tyBM * nod;
                 value_tyBM crm; const objectval_tyBM * conn;
                 objectval_tyBM * parsob;);
  _.parsob = checkedparserowner_BM (pars);
  _.nod = nod;
  if (depth > MAXDEPTHPARSE_BM)
    parsererrorprintf_BM (pars,
                          (struct
                           stackframe_stBM *)
                          &_, lineno, colpos, "too deep %d readmacro", depth);
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
        parsererrorprintf_BM (pars,
                              (struct
                               stackframe_stBM
                               *) &_,
                              lineno,
                              colpos,
                              "readmacro ^ %s |=%s| has bad `command_readmacro` attribute",
                              crmidbuf, crmname);
      else
        parsererrorprintf_BM (pars,
                              (struct
                               stackframe_stBM *)
                              &_, lineno, colpos,
                              "readmacro ^ %s has bad `command_readmacro` attribute",
                              crmidbuf);
    }
  _.resval =                    //
    apply4_BM (_.crm, (struct stackframe_stBM *) &_, (value_tyBM) _.nod,
               taggedint_BM (lineno), taggedint_BM (colpos), _.parsob);
  return _.resval;
}                               /* end parsreadmacroexp_newguicmd_BM */

void
parserror_newguicmd_BM (struct parser_stBM
                        *pars,
                        struct
                        stackframe_stBM
                        *stkf, unsigned lineno, unsigned colpos, char *msg)
{
  LOCALFRAME_BM ( /*prev: */ stkf,
                 /*descr: */ NULL,
                 objectval_tyBM * parsob; value_tyBM errstrv;
                 value_tyBM errnodv;);
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
      snprintf (errbuf, sizeof (errbuf), "command error L%dC%d:", lineno,
                colpos);
      GtkTextIter logit = EMPTY_TEXT_ITER_BM;
      gtk_text_buffer_get_end_iter (logbuf_BM, &logit);
      gtk_text_buffer_insert_with_tags (logbuf_BM, &logit, errbuf, -1,
                                        error_logtag_BM, NULL);
      log_puts_message_BM (msg);
      log_end_message_BM ();
      gtk_text_view_scroll_to_iter (GTK_TEXT_VIEW (commandview_BM), &it, 0.1,
                                    false, 0.5, 0.2);
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
  gtk_style_context_add_provider_for_screen     //
    (gtk_window_get_screen
     (GTK_WINDOW (obwin)),
     GTK_STYLE_PROVIDER
     (cssprovider_newgui_bm), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
  GtkWidget *mainvbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 3);
  gtk_container_add (GTK_CONTAINER (obwin), mainvbox);
  char labelbuf[32];
  wincount++;
  newobw->obw_rank = wincount;
  memset (labelbuf, 0, sizeof (labelbuf));
  snprintf (labelbuf, sizeof (labelbuf), "bismonob#%d", newobw->obw_rank);
  gtk_window_set_title (GTK_WINDOW (obwin), labelbuf);
  GtkWidget *tophbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL,
                                    2);
  newobw->obw_label = gtk_label_new (labelbuf);
  gtk_box_pack_start (GTK_BOX (mainvbox),
                      tophbox, BOXNOEXPAND_BM, BOXNOFILL_BM, 2);
  gtk_box_pack_start (GTK_BOX (tophbox),
                      newobw->obw_label, BOXNOEXPAND_BM, BOXNOFILL_BM, 2);
  {
    GtkWidget *wincommententry = gtk_entry_new ();
    gtk_box_pack_start (GTK_BOX (tophbox),
                        wincommententry, BOXEXPAND_BM, BOXFILL_BM, 2);
    GtkWidget *delaylabel = gtk_label_new (" delay:");
    gtk_box_pack_start (GTK_BOX (tophbox),
                        delaylabel, BOXNOEXPAND_BM, BOXNOFILL_BM, 2);
    GtkWidget *refreshspinbox =
      newobw->obw_refreshspinbox = gtk_spin_button_new_with_range (0.0,
                                                                   (double)
                                                                   BROWSE_MAXREFRESHDELAY_NEWGUI_BM,
                                                                   1.0);
    gtk_box_pack_start (GTK_BOX (tophbox),
                        refreshspinbox, BOXNOEXPAND_BM, BOXNOFILL_BM, 2);
    g_signal_connect (refreshspinbox,
                      "value-changed", spinrefresh_obwin_newgui_cbBM, newobw);
  }
  GtkWidget *paned = gtk_paned_new (GTK_ORIENTATION_VERTICAL);
  gtk_paned_set_wide_handle (GTK_PANED (paned), true);
  gtk_paned_set_position (GTK_PANED (paned), 250);
  gtk_box_pack_start (GTK_BOX (mainvbox), paned, BOXEXPAND_BM, BOXFILL_BM, 2);
  GtkWidget *uppervbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 1);
  GtkWidget *lowervbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 1);
  gtk_paned_add1 (GTK_PANED (paned), uppervbox);
  gtk_paned_add2 (GTK_PANED (paned), lowervbox);
  gtk_box_pack_start (GTK_BOX (uppervbox),
                      gtk_separator_new
                      (GTK_ORIENTATION_HORIZONTAL),
                      BOXNOEXPAND_BM, BOXNOFILL_BM, 2);
  gtk_box_pack_start (GTK_BOX (lowervbox),
                      gtk_separator_new
                      (GTK_ORIENTATION_HORIZONTAL),
                      BOXNOEXPAND_BM, BOXNOFILL_BM, 2);
  GtkWidget *upperscrowin = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy
    (GTK_SCROLLED_WINDOW (upperscrowin),
     GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
  gtk_box_pack_start (GTK_BOX (uppervbox),
                      upperscrowin, BOXEXPAND_BM, BOXFILL_BM, 2);
  GtkWidget *lowerscrowin = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy
    (GTK_SCROLLED_WINDOW (lowerscrowin),
     GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
  gtk_box_pack_start (GTK_BOX (lowervbox),
                      lowerscrowin, BOXEXPAND_BM, BOXFILL_BM, 2);
  GtkWidget *upperobvbox =
    newobw->obw_upperobjvbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 3);
  GtkWidget *lowerobvbox =
    newobw->obw_lowerobjvbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 3);
  gtk_container_add (GTK_CONTAINER (upperscrowin), upperobvbox);
  gtk_container_add (GTK_CONTAINER (lowerscrowin), lowerobvbox);
  gtk_window_set_default_size (GTK_WINDOW (obwin), 660, 450);
  gtk_widget_show_all (obwin);
  DBGPRINTF_BM
    ("make_obwin_newgui_BM incomplete obwin@%p rank#%d",
     obwin, newobw->obw_rank);
  g_signal_connect (obwin, "delete-event",
                    (GCallback) deleteobjectwin_newgui_cbBM, newobw);
#warning make_obwin_newgui_BM incomplete
  // should connect destructor on that window, etc...
  return newobw;
}                               /* end make_obwin_newgui_BM */



void
destroy_obwin_newgui_BM (struct objectwindow_newgui_stBM *oldobw,
                         bool destroywin)
{
  if (!oldobw)
    return;
  GtkWidget *widget = oldobw->obw_window;
  gtk_widget_hide (widget);
  obwin_stop_refresh_newgui_BM (oldobw);
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
      oldobw->obw_arr = NULL;
    }
  oldobw->obw_asiz = oldobw->obw_ulen = 0;
  if (obwin_current_newgui_BM == oldobw)
    obwin_current_newgui_BM = NULL;
  memset (oldobw, 0, sizeof (*oldobw));
  if (destroywin)
    gtk_widget_destroy (widget);
  free (oldobw);
}                               /* end destroy_obwin_newgui_BM */

bool
deleteobjectwin_newgui_cbBM (GtkWidget * widget,
                             GdkEvent * ev __attribute__ ((unused)),
                             gpointer data)
{
  struct objectwindow_newgui_stBM *oldobw =
    (struct objectwindow_newgui_stBM *) data;
  assert (oldobw != NULL && oldobw->obw_window == widget);
  DBGPRINTF_BM
    ("deleteobjectwin_newgui oldobw@%p #%d", oldobw, oldobw->obw_rank);
  destroy_obwin_newgui_BM (oldobw, false);
  return false;                 // to let the window be destroyed
}                               /* end deleteobjectwin_newgui_BM */



int
  index_shown_object_in_obwin_newgui_BM
  (struct objectwindow_newgui_stBM *obw, objectval_tyBM * shobj)
{
  if (!obw)
    return -1;
  if (!isobject_BM ((value_tyBM) shobj))
    return -1;
  if (!obw->obw_arr || obw->obw_ulen <= 0)
    return -1;
  struct objectview_newgui_stBM **obvarr = obw->obw_arr;
  int ulen = obw->obw_ulen;
  assert (ulen < obw->obw_asiz);
  int lo = 0, hi = ulen, md = 0;
  while (lo + 4 < hi)
    {
      md = (lo + hi) / 2;
      struct objectview_newgui_stBM *curobv = obvarr[md];
      assert (curobv != NULL);
      assert (curobv->obv_object != NULL);
      if (shobj == curobv->obv_object)
        return md;
      int cmp = objectnamedcmp_BM (shobj,
                                   curobv->obv_object);
      assert (cmp != 0);
      if (cmp < 0)
        hi = md;
      else if (cmp > 0)
        lo = md;
      else
        FATAL_BM ("corrupted objectwindow #%d", obw->obw_rank);
    }
  for (md = lo; md < hi; md++)
    {
      struct objectview_newgui_stBM *curobv = obvarr[md];
      assert (curobv != NULL);
      assert (curobv->obv_object != NULL);
      if (shobj == curobv->obv_object)
        return md;
    }
  return -1;
}                               /* end index_shown_object_in_obwin_newgui_BM */

static char
  *labstr_object_in_obwin_newgui_BM (struct
                                     objectwindow_newgui_stBM
                                     *obw,
                                     objectval_tyBM
                                     * obj, objectval_tyBM * shobsel);
char *
labstr_object_in_obwin_newgui_BM (struct
                                  objectwindow_newgui_stBM
                                  *obw,
                                  objectval_tyBM
                                  * obj, objectval_tyBM * shobsel)
{
  assert (obw != NULL);
  char objectidbuf[32];
  memset (objectidbuf, 0, sizeof (objectidbuf));
  char *objectstr = findobjectname_BM (obj);
  char shobjselidbuf[32];
  memset (shobjselidbuf, 0, sizeof (shobjselidbuf));
  char *shobjselstr = findobjectname_BM (shobsel);
  char *labstr = NULL;
  if (objectstr)
    {
      if (shobjselstr)
        {
          labstr = g_markup_printf_escaped ("<big><b>%s</b></big>\n"
                                            //U+2B6C RIGHTWARDS TRIANGLE-HEADED DASHED ARROW ⭬
                                            "\342\255\254 "
                                            "<i>%s</i>", objectstr,
                                            shobjselstr);
        }
      else
        {
          labstr = g_markup_printf_escaped ("<big><b>%s</b></big>\n"
                                            //U+2B6C RIGHTWARDS TRIANGLE-HEADED DASHED ARROW ⭬
                                            "\342\255\254 "
                                            "<i><tt>%s</tt></i>",
                                            objectstr,
                                            idtocbuf32_BM (objid_BM
                                                           (shobsel),
                                                           shobjselidbuf));
        }
    }
  else
    {                           /* no objectstr */
      idtocbuf32_BM (objid_BM (obj), objectidbuf);
      if (shobjselstr)
        {
          labstr = g_markup_printf_escaped ("<big><b><tt>%s</tt></b></big>\n"
                                            //U+2B6C RIGHTWARDS TRIANGLE-HEADED DASHED ARROW ⭬
                                            "\342\255\254 " "<i>%s</i>",
                                            objectidbuf, shobjselstr);
        }
      else
        {
          idtocbuf32_BM (objid_BM (shobsel), shobjselidbuf);
          labstr = g_markup_printf_escaped ("<big><b><tt>%s</tt></b></big>\n"
                                            //U+2B6C RIGHTWARDS TRIANGLE-HEADED DASHED ARROW ⭬
                                            "\342\255\254 "
                                            "<i><tt>%s</tt></i>",
                                            objectidbuf, shobjselidbuf);
        }
    };
  return labstr;
}                               /* end labstr_object_in_obwin_newgui_BM */





void
  show_object_in_obwin_newgui_BM
  (struct objectwindow_newgui_stBM *obw,
   objectval_tyBM * obj,
   objectval_tyBM * shobsel, int depth, struct stackframe_stBM *stkf)
{
  if (!obw)
    return;
  if (!isobject_BM ((value_tyBM) obj))
    return;
  if (!isobject_BM ((value_tyBM) shobsel))
    return;
  if (depth < 2)
    depth = 2;
  else if (depth > BROWSE_MAXDEPTH_NEWGUI_BM)
    depth = BROWSE_MAXDEPTH_NEWGUI_BM;
  LOCALFRAME_BM ( /*prev: */ stkf,
                 /*descr: */ NULL,
                 objectval_tyBM * obj;
                 objectval_tyBM * shobsel; objectval_tyBM * curobj;
                 objectval_tyBM * curshobsel;);
  _.obj = obj;
  _.shobsel = shobsel;
  DBGPRINTF_BM
    ("show_object_in_obwin_newgui obj=%s shobsel=%s depth=%d ulen=%d obw@%p start",
     objectdbg_BM (_.obj), objectdbg1_BM (_.shobsel), obw->obw_ulen, obw);
  if (obw->obw_asiz <= 0)
    {
      assert (obw->obw_arr == NULL);
      int newsiz = 11;
      struct objectview_newgui_stBM **newarr =
        calloc (newsiz, sizeof (void *));
      if (!newarr)
        FATAL_BM ("failed to allocate array of %d objectviews", newsiz);
      obw->obw_arr = newarr;
      obw->obw_asiz = newsiz;
      obw->obw_ulen = 0;
    }
  else if (obw->obw_ulen + 1 <= obw->obw_asiz)
    {
      int newsiz = prime_above_BM (4 * obw->obw_ulen / 3 + 5);
      struct objectview_newgui_stBM **newarr =
        calloc (newsiz, sizeof (void *));
      if (!newarr)
        FATAL_BM ("failed to grow array of %d objectviews", newsiz);
      memcpy (newarr, obw->obw_arr,
              obw->obw_ulen * sizeof (struct objectview_newgui_stBM *));
      free (obw->obw_arr), obw->obw_arr = newarr;
      obw->obw_asiz = newsiz;
    };
  DBGPRINTF_BM
    ("show_object_in_obwin_newgui obj=%s ulen=%d",
     objectdbg_BM (_.obj), obw->obw_ulen);
  ///
  if (obw->obw_ulen <= 0)
    {
      assert (obw->obw_asiz > 2);
      assert (obw->obw_arr != NULL);
      struct objectview_newgui_stBM *newobv = calloc (1,
                                                      sizeof (struct
                                                              objectview_newgui_stBM));
      if (!newobv)
        FATAL_BM
          ("failed to allocate new objectview for %s", objectdbg_BM (_.obj));
      newobv->obv_rank = 0;
      newobv->obv_depth = depth;
      newobv->obv_object = _.obj;
      newobv->obv_obsel = _.shobsel;
      newobv->obv_obwindow = obw;
      newobv->obv_blinkix = -1;
      newobv->obv_tbuffer = gtk_text_buffer_new (browsertagtable_BM);
      char *labstr = labstr_object_in_obwin_newgui_BM (obw,
                                                       _.obj,
                                                       _.shobsel);
      g_signal_connect (newobv->obv_tbuffer,
                        "mark-set",
                        G_CALLBACK (markset_newgui_objview_BM), newobv);
      g_signal_connect (newobv->obv_tbuffer,
                        "begin-user-action",
                        G_CALLBACK (beginuact_newgui_objview_BM), newobv);
      g_signal_connect (newobv->obv_tbuffer,
                        "end-user-action",
                        G_CALLBACK (enduact_newgui_objview_BM), newobv);
      fill_objectviewthing_BM (newobv, labstr,
                               true, (struct stackframe_stBM *) &_);
      fill_objectviewthing_BM (newobv, labstr,
                               false, (struct stackframe_stBM *) &_);
      fill_objectviewbuffer_BM (newobv, (struct stackframe_stBM *) &_);
      assert (newobv->obv_upper.obvt_frame != NULL);
      assert (newobv->obv_lower.obvt_frame != NULL);
      if (obw->obw_refreshperiod > 0)
        obwin_start_refresh_newgui_BM (obw);
      else
        obwin_stop_refresh_newgui_BM (obw);
      g_free (labstr), labstr = NULL;
      obw->obw_arr[0] = newobv;
      obw->obw_ulen = 1;
    }
  else
    {                           //obw->obw_ulen>0
      struct objectview_newgui_stBM **obvarr = obw->obw_arr;
      int ulen = obw->obw_ulen;
      assert (ulen < obw->obw_asiz);
      int lo = 0, hi = ulen, md = 0;
      while (lo + 4 < hi)
        {
          md = (lo + hi) / 2;
          struct objectview_newgui_stBM *curobv = obvarr[md];
          assert (curobv != NULL);
          assert (curobv->obv_object != NULL);
          if (_.obj == curobv->obv_object)
            {
            }
          int cmp = objectnamedcmp_BM (_.obj,
                                       curobv->obv_object);
          assert (cmp != 0);
          if (cmp < 0)
            hi = md;
          else if (cmp > 0)
            lo = md;
          else
            FATAL_BM ("corrupted objectwindow #%d", obw->obw_rank);
        }
      for (md = lo; md < hi; md++)
        {
          struct objectview_newgui_stBM *curobv = obvarr[md];
          assert (curobv != NULL);
          assert (curobv->obv_object != NULL);
          assert (curobv->obv_obwindow == obw);
          _.curobj = curobv->obv_object;
          if (_.curobj == _.obj)
            {
              curobv->obv_obsel = _.shobsel;
              curobv->obv_depth = depth;
              char *labstr = labstr_object_in_obwin_newgui_BM (obw,
                                                               _.obj,
                                                               _.shobsel);
              fill_objectviewthing_BM (curobv, labstr,
                                       true, (struct stackframe_stBM *) &_);
              fill_objectviewthing_BM (curobv, labstr,
                                       false, (struct stackframe_stBM *) &_);
              fill_objectviewbuffer_BM (curobv,
                                        (struct stackframe_stBM *) &_);
              g_free (labstr), labstr = NULL;
              return;
            };
          int cmp = objectnamedcmp_BM (_.obj, _.curobj);
          assert (cmp != 0);
          if (cmp > 0)
            break;
        }
      // insert before md
      DBGPRINTF_BM
        ("show_object_in_obwin_newgui obj=%s depth=%d ulen=%d md=%d lo=%d hi=%d",
         objectdbg_BM (_.obj), depth, obw->obw_ulen, md, lo, hi);
      assert (md >= 0);
      for (int ix = ulen; ix > md; ix--)
        {
          obvarr[ix] = obvarr[ix - 1];
          if (obvarr[ix])
            obvarr[ix]->obv_rank = ix;
        }
      obvarr[md] = NULL;
      struct objectview_newgui_stBM *newobv = calloc (1,
                                                      sizeof (struct
                                                              objectview_newgui_stBM));
      if (!newobv)
        FATAL_BM
          ("failed to allocate new objectview for %s", objectdbg_BM (_.obj));
      newobv->obv_rank = md;
      newobv->obv_depth = depth;
      newobv->obv_object = _.obj;
      newobv->obv_obsel = _.shobsel;
      newobv->obv_obwindow = obw;
      newobv->obv_tbuffer = gtk_text_buffer_new (browsertagtable_BM);
      char *labstr = labstr_object_in_obwin_newgui_BM (obw,
                                                       _.obj,
                                                       _.shobsel);
      fill_objectviewbuffer_BM (newobv, (struct stackframe_stBM *) &_);
      fill_objectviewthing_BM (newobv, labstr,
                               true, (struct stackframe_stBM *) &_);
      fill_objectviewthing_BM (newobv, labstr,
                               false, (struct stackframe_stBM *) &_);
      assert (newobv->obv_upper.obvt_frame != NULL);
      assert (newobv->obv_lower.obvt_frame != NULL);
      g_free (labstr), labstr = NULL;
      obvarr[md] = newobv;
      obw->obw_ulen++;
      if (obw->obw_refreshperiod > 0)
        obwin_start_refresh_newgui_BM (obw);
      else
        obwin_stop_refresh_newgui_BM (obw);
      return;
#warning perhaps incomplete show_object_in_obwin_newgui_BM
    }
  DBGPRINTF_BM
    ("show_object_in_obwin_newgui obj=%s shobsel=%s depth=%d ulen=%d obw@%p end",
     objectdbg_BM (_.obj),
     objectdbg1_BM (_.shobsel), depth, obw->obw_ulen, obw);
  // do we handle the append to end case?
}                               /* end show_object_in_obwin_newgui_BM */



static void closebut_obview_newgui_cbBM (GtkWidget * wbut, gpointer data);
static void
spindepth_obview_newgui_cbBM (GtkSpinButton * spbut, gpointer data);
#warning objectwindow should probably keep the focused objview and focus line & column
void
fill_objectviewthing_BM (struct objectview_newgui_stBM *obv,
                         const char *labstr, bool upper,
                         struct stackframe_stBM *stkf)
{
  LOCALFRAME_BM ( /*prev: */ stkf,
                 /*descr: */ NULL,
                 objectval_tyBM * object; objectval_tyBM * obsel;
                 value_tyBM val);
  assert (obv != NULL);
  struct objectwindow_newgui_stBM *obwin = obv->obv_obwindow;
  assert (obwin != NULL);
  assert (obv->obv_object != NULL);
  assert (obv->obv_obsel != NULL);
  _.object = obv->obv_object;
  assert (isobject_BM ((value_tyBM) _.object));
  _.obsel = obv->obv_obsel;
  assert (isobject_BM ((value_tyBM) _.obsel));
  struct objectviewthings_stBM *obth =
    upper ? (&obv->obv_upper) : (&obv->obv_lower);
  obth->obvt_frame = gtk_frame_new (NULL);
  int rk = obv->obv_rank;
  assert (rk >= 0 && rk <= obwin->obw_ulen);
  int depth = obv->obv_depth;
  DBGPRINTF_BM
    ("fill_objectviewthing start %s labstr %s\n"
     "obv@%p obwin@%p rk#%d object %s obsel %s depth %d",
     upper ? "upper" : "lower", labstr, obv,
     obwin, rk, objectdbg_BM (obv->obv_object),
     objectdbg1_BM (obv->obv_obsel), depth);
  GtkBox *inbox =
    GTK_BOX (upper ? obwin->obw_upperobjvbox : obwin->obw_lowerobjvbox);
  assert (inbox != NULL);
  gtk_box_pack_end (inbox, obth->obvt_frame, BOXEXPAND_BM, BOXFILL_BM, 2);
#warning perhaps ordering of obvt_frame is wrong...
  // @@@ the lower box starts with a separator, so...
  gtk_box_reorder_child         //
    (inbox, obth->obvt_frame,   //upper ? rk : (rk + 1)
     rk);
  obth->obvt_vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 2);
  gtk_container_add (GTK_CONTAINER (obth->obvt_frame), obth->obvt_vbox);
  GtkWidget *headb = obth->obvt_headb = gtk_header_bar_new ();
  GtkWidget *clobut =           //
    gtk_button_new_from_icon_name ("window-close", GTK_ICON_SIZE_MENU);
  g_signal_connect (clobut, "clicked", closebut_obview_newgui_cbBM, obv);
  GtkWidget *titwidg = gtk_label_new (NULL);
  gtk_label_set_markup (GTK_LABEL (titwidg), labstr);
  gtk_label_set_selectable (GTK_LABEL (titwidg), true);
  gtk_header_bar_set_custom_title (GTK_HEADER_BAR (headb), titwidg);
  gtk_header_bar_pack_end (GTK_HEADER_BAR (headb), clobut);
  GtkWidget *spinbut = obth->obvt_spindepth =   //
    gtk_spin_button_new_with_range (2.0, (double) BROWSE_MAXDEPTH_NEWGUI_BM,
                                    1.0);
  gtk_spin_button_set_value (GTK_SPIN_BUTTON (spinbut),
                             (double) obv->obv_depth);
  g_signal_connect (spinbut, "value-changed", spindepth_obview_newgui_cbBM,
                    obv);
  gtk_header_bar_pack_end (GTK_HEADER_BAR (headb), spinbut);
  gtk_box_pack_start (GTK_BOX (obth->obvt_vbox), obth->obvt_headb,
                      BOXNOEXPAND_BM, BOXNOFILL_BM, 1);
  GtkWidget *txview = obth->obvt_textview =
    gtk_text_view_new_with_buffer (obv->obv_tbuffer);
  gtk_text_view_set_editable (GTK_TEXT_VIEW (txview), false);
  g_signal_connect (txview, "populate-popup",
                    G_CALLBACK (populatepopup_objview_newgui_BM), obv);
  g_signal_connect (obv->obv_tbuffer, "mark-set",
                    G_CALLBACK (markset_newgui_objview_BM), obv);
  gtk_text_view_set_cursor_visible (GTK_TEXT_VIEW (txview), true);
  gtk_box_pack_start (GTK_BOX (obth->obvt_vbox), txview, BOXEXPAND_BM,
                      BOXFILL_BM, 1);
  assert (GTK_IS_WIDGET (inbox));
  gtk_widget_show_all (GTK_WIDGET (inbox));
}                               /* end of fill_objectviewthing_BM */


void
closebut_obview_newgui_cbBM (GtkWidget * wbut, gpointer data)
{
  assert (data != NULL);
  struct objectview_newgui_stBM *obv = (struct objectview_newgui_stBM *) data;
  struct objectwindow_newgui_stBM *obwin = obv->obv_obwindow;
  assert (obwin != NULL);
  DBGPRINTF_BM ("closebut_obview_newgui_cbBM obv rank#%d object %s obwin#%d",
                obv->obv_rank, objectdbg_BM (obv->obv_object),
                obwin->obw_rank);
  remove_objectview_newgui_BM (obv, NULL);
}                               /* end closebut_obview_newgui_cbBM */


void
spindepth_obview_newgui_cbBM (GtkSpinButton * spbut, gpointer data)
{
  assert (data != 0);
  int newdepth = gtk_spin_button_get_value_as_int (spbut);
  struct objectview_newgui_stBM *obv = (struct objectview_newgui_stBM *) data;
  DBGPRINTF_BM
    ("spindepth_obview_newgui_cbBM obv rank#%d object %s newdepth %d",
     obv->obv_rank, objectdbg_BM (obv->obv_object), newdepth);
  if (newdepth < 2)
    newdepth = 2;
  else if (newdepth > BROWSE_MAXDEPTH_NEWGUI_BM)
    newdepth = BROWSE_MAXDEPTH_NEWGUI_BM;
  obv->obv_depth = newdepth;
  fill_objectviewbuffer_BM (obv, NULL);
}                               /* end spindepth_obview_newgui_cbBM  */


void
fill_objectviewbuffer_BM (struct
                          objectview_newgui_stBM
                          *obv, struct stackframe_stBM *stkf)
{
  LOCALFRAME_BM ( /*prev: */ stkf,
                 /*descr: */ NULL,
                 objectval_tyBM * object;
                 objectval_tyBM * shobsel; volatile value_tyBM failreason;
                 value_tyBM val;);
  assert (obv != NULL);
  assert (pthread_self () == mainthreadid_BM);
  GtkTextBuffer *tbuf = obv->obv_tbuffer;
  assert (tbuf != NULL);
  // both should be objects, so the check should never trigger
  _.object = obv->obv_object;
  _.shobsel = obv->obv_obsel;
  if (!isobject_BM (_.object))
    FATAL_BM ("fill_objectviewbuffer_BM bad object rank#%d", obv->obv_rank);
  if (!isobject_BM (_.shobsel))
    FATAL_BM ("fill_objectviewbuffer_BM bad shobsel rank#%d", obv->obv_rank);
  DBGPRINTF_BM
    ("fill_objectviewbuffer start rank#%d obwin@%p",
     obv->obv_rank, obv->obv_obwindow);
  disable_blink_objectview_BM (obv);
  gtk_text_buffer_set_text (tbuf, "", 0);
  int prevbrowdepth = browserdepth_BM;
  browserdepth_BM = obv->obv_depth;
  browserbuf_BM = tbuf;
  gtk_text_buffer_get_start_iter (tbuf, &browserit_BM);
  struct failurehandler_stBM *prevfailureh = curfailurehandle_BM;
  int failcod = 0;
  _.failreason = NULL;
  LOCAL_FAILURE_HANDLE_BM (failcod, _.failreason);
  curfailurehandle_BM = prevfailureh;
  curobjview_newgui_BM = obv;
  if (failcod)
    {                           // error case....
      DBGPRINTF_BM ("fill_objectviewbuffer_BM failed failcod=%d", failcod);
      // should show some error thing....
      gtk_text_buffer_get_end_iter (tbuf, &browserit_BM);
      gtk_text_buffer_insert (tbuf, &browserit_BM, "\n", -1);
      // should show some error epilogue....
      browserbuf_BM = tbuf;
      gtk_text_buffer_insert_with_tags (tbuf,
                                        &browserit_BM,
                                        "///! ",
                                        -1, epilogue_brotag_BM, NULL);
      {
        char failurebuf[40];
        memset (failurebuf, 0, sizeof (failurebuf));
        snprintf (failurebuf, sizeof (failurebuf), "failure %d :\n", failcod);
        gtk_text_buffer_insert_with_tags (tbuf,
                                          &browserit_BM,
                                          failurebuf,
                                          -1, epilogue_brotag_BM, NULL);
      }
      {
        int faildepth = obv->obv_depth;
        if (faildepth < 2)
          faildepth = 2;
        else if (faildepth > BROWSE_MAXDEPTH_NEWGUI_BM)
          faildepth = BROWSE_MAXDEPTH_NEWGUI_BM;
        char *failvalstr = debug_outstr_value_BM (_.failreason,
                                                  (struct
                                                   stackframe_stBM *)
                                                  &_, faildepth);
        char *nextnl = NULL;
        for (const char *curpc = failvalstr;
             curpc != NULL
             && ((nextnl = strchr (curpc, '\n')),
                 curpc);
             (curpc = nextnl ? (nextnl + 1) : NULL), (nextnl = NULL))
          {
            gtk_text_buffer_insert_with_tags (tbuf,
                                              &browserit_BM,
                                              "///!! ",
                                              -1, epilogue_brotag_BM, NULL);
            if (nextnl)
              gtk_text_buffer_insert_with_tags (tbuf,
                                                &browserit_BM,
                                                curpc,
                                                nextnl -
                                                curpc,
                                                epilogue_brotag_BM,
                                                miscomm_brotag_BM, NULL);
            else
              gtk_text_buffer_insert_with_tags (tbuf,
                                                &browserit_BM,
                                                curpc,
                                                -1,
                                                epilogue_brotag_BM,
                                                miscomm_brotag_BM, NULL);
          }
        gtk_text_buffer_insert (tbuf, &browserit_BM, "\n", -1);
      }
      curobjview_newgui_BM = NULL;
    }
  else
    {                           // first run
      int depth = browserdepth_BM;
      DBGPRINTF_BM
        ("fill_objectviewbuffer_BM object %s shobsel %s depth %d",
         objectdbg_BM (_.object), objectdbg1_BM (_.shobsel), depth);
      send1_BM ((const value_tyBM) _.object,
                _.shobsel,
                (struct stackframe_stBM *) &_, taggedint_BM (depth));
      browserdepth_BM = depth;
      gtk_text_buffer_get_end_iter (tbuf, &browserit_BM);
      gtk_text_buffer_insert (tbuf, &browserit_BM, "\n", -1);
      DBGPRINTF_BM
        ("fill_objectviewbuffer_BM object %s shobsel %s depth %d epilogue",
         objectdbg_BM (_.object), objectdbg1_BM (_.shobsel), depth);
      // should show some epilogue....
      browserbuf_BM = tbuf;
      gtk_text_buffer_insert_with_tags (tbuf,
                                        &browserit_BM,
                                        "///- ",
                                        -1, epilogue_brotag_BM, NULL);
      {
        char objectidbuf[32];
        memset (objectidbuf, 0, sizeof (objectidbuf));
        idtocbuf32_BM (objid_BM (_.object), objectidbuf);
        char *objectnamstr = findobjectname_BM (_.object);
        if (objectnamstr)
          gtk_text_buffer_insert_with_tags
            (browserbuf_BM, &browserit_BM,
             objectnamstr, -1, epilogue_brotag_BM, objname_brotag_BM, NULL);
        else
          gtk_text_buffer_insert_with_tags
            (browserbuf_BM, &browserit_BM, objectidbuf,
             -1, epilogue_brotag_BM, objid_brotag_BM, NULL);
        gtk_text_buffer_insert_with_tags
          (browserbuf_BM, &browserit_BM, " \342\207\242"
           /* U+21E2 RIGHTWARDS DASHED ARROW ⇢ */
           , -1, epilogue_brotag_BM, miscomm_brotag_BM, NULL);
      }
      {
        char shobselidbuf[32];
        memset (shobselidbuf, 0, sizeof (shobselidbuf));
        idtocbuf32_BM (objid_BM (_.shobsel), shobselidbuf);
        char *shobselnamstr = findobjectname_BM (_.shobsel);
        if (shobselnamstr)
          gtk_text_buffer_insert_with_tags
            (browserbuf_BM, &browserit_BM,
             shobselnamstr, -1, epilogue_brotag_BM, objname_brotag_BM, NULL);
        else
          gtk_text_buffer_insert_with_tags
            (browserbuf_BM, &browserit_BM,
             shobselidbuf, -1, epilogue_brotag_BM, objid_brotag_BM, NULL);
      }
      {
        char depthbuf[32];
        memset (depthbuf, 0, sizeof (depthbuf));
        snprintf (depthbuf, sizeof (depthbuf), " \342\210\207"  // U+2207 NABLA ∇
                  " %d", obv->obv_depth);
        gtk_text_buffer_insert_with_tags
          (browserbuf_BM, &browserit_BM, depthbuf,
           -1, epilogue_brotag_BM, miscomm_brotag_BM, NULL);
      }
      gtk_text_buffer_insert (browserbuf_BM, &browserit_BM, "\n", -1);
      for (int pix = 0; pix < (int) obv->obv_parenulen; pix++)
        {
          struct parenoffset_stBM *curpar = obv->obv_parenarr + pix;
          DBGPRINTF_BM
            ("fill_objectviewbuffer_BM par#%d open:%d/l%d close:%d/l%d", pix,
             curpar->paroff_open, curpar->paroff_openlen,
             curpar->paroff_close, curpar->paroff_closelen);
        };
      curobjview_newgui_BM = NULL;
    };
  curobjview_newgui_BM = NULL;
  assert (obv->obv_upper.obvt_frame != NULL);
  assert (GTK_IS_WIDGET (obv->obv_upper.obvt_frame));
  gtk_widget_show_all (GTK_WIDGET (obv->obv_upper.obvt_frame));
  assert (obv->obv_lower.obvt_frame != NULL);
  assert (GTK_IS_WIDGET (obv->obv_lower.obvt_frame));
  gtk_widget_show_all (GTK_WIDGET (obv->obv_lower.obvt_frame));
  assert (obv->obv_obwindow != NULL);
  DBGPRINTF_BM
    ("fill_objectviewbuffer_BM end object %s shobsel %s depth %d rank#%d window#%d",
     objectdbg_BM (_.object), objectdbg1_BM (_.shobsel), obv->obv_depth,
     obv->obv_rank, obv->obv_obwindow->obw_rank);
}                               /* end fill_objectviewbuffer_BM */



void
destroy_objectviewbuffer_BM (struct
                             objectview_newgui_stBM
                             *obv, struct stackframe_stBM *stkf)
{
  if (!obv)
    return;
  LOCALFRAME_BM ( /*prev: */ stkf,
                 /*descr: */ NULL,
                 objectval_tyBM * object; objectval_tyBM * obsel;);
  curobjview_newgui_BM = NULL;
  struct objectwindow_newgui_stBM *obwin = obv->obv_obwindow;
  assert (obwin != NULL);
  _.object = obv->obv_object;
  _.obsel = obv->obv_obsel;
  DBGPRINTF_BM
    ("destroy_objectviewbuffer obv@%p obwin@%p rank#%d object %s obsel %s",
     obv, obwin, obv->obv_rank, objectdbg_BM (_.object),
     objectdbg1_BM (_.obsel));
  if (obv->obv_rank < obwin->obw_ulen && obwin->obw_arr[obv->obv_rank] == obv)
    obwin->obw_arr[obv->obv_rank] = NULL;
  GtkWidget *upperframe = obv->obv_upper.obvt_frame;
  GtkWidget *lowerframe = obv->obv_lower.obvt_frame;
  gtk_container_remove (GTK_CONTAINER (obwin->obw_upperobjvbox), upperframe);
  gtk_container_remove (GTK_CONTAINER (obwin->obw_lowerobjvbox), lowerframe);
  memset (obv, 0, sizeof (struct objectview_newgui_stBM));
  free (obv);
}                               /* end of destroy_objectviewbuffer_BM */


void
newgui_browse_add_parens_BM (int openoff,
                             int closeoff,
                             int xtraoff,
                             unsigned
                             openlen,
                             unsigned
                             closelen,
                             unsigned
                             xtralen, int depth, struct stackframe_stBM *stkf)
{
  LOCALFRAME_BM ( /*prev: */ stkf,
                 /*descr: */ NULL,
                 objectval_tyBM * object;
                 objectval_tyBM * obsel;);
  assert (curobjview_newgui_BM != NULL);
  _.object = curobjview_newgui_BM->obv_object;
  _.obsel = curobjview_newgui_BM->obv_obsel;
  if (curobjview_newgui_BM->obv_parenarr == NULL)
    {
      int inisiz = 7;
      struct parenoffset_stBM *newarr = calloc (inisiz,
                                                sizeof (struct
                                                        parenoffset_stBM));
      if (!newarr)
        FATAL_BM
          ("out of memory for parenoffset of size %d for object %s",
           inisiz, objectdbg_BM (_.object));
      curobjview_newgui_BM->obv_parenarr = newarr;
      curobjview_newgui_BM->obv_parenasiz = inisiz;
      curobjview_newgui_BM->obv_parenulen = 0;
    }
  else
    if (curobjview_newgui_BM->obv_parenulen +
        1 >= curobjview_newgui_BM->obv_parenasiz)
    {
      int newsiz =
        prime_above_BM (4 * curobjview_newgui_BM->obv_parenulen / 3 + 6);
      struct parenoffset_stBM *newarr = calloc (newsiz,
                                                sizeof (struct
                                                        parenoffset_stBM));
      if (!newarr)
        FATAL_BM
          ("out of memory for parenoffset growing to size %d for object %s",
           newsiz, objectdbg_BM (_.object));
      memcpy (newarr,
              curobjview_newgui_BM->obv_parenarr,
              curobjview_newgui_BM->obv_parenulen
              * sizeof (struct parenoffset_stBM));
      free (curobjview_newgui_BM->obv_parenarr),
        curobjview_newgui_BM->obv_parenarr = newarr;
      curobjview_newgui_BM->obv_parenasiz = newsiz;
    }
  struct parenoffset_stBM *curpar =
    curobjview_newgui_BM->obv_parenarr + curobjview_newgui_BM->obv_parenulen;
  struct objectwindow_newgui_stBM *obwin = curobjview_newgui_BM->obv_obwindow;
  assert (obwin != NULL);
  curpar->paroff_open = openoff;
  curpar->paroff_close = closeoff;
  curpar->paroff_xtra = xtraoff;
  curpar->paroff_openlen = openlen;
  curpar->paroff_closelen = closelen;
  curpar->paroff_xtralen = xtralen;
  curpar->paroff_depth = depth;
  DBGPRINTF_BM ("newgui_browse_add_parens_BM object %s"
                "\n... obwin#%d open#%d/l%d close#%d/l%d xtra#%d/l%d depth %d ulen %d",
                objectdbg_BM (_.object), obwin->obw_rank, openoff, openlen,
                closeoff, closelen, xtraoff, xtralen, depth,
                curobjview_newgui_BM->obv_parenulen);
  curobjview_newgui_BM->obv_parenulen++;
}                               /* end newgui_browse_add_parens_BM */

static void
remove_objectview_newgui_BM (struct
                             objectview_newgui_stBM
                             *obv, struct stackframe_stBM *stkf)
{
  if (!obv)
    return;
  LOCALFRAME_BM ( /*prev: */ stkf,
                 /*descr: */ NULL,
                 objectval_tyBM * object;
                 objectval_tyBM * obsel;);
  curobjview_newgui_BM = NULL;
  struct objectwindow_newgui_stBM *obwin = obv->obv_obwindow;
  assert (obwin != NULL);
  _.object = obv->obv_object;
  _.obsel = obv->obv_obsel;
  int rk = obv->obv_rank;
  DBGPRINTF_BM
    ("remove_objectview_newgui_BM obv@%p obwin@%p rank#%d object %s obsel %s",
     obv, obwin, obv->obv_rank,
     objectdbg_BM (_.object), objectdbg1_BM (_.obsel));
  destroy_objectviewbuffer_BM (obv, (struct stackframe_stBM *) &_);
  int winlen = obwin->obw_ulen;
  struct objectview_newgui_stBM **warr = obwin->obw_arr;
  assert (warr != NULL);
  for (int ix = rk; ix < winlen - 1; ix++)
    {
      struct objectview_newgui_stBM *curobv = warr[ix] = warr[ix + 1];
      curobv->obv_rank = ix;
    };
  warr[winlen] = NULL;
  obwin->obw_ulen = winlen - 1;
  if (obwin->obw_asiz > 10 && 2 * winlen < obwin->obw_asiz)
    {
      int newsiz = prime_above_BM (5 * winlen / 4 + 3);
      if (newsiz < obwin->obw_asiz)
        {
          struct objectview_newgui_stBM **newarr = calloc (newsiz,
                                                           sizeof (struct
                                                                   objectview_newgui_stBM
                                                                   *));
          if (!newarr)
            FATAL_BM
              ("cannot shrink obwindow#%d to %d", obwin->obw_rank, newsiz);
          memcpy (newarr, warr,
                  (winlen - 1) * sizeof (struct objectview_newgui_stBM *));
          free (obwin->obw_arr), obwin->obw_arr = newarr;
        }
    }
}                               /* end remove_objectview_newgui_BM  */


void
obwin_start_refresh_newgui_BM (struct objectwindow_newgui_stBM *obw)
{
  if (!obw)
    return;
  if (obw->obw_refreshid)
    g_source_remove (obw->obw_refreshid), obw->obw_refreshid = 0;
  if (obw->obw_refreshperiod > 0)
    obw->obw_refreshid =
      g_timeout_add_seconds (obw->obw_refreshperiod,
                             refresh_obwin_newgui_cbBM, obw);
}                               /* end obwin_start_refresh_newgui_BM */

void
obwin_stop_refresh_newgui_BM (struct objectwindow_newgui_stBM *obw)
{
  if (!obw)
    return;
  if (obw->obw_refreshid)
    g_source_remove (obw->obw_refreshid), obw->obw_refreshid = 0;
}                               /* end obwin_start_refresh_newgui_BM */

void
spinrefresh_obwin_newgui_cbBM (GtkSpinButton * spbut, gpointer data)
{
  assert (data != 0);
  struct objectwindow_newgui_stBM *obw =
    (struct objectwindow_newgui_stBM *) data;
  assert (obw->obw_refreshspinbox == GTK_WIDGET (spbut));
  int newdelay = gtk_spin_button_get_value_as_int (spbut);
  DBGPRINTF_BM
    ("spinrefresh_obwin_newgui_cbBM obwrank#%d olddelay %d newdelay %d incomplete obw@%p",
     obw->obw_rank, obw->obw_refreshperiod, newdelay, obw);
  obw->obw_refreshperiod = newdelay;
  if (newdelay > 0)
    obwin_start_refresh_newgui_BM (obw);
  else if (obw->obw_refreshid > 0)
    obwin_stop_refresh_newgui_BM (obw);
  DBGPRINTF_BM ("spinrefresh_obwin_newgui_cbBM ending obw@%p", obw);
}                               /* end spinrefresh_obwin_newgui_cbBM */


gboolean
refresh_obwin_newgui_cbBM (gpointer data)
{
  assert (data != 0);
  struct objectwindow_newgui_stBM *obw =
    (struct objectwindow_newgui_stBM *) data;
  LOCALFRAME_BM ( /*prev: */ NULL, /*descr: */ NULL,
                 objectval_tyBM * obj;
                 objectval_tyBM * shobsel;);
  int ulen = obw->obw_ulen;
  DBGPRINTF_BM ("refresh_obwin_newgui_cbBM obw@%p #%d start ulen=%d", obw,
                obw->obw_rank, ulen);
  struct objectview_newgui_stBM **warr = obw->obw_arr;
  assert (ulen == 0 || warr != NULL);
  for (int ix = 0; ix < ulen; ix++)
    {
      struct objectview_newgui_stBM *obv = warr[ix];
      if (!obv)
        continue;
      _.obj = obv->obv_object;
      _.shobsel = obv->obv_obsel;
      fill_objectviewbuffer_BM (obv, (struct stackframe_stBM *) &_);
    }
  DBGPRINTF_BM ("refresh_obwin_newgui_cbBM obw#%d end", obw->obw_rank);
  if (obw->obw_refreshperiod > 0)
    return G_SOURCE_CONTINUE;
  else
    return G_SOURCE_REMOVE;
}                               /* end refresh_obwin_newgui_cbBM */



void
markset_newgui_objview_BM (GtkTextBuffer * tbuf, GtkTextIter * titer,
                           GtkTextMark * tmark, gpointer cdata)
{
  assert (cdata != NULL);
  struct objectview_newgui_stBM *obv =
    (struct objectview_newgui_stBM *) cdata;
  assert (obv->obv_tbuffer == tbuf);
  if (tmark != gtk_text_buffer_get_insert (tbuf))
    return;
  unsigned off = gtk_text_iter_get_offset (titer);
  int parulen = obv->obv_parenulen;
  struct objectwindow_newgui_stBM *obwin = obv->obv_obwindow;
  assert (obwin != NULL);
#warning should probably deal with obwin kept focus and blink...
  struct parenoffset_stBM *pararr = obv->obv_parenarr;
  DBGPRINTF_BM
    ("markset_newgui_objview obv@%p #%d object %s titer=%s off=%u parulen=%d",
     obv, obv->obv_rank, objectdbg_BM (obv->obv_object),
     textiterstrdbg_BM (titer), off, parulen);
  struct parenoffset_stBM *par = paren_objview_at_offset_newgui_BM (obv, off);
  if (par != NULL)
    {
      int parix = (int) (par - obv->obv_parenarr);
      DBGPRINTF_BM
        ("markset_newgui_objview will blink parix#%d off=%u open:%u close:%u",
         parix, off, par->paroff_open, par->paroff_close);
      enable_blink_objectview_BM (obv, parix);
    }
  else
    {
      DBGPRINTF_BM ("markset_newgui_objview no blink off=%u", off);
      disable_blink_objectview_BM (obv);
      obwin_start_refresh_newgui_BM (obv->obv_obwindow);
    }
}                               /* end markset_newgui_cmd_BM */


struct parenoffset_stBM *
paren_objview_at_offset_newgui_BM (struct objectview_newgui_stBM *obv,
                                   unsigned off)
{
  if (!obv)
    return NULL;
  int parulen = obv->obv_parenulen;
  DBGPRINTF_BM
    ("paren_objview_at_offset_newgui start objview %s off %u ulen %d",
     objectdbg_BM (obv->obv_object), off, parulen);
  struct objectwindow_newgui_stBM *obwin = obv->obv_obwindow;
  assert (obwin != NULL);
  struct parenoffset_stBM *pararr = obv->obv_parenarr;
  if (parulen == 0)
    return NULL;
  assert (pararr != NULL);
  int lo = 0, hi = (int) parulen, md = 0;
  while (lo + 4 < hi)
    {
      DBGPRINTF_BM ("paren_objview_at_offset_newgui lo=%d hi=%d", lo, hi);
      if (pararr[lo].paroff_open <= off && off <= pararr[hi - 1].paroff_close)
        break;
      md = (lo + hi) / 2;
      if (pararr[md].paroff_open < off && pararr[md].paroff_close < off)
        lo = md;
      else if (pararr[md].paroff_close > off)
        hi = md;
      else
        break;
    }
  int ix = -1, w = -1;
  for (md = lo; md < hi; md++)
    {
      if (parens_surrounds_BM (pararr + md, off))
        {
          if (ix < 0)
            {
              ix = md;
              w = pararr[md].paroff_close - pararr[md].paroff_open;
            }
          else
            {
              assert (w >= 0);
              int newi = pararr[md].paroff_close - pararr[md].paroff_open;
              if (newi < w)
                {
                  ix = md;
                  w = newi;
                };
            }
        }
    }
  DBGPRINTF_BM ("paren_objview_at_offset_newgui off=%d ix=%d", off, ix);
  if (ix >= 0)
    return pararr + ix;
  return NULL;
}                               /* end paren_objview_at_offset_newgui_BM */



void
beginuact_newgui_objview_BM (GtkTextBuffer * tbuf, gpointer cdata)
{
  assert (cdata != NULL);
  struct objectview_newgui_stBM *obv =
    (struct objectview_newgui_stBM *) cdata;
  assert (obv->obv_tbuffer == tbuf);
  struct objectwindow_newgui_stBM *obwin = obv->obv_obwindow;
  assert (obwin != NULL);
  DBGPRINTF_BM ("beginuact_newgui_objview obv@%p #%d object %s incomplete",
                obv, obv->obv_rank, objectdbg_BM (obv->obv_object));
#warning beginuact_newgui_objview_BM incomplete
}                               /* end  beginuact_newgui_objview_BM */

void
enduact_newgui_objview_BM (GtkTextBuffer * tbuf, gpointer cdata)
{
  assert (cdata != NULL);
  struct objectview_newgui_stBM *obv =
    (struct objectview_newgui_stBM *) cdata;
  assert (obv->obv_tbuffer == tbuf);
  struct objectwindow_newgui_stBM *obwin = obv->obv_obwindow;
  assert (obwin != NULL);
  DBGPRINTF_BM ("enduact_newgui_objview obv@%p #%d object %s incomplete",
                obv, obv->obv_rank, objectdbg_BM (obv->obv_object));
#warning enduact_newgui_objview_BM incomplete
}                               /* end  enduact_newgui_objview_BM */

void
enable_blink_objectview_BM (struct objectview_newgui_stBM *obv, int ix)
{
  assert (obv != NULL);
  if (obv->obv_blinkid > 0)
    g_source_remove (obv->obv_blinkid), obv->obv_blinkid = 0;
  obv->obv_blinkix = ix;
  obv->obv_blinkcount = 0;
  GtkTextIter startit = EMPTY_TEXT_ITER_BM;
  GtkTextIter endit = EMPTY_TEXT_ITER_BM;
  gtk_text_buffer_get_bounds (obv->obv_tbuffer, &startit, &endit);
  gtk_text_buffer_remove_tag (obv->obv_tbuffer, blink_brotag_BM, &startit,
                              &endit);
  if (ix > 0)
    {
      (void) blink_objectview_cbBM (obv);
      obv->obv_blinkid =
        g_timeout_add (BROWSE_BLINKMILLISECOND_NEWGUI_BM,
                       blink_objectview_cbBM, obv);
    }
}                               /* end enable_blink_objectview_BM */

void
disable_blink_objectview_BM (struct objectview_newgui_stBM *obv)
{
  if (obv->obv_blinkid > 0)
    g_source_remove (obv->obv_blinkid);
  obv->obv_blinkid = 0;
  obv->obv_blinkix = -1;
  GtkTextIter startit = EMPTY_TEXT_ITER_BM;
  GtkTextIter endit = EMPTY_TEXT_ITER_BM;
  gtk_text_buffer_get_bounds (obv->obv_tbuffer, &startit, &endit);
  gtk_text_buffer_remove_tag (obv->obv_tbuffer, blink_brotag_BM, &startit,
                              &endit);
}                               /* end disable_blink_objectview_BM */

gboolean
blink_objectview_cbBM (gpointer data)
{
  struct objectview_newgui_stBM *obv = (struct objectview_newgui_stBM *) data;
  assert (obv != NULL);
  assert (pthread_self () == mainthreadid_BM);
  if (obv->obv_blinkix < 0 || obv->obv_blinkix >= (int) obv->obv_parenulen)
    return G_SOURCE_REMOVE;
  struct parenoffset_stBM *par = obv->obv_parenarr + obv->obv_blinkix;
  obv->obv_blinkcount++;
  GtkTextIter startit = EMPTY_TEXT_ITER_BM;
  GtkTextIter endit = EMPTY_TEXT_ITER_BM;
  if (obv->obv_blinkcount % 3 != 0)
    {
      if (par->paroff_open >= 0 && par->paroff_openlen > 0)
        {
          gtk_text_buffer_get_iter_at_offset (obv->obv_tbuffer, &startit,
                                              par->paroff_open);
          endit = startit;
          gtk_text_iter_forward_chars (&endit, par->paroff_openlen);
          gtk_text_buffer_apply_tag (obv->obv_tbuffer, blink_brotag_BM,
                                     &startit, &endit);
        }
      if (par->paroff_close >= 0 && par->paroff_closelen > 0)
        {
          gtk_text_buffer_get_iter_at_offset (obv->obv_tbuffer, &endit,
                                              par->paroff_close);
          startit = endit;
          gtk_text_iter_backward_chars (&endit, par->paroff_closelen);
          gtk_text_buffer_apply_tag (obv->obv_tbuffer, blink_brotag_BM,
                                     &startit, &endit);
        }
      if (par->paroff_xtra >= 0 && par->paroff_xtralen > 0)
        {
          gtk_text_buffer_get_iter_at_offset (obv->obv_tbuffer, &startit,
                                              par->paroff_xtra);
          endit = startit;
          gtk_text_iter_forward_chars (&endit, par->paroff_xtralen);
          gtk_text_buffer_apply_tag (obv->obv_tbuffer, blink_brotag_BM,
                                     &startit, &endit);
        }
    }
  else
    {
      GtkTextIter startit = EMPTY_TEXT_ITER_BM;
      GtkTextIter endit = EMPTY_TEXT_ITER_BM;
      gtk_text_buffer_get_bounds (obv->obv_tbuffer, &startit, &endit);
      gtk_text_buffer_remove_tag (obv->obv_tbuffer, blink_brotag_BM,
                                  &startit, &endit);
    }
  return G_SOURCE_CONTINUE;
}                               /* end blink_objectview_cbBM */


void
populatepopup_objview_newgui_BM (GtkTextView * txview, GtkWidget * popup,
                                 gpointer data)
{
  struct objectview_newgui_stBM *obv = (struct objectview_newgui_stBM *) data;
  assert (obv != NULL);
  assert (pthread_self () == mainthreadid_BM);
  assert (txview == GTK_TEXT_VIEW (obv->obv_upper.obvt_textview)
          || txview == GTK_TEXT_VIEW (obv->obv_lower.obvt_textview));
  char cursinfobuf[32];
  memset (cursinfobuf, 0, sizeof (cursinfobuf));
  GtkTextIter cursit = EMPTY_TEXT_ITER_BM;
  gtk_text_buffer_get_iter_at_mark      //
    (obv->obv_tbuffer, &cursit,
     gtk_text_buffer_get_insert (obv->obv_tbuffer));
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
  gtk_widget_show_all (popup);
}                               /* end populatepopup_objview_newgui_BM */
