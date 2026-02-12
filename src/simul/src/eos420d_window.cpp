
#include <gtk/gtk.h>
#include "eos420d_window.h"

struct ButtonContext {
    EOS400DWindow* ui;
    Button btn;
};

EOS400DWindow::EOS400DWindow(FirmwareBridge& fw)
    : firmware(fw)
{
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Canon EOS 400D Simulator");
    gtk_window_set_default_size(GTK_WINDOW(window), 520, 320);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), nullptr);

    GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    /* LCD */
    lcdView = gtk_text_view_new();
    lcdBuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(lcdView));

    gtk_text_view_set_editable(GTK_TEXT_VIEW(lcdView), FALSE);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(lcdView), FALSE);

    GtkStyleContext* context = gtk_widget_get_style_context(lcdView);

    GtkCssProvider* css = gtk_css_provider_new();
    gtk_css_provider_load_from_data(
        css,
        "textview { font-family: monospace; font-size: 12pt; }",
        -1,
        nullptr
    );

    gtk_style_context_add_provider(
        context,
        GTK_STYLE_PROVIDER(css),
        GTK_STYLE_PROVIDER_PRIORITY_USER
    );

    g_object_unref(css);

    gtk_widget_set_size_request(lcdView, LCD_COLS * 10, LCD_LINES * 20);
    gtk_box_pack_start(GTK_BOX(vbox), lcdView, FALSE, FALSE, 4);

    /* Buttons */
    GtkWidget* grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 5);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 5);
    gtk_box_pack_start(GTK_BOX(vbox), grid, TRUE, TRUE, 4);

    gtk_grid_attach(GTK_GRID(grid), makeButton("WHEEL LEFT", Button::WHEEL_L),  2, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), makeButton("WHEEL RIGHT", Button::WHEEL_R), 4, 0, 1, 1);

    gtk_grid_attach(GTK_GRID(grid), makeButton("DP", Button::DP),       0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), makeButton("MENU", Button::MENU),   0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), makeButton("INFO", Button::INFO),   0, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), makeButton("PLAY", Button::PLAY),   0, 4, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), makeButton("TRASH", Button::TRASH), 0, 5, 1, 1);

    gtk_grid_attach(GTK_GRID(grid), makeButton("UP", Button::UP),       2, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), makeButton("LEFT", Button::LEFT),   1, 4, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), makeButton("SET", Button::SET),     2, 4, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), makeButton("RIGHT", Button::RIGHT), 3, 4, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), makeButton("DOWN", Button::DOWN),   2, 5, 1, 1);

    gtk_grid_attach(GTK_GRID(grid), makeButton("SHUTTER", Button::SHUTTER_FULL),
                    3, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), makeButton("AV", Button::AV),             1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), makeButton("ZOOM OUT", Button::ZOOM_OUT), 2, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), makeButton("ZOOM IN", Button::ZOOM_IN),   3, 1, 1, 1);

    faceSensorToggle = gtk_toggle_button_new_with_label("Face Sensor");
    g_signal_connect(faceSensorToggle, "toggled",
                    G_CALLBACK(EOS400DWindow::onFaceSensorToggled),
                    this);

    // Placement dans la grille (par ex. dernière ligne)
    gtk_grid_attach(GTK_GRID(grid), faceSensorToggle, 4, 1, 1, 1);
}

void EOS400DWindow::onFaceSensorToggled(GtkWidget* widget, gpointer user_data)
{
    EOS400DWindow* ui = static_cast<EOS400DWindow*>(user_data);
    bool active = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ui->faceSensorToggle));

    // Mise à jour du firmware bridge
    ui->firmware.setFaceSensor(active);

    g_print("Face sensor is %s\n", active ? "ACTIVE" : "INACTIVE");
}

GtkWidget* EOS400DWindow::makeButton(const char* label, Button btn)
{
    GtkWidget* button = gtk_button_new_with_label(label);
    ButtonContext* ctx = new ButtonContext{ this, btn };

    g_signal_connect(button, "clicked",
                     G_CALLBACK(EOS400DWindow::onButtonClicked),
                     ctx);
    return button;
}

void EOS400DWindow::onButtonClicked(GtkWidget*, gpointer user_data)
{
    auto* ctx = static_cast<ButtonContext*>(user_data);
    ctx->ui->firmware.onButton(ctx->btn);
}

void EOS400DWindow::updateLCD(const std::array<std::string, LCD_LINES>& lines)
{
    GtkTextIter start, end;
    gtk_text_buffer_get_start_iter(lcdBuffer, &start);
    gtk_text_buffer_get_end_iter(lcdBuffer, &end);
    gtk_text_buffer_delete(lcdBuffer, &start, &end);

    for (const auto& line : lines) {
        gtk_text_buffer_insert(lcdBuffer, &start, line.c_str(), -1);
        gtk_text_buffer_insert(lcdBuffer, &start, "\n", 1);
    }
}

void EOS400DWindow::show()
{
    gtk_widget_show_all(window);
}

bool EOS400DWindow::isFaceSensorActive() const {
        return gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(faceSensorToggle));
}
