#include <math.h>
#include "gtkcharts.h"
static void on_activate(GtkApplication*, gpointer);
int main(int argc, char *argv[])
{
    GtkApplication *app = gtk_application_new("nook.inc", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(on_activate), NULL);
    int appStatus = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return appStatus;
}
static void on_activate(GtkApplication *app, gpointer data)
{
    struct Point *point = malloc(sizeof(struct Point) * 100);
    for (int i = 0; i < 100; ++i) {
        point[i].x = i;
        point[i].y = sin(i * 0.9f);
    }
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_default_size(GTK_WINDOW(window), 1024, 768);
    GtkWidget *charts = gtk_charts_new();
    gtk_charts_set_data(GTK_CHARTS(charts), point, 100);
    gtk_container_add(GTK_CONTAINER(window), charts);
    gtk_widget_show_all(window);
    return;
}