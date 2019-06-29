// Copyright 2019 Tuomas Siipola
// SPDX-License-Identifier: GPL-3.0-or-later

#include <stdbool.h>

#include <gtk/gtk.h>
#include <glib.h>

#include "alsa.h"
#include "metronome.h"

int bpm = 120;
bool running = false;

GtkWidget *button = NULL;

void bpm_adjustment_value_changed(GtkAdjustment *adjustment, gpointer user_data)
{
    bpm = gtk_adjustment_get_value(adjustment);
}

void button_clicked(GtkButton *button, gpointer user_data)
{
    if (running) {
        running = false;
        gtk_button_set_label(button, "Start");
    } else {
        running = true;
        gtk_button_set_label(button, "Stop");
    }
}

gpointer alsa_thread(gpointer data)
{
    alsa_init(48000);
    while (true) {
        if (running) {
            uint16_t *buffer = NULL;
            size_t size;
            metronome_generate(&buffer, &size, 48000, bpm, 4, 4, SOUND_SINE);
            alsa_play(buffer, size);
            free(buffer);
        }
    }
    alsa_free();
    return NULL;
}

int main(int argc, char *argv[])
{
    gtk_init(&argc, &argv);

    g_thread_new("alsa", alsa_thread, NULL);

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Tahdin");
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(window), box);

    GtkAdjustment *bpm_adjustment = gtk_adjustment_new(bpm, 30.0, 300.0, 1.0, 5.0, 0.0);
    g_signal_connect(G_OBJECT(bpm_adjustment), "value-changed", G_CALLBACK(bpm_adjustment_value_changed), NULL);

    GtkWidget *spin_button = gtk_spin_button_new(bpm_adjustment, 1.0, 0);
    gtk_container_add(GTK_CONTAINER(box), spin_button);

    button = gtk_button_new_with_label("Start");
    gtk_container_add(GTK_CONTAINER(box), button);
    g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(button_clicked), NULL);

    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}
