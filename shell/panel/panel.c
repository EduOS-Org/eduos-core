#include <gtk/gtk.h>
#include <gtk4-layer-shell/gtk4-layer-shell.h>
#include <unistd.h>

static void on_launcher_clicked(GtkWidget *widget, gpointer user_data) {
    g_print("Launcher button clicked\n");
    if (fork() == 0) {
        execl("/bin/sh", "/bin/sh", "-c",
            "/home/davidokocha/eduos-core/shell/launcher/launcher",
            (void *)NULL);
    }
}

static void on_activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window = gtk_application_window_new(app);

    /* Initialize layer shell for this window */
    gtk_layer_init_for_window(GTK_WINDOW(window));

    /* Anchor to bottom edge */
    gtk_layer_set_anchor(GTK_WINDOW(window), GTK_LAYER_SHELL_EDGE_BOTTOM, TRUE);
    gtk_layer_set_anchor(GTK_WINDOW(window), GTK_LAYER_SHELL_EDGE_LEFT, TRUE);
    gtk_layer_set_anchor(GTK_WINDOW(window), GTK_LAYER_SHELL_EDGE_RIGHT, TRUE);

    /* Layer — top so it stays above app windows */
    gtk_layer_set_layer(GTK_WINDOW(window), GTK_LAYER_SHELL_LAYER_TOP);

    /* Exclusive zone — reserve 48px at bottom */
    gtk_layer_set_exclusive_zone(GTK_WINDOW(window), 48);

    /* Fixed height */
    gtk_widget_set_size_request(window, -1, 48);

    /* CSS */
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_string(provider,
        "window {"
        "  background-color: #0d1117;"
        "  border-top: 1px solid #D4AF37;"
        "}"
        ".panel-btn {"
        "  background-color: transparent;"
        "  color: #D4AF37;"
        "  border-radius: 6px;"
        "  padding: 4px 16px;"
        "  font-size: 13px;"
        "}"
        ".panel-btn:hover {"
        "  background-color: #1a1f2e;"
        "}"
        ".panel-clock {"
        "  color: white;"
        "  font-size: 13px;"
        "  margin-right: 16px;"
        "}"
    );
    gtk_style_context_add_provider_for_display(
        gtk_widget_get_display(window),
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_USER
    );
    g_object_unref(provider);

    /* Panel layout — horizontal box */
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    gtk_widget_set_margin_start(box, 8);
    gtk_widget_set_margin_end(box, 8);

    /* Left — launcher button */
    GtkWidget *launcher_btn = gtk_button_new_with_label("⊞ Apps");
    gtk_widget_add_css_class(launcher_btn, "panel-btn");
    g_signal_connect(launcher_btn, "clicked",
        G_CALLBACK(on_launcher_clicked), NULL);

    /* Center spacer */
    GtkWidget *spacer = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_hexpand(spacer, TRUE);

    /* Right — clock label */
    GtkWidget *clock_label = gtk_label_new("EduOS");
    gtk_widget_add_css_class(clock_label, "panel-clock");

    gtk_box_append(GTK_BOX(box), launcher_btn);
    gtk_box_append(GTK_BOX(box), spacer);
    gtk_box_append(GTK_BOX(box), clock_label);

    gtk_window_set_child(GTK_WINDOW(window), box);
    gtk_window_present(GTK_WINDOW(window));
}

int main(int argc, char *argv[]) {
    GtkApplication *app = gtk_application_new("org.eduos.panel",
        G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(on_activate), NULL);
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}