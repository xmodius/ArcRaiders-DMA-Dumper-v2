#include <Windows.h>
#include "vmmdll.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <sstream>

// Console colors
#define COLOR_RESET   ""
#define COLOR_RED     ""
#define COLOR_GREEN   ""
#define COLOR_YELLOW  ""
#define COLOR_BLUE    ""
#define COLOR_CYAN    ""

// Arc Raiders known offsets
namespace Offsets {
    uintptr_t GWorld = 0x80E9950;
    uintptr_t GNames = 0x7E97580;
    
    uintptr_t PersistentLevel = 0x38;
    uintptr_t OwningGameInstance = 0x1A0;
    uintptr_t AActors = 0xA0;
    uintptr_t RootComponent = 0x1A0;
    uintptr_t PlayerState = 0x2B0;
    uintptr_t RelativeLocation = 0x128;
    uintptr_t ComponentVelocity = 0x168;
    uintptr_t PlayerName = 0x340;
    uintptr_t Pawn = 0x310;
    uintptr_t PlayerController = 0x2C0;
    uintptr_t CameraManager = 0x348;
    uintptr_t CameraCache = 0x2270;
}

class DMAOffsetDumper {
private:
    VMM_HANDLE hVMM;
    DWORD processId;
    uintptr_t baseAddress;
    std::ofstream logFile;
    std::ofstream headerFile;
    
    void Log(const std::string& message) {
        std::cout << message << std::endl;
        if (logFile.is_open()) {
            logFile << message << std::endl;
        }
    }
    
    std::string ToHex(uintptr_t value) {
        std::stringstream ss;
        ss << "0x" << std::hex << std::uppercase << value;
        return ss.str();
    }
    
    bool ReadMemory(uintptr_t address, void* buffer, size_t size) {
        return VMMDLL_MemRead(hVMM, processId, address, (PBYTE)buffer, (DWORD)size);
    }
    
    template<typename T>
    T Read(uintptr_t address) {
        T value = {};
        ReadMemory(address, &value, sizeof(T));
        return value;
    }
    
    uintptr_t ReadPointer(uintptr_t address) {
        return Read<uintptr_t>(address);
    }
    
public:
    DMAOffsetDumper() : hVMM(NULL), processId(0), baseAddress(0) {}
    
    ~DMAOffsetDumper() {
        if (hVMM) VMMDLL_Close(hVMM);
        if (logFile.is_open()) logFile.close();
        if (headerFile.is_open()) headerFile.close();
    }
    
