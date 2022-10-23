#include <obs-module.h>
#include "../util/dbus.h"

struct overplay {
	obs_source_t *src;
	uint32_t cx;
	uint32_t cy;
	obs_source_t *textSource;
	struct metadata *data;
  bool showArt;
	volatile bool updateThread;
};

extern void register_overlay();
