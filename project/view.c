#include <gtk/gtk.h>
#include <string.h>
#include <stdlib.h>
#include "model.h"
#include "controller.h"

GtkWidget *output_view, *input_entry, *message_view;
ShmBuf *shmp;

void update_messages() {
    char *msg = model_read_messages(shmp);
    gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(message_view)), msg, -1);
    free(msg);
}

gboolean timer_callback(gpointer data) {
    update_messages();
    return TRUE;
}

void on_enter(GtkEntry *entry, gpointer user_data) {
    const char *input = gtk_entry_get_text(entry);
    char *result = handle_input(shmp, input);

    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(output_view));
    gtk_text_buffer_set_text(buffer, result, -1);

    gtk_entry_set_text(entry, "");
    free(result);
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);
    shmp = buf_init();

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Multi-User Shell");
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 400);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);

    output_view = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(output_view), FALSE);
    gtk_box_pack_start(GTK_BOX(vbox), output_view, TRUE, TRUE, 0);

    message_view = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(message_view), FALSE);
    gtk_box_pack_start(GTK_BOX(vbox), message_view, TRUE, TRUE, 0);

    input_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(input_entry), "Enter command or @msg message");
    g_signal_connect(input_entry, "activate", G_CALLBACK(on_enter), NULL);
    gtk_box_pack_start(GTK_BOX(vbox), input_entry, FALSE, FALSE, 0);

    gtk_container_add(GTK_CONTAINER(window), vbox);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    g_timeout_add(1000, timer_callback, NULL); // 1 saniyede bir shared memory güncelle

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}
