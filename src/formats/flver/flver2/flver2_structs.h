#pragma once
#include "../stdafx.h"

namespace cfr
{
	struct header_s
	{
		char magic[6]; //this was working with size 4 somehow. it shouldn't
		char endian[2];
		uint32_t version = 0;

		uint32_t dataOffset = 0;
		uint32_t dataLength = 0;

		int32_t dummyCount = 0;
		int32_t materialCount = 0;
		int32_t boneCount = 0;
		int32_t meshCount = 0;
		int32_t vertexBufferCount = 0;

		cfr_vec3 boundingBoxMin = {0.0f,0.0f,0.0f};
		cfr_vec3 boundingBoxMax = {0.0f,0.0f,0.0f};

		int32_t trueFaceCount  = 0; //not incl shadow meshes or degen faces
		int32_t totalFaceCount = 0;

		int8_t vertexIndexSize = 0;
		int8_t unicode = 0;

		int8_t unk4A = 0;
		int8_t unk4B = 0;

		int32_t primitiveRestartConstant = 0; //guess, needs confirmation
		int32_t facesetCount = 0;
		int32_t bufferLayoutCount = 0;
		int32_t textureCount = 0;

		int8_t unk5C = 0;
		int8_t unk5D = 0;
		int8_t unk5E = 0; //assert(0)
		int8_t unk5F = 0; //assert(0)

		int32_t unk60 = 0; //assert(0)
		int32_t unk64 = 0; //assert(0)
		int32_t unk68 = 0;
		int32_t unk6C = 0; //assert(0)
		int32_t unk70 = 0; //assert(0)
		int32_t unk74 = 0; //assert(0)
		int32_t unk78 = 0; //assert(0)
		int32_t unk7C = 0; //assert(0)
	};

	struct gxitem_s
	{
		int32_t id = 0;
		int32_t unk04 = 0; //maybe assert(100)? if header.version < 0x20010
		int32_t length = 0; //length includes struct size
	};

	struct material_s
	{
		uint32_t nameOffset = 0;
		uint32_t mtdOffset  = 0;

		int32_t textureCount = 0;
		int32_t textureIndex = 0;

		uint32_t flags = 0;
		uint32_t gxOffset = 0;

		int32_t unk18 = 0;
		int32_t unk1C = 0; //assert(0)
	};

	struct bone_s
	{
		cfr_vec3 translation = {0.0f,0.0f,0.0f};

		uint32_t nameOffset = 0;
		int32_t nameLength  = 0;

		cfr_vec3 rotation = {0.0f,0.0f,0.0f};

		int16_t parentIndex = 0;
		int16_t childIndex  = 0;

		cfr_vec3 scale = {0.0f,0.0f,0.0f};

		int16_t nextSiblingIndex = 0;
		int16_t previousSiblingIndex = 0;

		cfr_vec3 boundingBoxMin = {0.0f,0.0f,0.0f};
		
		int32_t unk3C = 0; //likely padding

		cfr_vec3 boundingBoxMax = {0.0f,0.0f,0.0f};

		char padding[52]; //meaningless data
	};

	struct dummy_s
	{
		cfr_vec3 position = {0.0f,0.0f,0.0f};;

		uint8_t color[4] = {0,0,0,0};

		cfr_vec3 forward = {0.0f,0.0f,0.0f};

		int16_t referenceID = 0;
		int16_t dummyBoneIndex = 0;

		cfr_vec3 upward = {0.0f,0.0f,0.0f};

		int16_t attachBoneIndex = 0;
		char unk2E = '\0';
		char useUpwardVector = '\0';

		int32_t unk30 = 0;
		int32_t unk34 = 0;
		int32_t unk38 = 0; //assert(0)
		int32_t unk3C = 0; //assert(0)
	};

	struct mesh_s_0
	{
		int8_t dynamic; // 0 or 1

		int8_t unk01; //assert(0)
		int8_t unk02; //assert(0)
		int8_t unk03; //assert(0)
		
		int32_t materialIndex;
		
		int32_t unk08; //assert(0)
		int32_t unk0C; //assert(0)
		
