// SigScanDLL.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "FindPattern.h"
#include <tlhelp32.h>
#include <map>
#include <string>
using std::map;
using std::string;
bool bIsLocal = false;
bool bInitialized = false;
BYTE *FFXiMemory = NULL;
DWORD_PTR BaseAddress = NULL;
DWORD_PTR ModSize = NULL;


typedef struct checks
{
	short start;
	short size;
	checks() { start = NULL; size = 0; }
	checks(short sstart, short ssize) { start = sstart; size = ssize; }
} checks;


//BOOL APIENTRY DllMain( HMODULE hModule,
//                       DWORD  ul_reason_for_call,
//                       LPVOID lpReserved
//					 )
//{
//	switch (ul_reason_for_call)
//	{
//		case DLL_PROCESS_DETACH:
//			FinalizeScan();
//			break;
//	}
//	return TRUE;
//}
void OutputDebugPrintf(const char* strOutputString, ...)
{
	char strBuffer[4096] = { 0 };
	va_list vlArgs;
	va_start(vlArgs, strOutputString);
	_vsnprintf(strBuffer, sizeof(strBuffer) - 1, strOutputString, vlArgs);
	//vsprintf(strBuffer,strOutputString,vlArgs);
	va_end(vlArgs);
	OutputDebugString(strBuffer);
}
GeekX_API void _stdcall InitializeSigCopyMemory(DWORD ProcessID, const char* Module)
{
	MODULEENTRY32 uModule;
	SecureZeroMemory(&uModule, sizeof(MODULEENTRY32));
	uModule.dwSize = sizeof(MODULEENTRY32); 
	//Create snapshot of modules and Iterate them
	HANDLE hModuleSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, ProcessID);
	for(BOOL bModule = Module32First(hModuleSnapshot, &uModule);bModule;bModule = Module32Next(hModuleSnapshot, &uModule))
	{
		uModule.dwSize = sizeof(MODULEENTRY32); 
		if(!_stricmp(uModule.szModule,Module))
		{
			FinalizeScan();
			BaseAddress = (DWORD_PTR)uModule.modBaseAddr;
			
			ModSize = uModule.modBaseSize;
		 
			if(GetCurrentProcessId() == ProcessID)
			{
				bIsLocal = true;
				bInitialized = true;
				FFXiMemory = (BYTE*)BaseAddress;
			}else{
				bIsLocal = false;
				FFXiMemory = new BYTE[ModSize];
				HANDLE hProcess = OpenProcess(PROCESS_VM_READ, FALSE, ProcessID);
				if(hProcess)
				{
					if(ReadProcessMemory(hProcess,(LPCVOID)BaseAddress,FFXiMemory,ModSize,NULL))
					{
						bInitialized = true;
					}
					CloseHandle(hProcess);
				}
			}
			break;
		}
	}
	CloseHandle(hModuleSnapshot);
}
GeekX_API void _stdcall FinalizeScan()
{
	if(FFXiMemory)
	{
		if(!bIsLocal)
		{
			delete FFXiMemory;
		}
		FFXiMemory = NULL;
		bInitialized = false;
	}
}
// 函数名称: GeekXFindPattern
// 函数说明: 根据特征码及偏移搜索地址 支持 通配符模糊匹配  如 ?? EB    但要成对出现
// 作    者: GeekX
// 时    间: 2019/9/30
// 参    数:   szPattern  特征码 字符
// 参    数: pmodulBase	取引用 得到 模块基址
// 参    数: offset	偏移 一般为0 可以自己得结果后再加 这里可以不需要 
//************************************************************

