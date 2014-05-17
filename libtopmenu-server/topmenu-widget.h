#ifndef _TOPMENU_WIDGET_H_
#define _TOPMENU_WIDGET_H_

#include <gtk/gtk.h>

#if GTK_MAJOR_VERSION == 3
#include <gtk/gtkx.h>
#endif

G_BEGIN_DECLS

#define TOPMENU_TYPE_WIDGET           topmenu_widget_get_type()
#define TOPMENU_WIDGET(obj)           (G_TYPE_CHECK_INSTANCE_CAST((obj), TOPMENU_TYPE_WIDGET, TopMenuWidget))
#define TOPMENU_IS_WIDGET(obj)        (G_TYPE_CHECK_INSTANCE_TYPE((obj), TOPMENU_TYPE_WIDGET))
#define TOPMENU_WIDGET_CLASS(c)       (G_TYPE_CHECK_CLASS_CAST((c), TOPMENU_TYPE_WIDGET, TopMenuWidgetClass))
#define TOPMENU_IS_WIDGET_CLASS(c)    (G_TYPE_CHECK_CLASS_TYPE((c), TOPMENU_TYPE_WIDGET))
#define TOPMENU_WIDGET_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), TOPMENU_TYPE_WIDGET, TopMenuWidgetClass))

typedef struct _TopMenuWidget        TopMenuWidget;
typedef struct _TopMenuWidgetClass   TopMenuWidgetClass;
typedef struct _TopMenuWidgetPrivate TopMenuWidgetPrivate;

struct _TopMenuWidget
{
	GtkBin parent_instance;

	TopMenuWidgetPrivate *priv;

	GtkSocket *socket;
};

struct _TopMenuWidgetClass
{
	GtkBinClass parent_class;
};

GType topmenu_widget_get_type(void);

GtkWidget *topmenu_widget_new(void);

G_END_DECLS

#endif /* _TOPMENU_WIDGET_H_ */
