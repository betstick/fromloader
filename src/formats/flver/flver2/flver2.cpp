#include "flver2.h"
#define VALIDATE_ALL 
//this method isn't that friendly to performance, but its reasonably clean.

namespace cfr
{
	FLVER2::Header::Header(){};

	FLVER2::Header::Header(UMEM* src)
	{
		uread(this,sizeof(FLVER2::Header),1,src);

#ifdef VALIDATE_ALL
		if(strncmp(this->magic,"FLVER\0",6) != 0)
		{
			printf("Failed to validate FLVER2!\n");
			throw std::runtime_error("Failed to validate FLVER2!\n");
		}
#endif
	};

	FLVER2::Dummy::Dummy(UMEM* src)
	{
		uread(this,sizeof(FLVER2::Dummy),1,src);

#ifdef VALIDATE_ALL
		assertMsg(this->unk38,"FLVER2::Dummy.unk38 is not 0!\n");
		assertMsg(this->unk3C,"FLVER2::Dummy.unk38 is not 0!\n");
#endif
	};

	FLVER2::Material::Material(UMEM* src, FLVER2* parent)
	{
		uread(&this->header,sizeof(Header),1,src);

		uint32_t nameOffset = this->header.nameOffset;
		
		int8_t unicode = parent->header.unicode;
		//printf("unicode: %i\n",unicode);
		this->name = getUniversalString(nameOffset,unicode,src,&this->nameLength);
		//printf("%ls\n",this->name);
		uint32_t mtdOffset = this->header.mtdOffset;
		this->mtdName = getUniversalString(mtdOffset,unicode,src,&this->mtdNameLength);

		//printf("this->mtdnamelength: %i\n",this->mtdNameLength);

		//idk even know what gxitems *are*

		//this->gxItems = (GxItem*)malloc(sizeof(GxItem));

		/*if(this->header.gxOffset == 0)
			gxIndex = -1;*/

		/*if(this->header.gxOffset != 0)
		{
			int gxCount = 0;
			long startpos = utell(src); //come back after enumeration
			useek(src,this->header.gxOffset,SEEK_CUR);
			
			//dry run to get count
			//if(parent->header.version <= 0x20010)
			if(parent->header.version <= 0x01002)
			{
				//read one 
				int gxCount = 1;
			}
			else
			{
				//read until terminator value found
				while(true)
				{
					GxItem* temp = new GxItem(src,parent);
					gxCount++;
					//uread(&value,sizeof(int32_t),1,src);
					//if(value != INT_MAX && value != -1)
					if(
						temp->id != 0x7FFFFFFF && 
						temp->id != 0xFFFFFFF7 //&& 
						//parent->header.version >= 0x20010
					)
					{
						//read it off into the void
						//GxItem(src,parent);
						//gxCount++;
					}
					else
					{
						break;
					}
				}
			}

			this->gxItemCount = gxCount;
			printf("gxcount: %i\n",this->gxItemCount);
			this->gxItems = (GxItem*)malloc(sizeof(GxItem) * gxCount);

			useek(src,this->header.gxOffset,SEEK_CUR);
			for(int i = 0; i < gxCount; i++)
				this->gxItems[i] = GxItem(src,parent);

			int32_t id; //assert int maxvalue
			uread(&id,sizeof(int32_t),1,src);
			int32_t unk100; //assert 100
			uread(&unk100,sizeof(int32_t),1,src);
			int32_t termLen;
			uread(&termLen,sizeof(int32_t),1,src);
			char* terminator = (char*)malloc(termLen - 0xC);
			uread(&terminator[0],termLen - 0xC,1,src);
			//assert all chars in terminator are 0x00
			
			useek(src,startpos,SEEK_SET);
		}*/
	};

	FLVER2::Material::~Material()
	{
		free(this->name);
		this->name = NULL;
		free(this->mtdName);
		this->mtdName = NULL;
	};

	FLVER2::GxItem::GxItem(UMEM* src, FLVER2* parent)
	{
		//depending on parent header version, this might be a string?
		uread(&this->id,sizeof(int32_t),1,src);
		
		uread(&this->unk04,sizeof(int32_t),1,src);
		uread(&this->length,sizeof(int32_t),1,src);

		if(this->length > 12)
		{
			this->data = (char*)malloc(this->length-12);
			uread(&this->data[0],this->length-12,1,src);
		}
	};

	FLVER2::GxItem::~GxItem()
	{
		if(this->length > 12)
		{
			free(this->data);
			this->data = NULL;
		}
	};

