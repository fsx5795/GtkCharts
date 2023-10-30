#include <float.h>
#include <stdbool.h>
#include "gtkcharts.h"
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

enum { CHARTS_SIGNAL, LAST_SIGNAL };
static gint chartsSignals[LAST_SIGNAL] = { 0 };
static void draw_update(void);

void gtk_charts_init(GtkCharts*);
void gtk_charts_class_init(GtkChartsClass*);
GType gtk_charts_get_type(void)
{
    static GType chartsType = 0;
    if (chartsType == 0) {
        static const GTypeInfo chartsInfo = {
            sizeof(GtkChartsClass),
            NULL, NULL,
            (GClassInitFunc)gtk_charts_class_init,
            NULL, NULL,
            sizeof(GtkCharts),
            0,
            (GInstanceInitFunc)gtk_charts_init
        };
        chartsType = g_type_register_static(GTK_TYPE_DRAWING_AREA, "GtkCharts", &chartsInfo, 0);
    }
    return chartsType;
}

void gtk_charts_init(GtkCharts *charts)
{
    charts->points = malloc(sizeof(struct Points));
    return;
}

void gtk_charts_class_init(GtkChartsClass *chartsClass)
{
    GObjectClass *objectClass = G_OBJECT_CLASS(chartsClass);
    chartsSignals[CHARTS_SIGNAL] = g_signal_new("charts_update",
        G_TYPE_FROM_CLASS(objectClass),
        G_SIGNAL_RUN_FIRST,
        G_STRUCT_OFFSET(GtkChartsClass, draw_update),
        NULL, NULL,
        g_cclosure_marshal_VOID__VOID,
        G_TYPE_NONE, 0, NULL
    );
    chartsClass->draw_update = draw_update;
    return;
}

static void draw_update(void)
{
    puts("draw update");
    return;
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
        cairo_set_source_rgb(cr, 0, 0, 0);
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
        cairo_stroke(cr);
        cairo_set_source_rgb(cr, 1, 0, 0);
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
    g_signal_emit(G_OBJECT(widget), chartsSignals[CHARTS_SIGNAL], 0);
    return false;
}

GtkWidget* gtk_charts_new(void)
{
    GtkWidget *charts = GTK_WIDGET(g_object_new(GTK_CHARTS_TYPE, 0));
    g_signal_connect(charts, "draw", G_CALLBACK(on_draw), GTK_CHARTS(charts)->points);
    return charts;
}

void gtk_charts_set_data(GtkCharts *charts, struct Point *point, size_t leng)
{
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
    charts->points->pt = point;
    charts->points->minX = minX;
    charts->points->minY = minY;
    charts->points->maxX = maxX;
    charts->points->maxY = maxY;
    charts->points->leng = 100;
}