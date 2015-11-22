# Screenshots

![](https://pbs.twimg.com/media/Be2QBA9CQAEYosy.png)

![](https://pbs.twimg.com/media/BmkXFUwCcAADhCs.png)

![](http://depot.javispedro.com/mate/topmenu/thunderbird.png)

# Requirements and compatibility

Currently, TopMenu ships as a panel applet for either Mate 1.8, Xfce >= 4.8 (Gtk+2 versions only), or LXPanel. The Mate applet would be relatively easy to port to Gnome 2.

Gtk+2 is the preferred toolkit, albeit Gtk+3 is partially supported. Additionally, plugins are shipped for Qt 4 and some Mozilla apps (Firefox, Thunderbird and Zotero). Note that, except in the Gtk+ 3 case, menu bars will always be rendered using the Gtk+ 2 theme.

# Install

`autoreconf --install`, `./configure --prefix=/usr`, `make` and `sudo make install` should work for topmenu-gtk. Then add the panel applet. You need to ensure the Gtk+ module `topmenu-gtk-module`is loaded. A simple way to do this is to create a `.gtkrc-2.0` file in your `$HOME` with the following contents:

    gtk-modules = "canberra-gtk-module:topmenu-gtk-module"

![The current icon](https://git.javispedro.com/cgit/topmenu-gtk.git/plain/icons/48x48/topmenu-applet.png) Look for this icon in the Mate/Xfce "Add panel applet" dialogs.

For Gtk+3 , use `./configure --prefix=/usr --with-gtk=3`. Because of a [gtk bug](https://bugzilla.gnome.org/show_bug.cgi?id=730306) in Gtk+ 3.12, you may need to use a different method to load modules, such as globally setting environment variable `GTK_MODULES`.

topmenu-qt ships with a qmake .pro file. Installation should be as simple as `qmake`, `make`, and `sudo make install`. No additional changes are required.

The Mozilla extension can be built using `make`. Being a pure javascript extension, it should not be hard to build. Install the resulting .xpi file using the normal methods (e.g. drag and drop into the "Extensions" dialog).

# Bugs and feature requests

While I'm looking for a simple enough issue tracker,
please send questions or problem reports to dev.bugs at javispedro com. Alternatively, see [my contact page](http://javispedro.com/me.html).

## Longer-term feature ideas

* Multi-monitor support. Currently only one applet instance is supported.  Ideally, IF there are multiple panel applets, they should synchronize to only embed menubars from the same display.
* Allow Qt programs to render the menubars using Qt itself instead of using Gtk+.
* Transparent panels without XComposite (relative X11 background pixmaps, maybe?)
* Add some toggables for some commonely requested options: hide/show the application menu, "menu button" instead of menu bar.

# Design

TopMenu uses XEmbed instead of other approaches. Thus, the global menubar is actually rendered by each client process, but the menubar applet _embeds_ it, giving the appearence that the menubar is on a different place. This is the way e.g. OS X works, and it works quite well for me.

However, using XEmbed generally causes incosistency when multiple client toolkits are used. E.g. Qt programs will render their menubars in Qt style. To alleviate this problem in TopMenu, we have made the drastic choice of ensuring that client applications always render their exported menubars using Gtk+ itself, version 2.0. This is not actually that bad on a Gtk+ desktop enviroment as most used toolkits already have a soft dependency to Gtk+ (e.g. Qt depends on it for QGtkStyle, XUL, LibreOffice, etc.). However, it makes TopMenu completely useless on a e.g. Qt desktop such as KDE.


Most alternative approaches for global menu bars _serialize_ menubars into a a more or less defined text/XML/GVariant format and then expose them using IPC protocols such as D-Bus. However, using D-Bus means losing network/user transparency. Windows displaying on the local X server from clients in another workstation/VM will not be able to expose the menu bar. Neither will clients in the same workstation but run as a different user such as root. These limitations were not acceptable to me.

I tried to hack around Canonical's dbusmenu to use X11 as IPC but that was an exercise in futility as X11 is just useless as an IPC mechanism. 
Besides, at the time this document was written, even with the use of dbusmenu (now upstreamed in GIO itself) I would need to supply patches for virtually every toolkit (including Gtk+3), negating the benefits of using a "upstreamed" protocol.

## Protocol

Every X11 toplevel window with a menubar _should_ have a property with name `TOPMENU_WINDOW` (format = Window) containing a single reference to a different X11 window, called the _menu window_. This menu window _must_ support acting as a [XEmbed](http://standards.freedesktop.org/xembed-spec/xembed-spec-latest.html) client. The active TopMenu server, may, at any time, start an XEmbed session by embedding the menu window. Only one TopMenu server may embed this window at any given time.

A active topmenu server will always own the X11 selection named `TOPMENU_SERVER`. If such a selection exists, an application _must_ assume that a server is currently embedding the menu window (even if no XEmbed session is active) and thus _must_ hide its native menubar, if any.

