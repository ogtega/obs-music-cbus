#include "overplay.h"
#include "../util/constants.h"

static bool show_art_changed(obs_properties_t *props, obs_property_t *property,
			     obs_data_t *settings)
{
	UNUSED_PARAMETER(property);
	obs_property_t *prop = obs_properties_get(props, S_SOURCE_ART);
	obs_property_set_visible(prop, obs_data_get_bool(settings,
							 S_SOURCE_SHOW_ART));
	return true;
}

obs_properties_t *get_properties(void *data)
{
	UNUSED_PARAMETER(data);
	obs_properties_t *props = obs_properties_create();
	obs_property_t *show_art = obs_properties_add_bool(
		props, S_SOURCE_SHOW_ART, T_SOURCE_SHOW_ART);
	obs_property_set_modified_callback(show_art, show_art_changed);
	return props;
}

void register_overlay()
{
	static struct obs_source_info si = {.id = S_PLUGIN_ID,
					    .type = OBS_SOURCE_TYPE_INPUT,
					    .output_flags = OBS_SOURCE_VIDEO,
					    .get_properties = get_properties};

	obs_register_source(&si);
}
