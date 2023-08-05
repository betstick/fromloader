#include "flver2_rw.h"
#include "flver2_structs.h"
#define VALIDATE_ALL 

//Why are all the classes inhereted and defined in here????
//so they can access each other's internal data and such. they need to.
//but also so i can "hide" this from the users. they don't need to see it.

namespace cfr
{
	//attempts to find a bone in a flver. if it cannot be found, it returns -1
	//returns the index of the bone
	int getBoneIndex(FLVER2::Bone* b, FLVER2* f)
	{
		for(int i = 0; i < f->bones.size(); i++)
		{
			if(f->bones[i] == b)
			{
				return i;
			}
		}

		return -1;
	};

	//this is a stupid function
	FLVER2::Bone* getBonePointer(int index, FLVER2* f)
	{
		return f->bones[index];
	};

	FLVER2::Dummy::Dummy(Bone* parent)
	{
		this->parent = parent;
		this->f_parent = this->parent->parent;
	};

	FLVER2::Dummy::~Dummy()
	{
		this->parent = nullptr;
		this->f_parent = nullptr;
	};

	void FLVER2::Dummy::read(UMEM* src)
	{
		dummy_s* temp = (dummy_s*)malloc(sizeof(dummy_s));
		uread(temp,sizeof(dummy_s),1,src);

		this->id = temp->referenceID;

		this->position = temp->position;
		this->forward = temp->forward;
		this->upward = temp->upward;

		this->useUpdward = temp->useUpwardVector;
		
		memcpy(this->color,temp->color,4);

		free(temp);
		temp = nullptr;
	};

	void FLVER2::Dummy::write(UMEM* dst)
	{
		dummy_s* temp = (dummy_s*)malloc(sizeof(dummy_s));

		temp->referenceID = this->id;

		temp->position = this->position;
		temp->forward = this->forward;
		temp->upward = this->upward;

		temp->useUpwardVector = this->useUpdward;
		
		memcpy(temp->color,this->color,4);

		uwrite((char*)temp,sizeof(dummy_s),1,dst);

		free(temp);
		temp = nullptr;
	};

	FLVER2::Bone::Bone(FLVER2* parent)
	{
		this->parent = parent;
	};

	FLVER2::Bone::~Bone()
	{
		this->parent_bone = nullptr;
		this->child_bone = nullptr;
		this->siblingNext = nullptr;
		this->siblingPrev = nullptr;
		this->dummies.clear();

		this->parent = nullptr;
	};

	void FLVER2::Bone::read(UMEM* src)
	{
		bone_s* temp = (bone_s*)malloc(sizeof(bone_s));
		uread(temp,sizeof(bone_s),1,src);

		this->name = readUniStr(this->parent->header.unicode,true,src,temp->nameOffset);

		this->translation = temp->translation;
		this->rotation = temp->rotation;
		this->scale = temp->scale;
		this->boundingBoxMin = temp->boundingBoxMin;
		this->boundingBoxMax = temp->boundingBoxMax;

		this->parent_bone = this->parent->bones[temp->parentIndex];
		this->child_bone  = this->parent->bones[temp->childIndex];
		this->siblingNext = this->parent->bones[temp->nextSiblingIndex];
		this->siblingPrev = this->parent->bones[temp->previousSiblingIndex];

		free(temp);
		temp = nullptr;
	};

	void FLVER2::Bone::write(UMEM* dst)
	{
		bone_s* temp = (bone_s*)malloc(sizeof(bone_s));

		temp->nameOffset = 0;
		temp->nameLength = this->name.size();

		temp->translation = this->translation;
		temp->rotation = this->rotation;
		temp->scale = this->scale;
		temp->boundingBoxMax = this->boundingBoxMax;
		temp->boundingBoxMin = this->boundingBoxMin;

		temp->parentIndex = getBoneIndex(this->parent_bone,this->parent);
		temp->childIndex = getBoneIndex(this->child_bone,this->parent);
		temp->nextSiblingIndex = getBoneIndex(this->siblingNext,this->parent);
		temp->previousSiblingIndex = getBoneIndex(this->siblingPrev,this->parent);

		this->position = utell(dst);
		uwrite((char*)temp,sizeof(bone_s),1,dst);

		free(temp);
		temp = nullptr;
	};

	void FLVER2::Bone::writeData(UMEM* dst)
	{
		uint32_t pos = utell(dst); //get position string will be written to
		writeUniStr(this->parent->header.unicode,true,this->name,dst); //write the string
		
		int64_t o = utell(dst); //get current write location

		useek(dst,this->position,SEEK_SET); //seek back to the struct
		useek(dst,sizeof(cfr_vec3),SEEK_CUR); //seek forward to string offset position
		uwrite((char*)&pos,sizeof(uint32_t),1,dst); //write string position

		useek(dst,o,SEEK_CUR); //go back to the end of the write buffer
	};
	
	FLVER2::Mesh::Mesh(FLVER2* parent)
	{
		this->parent = parent;
	};

	void FLVER2::Mesh::read(UMEM* src, FLVER2* parent)
	{
		mesh_s_0* temp_0 = (mesh_s_0*)malloc(sizeof(mesh_s_0));
		uread(temp_0,sizeof(mesh_s_0),1,src);

		this->dynamic = temp_0->dynamic;
		this->material = nullptr;
		this->defaultBoneIndex = temp_0->defaultBoneIndex;
		this->
	};

	FLVER2* read(UMEM* src);

	FLVER2::FLVER2(UMEM* src)
	{

	};

	FLVER2::FLVER2(const char* path)
	{

	};

	FLVER2* read(UMEM* src)
	{
		FLVER2* f;

		//temp storage to maintain values and stuff

		return f;
	};

	void FLVER2::write(UMEM* dst)
	{
		
	};
};