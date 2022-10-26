#include <glib.h>
#include <gio/gio.h>
#include <dbus/dbus.h>
#include <obs-module.h>

struct overplay {
	obs_source_t *src;
	GDBusConnection *bus;
	struct metadata *meta;
	obs_source_t *textSource;
	guint signal_id;
	bool showArt;
	uint32_t cx;
	uint32_t cy;
};

extern void register_overlay();
