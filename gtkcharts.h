#ifndef GTKCHARTS_H
#define GTKCHARTS_H

#include <stdlib.h>
#include <gtk/gtk.h>

#define GTK_CHARTS_TYPE (gtk_charts_get_type())
#define GTK_CHARTS(obj) (G_TYPE_CHECK_INSTANCE_CAST(obj, GTK_CHARTS_TYPE, GtkCharts))

struct Point
{
    double x;
    double y;
};

typedef struct _GtkCharts GtkCharts;
struct _GtkCharts
{
    GtkDrawingArea parent;
    struct Points *points;
};

typedef struct _GtkChartsClass GtkChartsClass;
struct _GtkChartsClass
{
    GtkDrawingAreaClass parentClass;
    void (*draw_update)(void);
};

GType gtk_charts_get_type(void);
GtkWidget* gtk_charts_new(void);
void gtk_charts_set_data(GtkCharts*, struct Point*, size_t);

#endif