#include <glib.h>
#include <gio/gio.h>
#include <dbus/dbus.h>
#include <obs-module.h>

struct overplay_data {
	obs_source_t *src;
	GDBusConnection *conn;
	struct metadata *meta;
	obs_source_t *text_src;
	guint signal_id;
	bool showArt;
};

extern void register_overlay();
