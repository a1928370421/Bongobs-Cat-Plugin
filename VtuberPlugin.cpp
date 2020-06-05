/**
  Created by Weng Y on 2020/05/25.
  Copyright © 2020 Weng Y. Under GNU General Public License v2.0.
*/
#include "VtuberPlugin.hpp"
#include "VtuberFrameWork.hpp"
#include <obs-module.h>

#define V_MODELPATH "ModelsPath"
#define V_WIDTH "Width"
#define V_HEIGHT "Height"
#define V_SCALE "Scale"
#define V_X = "X"
#define V_Y = "Y"
#define V_SPEED = "Speed"
#define V_RANDOMMOTION = "RandomMotion"
#define V_Track = "Track"

namespace{
	struct Vtuber_data {
		obs_source_t *source;
		uint16_t modelId;
	};

	static int ModelCount = 0;

	}

const char * VtuberPlugin::VtuberPlugin::VtuberGetName(void *unused)
{
	UNUSED_PARAMETER(unused);
	return "bango cat";
}

void * VtuberPlugin::VtuberPlugin::VtuberCreate(obs_data_t *settings,
							   obs_source_t *source)
{
	Vtuber_data *vtb = (Vtuber_data *)malloc(sizeof(Vtuber_data));
	vtb->source = source;
	vtb->modelId = 0;

	double x = obs_data_get_double(settings, "x");
	double y = obs_data_get_double(settings, "y");
	int width = obs_data_get_int(settings, "width");
	int height = obs_data_get_int(settings, "height");
	double scale = obs_data_get_double(settings, "scale");
	double delayTime = obs_data_get_double(settings, "delaytime");
	bool random_motion = obs_data_get_bool(settings, "random_motion");
	bool breath = obs_data_get_bool(settings, "breath");
	bool eyeblink = obs_data_get_bool(settings, "eyeblink");
	bool track = obs_data_get_bool(settings, "track");
	//SETTING body
	VtuberFrameWork::UpData(0 ,x, y, width, height, scale,delayTime, random_motion, breath,eyeblink, NULL,track);
	//setting right hanf
	VtuberFrameWork::UpData(1, -0.3, 0.42, width, height, 4.0,delayTime, random_motion, breath,eyeblink, NULL,track);

	VtuberFrameWork::InitVtuber(0);

	VtuberFrameWork::InitVtuber(1);

	ModelCount++;

	return vtb;
}

void VtuberPlugin::VtuberPlugin::VtuberDestroy(void *data)
{
	
	Vtuber_data *spv = (Vtuber_data *)data;

	VtuberFrameWork::UinitVtuber(ModelCount);
	ModelCount--;

	delete spv;
}

void VtuberPlugin::VtuberPlugin::VtuberRender(
	void *data,
	gs_effect_t *effect)
{
	Vtuber_data *spv = (Vtuber_data *)data;

	int width, height;
	width = VtuberFrameWork::GetWidth(spv->modelId);
	height = VtuberFrameWork::GetHeight(spv->modelId);

	obs_enter_graphics();

	gs_texture_t *tex =gs_texture_create(width, height, GS_RGBA, 1, NULL, GS_DYNAMIC);

	uint8_t *ptr;
	uint32_t linesize;
	if (gs_texture_map(tex, &ptr, &linesize))
		VtuberFrameWork::ReanderVtuber(spv->modelId,(char*) ptr,width,height);
	gs_texture_unmap(tex);
	
	obs_source_draw(tex, 0, 0, 0, 0, true);

	gs_texture_destroy(tex);

	obs_leave_graphics();
}

uint32_t VtuberPlugin::VtuberPlugin::VtuberWidth(void *data)
{
	Vtuber_data *vtb = (Vtuber_data *)data;
	return VtuberFrameWork::GetWidth(vtb->modelId);
}