	FLVER2::Bone::Bone(UMEM* src, FLVER2* parent)
	{
		uread(&this->translation,sizeof(cfr_vec3),1,src);
		uread(&this->nameOffset,sizeof(uint32_t),1,src);

		bool unicode = false;
		if(parent->header.unicode == 1)
			unicode = true;

		this->name = getUniversalString(this->nameOffset,unicode,src,&this->nameLength);

		uread(&this->rot,sizeof(cfr_vec3),1,src);
		
		uread(&this->parentIndex,sizeof(int16_t),1,src);
		uread(&this->childIndex,sizeof(int16_t),1,src);

		uread(&this->scale,sizeof(cfr_vec3),1,src);

		uread(&this->nextSiblingIndex,sizeof(int16_t),1,src);
		uread(&this->previousSiblingIndex,sizeof(int16_t),1,src);
		
		uread(&this->boundingBoxMin,sizeof(cfr_vec3),1,src);

		uread(&this->unk3C,sizeof(int32_t),1,src);

		uread(&this->boundingBoxMax,sizeof(cfr_vec3),1,src);

		useek(src,52,SEEK_CUR); //skip empty junk padding
	};

	FLVER2::Bone::~Bone()
	{
		free(this->name);
		this->name = NULL;
	};

	FLVER2::Mesh::Mesh(UMEM* src, FLVER2* parent)
	{
		uread(&this->header,sizeof(Header),1,src);

#ifdef VALIDATE_ALL
		assertMsg(this->header.unk01,"FLVER2::Mesh.Header.unk01 is not 0!\n");
		assertMsg(this->header.unk02,"FLVER2::Mesh.Header.unk02 is not 0!\n");
		assertMsg(this->header.unk03,"FLVER2::Mesh.Header.unk03 is not 0!\n");

		assertMsg(this->header.unk08,"FLVER2::Mesh.Header.unk08 is not 0!\n");
		assertMsg(this->header.unk0C,"FLVER2::Mesh.Header.unk0C is not 0!\n");
#endif

		long pos = utell(src);

		if(this->header.boundingBoxOffset != 0)
		{
			useek(src,this->header.boundingBoxOffset,SEEK_SET);
			uread(&this->boundingBoxMin,sizeof(cfr_vec3),2,src);

			if(parent->header.version >= 0x2001A)
				uread(&this->boundingBoxUnk,sizeof(cfr_vec3),1,src);
		}

		if(this->header.boneCount > 0)
		{
			useek(src,this->header.boneIndicesOffset,SEEK_SET);
			this->boneIndices = (int32_t*)malloc(sizeof(int32_t) * this->header.boneCount);
			uread(&this->boneIndices[0],sizeof(int32_t),this->header.boneCount,src);
		}

		useek(src,this->header.facesetIndicesOffset,SEEK_SET);
		this->facesetIndices = (int32_t*)malloc(sizeof(int32_t) * this->header.facesetCount);
		uread(&facesetIndices[0],sizeof(int32_t),this->header.facesetCount,src);

		useek(src,this->header.vertexBufferIndicesOffset,SEEK_SET);
		this->vertexBufferIndices = (int32_t*)malloc(sizeof(int32_t) * this->header.vertexBufferCount);
		uread(&vertexBufferIndices[0],sizeof(int32_t),this->header.vertexBufferCount,src);

		this->parent = parent;

		useek(src,pos,SEEK_SET);
	};

	FLVER2::Mesh::~Mesh()
	{
		if(this->header.boneCount > 0)
		{
			free(this->boneIndices);
			this->boneIndices = NULL;
		}

		free(this->facesetIndices);
		this->facesetIndices = NULL;

		free(this->vertexBufferIndices);
		this->vertexBufferIndices = NULL;
	};

	FLVER2::Member::Member(UMEM* src, long startOffset)
	{
		uread(&this->header,sizeof(Header),1,src);

#ifdef VALIDATE_ALL
		assertMsg(this->header.unk08,"FLVER2::Member.Header.unk08 is not 0!\n");
		assertMsg(this->header.unk0C,"FLVER2::Member.Header.unk0C is not 0!\n");

		assertMsg(this->header.unk20,"FLVER2::Member.Header.unk20 is not 0!\n");
		assertMsg(this->header.unk24,"FLVER2::Member.Header.unk24 is not 0!\n");
		assertMsg(this->header.unk28,"FLVER2::Member.Header.unk28 is not 0!\n");
		assertMsg(this->header.unk2C,"FLVER2::Member.Header.unk2C is not 0!\n");

		assertMsg(this->header.unk3C == -1,"FLVER2::Member.Header.unk3C is not -1!\n");
#endif

		long pos = utell(src);

		useek(src,startOffset+this->header.dataOffset,SEEK_SET);

		this->data = (char*)malloc(this->header.dataLength);
		uread(this->data,this->header.dataLength,1,src);

		useek(src,pos,SEEK_SET);
	};

