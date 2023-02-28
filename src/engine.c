/* vim:set et sts=4: */

#include "engine.h"

typedef struct _IBusWhisperEngine IBusWhisperEngine;
typedef struct _IBusWhisperEngineClass IBusWhisperEngineClass;

struct _IBusWhisperEngine {
	IBusEngine parent;

    /* members */
    GString *preedit;
    gint cursor_pos;

    IBusLookupTable *table;
};

struct _IBusWhisperEngineClass {
	IBusEngineClass parent;
};

/* functions prototype */
static void	ibus_whisper_engine_class_init	(IBusWhisperEngineClass	*klass);
static void	ibus_whisper_engine_init		(IBusWhisperEngine		*engine);
static void	ibus_whisper_engine_destroy		(IBusWhisperEngine		*engine);
static gboolean
			ibus_whisper_engine_process_key_event
                                            (IBusEngine             *engine,
                                             guint               	 keyval,
                                             guint               	 keycode,
                                             guint               	 modifiers);
static void ibus_whisper_engine_focus_in    (IBusEngine             *engine);
static void ibus_whisper_engine_focus_out   (IBusEngine             *engine);
static void ibus_whisper_engine_reset       (IBusEngine             *engine);
static void ibus_whisper_engine_enable      (IBusEngine             *engine);
static void ibus_whisper_engine_disable     (IBusEngine             *engine);
static void ibus_engine_set_cursor_location (IBusEngine             *engine,
                                             gint                    x,
                                             gint                    y,
                                             gint                    w,
                                             gint                    h);
static void ibus_whisper_engine_set_capabilities
                                            (IBusEngine             *engine,
                                             guint                   caps);
static void ibus_whisper_engine_page_up     (IBusEngine             *engine);
static void ibus_whisper_engine_page_down   (IBusEngine             *engine);
static void ibus_whisper_engine_cursor_up   (IBusEngine             *engine);
static void ibus_whisper_engine_cursor_down (IBusEngine             *engine);
static void ibus_whisper_property_activate  (IBusEngine             *engine,
                                             const gchar            *prop_name,
                                             gint                    prop_state);
static void ibus_whisper_engine_property_show
											(IBusEngine             *engine,
                                             const gchar            *prop_name);
static void ibus_whisper_engine_property_hide
											(IBusEngine             *engine,
                                             const gchar            *prop_name);

static void ibus_whisper_engine_commit_string
                                            (IBusWhisperEngine      *whisper,
                                             const gchar            *string);
static void ibus_whisper_engine_update      (IBusWhisperEngine      *whisper);


G_DEFINE_TYPE (IBusWhisperEngine, ibus_whisper_engine, IBUS_TYPE_ENGINE)

static void
ibus_whisper_engine_class_init (IBusWhisperEngineClass *klass)
{
	IBusObjectClass *ibus_object_class = IBUS_OBJECT_CLASS (klass);
	IBusEngineClass *engine_class = IBUS_ENGINE_CLASS (klass);

	ibus_object_class->destroy = (IBusObjectDestroyFunc) ibus_whisper_engine_destroy;

    engine_class->process_key_event = ibus_whisper_engine_process_key_event;
}

static void
ibus_whisper_engine_init (IBusWhisperEngine *whisper)
{

}

static void
ibus_whisper_engine_destroy (IBusWhisperEngine *whisper)
{

}

static void
ibus_whisper_engine_update_lookup_table (IBusWhisperEngine *whisper)
{
}

static void
ibus_whisper_engine_update_preedit (IBusWhisperEngine *whisper)
{
    // IBusText *text;
    // gint retval;

    // text = ibus_text_new_from_static_string (whisper->preedit->str);
    // text->attrs = ibus_attr_list_new ();

    // ibus_attr_list_append (text->attrs,
    //                        ibus_attr_underline_new (IBUS_ATTR_UNDERLINE_SINGLE, 0, whisper->preedit->len));

    // if (whisper->preedit->len > 0) {
    //     retval = whisper_dict_check (dict, whisper->preedit->str, whisper->preedit->len);
    //     if (retval != 0) {
    //         ibus_attr_list_append (text->attrs,
    //                            ibus_attr_foreground_new (0xff0000, 0, whisper->preedit->len));
    //     }
    // }

    // ibus_engine_update_preedit_text ((IBusEngine *)whisper,
    //                                  text,
    //                                  whisper->cursor_pos,
    //                                  TRUE);

}

