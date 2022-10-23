#include <obs-module.h>
#include "../util/dbus.h"

struct overplay {
	obs_source_t *source;
	uint32_t cx;
	uint32_t cy;
	obs_source_t *textSource;
	float update_time_elapsed;
	struct metadata *data;
	pthread_t thread;
  bool showArt;
	volatile bool updateThread;
};

extern void register_overlay();
