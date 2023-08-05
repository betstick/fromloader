#pragma once

#include <stdexcept>
#include <map>

#include "../../stdafx.h"
//welcome to FLVER-town

//sub classes of FLVER2 usually have position member.
//this corresponds to where it was written to in the UMEM handle.
//exists so that writeData() can seek back and set the offsets.
//index is what position in the parent the object is
//each class is responsible for ensuring its own lo_data is correct
//at write time. other data is assumed to always be up to date

//natural flver layout. natural to a human, not the on disk format
/*

flver2
	header
	dummies
	bones
	meshes
		material
			textures
			gxitems
		facesets
		vertexbuffers
			bufferlayout
				layoutmembers


create flver2, using header, init all sub classes as far as possible
*/

namespace cfr
{
	//slightly more standardized way to interact with the raw vertex data.
	//unpacked from vertex buffers.
	class StdVertexData
	{
		public:
		std::vector<float> positions;
		std::vector<float> bone_weights;
		std::vector<short> bone_indices;
		std::vector<float> normals;
		std::vector<int>   normalws;
		std::vector<float> tangents;
		std::vector<float> bitangents;
		std::vector<float> uvs;
		std::vector<float> colors;

		int vertexCount;
		int normalCount;
		int uvCount;
		int colorCount;

		StdVertexData();
		StdVertexData(FLVER2::VertexBuffer* vb);
	};

	class FLVER2
	{
		public:
		class Header;
		
		class Dummy;
		class Material;
		class Bone;
		class Mesh;
		class Faceset;
		class VertexBuffer;
		class BufferLayout;
		class Texture;

		class GxItem;
		class Member;
		class EdgeIndices;
		class LayoutMember;
		

		class Header
		{
			friend class FLVER2;
			public:
			int version;
			cfr_vec3 boundingBoxMin = {0.0f,0.0f,0.0f};
			cfr_vec3 boundingBoxMax = {0.0f,0.0f,0.0f};

			bool bigEndian = false;
			bool unicode = true;

			Header();
			~Header();

			FLVER2* parent();

			protected:
			FLVER2* parent = nullptr;
		};

		class Dummy
		{
			friend class FLVER2;
			friend class Bone;
			public:
			int id;

			cfr_vec3 position = {0.0f,0.0f,0.0f};
			cfr_vec3 forward  = {0.0f,0.0f,0.0f};
			cfr_vec3 upward   = {0.0f,0.0f,0.0f};

			bool useUpdward = true;
			
			uint8_t color[4] = {0,0,0,0};

			Dummy(Bone* parent);
			~Dummy();

			Bone* parent();

			protected:
			void read(UMEM* src);
			void write(UMEM* dst);

			int index;
			Bone* parent = nullptr;
			FLVER2* f_parent = nullptr;
		};

		class GxItem
		{
			public:
			std::vector<char> data;

			GxItem();
			~GxItem();

			Material* parent();

			protected:
			int index;
			Material* parent = nullptr;
			FLVER2* f_parent = nullptr;
		};

		class Material
		{
			friend class FLVER2;
			public:
			std::string name;
			std::string mtd_name;

			std::vector<GxItem*> gxItems;
			std::vector<FLVER2::Texture*> textures;

			Material();
			~Material();

			Mesh* parent();

			protected:
			void read(UMEM* src, Mesh* parent);
			void write(UMEM* dst);

			int index;
			Mesh* parent = nullptr;
			FLVER2* f_parent = nullptr;
		};

		class Bone
		{
			friend class FLVER2;
			friend class Dummy;
			public:
			std::string name;

			cfr_vec3 translation = {0.0f,0.0f,0.0f};
			cfr_vec3 rotation    = {0.0f,0.0f,0.0f};
			cfr_vec3 scale       = {0.0f,0.0f,0.0f};

			cfr_vec3 boundingBoxMin = {0.0f,0.0f,0.0f};
			cfr_vec3 boundingBoxMax = {0.0f,0.0f,0.0f};

			Bone* parent_bone = NULL;
			Bone* child_bone  = NULL;
			Bone* siblingNext = NULL;
			Bone* siblingPrev = NULL;
			
			std::vector<Dummy*> dummies;

			Bone(FLVER2* parent);
			~Bone();

			protected:
			void read(UMEM* src);
			void write(UMEM* dst);
			void writeData(UMEM* dst);

			int index;
			int position;
			FLVER2* parent = nullptr;
		};

		class Mesh
		{
			friend class FLVER2;
			public:
			bool dynamic = true;
			int defaultBoneIndex = -1;

			cfr_vec3 boundingBoxMin = {0.0f,0.0f,0.0f};
			cfr_vec3 boundingBoxMax = {0.0f,0.0f,0.0f};
			cfr_vec3 boundingBoxUnk = {0.0f,0.0f,0.0f};

			Material* material = nullptr;

			std::vector<Faceset*> facesets;
			std::vector<VertexBuffer*> vertexBuffers;

			Mesh(FLVER2* parent);
			~Mesh();

			FLVER2* parent();

