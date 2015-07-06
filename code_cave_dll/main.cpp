// main.cpp : Defines the entry point for the DLL application.
// created by password123456


#include <windows.h>
#include <fcntl.h>
#include <stdio.h>
#include <io.h>
#include "stdafx.h"


DWORD ExtractScoreRetAddr = 0;


VOID CodecaveStatFull(DWORD destAddress, VOID (*func)(VOID), BYTE nopCount);
VOID CodecaveSkillStatFull(DWORD destAddress, VOID (*func)(VOID), BYTE nopCount);
VOID CodecaveGainExtraEXP(DWORD destAddress, VOID (*func)(VOID), BYTE nopCount);
VOID CodecaveGainExtraGold(DWORD destAddress, VOID (*func)(VOID), BYTE nopCount);

VOID WriteBytesASM(DWORD destAddress, LPVOID patch, DWORD numBytes);

DWORD WINAPI HACK(LPVOID lpParam); 


// This is our codecave function, we must  remember to make it a "__declspec(naked)" function
__declspec(naked) void StatFull(void)
{
	__asm
	{
		// The first thing we must do in our codecave is save the return address from the top of the stack
		// Since we know the current score is in EDX, copy it over into our variable
		mov [eax+0x3dc],0xff
	}
	
	// Invoke our C++ function now
	
	__asm 
	{
		// Restore everything to how it was before
		cmp dword ptr [eax+0x3dc],00
		ret
	}
}

__declspec(naked) void SkillStatFull(void)
{
	__asm
	{
		// The first thing we must do in our codecave is save the return address from the top of the stack
		// Since we know the current score is in EDX, copy it over into our variable
		mov [ecx+0x3e0],0xff
			
	}
	
	// Invoke our C++ function now
	
	__asm 
	{
		// Restore everything to how it was before
		cmp dword ptr [ecx+0x3e0],00
		ret
	}
}

__declspec(naked) void GainExtraEXP(void)
{
	__asm
	{
		// The first thing we must do in our codecave is save the return address from the top of the stack
		// Since we know the current score is in EDX, copy it over into our variable
		add ebx,0x3e8
	}
	
	// Invoke our C++ function now
	
	__asm 
	{
		// Restore everything to how it was before
		add [esi+0x3c8],ebx
		ret
	}
}

__declspec(naked) void GainExtraGold(void)
{
	__asm
	{
		// The first thing we must do in our codecave is save the return address from the top of the stack
		// Since we know the current score is in EDX, copy it over into our variable
		add edi,0x3e8
			
	}
	
	// Invoke our C++ function now
	
	__asm 
	{
		// Restore everything to how it was before
		add [esi+0x3c4],edi
		ret
	}
}


DWORD WINAPI HACK(LPVOID lpParam)
{
	for(;;) //for문으로 안하고 키 입력 체크는 못하는가??
	{
		Sleep(10);
	
		if (GetAsyncKeyState(VK_F1) & 0X8000)  // 여기서 부터 DLL에서 GetAsyncKeyState
		{
			CodecaveStatFull($YOUR_ADDRESS, StatFull, 2);
			MessageBeep(MB_ICONASTERISK);
		}

		if (GetAsyncKeyState(VK_F2) & 0X8000)
		{
			CodecaveSkillStatFull($YOUR_ADDRESS, SkillStatFull, 2);
			MessageBeep(MB_ICONASTERISK);
		}

		if (GetAsyncKeyState(VK_F3) & 0X8000)
		{
			CodecaveGainExtraEXP($YOUR_ADDRESS, GainExtraEXP, 1);
			MessageBeep(MB_ICONASTERISK);
		}
	
		if (GetAsyncKeyState(VK_F4) & 0X8000)
		{
			CodecaveGainExtraGold($YOUR_ADDRESS, GainExtraGold, 1);
			MessageBeep(MB_ICONASTERISK);
		}
	}

return 0;

}  

