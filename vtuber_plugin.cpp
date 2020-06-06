#include <obs-module.h>
#include "VtuberPlugin.hpp"

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE("bongobs-cat", "en-US")
MODULE_EXPORT const char *obs_module_description(void)
{
	return "Bongo Cat";
}

bool obs_module_load(void)
{
	obs_source_info Vtuber_video{
	};
	Vtuber_video.id = "bongobs-cat";
	Vtuber_video.type = OBS_SOURCE_TYPE_INPUT;
	Vtuber_video.output_flags = OBS_SOURCE_VIDEO;
	Vtuber_video.get_name =
		VtuberPlugin::VtuberPlugin::VtuberGetName;
	Vtuber_video.create =
		VtuberPlugin::VtuberPlugin::VtuberCreate;
	Vtuber_video.destroy =
		VtuberPlugin::VtuberPlugin::VtuberDestroy;
	Vtuber_video.video_render =
		VtuberPlugin::VtuberPlugin::VtuberRender;
	Vtuber_video.get_width =
		VtuberPlugin::VtuberPlugin::VtuberWidth;
	Vtuber_video.get_height =
		VtuberPlugin::VtuberPlugin::VtuberHeight;
	Vtuber_video.get_properties =
		VtuberPlugin::VtuberPlugin::VtuberGetProperties;
	Vtuber_video.update =
		VtuberPlugin::VtuberPlugin::Vtuber_update;
	Vtuber_video.get_defaults =
		VtuberPlugin::VtuberPlugin::Vtuber_defaults;

	obs_register_source(&Vtuber_video);

	return true;
}

#ifdef _WIN32
void obs_module_unload(void)
{

}
#endif
