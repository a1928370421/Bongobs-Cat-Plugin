/**
  Created by Weng Y on 2020/05/25.
  Copyright © 2020 Weng Y. Under GNU General Public License v2.0.
*/
#pragma once
#include <obs.h>

namespace VtuberPlugin {

	class VtuberPlugin
	{
	public:
		static const char *VtuberGetName(void *unused);

		static void *VtuberCreate(obs_data_t *settings,obs_source_t *source);

		static void VtuberDestroy(void *data);

		static void VtuberRender(void *data,gs_effect_t *effect);

		static uint32_t VtuberWidth(void *data);

		static uint32_t VtuberHeight(void *data);

		static obs_properties_t *VtuberGetProperties(void *data);

		static void Vtuber_update(void *data, obs_data_t *settings);

		static void Vtuber_defaults(obs_data_t *settings);
	};
}
