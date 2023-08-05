#include "flver2.h"

namespace cfr
{
	//this *should* be able to be optimized with and array of function pointers
	//ie, split each type into a specific function. then process the mtd and generate
	//an array of the specific functions needed to be run in a loop to encode/decode the data.
	//would cut the switch statement runcount from being $vertexCount to running once
	void writeVertexData(VertexType type, VertexSemantic semantic, UMEM* src, UMEM* src2, UMEM* dst, int uvFactor)
	{
		//int inputSize = getVertexTypeSize(type);

		uint8_t ui8 = 0;
		int8_t si8 = 0;

		uint16_t ui16 = 0;
		int16_t si16 = 0;

		uint32_t ui32 = 0;
		int32_t si32 = 0;

		float f1    =  0.0f;
		float f3[3] = {0.0f,0.0f,0.0f};
		float f4[4] = {0.0f,0.0f,0.0f,0.0f};

		uint8_t c[4] = {0,0,0,0};

		switch(semantic)
		{
			case(FVS_POSITION):
				switch(type)
				{
					case(FVT_FLOAT3):
						uread(&f3,12,1,src);
						uwrite((char*)&f3,12,1,dst);
						break;
					case(FVT_FLOAT4):
						uread(&f3,12,1,src);
						uwrite((char*)&f3,12,1,dst);
						useek(src,4,SEEK_CUR);
						break;
				}
				break;
			case(FVS_BONE_WEIGHTS):
				switch(type)
				{
					case(FVT_BYTE4A):
						for(int i = 0; i < 4; i++)
						{
							uread(&si8,1,1,src);
							f1 = (float)si8 / 127.0f;
							uwrite((char*)&f1,4,1,dst);
						}
						break;
					case(FVT_BYTE4C):
						for(int i = 0; i < 4; i++)
						{
							uread(&ui8,1,1,src);
							f1 = (float)ui8 / 255.0f;
							uwrite((char*)&f1,4,1,dst);
						}
						break;
					case(FVT_UV_PAIR):
					case(FVT_SHORT4_FLOAT4A):
						for(int i = 0; i < 4; i++)
						{
							uread(&si16,2,1,src);
							f1 = (float)si16 / 32767.0f;
							uwrite((char*)&f1,4,1,dst);
						}
						break;
					default:
						throw std::runtime_error("Unkown bone weight type!\n");
				}
				break;
			case(FVS_BONE_INDICES):
				switch(type)
				{
					case(FVT_BYTE4B):
					case(FVT_BYTE4E):
						for(int i = 0; i < 4; i++)
						{
							uread(&ui8,1,1,src);
							si16 = (short)ui8;
							uwrite((char*)&si16,2,1,dst);
						}
						break;
					case(FVT_SHORT_BONE_INDICES):
						for(int i = 0; i < 4; i++)
						{
							uread(&ui16,2,1,src);
							si16 = (float)ui16;
							uwrite((char*)&si16,2,1,dst);
						}
						break;
					default:
						throw std::runtime_error("Unkown bone index type!\n");
				}
				break;
			case(FVS_NORMAL): //always writes floats then int
				switch(type)
				{
					case(FVT_FLOAT3):
						uread(&f3,12,1,src);
						uwrite((char*)&f3,12,1,dst);
						uwrite((char*)&si32,4,1,dst2);
						break;
					case(FVT_FLOAT4):
						uread(&f3,12,1,src);
						uwrite((char*)&f3,12,1,dst);
						uread(&f1,4,1,src);
						si32 = (int)f1;
						uwrite((char*)&si32,4,1,dst2);
						break;
					case(FVT_BYTE4A):
					case(FVT_BYTE4B):
					case(FVT_BYTE4C):
					case(FVT_BYTE4E):
						//convertUByteXYZ(src,dst);
						uread(&ui8,1,1,src);
						si32 = (int)ui8;
						uwrite((char*)&si32,4,1,dst2);
						break;
					case(FVT_SHORT2_FLOAT2):
						uread(&ui8,1,1,src);
						si32 = (int)ui8;
						//convertSByteZYX(src,dst);
						uwrite((char*)&si32,4,1,dst2);
						break;
					case(FVT_SHORT4_FLOAT4A):
						for(int i = 0; i < 3; i++)
							//convertShortNorm(src,dst);
						uread(&si16,2,1,src);
						si32 = (int)si16;
						uwrite((char*)&si32,4,1,dst2);
						break;
					case(FVT_SHORT4_FLOAT4B):
						for(int i = 0; i < 3; i++)
							//convertUShortNorm(src,dst);
						uread(&si16,2,1,src);
						si32 = (int)si16;
						uwrite((char*)&si32,4,1,dst2);
						break;
					default:
						throw std::runtime_error("Unkown normal type!\n");
				}
				break;
			case(FVS_UV):
				switch(type)
				{
					case(FVT_FLOAT2):
						uread(&f3,8,1,src);
						f3[1] = 0 - f3[1];
						uwrite((char*)&f3,8,1,dst);
						break;
					case(FVT_FLOAT3):
						uread(&f3,12,1,src);
						f3[1] = 0 - f3[1];
						uwrite((char*)&f3,8,1,dst);
						break;
					case(FVT_FLOAT4):
						uread(&f3,8,1,src);
						f3[1] = 0 - f3[1];
						uwrite((char*)&f3,8,1,dst);
						uread(&f3,8,1,src);
						f3[1] = 0 - f3[1];
						uwrite((char*)&f3,8,1,dst);
						break;
					case(FVT_BYTE4A):
					case(FVT_BYTE4B):
					case(FVT_BYTE4C):
					case(FVT_SHORT2_FLOAT2):
					case(FVT_UV):
						uread(&si16,2,1,src);
						f1 = (float)si16 / uvFactor;
						uwrite((char*)&f1,4,1,dst);

						uread(&si16,2,1,src);
						f1 = 0 - ((float)si16 / uvFactor);
						uwrite((char*)&f1,4,1,dst);

						f1 = 0;
						break;
					case(FVT_UV_PAIR):
						uread(&si16,2,1,src);
						f1 = (float)si16 / uvFactor;
						uwrite((char*)&f1,4,1,dst);

						uread(&si16,2,1,src);
						f1 = -(float)si16 / uvFactor;
						uwrite((char*)&f1,4,1,dst);

						f1 = 0;

						uread(&si16,2,1,src);
						f1 = (float)si16 / uvFactor;
						uwrite((char*)&f1,4,1,dst);

						uread(&si16,2,1,src);
						f1 = -(float)si16 / uvFactor;
						uwrite((char*)&f1,4,1,dst);

						f1 = 0;
						break;
					case(FVT_SHORT4_FLOAT4B):						
						uread(&si16,2,1,src);
						f1 = (float)si16 / uvFactor;
						uwrite((char*)&f1,4,1,dst);

						uread(&si16,2,1,src);
						f1 = -(float)si16 / uvFactor;
						uwrite((char*)&f1,4,1,dst);

						uread(&si16,2,1,src);
						f1 = (float)si16 / uvFactor;

						useek(src,2,SEEK_CUR);
						break;
					default:
						printf("Unknown UV type! %i\n",type);
						throw std::runtime_error("Unknown UV type!\n");
				}
				break;
			case(FVS_TANGENT):
				switch(type)
				{
					case(FVT_FLOAT4):
						uread(&f4,16,1,src);
						uwrite((char*)&f4,16,1,dst);
						break;
					case(FVT_BYTE4A):
					case(FVT_BYTE4B):
					case(FVT_BYTE4C):
					case(FVT_BYTE4E):
						//convertUByteXYZ(src,dst);
						//convertUByteNorm(src,dst); //w
						break;
					case(FVT_SHORT4_FLOAT4A):
						//convertShortNorm(src,dst);
						//convertShortNorm(src,dst);
						//convertShortNorm(src,dst);
						//convertShortNorm(src,dst);
						break;
					default:
						throw std::runtime_error("Uknown tangent type!\n");
				}
				break;
			case(FVS_BITANGENT):
				switch(type)
				{
					case(FVT_BYTE4A):
					case(FVT_BYTE4B):
					case(FVT_BYTE4C):
					case(FVT_BYTE4E):
						//convertUByteXYZ(src,dst);
						//convertUByteNorm(src,dst);
						break;
					default:
						throw std::runtime_error("Unknown bitangent type!\n");
				}
				break;
			case(FVS_VERTEX_COLOR):
				switch(type)
				{
					case(FVT_FLOAT4):
						uread(&f4,16,1,src);
						uwrite((char*)&f4,16,1,dst);
						break;
					case(FVT_BYTE4A):
					case(FVT_BYTE4C):
						uread(&c,4,1,src);
						f4[0] = (float)c[0]/255.0f;
						f4[1] = (float)c[1]/255.0f;
						f4[2] = (float)c[2]/255.0f;
						f4[3] = (float)c[3]/255.0f;
						uwrite((char*)&f4,16,1,dst);
						break;
					default:
						printf("Unkown color type!\n");
						throw std::runtime_error("Unknown color type!\n");
				}
				break;
		}
	};
};