BOOL APIENTRY DllMain( HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved  ) 
{ 
	UNREFERENCED_PARAMETER(lpReserved);

	if (ul_reason_for_call == DLL_PROCESS_ATTACH) 
	{ 
		
		CreateThread(NULL,NULL,(LPTHREAD_START_ROUTINE)HACK,NULL,NULL,NULL); 		
		DisableThreadLibraryCalls(hModule);
	} 
	else
	{
		return TRUE;
	}
return TRUE; 
}


VOID WriteBytesASM(DWORD destAddress, LPVOID patch, DWORD numBytes) // Writeprocess Memory ASM 버젼
{
	DWORD oldProtect = 0;
	DWORD srcAddress = PtrToUlong(patch);

	VirtualProtect((void*)(destAddress), numBytes, PAGE_EXECUTE_READWRITE, &oldProtect);

	__asm
	{
		nop						
		nop						
		nop						

		mov esi, srcAddress		
		mov edi, destAddress	
		mov ecx, numBytes		
Start:
		cmp ecx, 0				
		jz Exit					

		mov al, [esi]			
		mov [edi], al			
		dec ecx					
		inc esi					
		inc edi					
		jmp Start				
Exit:
		nop						
		nop						
		nop						
	}

	VirtualProtect((void*)(destAddress), numBytes, oldProtect, &oldProtect);
}

VOID CodecaveStatFull(DWORD destAddress, VOID (*func)(VOID), BYTE nopCount)
{
	DWORD offset = (PtrToUlong(func) - destAddress) - 5;
	BYTE nopPatch[0xFF] = {0};
	BYTE patch[5] = {0xE8, 0x00, 0x00, 0x00, 0x00};
	memcpy(patch + 1, &offset, sizeof(DWORD));
	WriteBytesASM(destAddress, patch, 5);

	if(nopCount == 0)
		return;

	memset(nopPatch, 0x90, nopCount);
	WriteBytesASM(destAddress + 5, nopPatch, nopCount);
}

VOID CodecaveSkillStatFull(DWORD destAddress, VOID (*func)(VOID), BYTE nopCount)
{
	DWORD offset = (PtrToUlong(func) - destAddress) - 5;
	BYTE nopPatch[0xFF] = {0};
	BYTE patch[5] = {0xE8, 0x00, 0x00, 0x00, 0x00};
	memcpy(patch + 1, &offset, sizeof(DWORD));
	WriteBytesASM(destAddress, patch, 5);
	
	if(nopCount == 0)
		return;
	
	memset(nopPatch, 0x90, nopCount);
	WriteBytesASM(destAddress + 5, nopPatch, nopCount);
}

VOID CodecaveGainExtraEXP(DWORD destAddress, VOID (*func)(VOID), BYTE nopCount)
{
	DWORD offset = (PtrToUlong(func) - destAddress) - 5;
	BYTE nopPatch[0xFF] = {0};
	BYTE patch[5] = {0xE8, 0x00, 0x00, 0x00, 0x00};
	memcpy(patch + 1, &offset, sizeof(DWORD));
	WriteBytesASM(destAddress, patch, 5);
	
	if(nopCount == 0)
		return;
	
	memset(nopPatch, 0x90, nopCount);
	WriteBytesASM(destAddress + 5, nopPatch, nopCount);
}

VOID CodecaveGainExtraGold(DWORD destAddress, VOID (*func)(VOID), BYTE nopCount)
{
	DWORD offset = (PtrToUlong(func) - destAddress) - 5;
	BYTE nopPatch[0xFF] = {0};
	BYTE patch[5] = {0xE8, 0x00, 0x00, 0x00, 0x00};
	memcpy(patch + 1, &offset, sizeof(DWORD));
	WriteBytesASM(destAddress, patch, 5);
	
	if(nopCount == 0)
		return;
	
	memset(nopPatch, 0x90, nopCount);
	WriteBytesASM(destAddress + 5, nopPatch, nopCount);
}
