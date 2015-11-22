/*
 * Copyright 2015 Charles E. Lehner <cel@celehner.com>
 *
 * This file is part of TopMenu.
 *
 * TopMenu is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * TopMenu is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with TopMenu.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <lxpanel/plugin.h>
#include "../libtopmenu-server/topmenu-widget.h"

static GtkWidget *topmenu_lxpanel_new_instance(LXPanel *panel, config_setting_t *settings)
{
	GtkWidget *menu = GTK_WIDGET(topmenu_widget_new());
	gtk_widget_set_can_focus(menu, TRUE);
	gtk_widget_show_all(menu);

	return menu;
}

FM_DEFINE_MODULE(lxpanel_gtk, topmenu_lxpanel)

LXPanelPluginInit fm_module_init_lxpanel_gtk = {
	.name = "TopMenu",
	.description = "Shows the topmenu menu bar",
	.new_instance = topmenu_lxpanel_new_instance,
	.expand_available = 1,
	.expand_default = 1,
	.one_per_system = 1,
};