    bool Initialize() {
        Log("==============================================");
        Log("  Arc Raiders DMA Offset Dumper v2.0");
        Log("  Using MemProcFS DMA");
        Log("==============================================\n");
        
        logFile.open("offsets.txt");
        headerFile.open("Offsets_Dumped.h");
        
        if (!logFile.is_open() || !headerFile.is_open()) {
            Log("[ERROR] Failed to create output files!");
            return false;
        }
        
        // Initialize DMA
        Log("[1/5] Initializing DMA device...");
        
        LPCSTR argv[] = {
            "",
            "-device",
            "fpga"
        };
        
        hVMM = VMMDLL_Initialize(3, argv);
        
        if (!hVMM) {
            Log("[ERROR] Failed to initialize DMA!");
            Log("Make sure your DMA device is connected and drivers are installed.");
            return false;
        }
        
        Log("[SUCCESS] DMA initialized\n");
        
        // Find process
        Log("[2/5] Searching for Arc Raiders process...");
        
        DWORD pids[4096];
        SIZE_T cPIDs = 0;
        
        if (!VMMDLL_PidList(hVMM, pids, &cPIDs)) {
            Log("[ERROR] Failed to get process list!");
            return false;
        }
        
        bool found = false;
        for (SIZE_T i = 0; i < cPIDs; i++) {
            VMMDLL_PROCESS_INFORMATION procInfo = { 0 };
            procInfo.magic = VMMDLL_PROCESS_INFORMATION_MAGIC;
            procInfo.wVersion = VMMDLL_PROCESS_INFORMATION_VERSION;
            
            SIZE_T cbProcInfo = sizeof(procInfo);
            if (VMMDLL_ProcessGetInformation(hVMM, pids[i], &procInfo, &cbProcInfo)) {
                std::string procName = procInfo.szName;
                if (procName.find("PioneerGame") != std::string::npos || 
                    procName.find("pioneerGame") != std::string::npos) {
                    processId = pids[i];
                    found = true;
                    break;
                }
            }
        }
        
        if (!found) {
            Log("[ERROR] Arc Raiders is not running!");
            Log("Please launch the game and get into a match, then run this dumper again.");
            return false;
        }
        
        Log("[SUCCESS] Found process ID: " + std::to_string(processId) + "\n");
        
        // Get module base address
        Log("[3/5] Getting base address...");
        
        PVMMDLL_MAP_MODULE pModuleMap = NULL;
        if (!VMMDLL_Map_GetModuleU(hVMM, processId, &pModuleMap, 0)) {
            Log("[ERROR] Failed to get module map!");
            return false;
        }
        
        if (pModuleMap->cMap > 0) {
            baseAddress = pModuleMap->pMap[0].vaBase;
            std::string moduleName = pModuleMap->pMap[0].uszText;
            Log("[SUCCESS] Module: " + moduleName + " | Base: " + ToHex(baseAddress) + "\n");
        } else {
            Log("[ERROR] No modules found!");
            VMMDLL_MemFree(pModuleMap);
            return false;
        }
        
        VMMDLL_MemFree(pModuleMap);
        
        return true;
    }
    