uint32_t VtuberPlugin::VtuberPlugin::VtuberHeight(void *data)
{
	Vtuber_data *vtb = (Vtuber_data *)data;
	return VtuberFrameWork::GetHeight(vtb->modelId);
}

obs_properties_t * VtuberPlugin::VtuberPlugin::VtuberGetProperties(void *data)
{

	Vtuber_data *vtb = (Vtuber_data *)data;

	obs_properties_t *ppts = obs_properties_create();
	obs_properties_set_param(ppts, vtb, NULL);

	obs_property_t *p;
	int width, height;
	width = VtuberFrameWork::GetWidth(vtb->modelId);
	height = VtuberFrameWork::GetHeight(vtb->modelId);

        //obs_properties_add_path(ppts, "models_path",obs_module_text("ModelsPath"), OBS_PATH_FILE,"*.model3.json", "Resources/");

	/*
	p = obs_properties_add_list(ppts, "model","Model",
				    OBS_COMBO_TYPE_LIST,
				    OBS_COMBO_FORMAT_STRING);

	fill_vtuber_model_list(p, vtb);*/
	//obs_property_set_modified_callback(p, vtuber_model_callback);
	
	//obs_properties_add_int(ppts, "width", obs_module_text("Width"), 32, 1900, 32);
	//obs_properties_add_int(ppts, "height", obs_module_text("Height"), 32, 1050, 32);
	//obs_properties_add_float_slider(ppts,"scale",obs_module_text("Scale"),0.1,10.0,0.1);
	//obs_properties_add_float_slider(ppts, "x", obs_module_text("X"), -3.0, 3.0, 0.1);
	//obs_properties_add_float_slider(ppts, "y", obs_module_text("Y"), -3.0, 3.0, 0.1);
	obs_properties_add_float_slider(ppts, "delay", obs_module_text("Speed"), 0.0, 10.0, 0.1);
	//obs_properties_add_bool(ppts,"random_motion",obs_module_text("RandomMotion"));
	obs_properties_add_bool(ppts, "breath", obs_module_text("Breath"));
	obs_properties_add_bool(ppts, "eyeblink", obs_module_text("EyeBlink"));
	obs_properties_add_bool(ppts, "track", obs_module_text("Track"));

	return ppts;
}

void VtuberPlugin::VtuberPlugin::Vtuber_update(
	void *data, obs_data_t *settings)
{
	Vtuber_data *vtb = (Vtuber_data *)data;

	double x = obs_data_get_double(settings, "x");
	double y = obs_data_get_double(settings, "y");
	int width = obs_data_get_int(settings,"width");
	int height = obs_data_get_int(settings, "height");
	double vscale = obs_data_get_double(settings, "scale");
	double delayTime = obs_data_get_double(settings, "delay");
	bool random_motion = obs_data_get_bool(settings, "random_motion");
	bool breath = obs_data_get_bool(settings, "breath");
	bool eyeblink = obs_data_get_bool(settings, "eyeblink");
	bool track = obs_data_get_bool(settings, "track");
	const char *vtb_str = NULL; //obs_data_get_string(settings, "models_path");

	VtuberFrameWork::UpData(vtb->modelId, x, y, width, height, vscale,
				delayTime, random_motion, breath,
				eyeblink, vtb_str,track);
}

void VtuberPlugin::VtuberPlugin::Vtuber_defaults(
	obs_data_t *settings)
{
	obs_data_set_default_int(settings, "width", 1200);
	obs_data_set_default_int(settings, "height", 700);
	obs_data_set_default_double(settings, "x", 0);
	obs_data_set_default_double(settings, "y", 0);
	obs_data_set_default_double(settings, "scale", 2.0);
	obs_data_set_default_double(settings, "delaytime", 1.0);
	obs_data_set_default_bool(settings, "random_motion", true);
	obs_data_set_default_bool(settings, "breath",true);
	obs_data_set_default_bool(settings, "eyeblink",true);
	obs_data_set_default_bool(settings, "track", true);
}
