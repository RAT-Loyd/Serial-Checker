#pragma once
#include "Paint.h"
#include "Global.h"

#include <windows.h>
#include <string>
#include <iostream>
#include <array>
#ifdef _WIN32
#include <stdio.h>
#define popen _popen
#define pclose _pclose
#else
#include <stdio.h>
#endif

class serials{
public:
    static void getDiskserials();
    static void getDiskvolume();
    static void getSMbios();
    static void getMotherBoardserial();
    static void getMotherBoarduuid();
    static void getMotherBoardIdentifyingNumber();
    static void getMac();
    static void getCpu();
    static void getMemory();
    static void getTPM();
    static void getGPU();
    static void getCpuProc();
    static void writeSerialsToFile();
    static void writeSerialsToRegistry();
};

