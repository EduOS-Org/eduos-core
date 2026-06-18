#include <gtk/gtk.h>
#include <unistd.h>

/* One global provider — we reload it to switch themes */
static GtkCssProvider *css_provider = NULL;
static GtkWidget *stack = NULL;
static gboolean is_playground = TRUE;

static const char *PLAYGROUND_CSS =
    "window {"
    "  background-color: #1a0533;"
    "}"
    ".mode-label {"
    "  color: #FFD700;"
    "  font-size: 28px;"
    "  font-weight: bold;"
    "}"
    ".app-button {"
    "  background-color: #7B2FBE;"
    "  color: white;"
    "  border-radius: 24px;"
    "  padding: 20px;"
    "  font-size: 18px;"
    "}"
    ".app-button:hover {"
    "  background-color: #FFD700;"
    "  color: #1a0533;"
    "}"
    ".switch-btn {"
    "  background-color: #FF6B6B;"
    "  color: white;"
    "  border-radius: 20px;"
    "  padding: 12px 24px;"
    "  font-size: 14px;"
    "}";

static const char *EXPLORER_CSS =
    "window {"
    "  background-color: #0d1117;"
    "}"
    ".mode-label {"
    "  color: #D4AF37;"
    "  font-size: 20px;"
    "  font-weight: bold;"
    "}"
    ".app-button {"
    "  background-color: #1a1f2e;"
    "  color: #e0e0e0;"
    "  border-radius: 8px;"
    "  padding: 12px;"
    "  font-size: 14px;"
    "}"
    ".app-button:hover {"
    "  background-color: #D4AF37;"
    "  color: #0d1117;"
    "}"
    ".switch-btn {"
    "  background-color: #1a1f2e;"
    "  color: #D4AF37;"
    "  border-radius: 6px;"
    "  padding: 8px 16px;"
    "  font-size: 13px;"
    "}";

static void apply_theme(GtkWidget *window, const char *css) {
    gtk_css_provider_load_from_string(css_provider, css);
}

static void on_switch_mode(GtkWidget *widget, gpointer user_data) {
    GtkWidget *window = GTK_WIDGET(user_data);
    is_playground = !is_playground;

    if (is_playground) {
        gtk_stack_set_visible_child_name(GTK_STACK(stack), "playground");
        apply_theme(window, PLAYGROUND_CSS);
    } else {
        gtk_stack_set_visible_child_name(GTK_STACK(stack), "explorer");
        apply_theme(window, EXPLORER_CSS);
    }
}

static GtkWidget *make_playground_page(void) {
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 24);
    gtk_widget_set_margin_top(box, 40);
    gtk_widget_set_margin_bottom(box, 40);
    gtk_widget_set_margin_start(box, 40);
    gtk_widget_set_margin_end(box, 40);

    GtkWidget *label = gtk_label_new("🌟 Welcome, Explorer!");
    gtk_widget_add_css_class(label, "mode-label");

    GtkWidget *btn1 = gtk_button_new_with_label("📚 Learn");
    GtkWidget *btn2 = gtk_button_new_with_label("🎮 Play");
    GtkWidget *btn3 = gtk_button_new_with_label("🎨 Draw");

    gtk_widget_add_css_class(btn1, "app-button");
    gtk_widget_add_css_class(btn2, "app-button");
    gtk_widget_add_css_class(btn3, "app-button");

    gtk_box_append(GTK_BOX(box), label);
    gtk_box_append(GTK_BOX(box), btn1);
    gtk_box_append(GTK_BOX(box), btn2);
    gtk_box_append(GTK_BOX(box), btn3);

    return box;
}

static GtkWidget *make_explorer_page(void) {
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 16);
    gtk_widget_set_margin_top(box, 32);
    gtk_widget_set_margin_bottom(box, 32);
    gtk_widget_set_margin_start(box, 32);
    gtk_widget_set_margin_end(box, 32);

    GtkWidget *label = gtk_label_new("EduOS — Explorer Mode");
    gtk_widget_add_css_class(label, "mode-label");

    GtkWidget *btn1 = gtk_button_new_with_label("Terminal");
    GtkWidget *btn2 = gtk_button_new_with_label("Browser");
    GtkWidget *btn3 = gtk_button_new_with_label("Editor");

    gtk_widget_add_css_class(btn1, "app-button");
    gtk_widget_add_css_class(btn2, "app-button");
    gtk_widget_add_css_class(btn3, "app-button");

    gtk_box_append(GTK_BOX(box), label);
    gtk_box_append(GTK_BOX(box), btn1);
    gtk_box_append(GTK_BOX(box), btn2);
    gtk_box_append(GTK_BOX(box), btn3);

    return box;
}

static void on_activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "EduOS Desktop");
    gtk_window_set_default_size(GTK_WINDOW(window), 500, 450);

    /* Create the single global CSS provider */
    css_provider = gtk_css_provider_new();
    gtk_style_context_add_provider_for_display(
        gtk_widget_get_display(window),
        GTK_STYLE_PROVIDER(css_provider),
        GTK_STYLE_PROVIDER_PRIORITY_USER
    );

    /* Outer box */
    GtkWidget *outer = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    /* Stack — holds both pages */
    stack = gtk_stack_new();
    gtk_stack_set_transition_type(GTK_STACK(stack),
        GTK_STACK_TRANSITION_TYPE_CROSSFADE);
    gtk_stack_set_transition_duration(GTK_STACK(stack), 300);

    gtk_stack_add_named(GTK_STACK(stack), make_playground_page(), "playground");
    gtk_stack_add_named(GTK_STACK(stack), make_explorer_page(), "explorer");
    gtk_stack_set_visible_child_name(GTK_STACK(stack), "playground");

    /* Switch button at bottom */
    GtkWidget *switch_btn = gtk_button_new_with_label("Switch Mode");
    gtk_widget_add_css_class(switch_btn, "switch-btn");
    gtk_widget_set_margin_top(switch_btn, 8);
    gtk_widget_set_margin_bottom(switch_btn, 16);
    gtk_widget_set_margin_start(switch_btn, 40);
    gtk_widget_set_margin_end(switch_btn, 40);
    g_signal_connect(switch_btn, "clicked", G_CALLBACK(on_switch_mode), window);

    gtk_box_append(GTK_BOX(outer), stack);
    gtk_box_append(GTK_BOX(outer), switch_btn);

    gtk_window_set_child(GTK_WINDOW(window), outer);

    /* Load initial theme */
    apply_theme(window, PLAYGROUND_CSS);
    gtk_window_present(GTK_WINDOW(window));
}

int main(int argc, char *argv[]) {
    GtkApplication *app = gtk_application_new("org.eduos.desktop",
        G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(on_activate), NULL);
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(css_provider);
    g_object_unref(app);
    return status;
}