	FLVER2::Member::~Member()
	{
		free(this->data);
		this->data = NULL;
	};

	FLVER2::EdgeIndices::EdgeIndices(UMEM* src)
	{
		long pos = utell(src);

		uread(&this->header,sizeof(Header),1,src);

#ifdef VALIDATE_ALL
		assertMsg(this->header.unk08,"FLVER2::EdgeIndices.Header.unk08 is not 0!\n");
#endif
		
		this->members = (Member*)malloc(sizeof(Member) * this->header.memberCount);
		for(int i = 0; i < this->header.memberCount; i++)
			this->members[i] = Member(src,pos);
	};

	FLVER2::EdgeIndices::~EdgeIndices()
	{
		for(int i = 0; i < this->header.memberCount; i++)
		{
			this->members[i].~Member();
		}

		free(this->members);
		this->members = NULL;
	};

	FLVER2::Faceset::Faceset(UMEM* src, FLVER2* parent)
	{
		uread(&this->header,sizeof(Faceset::Header),1,src);

		int indexSize = 0;
		if(parent->header.version >= 0x20005)
			uread(&this->vertInfo,sizeof(vertInfo),1,src);

		if(this->vertInfo.vertexIndexSize == 0)
			this->vertInfo.vertexIndexSize = parent->header.vertexIndexSize;
		
		this->vertexSize = this->vertInfo.vertexIndexSize;
		
		long pos = utell(src);
		useek(src,parent->header.dataOffset + this->header.vertexIndicesOffset,SEEK_SET);

		if(this->vertInfo.vertexIndexSize == 8)
		{
			printf("Edge indices are not supported!\n");
			throw std::runtime_error("FLVER2::Faceset.VertexSize not implemented (8)!\n");
		}
		else if(this->vertInfo.vertexIndexSize == 16)
		{
			this->vertexIndicesShort = (uint16_t*)malloc(sizeof(uint16_t) * this->header.vertexIndexCount);
			uread(&this->vertexIndicesShort[0],sizeof(uint16_t),this->header.vertexIndexCount,src);
		}
		else if(this->vertexSize == 32)
		{
			//TODO: validate this
			this->vertexIndicesInt = (int32_t*)malloc(sizeof(int32_t) * this->header.vertexIndexCount);
			uread(&this->vertexIndicesInt[0],sizeof(int32_t),this->header.vertexIndexCount,src);
		}

		useek(src,pos,SEEK_SET);
	};

	FLVER2::Faceset::~Faceset()
	{
		if(this->vertInfo.vertexIndexSize == 16)
		{
			free(this->vertexIndicesShort);
			this->vertexIndicesShort = NULL;
		}
		else if(this->vertexSize == 32)
		{
			free(this->vertexIndicesInt);
			this->vertexIndicesInt = NULL;
		}
	};

	FLVER2::VertexBuffer::VertexBuffer(UMEM* src, FLVER2* parent)
	{
		uread(&this->header,sizeof(VertexBuffer::Header),1,src);
		long dataSize = this->header.vertexCount*this->header.vertexSize;

#ifdef VALIDATE_ALL
		assertMsg(this->header.unk10,"FLVER2::EdgeIndices.Header.unk10 is not 0!\n");
		assertMsg(this->header.unk14,"FLVER2::EdgeIndices.Header.unk14 is not 0!\n");
#endif

		long pos = utell(src);

		this->verts = (char*)malloc(dataSize);
		useek(src,this->header.bufferOffset + parent->header.dataOffset,SEEK_SET);
		uread(this->verts,dataSize,1,src);
		this->data = uopenMem(this->verts,dataSize);

		useek(src,pos,SEEK_SET);
	};

	FLVER2::VertexBuffer::~VertexBuffer()
	{
		uclose(this->data); //TODO: remove this stupid nonsense!
		free(this->verts);
		this->verts = NULL;
	};

	FLVER2::LayoutMember::LayoutMember(UMEM* src)
	{
		uread(this,sizeof(LayoutMember),1,src);

#ifdef VALIDATE_ALL
		//assertMsg(this->header.unk04,"FLVER2::LayoutMember.unk00 not from 0 to 2!\n");
#endif
	};

	FLVER2::LayoutMember::~LayoutMember()
	{

	};

