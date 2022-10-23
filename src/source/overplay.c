#include "overplay.h"
#include "../util/constants.h"

static void *overplay_create(obs_data_t *settings, obs_source_t *source)
{
	struct overplay *widget = bzalloc(sizeof(struct overplay));
	widget->data = bzalloc(sizeof(struct metadata));
	widget->source = source;
	const char *text_source_id = "text_ft2_source\0";
	widget->textSource = obs_source_create(text_source_id, text_source_id,
					       settings, NULL);
	obs_source_add_active_child(widget->source, widget->textSource);
	return widget;
}

static void overplay_destroy(void *data)
{
	struct overplay *widget = data;

	bfree(widget->data);
	widget->data = NULL;

	obs_source_remove(widget->textSource);
	obs_source_release(widget->textSource);
	widget->textSource = NULL;

	bfree(widget);
}

static bool hide_art_changed(obs_properties_t *props, obs_property_t *property,
			     obs_data_t *settings)
{
	UNUSED_PARAMETER(property);
	obs_property_t *prop = obs_properties_get(props, S_SOURCE_HIDE_ART);
	obs_property_set_visible(
		prop, obs_data_get_bool(settings, S_SOURCE_HIDE_ART));
	return true;
}

obs_properties_t *get_properties(void *data)
{
	UNUSED_PARAMETER(data);
	obs_properties_t *props = obs_properties_create();
	obs_property_t *hide_art = obs_properties_add_bool(
		props, S_SOURCE_HIDE_ART, T_SOURCE_HIDE_ART);
	obs_property_set_modified_callback(hide_art, hide_art_changed);
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
	UNUSED_PARAMETER(data);
	return 1;
}

uint32_t get_height(void *data)
{
	UNUSED_PARAMETER(data);
	return 1;
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
					    .get_defaults = get_defaults};

	obs_register_source(&si);
}
