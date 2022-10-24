#include <glib.h>
#include <gio/gio.h>
#include <pthread.h>
#include <dbus/dbus.h>
#include <obs-module.h>

struct overplay {
	obs_source_t *src;
	uint32_t cx;
	uint32_t cy;
	obs_source_t *textSource;
	struct metadata *data;
	bool showArt;
	pthread_t thread;
	GDBusConnection *bus;
	volatile bool updateThread;
};

extern void register_overlay();
