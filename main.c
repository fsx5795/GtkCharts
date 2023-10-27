#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <gtk-3.0/gtk/gtk.h>
struct Point
{
    double x;
    double y;
};
struct Size
{
    int width;
    int height;
};
struct Points
{
    struct Point *pt;
    double minX;
    double minY;
    double maxX;
    double maxY;
    int leng;
};
static void on_activate(GtkApplication*, gpointer);
static bool on_draw(GtkWidget*, GdkEventExpose*, gpointer);
static struct Size get_string_size(PangoLayout*, const char*);
int main(int argc, char *argv[])
{
    GtkApplication *app = gtk_application_new("nook.inc", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(on_activate), NULL);
    int appStatus = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return appStatus;
}
static struct Size get_string_size(PangoLayout *layout, const char *str)
{
    struct Size size;
    pango_layout_set_text(layout, str, strlen(str));
    pango_layout_get_pixel_size(layout, &size.width, &size.height);
    size.width = size.width - size.width * 0.4;
    size.height = size.height - size.height * 0.4;
    return size;
}
static void on_activate(GtkApplication *app, gpointer data)
{
    struct Point *point = malloc(sizeof(struct Point) * 100);
    for (int i = 0; i < 100; ++i) {
        point[i].x = i;
        point[i].y = sin(i * 0.9f);
        double x = point[i].x;
        double y = point[i].y;
        printf("%f %d", x, y);
    }
    double minX, maxX;
    double minY = minX = DBL_MAX;
    double maxY = maxX = DBL_MIN;
    for (int i = 0; i < 100; ++i) {
        if (point[i].x < minX)
            minX = point[i].x;
        if (point[i].x > maxX)
            maxX = point[i].x;
        if (point[i].y < minY)
            minY = point[i].y;
        if (point[i].y > maxY)
            maxY = point[i].y;
    }
    struct Points *points = malloc(sizeof(struct Points));
    points->pt = point;
    points->minX = minX;
    points->minY = minY;
    points->maxX = maxX;
    points->maxY = maxY;
    points->leng = 100;
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_default_size(GTK_WINDOW(window), 1024, 768);
    GtkWidget *area = gtk_drawing_area_new();
    gtk_container_add(GTK_CONTAINER(window), area);
    g_signal_connect(area, "draw", G_CALLBACK(on_draw), points);
    gtk_widget_show_all(window);
    return;
}
static bool on_draw(GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
    GdkWindow *window = gtk_widget_get_window(widget);
    int width = gdk_window_get_width(window);
    int height = gdk_window_get_height(window);
    cairo_region_t *region = cairo_region_create();
    GdkDrawingContext *context = gdk_window_begin_draw_frame(window, region);
    cairo_t *cr = gdk_drawing_context_get_cairo_context(context);
    cairo_move_to(cr, 50, 50);
    cairo_line_to(cr, 50, height - 50);
    cairo_line_to(cr, width - 50, height - 50);
    cairo_line_to(cr, width - 50, 50);
    cairo_line_to(cr, 50, 50);
    const int unitw = (width - 50 * 2) / 7;
    const int unith = (height - 50 * 2) / 5;
    char str[10] = { '\0' };
    cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 0, 0);
    cairo_t *ss = cairo_create(surface);
    cairo_surface_destroy(surface);
    PangoLayout *layout = pango_cairo_create_layout(ss);
    struct Points *ps = (struct Points*)data;
    if(ps->leng == 0) {
        //縦向線
        for (int i = 0; i < 7; ++i) {
            memset(str, '\0', sizeof(str));
            sprintf(str, "%d", i * 10);
            struct Size size = get_string_size(layout, str);
            cairo_move_to(cr, 50 + i * unitw, 50);
            cairo_line_to(cr, 50 + i * unitw, height - 50);
            cairo_move_to(cr, 50 + i * unitw - size.width / 2, height - 50 + size.height);
            cairo_show_text(cr, str);
            //橫向線
            if (i < 5) {
                cairo_move_to(cr, 50, 50 + i * unith);
                cairo_line_to(cr, width - 50, 50 + i * unith);
                cairo_move_to(cr, 50 - size.width, height - (50 + i * unith));
                cairo_show_text(cr, str);
            }
        }
    } else {
        double unitX = (ps->maxX - ps->minX) / 7;
        double unitY = (ps->maxY - ps->minY) / 5;
        //縦向線
        for (int i = 0; i < 7; ++i) {
            memset(str, '\0', sizeof(str));
            sprintf(str, "%lf", ps->minX + i * unitX);
            struct Size size = get_string_size(layout, str);
            cairo_move_to(cr, 50 + i * unitw, 50);
            cairo_line_to(cr, 50 + i * unitw, height - 50);
            cairo_move_to(cr, 50 + i * unitw - size.width / 2, height - 50 + size.height);
            cairo_show_text(cr, str);
            //橫向線
            if (i < 5) {
                memset(str, '\0', strlen(str) + 1);
                sprintf(str, "%.3lf", ps->minY + i * unitY);
                struct Size size = get_string_size(layout, str);
                cairo_move_to(cr, 50, 50 + i * unith);
                cairo_line_to(cr, width - 50, 50 + i * unith);
                cairo_move_to(cr, 50 - size.width, height - (50 + i * unith));
                cairo_show_text(cr, str);
            }
        }
        //表格宽高 / (最大表格坐標 - 最小表格坐標) * (所要添加的锚点坐標 - 最小锚点坐標) + 辺距 = 表格坐標点転界面坐標点
        double x = (width - 50 * 2) / (ps->maxX - ps->minX) * (ps->pt[0].x - ps->minX) + 50;
        double y = (height - 50 * 2) / (ps->maxY - ps->minY) * (ps->maxY - ps->pt[0].y) + 50;
        cairo_move_to(cr, x, y);
        for (int i = 0; i < ps->leng; ++i) {
            if (i > 0) {
                x = (width - 50 * 2) / (ps->maxX - ps->minX) * (ps->pt[i].x - ps->minX) + 50;
                y = (height - 50 * 2) / (ps->maxY - ps->minY) * (ps->maxY - ps->pt[i].y) + 50;
                cairo_line_to(cr, x, y);
            }
        }
    }
    g_object_unref(layout);
    cairo_stroke(cr);
    gdk_window_end_draw_frame(window, context);
    cairo_region_destroy(region);
    return false;
}