#include <glib.h>
#include <gio/gio.h>
#include <dbus/dbus.h>
#include <obs-module.h>

struct overplay {
	obs_source_t *src;
	uint32_t cx;
	uint32_t cy;
	obs_source_t *textSource;
	struct metadata *data;
	bool showArt;
	guint sub_id;
	GDBusConnection *bus;
};

extern void register_overlay();
