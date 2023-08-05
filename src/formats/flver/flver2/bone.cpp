#include "flver2_rw.h"

namespace cfr
{
	struct bone_s
	{
		cfr_vec3 translation = {0.0f,0.0f,0.0f};

		uint32_t nameOffset = 0;
		int32_t nameLength  = 0;

		cfr_vec3 rot = {0.0f,0.0f,0.0f};

		int16_t parentIndex = 0;
		int16_t childIndex  = 0;

		cfr_vec3 scale = {0.0f,0.0f,0.0f};

		int16_t nextSiblingIndex = 0;
		int16_t previousSiblingIndex = 0;

		cfr_vec3 boundingBoxMin = {0.0f,0.0f,0.0f};
		
		int32_t unk3C = 0;

		cfr_vec3 boundingBoxMax = {0.0f,0.0f,0.0f};

		char padding[52]; //meaningless data
	};

	 FLVER2::Bone::Bone()
	 {
		
	 };
};