GeekX_API DWORD_PTR _stdcall GeekXFindPattern(const char* szPattern, DWORD_PTR*pmodulBase ,int offset )//初始化
{

	//Get Pattern length
	unsigned int PatternLength = strlen(szPattern);
	//Pattern must be divisible by 2 to be valid.
	if(PatternLength % 2 != 0 || PatternLength < 2 || !bInitialized || !FFXiMemory || !BaseAddress) return NULL;
	//Get the buffer size
	unsigned int buffersize = PatternLength/2;
	//Setup custom ptr location. Default to buffersize(first byte after signature)+offset
	int PtrOffset = buffersize + offset;
	*pmodulBase = BaseAddress;
	bool Dereference = false;
	if(memcmp(szPattern,"##",2)==0)
	{
		Dereference = true;
		szPattern += 2;
		PtrOffset = 0 + offset;
		PatternLength -= 2;
		buffersize--;
	}
	//Dont follow the pointer, return the exact end of signature+offset.
	if(memcmp(szPattern,"@@",2)==0)
	{
		Dereference = true;
		szPattern += 2;
		PatternLength -= 2;
	}

	//Capitalize the strings and create a string for cache key.
	char Pattern[1024];
	ZeroMemory(Pattern,sizeof(Pattern));
	strcpy_s(Pattern,sizeof(Pattern),szPattern);
	_strupr_s(Pattern,sizeof(Pattern));


	//Create the buffer
	unsigned char* buffer = new unsigned char[buffersize];
	SecureZeroMemory(buffer,buffersize);

	//array for bytes we need to check and temporary holders for size/start
	checks memchecks[32];
	short cmpcount = 0;
	short cmpsize = 0;
	short cmpstart = 0;
	//Iterate the pattern and build the buffer.
	for(size_t i = 0; i < PatternLength / 2 ; i++)
	{
		//Read the values of the bytes for usage to reduce use of STL.
		unsigned char byte1 = Pattern[i*2];
		unsigned char byte2 = Pattern[(i*2)+1];
		//Check for valid hexadecimal digits.
		if(((byte1 >= '0' && byte1 <= '9') || (byte1 <= 'F' && byte1 >= 'A')) || ((byte2 >= '0' && byte2 <= '9') || (byte2 <= 'F' && byte2 >= 'A')))
		{
			//Increase the comparison size.
			cmpsize++;
			//convert the 2 byte string to a byte value ("14" == 0x14 == 20)
			if (byte1 <= '9') buffer[i] += byte1 - '0';
			else buffer[i] += byte1 - 'A' + 10;
			buffer[i] *= 16;	
			if (byte2 <= '9') buffer[i] += byte2 - '0';
			else buffer[i] += byte2 - 'A' + 10;
			continue;
		}
		//Wasnt valid hex, is it a custom ptr location?
		else if(byte1 == 'X' && byte2 == byte1 && (PatternLength/2) - i > 3) 
		{
			//Set the ptr to this current location + offset.
			PtrOffset = i + offset;
			//Fill the buffer with the ptr locations.
			buffer[i++]	= 'X';
			buffer[i++]	= 'X';
			buffer[i++]	= 'X';
			buffer[i]	= 'X';			
		}
		//Wasnt a custom ptr location nor valid hex, so set it as a wildcard.
		else 
		{
			//? for wildcard, unknown byte value.
			buffer[i]	= '?';
		}
		//Add the check to the array.
		if(cmpsize>0) memchecks[cmpcount++] = checks(cmpstart,cmpsize);
		//Increase the starting check byte and reset the size comparison size.
		cmpstart = i+1;
		cmpsize = 0;
	}
	//Add the final check 
	if(cmpsize>0) memchecks[cmpcount++] = checks(cmpstart,cmpsize);
	
	//Get the current base address and module size.
	char* mBaseAddr = (char*)FFXiMemory;
	unsigned int mModSize = ModSize;
	//Boolean that returns true or false for matching.
	bool bMatching = true;
	//Iterate the Module.
	for	(char* 
		addr = (char*)memchr(mBaseAddr	, buffer[0], mModSize - buffersize);
		addr && (DWORD_PTR)addr < (DWORD_PTR)((DWORD_PTR)mBaseAddr + mModSize - buffersize);
		addr = (char*)memchr(addr+1		, buffer[0], mModSize - buffersize - (addr+1 - mBaseAddr))
		)
	{
		bMatching = true;
		//Iterate each comparison we need to do. (seperated by wildcards)
		for(short c = 0;c<cmpcount; c++)
		{
			//Compare the memory.
			if(memcmp(buffer + memchecks[c].start,(void*)(addr + memchecks[c].start),memchecks[c].size) != 0)
			{
				//Did not match, try next byte.
				bMatching = false;
				break;
			}
		}
		//After full Pattern scan, check if it matched.
		if(bMatching)
		{
			//Find address wanted in FFXI's memory space - not ours.
			DWORD_PTR Address = NULL;
			if(Dereference)
			{
				Address = (DWORD_PTR)*((void **)(addr + PtrOffset));
				//OutputDebugPrintf("DEBUG_INFO |指针 %x\n", Address);
				 
			}else{
				 
					 
				Address = BaseAddress + (DWORD_PTR)((addr + PtrOffset) - (DWORD_PTR)FFXiMemory- buffersize);
				//OutputDebugPrintf("DEBUG_INFO |值 %x\n", Address);
			}
			//Clear buffer and return result.
			delete [] buffer;
			return Address;
		}
	}
	//Nothing matched. Clear buffer
	delete [] buffer;
	return NULL;
}