		int32_t defaultBoneIndex;
		int32_t boneCount;

		uint32_t boundingBoxOffset = 0;
		uint32_t boneIndicesOffset = 0;
		
		int32_t facesetCount;
		uint32_t facesetIndicesOffset;

		int32_t vertexBufferCount;
		uint32_t vertexBufferIndicesOffset; //loc of the VertBuffIndex array
	};

	struct mesh_s_1 //only if boundingBoxOffset != 0
	{
		cfr_vec3 boundingBoxMin = {0.0f,0.0f,0.0f};
		cfr_vec3 boundingBoxMax = {0.0f,0.0f,0.0f};
	};

	struct mesh_s_2 //only if header.version >= 0x2001A
	{
		cfr_vec3 boundingBoxUnk = {0.0f,0.0f,0.0f};
	};

	struct mesh_s_3
	{
		int32_t* boneIndices; //size of boneCount
		int32_t* facesetIndices; //size of faceSetCount
		int32_t* vertexBufferIndices; //size of vertexBufferCount
	};

	struct member_s
	{
		int32_t dataLength;
		int32_t dataOffset;

		int32_t unk08; //assert(0)
		int32_t unk0C; //assert(0)
		int16_t unk10;
		int16_t unk12;

		int16_t baseIndex;

		int16_t unk16;
		int32_t unk18;
		int32_t unk1C;
		int32_t unk20; //assert(0)
		int32_t unk24; //assert(0)
		int32_t unk28; //assert(0)
		int32_t unk2C; //assert(0)

		//edgeGeomSpuConfigInfo? not sure what this means
		int16_t unk30;
		int16_t unk32;
		int16_t unk34;
		int16_t unk36;
		int16_t unk38;

		int16_t indexCount;

		int32_t unk3C; //assert(-1)
	};

	struct edgeindices_s
	{
		int16_t memberCount = 0;
		int16_t unk02 = 0;
		int32_t unk04 = 0;
		int32_t unk08 = 0; //assert(0)
		int32_t unk0C = 0;
	};

	struct faceset_s_0
	{
		uint32_t flags = 0;
		uint8_t triangleStrip = 0;
		uint8_t cullBackFaces = 0;
		short unk06 = 0;
		int32_t vertexIndexCount = 0; //number of indices
		uint32_t vertexIndicesOffset = 0;
	};

	struct faceset_s_1 //only if header.version >= 0x20009
	{
		int32_t vertexIndicesLength = 0; //i don't know
		int32_t unk14 = 0; //assert(0)
		int32_t vertexIndexSize = 0; //byte size of indices
		int32_t unk1C = 0; //assert(0)
	};

	struct layoutmember_s
	{
		int32_t unk00 = 0; //0, 1, or 2
		int32_t structOffset = 0; //seems to be used in PTDE?
		uint32_t type = 0;
		uint32_t semantic = 0;
		int32_t index = 0; //this doesn't seem to be used?
	};

	struct bufferlayout_s
	{
		int32_t memberCount = 0;

		int32_t unk04 = 0; //assert(0)
		int32_t unk08 = 0; //assert(0)

		uint32_t membersOffset = 0; //file location for the members
	};

	struct vertexbuffer_s
	{
		int32_t bufferIndex = 0;
		int32_t layoutIndex = 0;
		int32_t vertexSize  = 0; //byte size of each vertex
		int32_t vertexCount = 0; //total number of vertices

		int32_t unk10 = 0; //assert(0)
		int32_t unk14 = 0; //assert(0)

		uint32_t verticesLength = 0; //0 in version 20005, non 0 in 20008
		int32_t  bufferOffset   = 0; //location in flver file
	};

	struct texture_s
	{
		uint32_t pathOffset = 0;
		uint32_t typeOffset = 0;

		cfr_vec2 scale = {0.0f,0.0f};

		char unk10 = '\0';
		char unk11 = '\0';
		int16_t unk12 = 0; //assert(0)

		float unk14 = 0.0f;
		float unk18 = 0.0f;
		float unk1C = 0.0f;
	};
};
