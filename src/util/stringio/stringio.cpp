#include "stringio.h"
#include "sjis_table.h"

namespace cfr
{
	std::string readString(UMEM* src)
	{
		std::string ret;
		bool nullFound = false;
		char c = 'a';

		while(nullFound == false)
		{
			uread(&c,sizeof(char),1,src);

			if(c == '\0')
				nullFound = true;
			else
				ret.push_back(c);
		}

		return ret;
	};

	std::wstring readWString(UMEM* src, bool endian)
	{
		std::wstring ret;
		bool nullFound = false;
		wchar_t c = 'a';

		while(nullFound == false)
		{
			ureade(&c,sizeof(wchar_t),1,src,endian);

			if(c == '\0')
				nullFound = true;
			else
				ret.push_back(c);
		}

		return ret;
	};

	std::string freadString(UMEM* src, bool nullTerm, int length)
	{
		std::string ret = "";

		if(nullTerm)
		{
			bool found = false;

			while(!found)
			{
				char c = ugetc(src);

				if(ueod(src) != 0)
					throw std::runtime_error("Reached end of file while reading string...\n");
				if(c == 0)
					found = true;
				else
					ret.push_back(c);
			}

			return ret;
		}
		else
		{
			//this could be better optimized
			for(int i = 0; i < length; i++)
			{
				if(ueod(src) != 0)
					throw std::runtime_error("Reached end of file while reading string...\n");
				ret.push_back(ugetc(src));
			}

			return ret;
		}
	};

	std::pair<std::string,std::string> splitFullPath(std::string fullpath)
	{
		int len = fullpath.size();
		std::pair<std::string, std::string> combo;
		combo.first = "";
		combo.second = "";
		bool found = false;

		while(!found)
		{
			if(len < 1)
				throw std::runtime_error("Reached end of string while getting name...\n");

			char c = fullpath.at(len);

			if((c == '\\')||(c == '/'))	
				found = true;
			else
			{
				combo.first.insert(0,1,c);
				len--;
			}
		}
		
		while(len > 0)
		{
			char c = fullpath.at(len);
			combo.second.insert(0,1,c);
			len--;
		}
		
		return combo;
	};

	wchar_t* getUniversalString(long offset, bool unicode, UMEM* src, int* len)
	{
		long pos = utell(src);
		useek(src,offset,SEEK_SET);

		int length = 0;
		wchar_t* ret;

		if(unicode)
		{
			wchar_t c;
			
			while(true)
			{
				//can't read by size of wchar_t
				uread(&c,2,1,src);
				length++;
				if(c == L'\0')
					break;
			}

			useek(src,-sizeof(wchar_t) * length,SEEK_CUR);
			ret = (wchar_t*)malloc(sizeof(wchar_t) * length);
			uread(&ret[0],sizeof(wchar_t),length,src);
		}
		else
		{			
			char c;
			
			while(true)
			{
				uread(&c,sizeof(char),1,src);
				length++;
				if(c == '\0')
					break;
			}

			useek(src,-length,SEEK_CUR);
			char* buffer = (char*)malloc(length);
			uread(&buffer[0],1,length,src);
			
			ret = (wchar_t*)malloc(length * sizeof(wchar_t));

			mbstowcs(ret,buffer,length);
			free(buffer);
			buffer = NULL;
		}

		useek(src,pos,SEEK_SET);

		memcpy(len,&length,sizeof(int));
		return ret;
	};

	std::string readUniStr(bool unicode, bool jis, UMEM* src, long offset)
	{
		long pos = utell(src);
		if(offset != INT64_MIN)
			useek(src,offset,SEEK_SET);
		
		std::string ret;

		if(unicode) //need to convert utf16 -> utf8
		{
			//wchar always stored w 2 bytes cause winderss
			char16_t wc;
			std::u16string str16;

			while(true)
			{
				uread(&wc,2,1,src); 
				str16.push_back(wc);
				if(wc == 0)
					break;
			}

			//wstring_convert leaks memory on windows because microsoft sucks and wont fix.
			//wstring_convert is depricated but won't be removed till replacement exists.
			std::wstring_convert<std::codecvt_utf8<char16_t>,char16_t> conversion;
			ret = conversion.to_bytes(str16);
		}
		else
		{
			char c;
			std::string str;

			while(true)
			{
				uread(&c,1,1,src);
				str.push_back(c);
				if(c == '\0')
					break;
			}

			if(jis)
				ret = jisToUtf8(str);
			else
				ret = str;
		}

		if(offset != INT64_MIN)
			useek(src,pos,SEEK_SET);

		return ret;
	};