			protected:
			void read(UMEM* src, FLVER2* parent);
			void write(UMEM* dst);

			//needed for ptde models
			int boneOffset = 0;
			std::vector<int> bone_indices;

			int index;
			FLVER2* parent = nullptr;
		};

		class Member
		{
			public:
			char* data; //size of dataLength

			Member();
			~Member();
		};

		class EdgeIndices
		{
			public:
			std::vector<int> memberIndices;

			EdgeIndices();
			~EdgeIndices();
		};

		class Faceset
		{
			friend class FLVER2;
			public:
			std::vector<int> indices;
			bool cullBackfaces = true;

			Faceset();
			~Faceset();

			Mesh* parent();
			bool isTriStrip();
			void makeTriStrip();
			void makeTriList();

			protected:
			void read(UMEM* src, Mesh* parent);
			void write(UMEM* dst);

			int index;
			Mesh* parent = nullptr;
			FLVER2* f_parent = nullptr;
		};

		class LayoutMember
		{
			public:
			uint32_t type, semantic;

			LayoutMember();
			~LayoutMember();

			BufferLayout* parent();

			protected:
			void read(UMEM* src, BufferLayout* parent);
			void write(UMEM* dst);
			
			int index;
			BufferLayout* parent = nullptr;
		};

		class BufferLayout
		{
			public:
			std::vector<FLVER2::LayoutMember*> layoutMembers;

			BufferLayout();
			~BufferLayout();

			VertexBuffer* parent();

			protected:
			void read(UMEM* src, VertexBuffer* parent);
			void write(UMEM* dst);

			int index;
			VertexBuffer* parent = nullptr;
			FLVER2* f_parent = nullptr;
		};

		class VertexBuffer
		{
			public:
			//TODO vertex class

			VertexBuffer();
			~VertexBuffer();

			Mesh* parent();

			protected:
			void read(UMEM* src, Mesh* parent);
			void write(UMEM* dst);

			int index;
			Mesh* parent = nullptr;
		};

		class Texture
		{
			public:
			std::string path;
			std::string type;
			cfr_vec2 scale = {0.0f,0.0f};

			Texture();
			~Texture();

			Material* parent();

			protected:
			void read(UMEM* src, Material* parent);
			void write(UMEM* dst);

			int index;
			Material* parent = nullptr;
			FLVER2* f_parent = nullptr;
		};

		class Vertex
		{
			public:
			class Position
			{
				public:
				float x,y,z;
				static void pack(int type, int idx, Position* src, UMEM* dst);
				static void unpack(int type, UMEM* src, Position* dst);
			};

			class BoneWeights
			{
				public:
				float a,b,c,d;
				static void pack(int type, int idx, BoneWeights* src, UMEM* dst);
				static void unpack(int type, UMEM* src, BoneWeights* dst);
			};

			class BoneIndices
			{
				public:
				Bone* a,* b,* c,* d;
				static void pack(int type, int idx, BoneIndices* src, UMEM* dst);
				static void unpack(int type, UMEM* src, BoneIndices* dst);
			};

			class Normal
			{
				public:
				float x,y,z;
				int w;
				static void pack(int type, int idx, Normal* src, UMEM* dst);
				static void unpack(int type, UMEM* src, Normal* dst);
			};

			class Tangent
			{
				public:
				float x,y,z,w;
				static void pack(int type, int idx, Tangent* src, UMEM* dst);
				static void unpack(int type, UMEM* src, Tangent* dst);
			};

			class Bitangent
			{
				public:
				float x,y,z,w;
				static void pack(int type, int idx, Bitangent* src, UMEM* dst);
				static void unpack(int type, UMEM* src, Bitangent* dst);
			};

			class UV
			{
				public:
				float u,v;
				static void pack(int type, int idx, UV* src, UMEM* dst);
				static void unpack(int type, UMEM* src, UV* dst);
			};

			class Color
			{
				public:
				uint8_t r,g,b,a;
				static void pack(int type, int idx, Color* src, UMEM* dst);
				static void unpack(int type, UMEM* src, Color* dst);
			};

			Position position;
			BoneWeights boneWeights;
			BoneIndices boneIndices;
			Normal normal;
			Tangent tangent;
			Bitangent bitangent;
			std::vector<UV> uvs;
			std::vector<Color> colors;	
		};

		Header header;
		std::vector<Dummy*> dummies;
		std::vector<Material*> materials;
		std::vector<Bone*> bones;
		std::vector<Mesh*> meshes;
		std::vector<Faceset*> facesets;
		std::vector<VertexBuffer*> vertexBuffers;
		std::vector<BufferLayout*> bufferLayouts;
		std::vector<Texture*> textures;

		FLVER2(const char* path);
		FLVER2(UMEM* src);
		FLVER2(); //for generating new ones
		~FLVER2();

		void write(UMEM* dst);
		void reinit();

		void addDummy(Dummy* d);
		void addMaterial(Material* m);
		void addBone(Bone* b);
		void addMesh(Mesh* m);
	};
};