	FLVER2::BufferLayout::BufferLayout(UMEM* src)
	{
		uread(&this->header,sizeof(Header),1,src);

#ifdef VALIDATE_ALL
		assertMsg(this->header.unk04,"FLVER2::BufferLayout.header.unk04 not 0!\n");
		assertMsg(this->header.unk08,"FLVER2::BufferLayout.header.unk04 not 0!\n");
#endif

		long pos = utell(src);
		useek(src,this->header.membersOffset,SEEK_SET);

		this->members = (FLVER2::LayoutMember*)malloc(sizeof(FLVER2::LayoutMember) * this->header.memberCount);

		for(int i = 0; i < this->header.memberCount; i++)
			this->members[i] = LayoutMember(src);

		useek(src,pos,SEEK_SET);
	};

	FLVER2::BufferLayout::~BufferLayout()
	{
		for(int i = 0; i < this->header.memberCount; i++)
		{
			this->members[i].~LayoutMember();
		}
		free(this->members);
		this->members = NULL;
	};

	FLVER2::Texture::Texture(UMEM* src, FLVER2* parent)
	{
		uread(&this->pathOffset,sizeof(uint32_t),1,src);
		this->path = getUniversalString(this->pathOffset,parent->header.unicode,src,&this->pathLength);

		uread(&this->typeOffset,sizeof(uint32_t),1,src);
		this->type = getUniversalString(this->typeOffset,parent->header.unicode,src,&this->typeLength);

		uread(&this->scale,sizeof(cfr_vec2),1,src);

		uread(&this->unk10,sizeof(char),1,src);
		uread(&this->unk11,sizeof(char),1,src);
		uread(&this->unk12,sizeof(short),1,src);
		uread(&this->unk14,sizeof(float),1,src);
		uread(&this->unk18,sizeof(float),1,src);
		uread(&this->unk1C,sizeof(float),1,src);

#ifdef VALIDATE_ALL
		assertMsg(this->unk12,"FLVER2::Texture.unk12 not 0!\n");
#endif
	};

	FLVER2::Texture::~Texture()
	{
		free(this->path);
		this->path = NULL;

		free(this->type);
		this->type = NULL;
	};

	//TODO: VALIDATE THESE
	FLVER2::VertexBoneWeights::VertexBoneWeights(UMEM* src)
	{
		uread(this,sizeof(VertexBoneWeights),1,src);

#ifdef VALIDATE_ALL
		assertMsg((this->length == 4),"FLVER2::VertexBoneWeights.length not 4!\n");
#endif
	};

	FLVER2::VertexBoneIndices::VertexBoneIndices(UMEM* src)
	{
		uread(this,sizeof(VertexBoneIndices),1,src);

#ifdef VALIDATE_ALL
		assertMsg((this->length == 4),"FLVER2::VertexBoneIndices.length not 4!\n");
#endif
	};

	//early declare to be called by specific initializers
	void initFLVER2(FLVER2* f, UMEM* src);

	FLVER2::FLVER2(const char* path)
	{
		UMEM* src;
		if (src = uopenFile(path,"rb"))
		{
			initFLVER2(this,src);
		}
		else
		{
			printf("Failed to open file!\n");
			throw std::runtime_error("Failed to open file!\n");
		}
	};

	FLVER2::FLVER2(UMEM* src)
	{
		useek(src,0,SEEK_SET);
		initFLVER2(this,src);
	};

	//for generating new flvers
	FLVER2::FLVER2()
	{

	};

	FLVER2::~FLVER2()
	{
		for(int i = 0; i < this->header.dummyCount; i++)
		{
			delete(this->dummies[i]);
			this->dummies[i] = NULL;
		}

		for(int i = 0; i < this->header.materialCount; i++)
		{
			delete(this->materials[i]);
			this->materials[i] = NULL;
		}

		for(int i = 0; i < this->header.boneCount; i++)
		{
			delete(this->bones[i]);
			this->bones[i] = NULL;
		}

		for(int i = 0; i < this->header.meshCount; i++)
		{
			delete(this->meshes[i]);
			this->meshes[i] = NULL;
		}

		for(int i = 0; i < this->header.facesetCount; i++)
		{
			delete(this->facesets[i]);
			this->facesets[i] = NULL;
		}

		for(int i = 0; i < this->header.vertexBufferCount; i++)
		{
			delete(this->vertexBuffers[i]);
			this->vertexBuffers[i] = NULL;
		}

		for(int i = 0; i < this->header.bufferLayoutCount; i++)
		{
			delete(this->bufferLayouts[i]);
			this->bufferLayouts[i] = NULL;
		}

		for(int i = 0; i < this->header.textureCount; i++)
		{
			delete(this->textures[i]);
			this->textures[i] = NULL;
		}

		free(this->dummies);
		this->dummies = NULL;

		free(this->materials);
		this->materials = NULL;

		free(this->bones);
		this->bones = NULL;

		free(this->meshes);
		this->meshes = NULL;

		free(this->facesets);
		this->facesets = NULL;

		free(this->vertexBuffers);
		this->vertexBuffers = NULL;

		free(this->bufferLayouts);
		this->bufferLayouts = NULL;

		free(this->textures);
		this->textures = NULL;
	};

