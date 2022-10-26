#include "overplay.h"
#include "../util/constants.h"
#include "../util/dbus.h"

static void *overplay_create(obs_data_t *settings, obs_source_t *source)
{
	struct overplay *widget = bzalloc(sizeof(struct overplay));
	widget->meta = bzalloc(sizeof(struct metadata));
	widget->src = source;
	const char *text_source_id = "text_ft2_source\0";
	widget->textSource = obs_source_create_private(
		text_source_id, text_source_id, settings);
	widget->bus = bus_get();
	obs_source_add_active_child(widget->src, widget->textSource);
	return widget;
}

static void start_thread(struct overplay *widget)
{
	pthread_mutex_init(&widget->meta->lock, NULL);
	widget->signal_id = bus_subscribe(widget->bus, widget->meta);
}

static void stop_thread(struct overplay *widget)
{
	bus_unsubscribe(widget->bus, widget->signal_id);
	pthread_mutex_destroy(&widget->meta->lock);
}

static void overplay_destroy(void *data)
{
	struct overplay *widget = data;

	stop_thread(widget);

	bfree(widget->meta);
	widget->meta = NULL;

	obs_source_remove(widget->textSource);
	obs_source_release(widget->textSource);
	widget->textSource = NULL;

	bfree(widget);
}

static void overplay_video_tick(void *data, float seconds)
{
	UNUSED_PARAMETER(seconds);
	struct overplay *widget = data;

	if (!obs_source_showing(widget->src) || !widget->meta->str)
		return;

	obs_data_set_string(obs_source_get_settings(widget->textSource), "text",
			    widget->meta->str);

	obs_source_update(widget->textSource,
			  obs_source_get_settings(widget->textSource));
}

static void overplay_render(void *data, gs_effect_t *effect)
{
	UNUSED_PARAMETER(effect);
	struct overplay *widget = data;
	obs_source_video_render(widget->textSource);
}

static void overplay_show(void *data)
{
	struct overplay *widget = data;
	start_thread(widget);
}

static bool hide_art_changed(obs_properties_t *props, obs_property_t *property,
			     obs_data_t *settings)
{
	UNUSED_PARAMETER(property);
	obs_property_t *prop = obs_properties_get(props, S_SOURCE_HIDE_ART);
	obs_property_set_visible(prop, obs_data_get_bool(settings,
							 S_SOURCE_HIDE_ART));
	return true;
}

obs_properties_t *get_properties(void *data)
{
	UNUSED_PARAMETER(data);
	obs_properties_t *props = obs_properties_create();
	obs_properties_add_bool(props, S_SOURCE_HIDE_ART, T_SOURCE_HIDE_ART);
	// obs_property_t *hide_art = obs_properties_add_bool(
	// 	props, S_SOURCE_HIDE_ART, T_SOURCE_HIDE_ART);
	// obs_property_set_modified_callback(hide_art, hide_art_changed);
	return props;
}

void get_defaults(obs_data_t *settings)
{
	obs_data_set_default_bool(settings, S_SOURCE_HIDE_ART, true);
}

const char *get_name(void *data)
{
	UNUSED_PARAMETER(data);
	return T_OVERPLAY_NAME;
}

uint32_t get_width(void *data)
{
	struct overplay *widget = data;
	return obs_source_get_width(widget->textSource);
}

uint32_t get_height(void *data)
{
	struct overplay *widget = data;
	return obs_source_get_height(widget->textSource);
}

void register_overlay()
{
	static struct obs_source_info si = {.id = S_PLUGIN_ID,
					    .type = OBS_SOURCE_TYPE_INPUT,
					    .output_flags = OBS_SOURCE_VIDEO,
					    .create = overplay_create,
					    .destroy = overplay_destroy,
					    .get_properties = get_properties,
					    .get_name = get_name,
					    .get_width = get_width,
					    .get_height = get_height,
					    .get_defaults = get_defaults,
					    .video_render = overplay_render,
					    .video_tick = overplay_video_tick,
					    .show = overplay_show};

	obs_register_source(&si);
}
