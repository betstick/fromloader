#include "flver2.h"

namespace cfr
{
	struct VertexDataSpec
	{
		int vertexCount;
		int normalCount;
		int uvCount;
		int colorCount;
	};

	struct VertexData
	{
		float* positions;
		float* bone_weights;
		short* bone_indices;
		float* normals;
		int*   normalws;
		float* uvs;
		float* tangents;
		float* bitangents;
		float* colors;

		int vertexCount;
		int normalCount;
		int uvCount;
		int colorCount;
	};

	class VertexDataIO
	{
		VertexData data;
		VertexDataSpec spec;

		VertexDataIO(FLVER2* f);
		VertexDataIO(VertexDataSpec s, void* data);
		~VertexDataIO();

		void unpackVertices(FLVER2* f, int vertexBufferIndex, VertexData* data);
		void packVertices(FLVER2* f, int vertexBufferIndex, VertexData* data);
	};

	class vert_pos;
	class vert_bone_weights;
	class vert_bone_indices;
	class vert_norms;
	class vert_tangents;
	class vert_bitangents;
	class vert_uvs;
	class vert_colors;

	class vert_pos
	{
		void read(UMEM* src, VertexType t, VertexData* dst);
		void write(VertexData* src, VertexType t, UMEM* dst);
	};
};