	void initFLVER2(FLVER2* f, UMEM* src)
	{
		UMEM* data = src;
		f->header = FLVER2::Header(data);

		//printf("[F] Bone count: %i\n",f->header.boneCount);
		//printf("[F] Faceset count: %i\n",f->header.facesetCount);
		//printf("mesh count: %i\n",f->header.meshCount);
		//printf("vert buff count: %i\n",f->header.vertexBufferCount);
		//printf("dummy count: %i\n",f->header.dummyCount);

		f->dummies = (FLVER2::Dummy**)malloc(sizeof(FLVER2::Dummy) * f->header.dummyCount);
		f->materials = (FLVER2::Material**)malloc(sizeof(FLVER2::Material) * f->header.materialCount);
		f->bones = (FLVER2::Bone**)malloc(sizeof(FLVER2::Bone) * f->header.boneCount);
		f->meshes = (FLVER2::Mesh**)malloc(sizeof(FLVER2::Mesh) * f->header.meshCount);
		f->facesets = (FLVER2::Faceset**)malloc(sizeof(FLVER2::Faceset) * f->header.facesetCount);
		f->vertexBuffers = (FLVER2::VertexBuffer**)malloc(sizeof(FLVER2::VertexBuffer) * f->header.vertexBufferCount);
		f->bufferLayouts = (FLVER2::BufferLayout**)malloc(sizeof(FLVER2::BufferLayout) * f->header.bufferLayoutCount);
		f->textures = (FLVER2::Texture**)malloc(sizeof(FLVER2::Texture) * f->header.textureCount);

		for(int i = 0; i < f->header.dummyCount; i++)
			f->dummies[i] = new FLVER2::Dummy(data);
		
		//printf("dummy0: %f\n",f->dummies[0]->position.x);

		for(int i = 0; i < f->header.materialCount; i++)
			f->materials[i] = new FLVER2::Material(data,f);

		//f->materials[0]->print();

		for(int i = 0; i < f->header.boneCount; i++)
			f->bones[i] = new FLVER2::Bone(data,f);
	
		//f->bones[22]->print();
		
		for(int i = 0; i < f->header.meshCount; i++)
			f->meshes[i] = new FLVER2::Mesh(data,f);

		//printf("mesh0 fc count: %i\n",f->meshes[0]->header.facesetCount);

		for(int i = 0; i < f->header.facesetCount; i++)
			f->facesets[i] = new FLVER2::Faceset(data,f);

		//printf("faceset0: %i\n",f->facesets[0]->header.vertexIndexCount);
		
		for(int i = 0; i < f->header.vertexBufferCount; i++)
			f->vertexBuffers[i] = new FLVER2::VertexBuffer(data,f);

		//printf("vertexbuffer0: %i\n",f->vertexBuffers[0]->header.vertexCount);
		
		for(int i = 0; i < f->header.bufferLayoutCount; i++)
			f->bufferLayouts[i] = new FLVER2::BufferLayout(data);
		
		for(int i = 0; i < f->header.textureCount; i++)
			f->textures[i] = new FLVER2::Texture(data,f);

		//f->textures[0]->print();

		uclose(data);
	};

	void FLVER2::Bone::print()
	{
		//setlocale(LC_ALL, "");
		printf("name: %LS\n",this->name);
		printf("\ttranslation: %8.2f %8.2f %8.2f\n",this->translation.x,this->translation.y,this->translation.z);
		printf("\trotation:    %8.2f %8.2f %8.2f\n",this->rot.x,this->rot.y,this->rot.z);
		printf("\tscale:       %8.2f %8.2f %8.2f\n",this->scale.x,this->scale.y,this->scale.z);
	};

	void FLVER2::Material::print()
	{
		printf("name: %LS\n",this->name);
		printf("mtd name: %LS\n",this->mtdName);
		printf("\titem count: %i\n",this->gxItemCount);
	};

	void FLVER2::Texture::print()
	{
		printf("path: %LS\n",this->path);
		printf("type: %LS\n",this->type);
	};
};