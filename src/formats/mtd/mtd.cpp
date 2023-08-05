#include "mtd.h"
#include <locale.h>

namespace cfr
{
	MTD::Marker::Marker(UMEM* src)
	{
		uread(&this->value,sizeof(char),1,src);
		//printf("this->value: %i\n",this->value);

		//realignment code
		long pos = utell(src);
		if(pos % 4 != 0)
			useek(src,4-(pos%4),SEEK_CUR);
	};

	MTD::Block::Block(UMEM* src)
	{
		//realignment code
		/*long pos = utell(src);
		if(pos % 4 > 0)
			useek(src,4-(pos%4),SEEK_CUR);*/

		//printf("block start!\n");
		//printf("src pos: 0x%x\n",utell(src));
		uread(&this->zero,sizeof(int32_t),1,src);
		uread(&this->length,sizeof(int32_t),1,src);
		//printf("this->length: %i",this->length);
		uread(&this->type,sizeof(int32_t),1,src);
		uread(&this->version,sizeof(int32_t),1,src);
		//printf("this->vertsion: %i\n",this->version);
		
		this->marker = new Marker(src);
	};
	
	MTD::Block::~Block()
	{
		//printf("BLOCK DOWN!\n");
		delete(this->marker);
		//printf("BLOCK GONE!\n");
	};

	MTD::MarkedString::MarkedString(UMEM* src)
	{
		uread(&this->length,sizeof(int32_t),1,src);
		if(this->length > 0)
		{
			this->str = (char*)malloc(this->length*sizeof(char));
			uread(&this->str[0],sizeof(char),this->length,src);
		}
		else
		{
			this->length = 0;
			this->str = NULL;
		}

		this->marker = new Marker(src);
	};

	MTD::MarkedString::~MarkedString()
	{
		//printf("STRING DOWN!\n");
		if(this->length > 0 && this->str != NULL)
		{
			free(this->str);
			this->str = NULL;
		}

		delete(this->marker);
		//printf("STRING GONE!\n");
	};

	MTD::Texture::Texture(UMEM* src)
	{
		this->block = new Block(src);
		this->type = new MarkedString(src);
		uread(&this->uvNumer,sizeof(int32_t),1,src);
		this->marker = new Marker(src);
		uread(&this->shaderDataIndex,sizeof(int32_t),1,src);
		
		if(this->block->version == 5)
		{
			uread(&this->unkB0C,sizeof(int32_t),1,src);
			this->path = new MarkedString(src);
			uread(&this->floatCount,sizeof(float),1,src);
			this->floats = (float*)malloc(this->floatCount * sizeof(float));
			uread(&this->floats[0],sizeof(float),this->floatCount,src);
		}
	};

	MTD::Texture::~Texture()
	{
		//printf("TEXTURE DOWN!\n");
		if(this->type != NULL)
		{
			//this->type->~MarkedString();
			delete(this->type);
			this->type = NULL;
		}
		if(this->block->version == 5)
		{
			if(this->path != NULL)
			{
				//this->path->~MarkedString();
				delete(this->path);
				this->path = NULL;
			}
		}

		delete(this->block);
		delete(this->marker);

		if(this->floatCount > 0 && this->floats != NULL)
		{
			free(this->floats);
			this->floats = NULL;
		}
		//printf("TEXTURE GONE!\n");
	};

	MTD::Value::Value(UMEM* src)
	{
		this->block = new Block(src);
		uread(&this->valueCount,sizeof(int32_t),1,src);
		
		if(this->block->type == 0x1000)
		{
			this->byteValues = (int8_t*)malloc(this->valueCount * sizeof(int8_t));
			uread(&this->byteValues[0],sizeof(int8_t),this->valueCount,src);
		}
		else if(this->block->type == 0x1001)
		{
			this->intValues = (int32_t*)malloc(this->valueCount * sizeof(int32_t));
			uread(&this->intValues[0],sizeof(int32_t),this->valueCount,src);
		}
		else if(this->block->type == 0x1002)
		{
			this->floatValues = (float*)malloc(this->valueCount * sizeof(float));
			uread(&this->floatValues[0],sizeof(float),this->valueCount,src);
		}
	};

	MTD::Value::~Value()
	{
		delete(this->block);
		if(this->valueCount > 0)
		{
			if(this->block->type == 0x1000)
			{
				free(this->byteValues);
				this->byteValues = NULL;
			}
			if(this->block->type == 0x1001)
			{
				free(this->intValues);
				this->intValues = NULL;
			}
			if(this->block->type == 0x1002)
			{
				free(this->floatValues);
				this->floatValues = NULL;
			}
		}			
	};

	MTD::Param::Param(UMEM* src)
	{
		this->block = new Block(src);
		this->name = new MarkedString(src);
		this->type = new MarkedString(src);
		uread(&this->unkB00,sizeof(int32_t),1,src);
		this->value = new Value(src);
		this->end = new Marker(src);
		uread(&this->unkC00,sizeof(int32_t),1,src);
	};

	MTD::Param::~Param()
	{
		//printf("PARAM DOWN!\n");
		delete(this->block);
		delete(this->value);
		delete(this->end);
		//this->name->~MarkedString();
		delete(this->name);
		this->name = NULL;
		//this->type->~MarkedString();
		delete(this->type);
		this->type = NULL;
		//printf("PARAM GONE!\n");
	};

