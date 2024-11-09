#include "SerialCheck.h"
#include "Paint.h"

#include <string>
#include <iostream>
#include <fstream>
#include <thread>
#include <sstream>
#include <Windows.h>

using namespace std;

void SerialCheck()
{
    system("cls");
    serials::getDiskserials();
    serials::getDiskvolume();
    serials::getSMbios();
    serials::getMotherBoardserial();
    serials::getMotherBoarduuid();
    serials::getMotherBoardIdentifyingNumber();
    serials::getCpu();
    serials::getCpuProc();
    serials::getMemory();
    serials::getGPU();
    serials::getTPM();
    serials::getMac();
    serials::writeSerialsToFile();
    serials::writeSerialsToRegistry();
    print::set_text("\nPress any key to reload\n", Yellow);


    system("pause>nul");

};

int main() {
    SetConsoleTitleA("Simple serial checker | @le4k_");


    while (true) {
        system("cls");
        system("mode con: cols=120 lines=63");
        SerialCheck();

    }


}