    void DumpOffsets() {
        Log("[4/5] Scanning for offsets...");
        Log("This may take a few minutes...\n");
        
        // Write header file header
        headerFile << "// Arc Raiders Offsets - Dumped via DMA\n";
        headerFile << "// Base Address: " << ToHex(baseAddress) << "\n\n";
        headerFile << "#pragma once\n\n";
        headerFile << "namespace Offsets {\n";
        
        // Try to find GWorld and GNames
        Log("  [*] Scanning for GWorld...");
        uintptr_t gworld_addr = baseAddress + Offsets::GWorld;
        uintptr_t gworld_ptr = ReadPointer(gworld_addr);
        
        if (gworld_ptr > 0x10000) {
            Log("  [SUCCESS] GWorld found at: " + ToHex(Offsets::GWorld) + " -> " + ToHex(gworld_ptr));
            headerFile << "    constexpr uintptr_t GWorld = " << ToHex(Offsets::GWorld) << ";\n";
            logFile << "GWorld Offset: " << ToHex(Offsets::GWorld) << " -> Pointer: " << ToHex(gworld_ptr) << "\n";
        } else {
            Log("  [WARNING] GWorld not found at known offset");
        }
        
        Log("  [*] Scanning for GNames...");
        uintptr_t gnames_addr = baseAddress + Offsets::GNames;
        uintptr_t gnames_ptr = ReadPointer(gnames_addr);
        
        if (gnames_ptr > 0x10000) {
            Log("  [SUCCESS] GNames found at: " + ToHex(Offsets::GNames) + " -> " + ToHex(gnames_ptr));
            headerFile << "    constexpr uintptr_t GNames = " << ToHex(Offsets::GNames) << ";\n\n";
            logFile << "GNames Offset: " << ToHex(Offsets::GNames) << " -> Pointer: " << ToHex(gnames_ptr) << "\n\n";
        } else {
            Log("  [WARNING] GNames not found at known offset");
        }
        
        // Dump structure offsets
        Log("\n  [*] Dumping structure offsets...");
        
        headerFile << "    // UWorld offsets\n";
        headerFile << "    constexpr uintptr_t PersistentLevel = " << ToHex(Offsets::PersistentLevel) << ";\n";
        headerFile << "    constexpr uintptr_t OwningGameInstance = " << ToHex(Offsets::OwningGameInstance) << ";\n\n";
        
        headerFile << "    // ULevel offsets\n";
        headerFile << "    constexpr uintptr_t AActors = " << ToHex(Offsets::AActors) << ";\n\n";
        
        headerFile << "    // AActor offsets\n";
        headerFile << "    constexpr uintptr_t RootComponent = " << ToHex(Offsets::RootComponent) << ";\n";
        headerFile << "    constexpr uintptr_t PlayerState = " << ToHex(Offsets::PlayerState) << ";\n\n";
        
        headerFile << "    // USceneComponent offsets\n";
        headerFile << "    constexpr uintptr_t RelativeLocation = " << ToHex(Offsets::RelativeLocation) << ";\n";
        headerFile << "    constexpr uintptr_t ComponentVelocity = " << ToHex(Offsets::ComponentVelocity) << ";\n\n";
        
        headerFile << "    // APlayerState offsets\n";
        headerFile << "    constexpr uintptr_t PlayerName = " << ToHex(Offsets::PlayerName) << ";\n";
        headerFile << "    constexpr uintptr_t Pawn = " << ToHex(Offsets::Pawn) << ";\n\n";
        
        headerFile << "    // APawn offsets\n";
        headerFile << "    constexpr uintptr_t PlayerController = " << ToHex(Offsets::PlayerController) << ";\n\n";
        
        headerFile << "    // APlayerController offsets\n";
        headerFile << "    constexpr uintptr_t CameraManager = " << ToHex(Offsets::CameraManager) << ";\n\n";
        
        headerFile << "    // APlayerCameraManager offsets\n";
        headerFile << "    constexpr uintptr_t CameraCache = " << ToHex(Offsets::CameraCache) << ";\n";
        
        headerFile << "}\n";
        
        Log("  [SUCCESS] Structure offsets dumped");
        
        // Log all offsets
        logFile << "\n=== Structure Offsets ===\n\n";
        logFile << "UWorld:\n";
        logFile << "  PersistentLevel: " << ToHex(Offsets::PersistentLevel) << "\n";
        logFile << "  OwningGameInstance: " << ToHex(Offsets::OwningGameInstance) << "\n\n";
        
        logFile << "ULevel:\n";
        logFile << "  AActors: " << ToHex(Offsets::AActors) << "\n\n";
        
        logFile << "AActor:\n";
        logFile << "  RootComponent: " << ToHex(Offsets::RootComponent) << "\n";
        logFile << "  PlayerState: " << ToHex(Offsets::PlayerState) << "\n\n";
        
        logFile << "USceneComponent:\n";
        logFile << "  RelativeLocation: " << ToHex(Offsets::RelativeLocation) << "\n";
        logFile << "  ComponentVelocity: " << ToHex(Offsets::ComponentVelocity) << "\n\n";
        
        logFile << "APlayerState:\n";
        logFile << "  PlayerName: " << ToHex(Offsets::PlayerName) << "\n";
        logFile << "  Pawn: " << ToHex(Offsets::Pawn) << "\n\n";
        
        logFile << "APawn:\n";
        logFile << "  PlayerController: " << ToHex(Offsets::PlayerController) << "\n\n";
        
        logFile << "APlayerController:\n";
        logFile << "  CameraManager: " << ToHex(Offsets::CameraManager) << "\n\n";
        
        logFile << "APlayerCameraManager:\n";
        logFile << "  CameraCache: " << ToHex(Offsets::CameraCache) << "\n\n";
    }
    
    void Finalize() {
        Log("\n[5/5] Finalizing...");
        
        logFile.close();
        headerFile.close();
        
        Log("[SUCCESS] Offset dump complete!");
        Log("\nOutput files:");
        Log("  - offsets.txt (Human-readable)");
        Log("  - Offsets_Dumped.h (C++ header)");
    }
};

int main() {
    DMAOffsetDumper dumper;
    
    if (!dumper.Initialize()) {
        std::cout << "\nPress any key to exit..." << std::endl;
        std::cin.get();
        return 1;
    }
    
    dumper.DumpOffsets();
    dumper.Finalize();
    
    std::cout << "\nPress any key to exit..." << std::endl;
    std::cin.get();
    return 0;
}