	MTD::Lists::Lists(UMEM* src)
	{
		this->block = new Block(src);
		uread(&this->unkA04,sizeof(int32_t),1,src);
		this->marker = new Marker(src);

		uread(&this->paramCount,sizeof(int32_t),1,src);
		this->params = (Param**)malloc(this->paramCount * sizeof(Param));
		for(int i = 0; i < this->paramCount; i++)
			this->params[i] = new Param(src);
			
		this->paramsEnd = new Marker(src);

		uread(&this->textureCount,sizeof(int32_t),1,src);
		this->textures = (Texture**)malloc(this->textureCount * sizeof(Texture));
		for(int i = 0; i < this->textureCount; i++)
			this->textures[i] = new Texture(src);

		this->end = new Marker(src);
		uread(&this->unkC00,sizeof(int32_t),1,src);
	};

	MTD::Lists::~Lists()
	{
		//printf("LISTS DOWN!\n");

		delete(this->block);
		delete(this->marker);
		delete(this->paramsEnd);
		delete(this->end);

		for(int i = 0; i < this->paramCount; i++)
		{
			delete(this->params[i]);
			//this->params[i]->~Param();
			this->params[i] = NULL;
		}

		if(this->paramCount > 0)
		{
			free(this->params);
			this->params = NULL;
		}
		
		for(int i = 0; i < this->textureCount; i++)
		{
			//this->textures[i]->~Texture();
			delete(this->textures[i]);
			this->textures[i] = NULL;
		}

		if(this->textureCount > 0)
		{
			free(this->textures);
			this->textures = NULL;
		}
		//printf("LISTS GONES!\n");
	};

	MTD::Data::Data(UMEM* src)
	{
		this->block = new Block(src);
		this->shader = new MarkedString(src);
		this->description = new MarkedString(src);
		//printf("desc: %s\n",this->description->toUtf8(NULL));
		uread(&this->unkB00,sizeof(int32_t),1,src);
		this->lists = new Lists(src);
		this->end = new Marker(src);
		uread(&this->unkC00,sizeof(int32_t),1,src);
	};

	MTD::Data::~Data()
	{
		//printf("DATA DOWN!\n");
		delete(this->block);
		this->shader = NULL;
		//this->shader->~MarkedString();
		delete(this->shader);
		this->shader = NULL;
		//this->description->~MarkedString();
		delete(this->description);
		this->description = NULL;
		//this->lists->~Lists();
		delete(this->lists);
		this->lists = NULL;
		delete(this->end);
		this->end = NULL;
	};

	MTD::Header::Header(UMEM* src)
	{
		this->block = new Block(src);
		this->magic = new MarkedString(src);
		//printf("header magic: %s",this->magic->toUtf8(NULL));
		uread(&this->unkB00,sizeof(int32_t),1,src);
	};

	MTD::Header::~Header()
	{
		//printf("STUPID!\n");
		delete(this->block);
		//this->magic->~MarkedString();
		delete(this->magic);
		this->magic = NULL;
	};

	void initMTD(MTD* f, UMEM* src);

	MTD::MTD(const char* path)// : File(path)
	{
		UMEM* src;
		if (src = uopenFile(path,"rb"))
		{
			initMTD(this,src);
		}
		else
		{
			printf("Failed to open MTD!\n");
			throw std::runtime_error("Failed to open file!\n");
		}
	}

	MTD::MTD(UMEM* src)// : File(src)
	{
		useek(src,0,SEEK_SET);
		initMTD(this,src);
	};

	MTD::~MTD()
	{
		//printf("WHY\n");
		delete(this->block);
		delete(this->header);
		delete(this->headerEnd);
		//this->mtdData->~Data();
		delete(this->mtdData);
		delete(this->end);
		this->mtdData = NULL;
	};

	void initMTD(MTD* m, UMEM* src)
	{
		//printf("HUH\n");
		//printf("beginning block\n");
		m->block = new MTD::Block(src);
		//printf("block: %i\n",m->block->length);
		m->header = new MTD::Header(src);
		//printf("hdr: %i\n",m->header->magic->length);
		m->headerEnd = new MTD::Marker(src);
		//printf("hdre: %i\n",m->headerEnd->value);
		m->mtdData = new MTD::Data(src);
		//printf("data p count: %i\n",m->mtdData->lists->paramCount);
		m->end = new MTD::Marker(src);
		//printf("end: %i\n",m->end->value);
		uread(&m->unkC00,sizeof(int32_t),1,src);
		//printf("unkc00: %i\n",m->unkC00);

		useek(src,0,SEEK_SET);
		uclose(src);
	};

	void MTD::MarkedString::print()
	{
		printf("%s",&this->str);
	};

	int MTD::MarkedString::toUtf8(char* output)
	{
		return jisToUtf8(this->str,this->length,output);
	};

	void MTD::Param::print()
	{
		printf("name: ");
		this->name->print();
		printf("\n");

		printf("type: ");
		this->type->print();
		printf("\n");

		printf("value: idk\n");	
	};

	void MTD::print()
	{
		int i = 0;
		printf("params:\n");
		printf("\tname: %s\n",&this->mtdData->lists->params[i]->name->str);
		//printf("\tvalue: %0",this->mtdData.lists.params[i].value.floatValues);
	};
};