	int writeUniStr(bool unicode, bool jis, std::string str, UMEM* dst, long offset)
	{
		long pos = utell(dst);
		if(offset != INT64_MIN)
			useek(dst,offset,SEEK_SET);

		int ret = -1;

		if(unicode)
		{
			//TODO: add check for null term
			std::wstring_convert<std::codecvt_utf8<char16_t>,char16_t> conversion;
			std::u16string s16 = conversion.from_bytes(str);
			ret = uwrite((char*)s16.data(),s16.size(),1,dst);
		}
		else
		{
			if(jis)
			{
				std::string jstr = utf8ToJIS(str);
				ret = uwrite(jstr.data(),jstr.size(),1,dst);
			}
			else
			{
				ret = uwrite(str.data(),str.size(),1,dst);
			}
		}

		if(offset != INT64_MIN)
			useek(dst,pos,SEEK_SET);

		return ret;
	}

	int getWideStringLength(wchar_t* str)
	{
		int offset = 0;
		wchar_t c;

		while(true)
		{
			c = str[offset];
			offset++;
			if(c == L'\0')
				break;
		}

		return offset;
	};

	std::string jisToUtf8(std::string str)
	{
		bool skip = false;
		int outIndex = 0;
		std::string ret;

		for(int i = 0; i < str.length(); i++)
		{
			if(!skip)
			{
				char section = (uint8_t)str[i] >> 4;

				if(section == 0x8 || section == 0x9 || section == 0xE)
				{
					char16_t w;
					memcpy(&w,&str.data()[i],2);
					char utf = sjis_table[(int)w];
					ret.push_back(utf);

					skip = true;
				}
				else
				{
					char c = '\0';
					memcpy(&c,&str.data()[i],1);
					char utf = sjis_table[(int)c];
					ret.push_back(utf);
				}
				outIndex++;
			}
			skip = false;
		}

		return ret;
	};

	//this function is very slow and terrible.
	//might be a good idea to store a reverse map.
	std::string utf8ToJIS(std::string str)
	{
		std::string ret;

		for(int i = 0; i < str.length(); i++)
		{
			for(auto &t : sjis_table)
			{
				if(t.second == (int)str[i])
				{
					uint16_t jisChar = sjis_table[t.first];
					char section = (uint8_t)jisChar >> 4;

					if(section == 0x8 || section == 0x9 || section == 0xE)
					{
						char temp;
						memcpy(&temp,&jisChar,1);
						ret.push_back(temp);
						memcpy(&temp,&jisChar+1,1);
						ret.push_back(temp);
					}
					else
					{
						ret.push_back((char)jisChar);
					}

					break;
				}
			}
		}

		return ret;
	};

	int jisToUtf8(char* str, int lengthIn, char* out)
	{
		bool skip = false;
		int outIndex = 0;
		for(int i = 0; i < lengthIn; i++)
		{
			if(skip == false)
			{
				char section = (uint8_t)str[i] >> 4;
				if(section == 0x8 || section == 0x9 || section == 0xE)
				{
					if(out != NULL)
					{
						wchar_t w = L'\0';
						memcpy(&w,&str[i],2);
						char utf = sjis_table[(int)w];
						memcpy(&out[outIndex],&utf,1);
					}
					
					skip = true; //TODO: this doesn't do anything????
				}
				else
				{
					if(out != NULL)
					{
						char c = '\0';
						memcpy(&c,&str[i],1);
						char utf = sjis_table[(int)c];
						memcpy(&out[outIndex],&utf,1);
					}
				}
				outIndex += 1;
			}
			skip = false;
		}

		return outIndex;
	};	
};