/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * main.c
 * Copyright (C) 2013 Ivan A. Flores C. <ifloresenator@gmail.com>
 * 
 * afinador-guitarra is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * afinador-guitarra is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <config.h>
#include <gtk/gtk.h>
#include <gst/gst.h>

#include <glib/gi18n.h>

/* Frequencies of the strings */
#define NOTE_E 329.63
#define NOTE_A 440
#define NOTE_D 587.33
#define NOTE_G 783.99
#define NOTE_B 987.77
#define NOTE_e 1318.5


typedef struct _Private Private;
static struct _Private
{
	/* ANJUTA: Widgets declaration for afinador_guitarra.ui - DO NOT REMOVE */
};

static struct Private* priv = NULL;

/* For testing propose use the local (not installed) ui file */
/* #define UI_FILE PACKAGE_DATA_DIR"/ui/afinador_guitarra.ui" */
#define UI_FILE "src/afinador_guitarra.ui"
#define TOP_WINDOW "window"

/* Signal handlers */
/* Note: These may not be declared static because signal autoconnection
 * only works with non-static methods
 */

/* Called when the window is closed */
void
destroy (GtkWidget *widget, gpointer data)
{
	gtk_main_quit ();
}

static GtkWidget*
create_window (void)
{
	GtkWidget *window;
	GtkBuilder *builder;
	GError* error = NULL;

	/* Load UI from file */
	builder = gtk_builder_new ();
	if (!gtk_builder_add_from_file (builder, UI_FILE, &error))
	{
		g_critical ("Couldn't load builder file: %s", error->message);
		g_error_free (error);
	}

	/* Auto-connect signal handlers */
	gtk_builder_connect_signals (builder, NULL);

	/* Get the window object from the ui file */
	window = GTK_WIDGET (gtk_builder_get_object (builder, TOP_WINDOW));
        if (!window)
        {
                g_critical ("Widget \"%s\" is missing in file %s.",
				TOP_WINDOW,
				UI_FILE);
        }

	priv = g_malloc (sizeof (struct _Private));
	/* ANJUTA: Widgets initialization for afinador_guitarra.ui - DO NOT REMOVE */

	g_object_unref (builder);
	
	return window;
}


int
main (int argc, char *argv[])
{
 	GtkWidget *window;


#ifdef ENABLE_NLS
	bindtextdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
	textdomain (GETTEXT_PACKAGE);
#endif

	gst_init (&argc, &argv);
	gtk_init (&argc, &argv);

	window = create_window ();
	gtk_widget_show (window);

	gtk_main ();

	g_free (priv);

	return 0;
}

#define LENGTH 500 /* Length of playing in ms */

static gboolean
pipeline_stop (GstElement* pipeline)
{
	gst_element_set_state (pipeline, GST_STATE_PAUSED);
	g_object_unref (pipeline);

	return FALSE;
}

static void
play_sound (gdouble frequency)
{
	GstElement *source, *sink;
	GstElement *pipeline;

	pipeline = gst_pipeline_new ("note");
	source   = gst_element_factory_make ("audiotestsrc",
	                                     "source");
	sink     = gst_element_factory_make ("autoaudiosink",
	                                     "output");

	/* set frequency */
	g_object_set (source, "freq", frequency, NULL);

	gst_bin_add_many (GST_BIN (pipeline), source, sink, NULL);
	gst_element_link (source, sink);

	gst_element_set_state (pipeline, GST_STATE_PLAYING);

	/* stop it after 500ms */
	g_timeout_add (LENGTH, (GSourceFunc) pipeline_stop, pipeline);
}


void
on_button_clicked (GtkButton *button, gpointer user_data)
{
	tkWidget* label = gtk_bin_get_child (GTK_BIN (button));
	const gchar* text = gtk_label_get_label (GTK_LABEL (label));

	if (g_str_equal (text, _("E")))
	    play_sound (NOTE_E);
	else if (g_str_equal (text, _("A")))
	    play_sound (NOTE_A);
	else if (g_str_equal (text, _("G")))
	    play_sound (NOTE_G);
	else if (g_str_equal (text, _("D")))
	    play_sound (NOTE_D);
	else if (g_str_equal (text, _("B")))
	    play_sound (NOTE_B);
	else if (g_str_equal (text, _("e")))
	    play_sound (NOTE_e);
}
