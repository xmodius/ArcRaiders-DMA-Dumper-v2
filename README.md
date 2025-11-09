# Arc Raiders DMA Offset Dumper v2.0

**One-command setup and execution for dumping Arc Raiders offsets using DMA hardware.**

---

## 🚀 Quick Start (Single Command)

### Step 1: Clone the Repository

```cmd
git clone https://github.com/xmodius/ArcRaiders-DMA-Dumper-v2.git
cd ArcRaiders-DMA-Dumper-v2
```

### Step 2: Run Setup Script

Open **Developer Command Prompt for VS 2022** and run:

```cmd
setup_and_run.bat
```

**That's it!** The script will:
- ✅ Compile the dumper
- ✅ Copy DMA DLLs automatically
- ✅ Wait for you to launch Arc Raiders
- ✅ Run the dumper and generate offsets

---

## 📋 Requirements

- **DMA Hardware**: ScreamerM2 or compatible FPGA DMA device
- **Visual Studio 2022**: Developer Command Prompt
- **Arc Raiders**: Game must be running in a match
- **DMA DLLs**: Located at `C:\Users\enfui\Desktop\lones-DMA-speed-test-main\lones-DMA-speed-test-main\`

---

## 🔧 Manual Setup (If Needed)

If the automatic script doesn't work, follow these steps:

### 1. Compile
```cmd
cl.exe /EHsc /std:c++17 /O2 /Fe:ArcRaiders_DMA_Dumper.exe main.cpp Advapi32.lib Shell32.lib
```

### 2. Copy DMA DLLs
```cmd
copy "C:\Users\enfui\Desktop\lones-DMA-speed-test-main\lones-DMA-speed-test-main\*.dll" .
```

### 3. Run
```cmd
ArcRaiders_DMA_Dumper.exe
```

---

## 📁 Output Files

After successful execution:

- **offsets.txt** - Human-readable offset dump
- **Offsets_Dumped.h** - C++ header file for integration

---

## ⚠️ Troubleshooting

### "cl.exe not found"
- Run from **Developer Command Prompt for VS 2022**, not regular CMD

### "Failed to initialize DMA"
- Check DMA device is connected
- Verify DMA drivers are installed
- Test with your DMA speed test first

### "Arc Raiders is not running"
- Launch the game
- Get into an actual match (not menu)
- Wait 15 seconds after match starts

### "Failed to get process list"
- DMA might need a moment to scan
- Wait a few seconds and try again
- Make sure game is fully loaded

---

## 🔥 How It Works

1. **Initializes DMA** using MemProcFS with FPGA device
2. **Finds PioneerGame.exe** via DMA process enumeration
3. **Gets module base address** using DMA module mapping
4. **Reads memory** directly through DMA hardware (bypasses EAC)
5. **Scans for offsets** including GWorld, GNames, and structures
6. **Generates output files** ready for integration

---

## 📞 Support

If you encounter issues:
1. Make sure your DMA speed test works first
2. Verify all DMA DLLs are present
3. Check that Arc Raiders is running in a match
4. Try running the dumper 2-3 times (DMA can be temperamental)

---

## 🎯 Next Steps

Once you have the offsets:
1. Integrate `Offsets_Dumped.h` into your main DMA project
2. Implement GWorld/GNames decryption
3. Build your DMA cheat features
4. Test on alt account

---

## Credits

- **MemProcFS**: Ulf Frisk
- **Offset Research**: UnknownCheats community
- **DMA Hardware**: ScreamerM2

---

## Disclaimer

Educational and research purposes only. Use at your own risk.
