#include <gtk/gtk.h>
#include <unistd.h>

typedef struct {
    const char *name;
    const char *icon;
    const char *command;
} App;

static App apps[] = {
    { "Terminal",   "utilities-terminal",    "alacritty" },
    { "Files",      "system-file-manager",   "nautilus" },
    { "Editor",     "text-editor",           "gedit" },
    { "Browser",    "web-browser",           "firefox" },
    { "Settings",   "preferences-system",    "gnome-control-center" },
    { "Calculator", "accessories-calculator","gnome-calculator" },
};
static int n_apps = 6;

static void on_app_clicked(GtkWidget *widget, gpointer user_data) {
    const char *command = (const char *)user_data;
    if (fork() == 0) {
        execl("/bin/sh", "/bin/sh", "-c", command, (void *)NULL);
    }
}

static GtkWidget *make_app_button(App *app) {
    /* Vertical box: icon on top, label below */
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_widget_set_margin_top(box, 8);
    gtk_widget_set_margin_bottom(box, 8);
    gtk_widget_set_margin_start(box, 8);
    gtk_widget_set_margin_end(box, 8);

    /* Icon */
    GtkWidget *image = gtk_image_new_from_icon_name(app->icon);
    gtk_image_set_pixel_size(GTK_IMAGE(image), 48);

    /* Label */
    GtkWidget *label = gtk_label_new(app->name);

    gtk_box_append(GTK_BOX(box), image);
    gtk_box_append(GTK_BOX(box), label);

    /* Button wraps the box */
    GtkWidget *button = gtk_button_new();
    gtk_button_set_child(GTK_BUTTON(button), box);
    gtk_widget_add_css_class(button, "app-button");

    g_signal_connect(button, "clicked", G_CALLBACK(on_app_clicked),
        (gpointer)app->command);

    return button;
}

static void apply_css(GtkWidget *window) {
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_string(provider,
        "window { background-color: #0d1117; }"
        ".app-button {"
        "  background-color: #1a1f2e;"
        "  border-radius: 12px;"
        "  padding: 8px;"
        "  color: white;"
        "}"
        ".app-button:hover {"
        "  background-color: #D4AF37;"
        "  color: #0d1117;"
        "}"
        ".search-entry {"
        "  background-color: #1a1f2e;"
        "  color: white;"
        "  padding: 8px;"
        "  border-radius: 8px;"
        "  margin-bottom: 16px;"
        "}"
    );
    gtk_style_context_add_provider_for_display(
        gtk_widget_get_display(window),
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_USER
    );
    g_object_unref(provider);
}

static void on_activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "EduOS Launcher");
    gtk_window_set_default_size(GTK_WINDOW(window), 500, 400);

    /* Outer vertical box */
    GtkWidget *outer = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_margin_top(outer, 24);
    gtk_widget_set_margin_bottom(outer, 24);
    gtk_widget_set_margin_start(outer, 24);
    gtk_widget_set_margin_end(outer, 24);

    /* Search bar */
    GtkWidget *entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry), "Search apps...");
    gtk_widget_add_css_class(entry, "search-entry");
    gtk_box_append(GTK_BOX(outer), entry);

    /* App grid — 3 columns */
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 8);

    for (int i = 0; i < n_apps; i++) {
        int col = i % 3;
        int row = i / 3;
        GtkWidget *btn = make_app_button(&apps[i]);
        gtk_grid_attach(GTK_GRID(grid), btn, col, row, 1, 1);
    }

    gtk_box_append(GTK_BOX(outer), grid);
    gtk_window_set_child(GTK_WINDOW(window), outer);
    apply_css(window);
    gtk_window_present(GTK_WINDOW(window));
}

int main(int argc, char *argv[]) {
    GtkApplication *app = gtk_application_new("org.eduos.launcher",
        G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(on_activate), NULL);
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}