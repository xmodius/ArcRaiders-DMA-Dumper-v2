#pragma once
#include <Windows.h>
#include "vmmdll.h"

// Function pointers
typedef VMM_HANDLE(*VMMDLL_Initialize_t)(DWORD, LPCSTR[]);
typedef VOID(*VMMDLL_Close_t)(VMM_HANDLE);
typedef VOID(*VMMDLL_MemFree_t)(PVOID);
typedef BOOL(*VMMDLL_MemRead_t)(VMM_HANDLE, DWORD, ULONG64, PBYTE, DWORD);
typedef BOOL(*VMMDLL_Map_GetModuleU_t)(VMM_HANDLE, DWORD, PVMMDLL_MAP_MODULE*, DWORD);
typedef BOOL(*VMMDLL_PidList_t)(VMM_HANDLE, PDWORD, PSIZE_T);
typedef BOOL(*VMMDLL_ProcessGetInformation_t)(VMM_HANDLE, DWORD, PVMMDLL_PROCESS_INFORMATION, PSIZE_T);

// Global function pointers
HMODULE g_hVmmDll = NULL;
VMMDLL_Initialize_t g_VMMDLL_Initialize = NULL;
VMMDLL_Close_t g_VMMDLL_Close = NULL;
VMMDLL_MemFree_t g_VMMDLL_MemFree = NULL;
VMMDLL_MemRead_t g_VMMDLL_MemRead = NULL;
VMMDLL_Map_GetModuleU_t g_VMMDLL_Map_GetModuleU = NULL;
VMMDLL_PidList_t g_VMMDLL_PidList = NULL;
VMMDLL_ProcessGetInformation_t g_VMMDLL_ProcessGetInformation = NULL;

// Load vmm.dll and get function pointers
bool LoadVmmDll() {
    g_hVmmDll = LoadLibraryA("vmm.dll");
    if (!g_hVmmDll) {
        return false;
    }
    
    g_VMMDLL_Initialize = (VMMDLL_Initialize_t)GetProcAddress(g_hVmmDll, "VMMDLL_Initialize");
    g_VMMDLL_Close = (VMMDLL_Close_t)GetProcAddress(g_hVmmDll, "VMMDLL_Close");
    g_VMMDLL_MemFree = (VMMDLL_MemFree_t)GetProcAddress(g_hVmmDll, "VMMDLL_MemFree");
    g_VMMDLL_MemRead = (VMMDLL_MemRead_t)GetProcAddress(g_hVmmDll, "VMMDLL_MemRead");
    g_VMMDLL_Map_GetModuleU = (VMMDLL_Map_GetModuleU_t)GetProcAddress(g_hVmmDll, "VMMDLL_Map_GetModuleU");
    g_VMMDLL_PidList = (VMMDLL_PidList_t)GetProcAddress(g_hVmmDll, "VMMDLL_PidList");
    g_VMMDLL_ProcessGetInformation = (VMMDLL_ProcessGetInformation_t)GetProcAddress(g_hVmmDll, "VMMDLL_ProcessGetInformation");
    
    if (!g_VMMDLL_Initialize || !g_VMMDLL_Close || !g_VMMDLL_MemFree || 
        !g_VMMDLL_MemRead || !g_VMMDLL_Map_GetModuleU || !g_VMMDLL_PidList || 
        !g_VMMDLL_ProcessGetInformation) {
        FreeLibrary(g_hVmmDll);
        g_hVmmDll = NULL;
        return false;
    }
    
    return true;
}

void UnloadVmmDll() {
    if (g_hVmmDll) {
        FreeLibrary(g_hVmmDll);
        g_hVmmDll = NULL;
    }
}

// Wrapper macros to use loaded functions
#define VMMDLL_Initialize g_VMMDLL_Initialize
#define VMMDLL_Close g_VMMDLL_Close
#define VMMDLL_MemFree g_VMMDLL_MemFree
#define VMMDLL_MemRead g_VMMDLL_MemRead
#define VMMDLL_Map_GetModuleU g_VMMDLL_Map_GetModuleU
#define VMMDLL_PidList g_VMMDLL_PidList
#define VMMDLL_ProcessGetInformation g_VMMDLL_ProcessGetInformation