/* commit preedit to client and update preedit */
static gboolean
ibus_whisper_engine_commit_preedit (IBusWhisperEngine *whisper)
{
    if (whisper->preedit->len == 0)
        return FALSE;

    ibus_whisper_engine_commit_string (whisper, whisper->preedit->str);
    g_string_assign (whisper->preedit, "");
    whisper->cursor_pos = 0;

    ibus_whisper_engine_update (whisper);

    return TRUE;
}


static void
ibus_whisper_engine_commit_string (IBusWhisperEngine *whisper,
                                   const gchar       *string)
{
    IBusText *text;
    text = ibus_text_new_from_static_string (string);
    ibus_engine_commit_text ((IBusEngine *)whisper, text);
}

static void
ibus_whisper_engine_update (IBusWhisperEngine *whisper)
{
    ibus_whisper_engine_update_preedit (whisper);
    ibus_engine_hide_lookup_table ((IBusEngine *)whisper);
}

#define is_alpha(c) (((c) >= IBUS_a && (c) <= IBUS_z) || ((c) >= IBUS_A && (c) <= IBUS_Z))

static gboolean
ibus_whisper_engine_process_key_event (IBusEngine *engine,
                                       guint       keyval,
                                       guint       keycode,
                                       guint       modifiers)
{
    IBusText *text;
    IBusWhisperEngine *whisper = (IBusWhisperEngine *)engine;

    if (modifiers & IBUS_RELEASE_MASK)
        return FALSE;

    modifiers &= (IBUS_CONTROL_MASK | IBUS_MOD1_MASK);

    if (modifiers == IBUS_CONTROL_MASK && keyval == IBUS_s) {
        ibus_whisper_engine_update_lookup_table (whisper);
        return TRUE;
    }

    if (modifiers != 0) {
        if (whisper->preedit->len == 0)
            return FALSE;
        else
            return TRUE;
    }


    switch (keyval) {
    case IBUS_space:
        g_string_append (whisper->preedit, " ");
        return ibus_whisper_engine_commit_preedit (whisper);
    case IBUS_Return:
        return ibus_whisper_engine_commit_preedit (whisper);

    case IBUS_Escape:
        if (whisper->preedit->len == 0)
            return FALSE;

        g_string_assign (whisper->preedit, "");
        whisper->cursor_pos = 0;
        ibus_whisper_engine_update (whisper);
        return TRUE;

    case IBUS_Left:
        if (whisper->preedit->len == 0)
            return FALSE;
        if (whisper->cursor_pos > 0) {
            whisper->cursor_pos --;
            ibus_whisper_engine_update (whisper);
        }
        return TRUE;

    case IBUS_Right:
        if (whisper->preedit->len == 0)
            return FALSE;
        if (whisper->cursor_pos < whisper->preedit->len) {
            whisper->cursor_pos ++;
            ibus_whisper_engine_update (whisper);
        }
        return TRUE;

    case IBUS_Up:
        if (whisper->preedit->len == 0)
            return FALSE;
        if (whisper->cursor_pos != 0) {
            whisper->cursor_pos = 0;
            ibus_whisper_engine_update (whisper);
        }
        return TRUE;

    case IBUS_Down:
        if (whisper->preedit->len == 0)
            return FALSE;

        if (whisper->cursor_pos != whisper->preedit->len) {
            whisper->cursor_pos = whisper->preedit->len;
            ibus_whisper_engine_update (whisper);
        }

        return TRUE;

    case IBUS_BackSpace:
        if (whisper->preedit->len == 0)
            return FALSE;
        if (whisper->cursor_pos > 0) {
            whisper->cursor_pos --;
            g_string_erase (whisper->preedit, whisper->cursor_pos, 1);
            ibus_whisper_engine_update (whisper);
        }
        return TRUE;

    case IBUS_Delete:
        if (whisper->preedit->len == 0)
            return FALSE;
        if (whisper->cursor_pos < whisper->preedit->len) {
            g_string_erase (whisper->preedit, whisper->cursor_pos, 1);
            ibus_whisper_engine_update (whisper);
        }
        return TRUE;
    }

    if (is_alpha (keyval)) {
        g_string_insert_c (whisper->preedit,
                           whisper->cursor_pos,
                           keyval);

        whisper->cursor_pos ++;
        ibus_whisper_engine_update (whisper);

        return TRUE;
    }

    return FALSE;
}
