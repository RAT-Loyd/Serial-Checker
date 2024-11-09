#include "SerialCheck.h"

#include <windows.h>
#include <windows.h>
#include <array>
#include <memory>
#include <stdexcept>
#include <string>
#include <regex>
#include <vector>
#include <fstream>
#include <iostream>
#include <ctime>
#include <iomanip>
#include <sstream>
const std::string registryPath = "SOFTWARE\\Serials\\HWIDInfo";

std::string disk_serial_number;
std::string disk_volume_id;
std::string smbios_serial;
std::string motherboard_serial;
std::string motherboard_uuid;
std::string motherboard_identifying_number;
std::string cpu_serial;
std::string cpu_processor_id;
std::string memory_serial;
std::string gpu_uuid;
std::string mac_address;
std::string MD5;
std::string SHA1;
std::string SHA256;
std::string error;

std::string readStringFromRegistry(const std::string& name)
{
    HKEY hKey;
    std::string value;
    if (RegOpenKeyExA(HKEY_CURRENT_USER, registryPath.c_str(), 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        char buffer[256];
        DWORD bufferSize = sizeof(buffer);
        if (RegQueryValueExA(hKey, name.c_str(), 0, NULL, reinterpret_cast<BYTE*>(buffer), &bufferSize) == ERROR_SUCCESS) {
            value.assign(buffer, bufferSize - 1);
        }
        RegCloseKey(hKey);
    }
    return value;
}

void displayWithRegistryCheck(const std::string& registryValue, const std::string& currentValue, const char* title)
{
    print::set_text(title, LightRed);

    if (registryValue != currentValue) {
        print::set_text(currentValue.c_str(), LightGreen);
    }
    else {
        print::set_text(currentValue.c_str(), White);
    }
}


std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(_popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("_popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

void serials::getDiskserials()
{
    disk_serial_number = exec("powershell -command \"Get-CimInstance -ClassName Win32_DiskDrive | Select-Object -ExpandProperty SerialNumber\"");
    std::string registryValue = readStringFromRegistry("Disk Serial Number");
    displayWithRegistryCheck(registryValue, disk_serial_number, "[Disk Serial Number]\n\n");
}

void serials::getDiskvolume()
{
    disk_volume_id = exec("vol");
    std::string registryValue = readStringFromRegistry("Disk Volume ID");
    displayWithRegistryCheck(registryValue, disk_volume_id, "\n[Disk VolumeID]\n\n");
}

void serials::getSMbios()
{
    smbios_serial = exec("powershell -command \"(Get-WmiObject -Class Win32_BIOS).SerialNumber\"");
    std::string registryValue = readStringFromRegistry("SMBIOS Serial");
    displayWithRegistryCheck(registryValue, smbios_serial, "\n[SMBIOS]\n\n");
}

void serials::getMotherBoardserial()
{
    motherboard_serial = exec("powershell -command \"(Get-WmiObject -Class Win32_BaseBoard).SerialNumber\"");
    std::string registryValue = readStringFromRegistry("Motherboard Serial");
    displayWithRegistryCheck(registryValue, motherboard_serial, "\n[MotherBoard Serial]\n\n");
}

void serials::getMotherBoarduuid()
{
    motherboard_uuid = exec("powershell -command \"(Get-WmiObject -Class Win32_ComputerSystemProduct).UUID\"");
    std::string registryValue = readStringFromRegistry("Motherboard UUID");
    displayWithRegistryCheck(registryValue, motherboard_uuid, "\n[MotherBoard UUID]\n\n");
}

void serials::getMotherBoardIdentifyingNumber()
{
    motherboard_identifying_number = exec("powershell -command \"(Get-WmiObject -Class Win32_ComputerSystemProduct).IdentifyingNumber\"");
    std::string registryValue = readStringFromRegistry("Motherboard Identifying Number");
    displayWithRegistryCheck(registryValue, motherboard_identifying_number, "\n[MotherBoard Identifying Number]\n\n");
}

void serials::getCpu()
{
    cpu_serial = exec("powershell -command \"(Get-WmiObject -Class Win32_Processor).SerialNumber\"");
    std::string registryValue = readStringFromRegistry("CPU Serial");
    displayWithRegistryCheck(registryValue, cpu_serial, "\n[CPU Serial]\n\n");
}

void serials::getCpuProc()
{
    cpu_processor_id = exec("powershell -command \"(Get-WmiObject -Class Win32_Processor).ProcessorId\"");
    std::string registryValue = readStringFromRegistry("CPU Processor ID");
    displayWithRegistryCheck(registryValue, cpu_processor_id, "\n- Processor Id -\n\n");
}

void serials::getMemory()
{
    memory_serial = exec("powershell -command \"(Get-WmiObject -Class Win32_PhysicalMemory | Select-Object -ExpandProperty SerialNumber)\"");
    std::string registryValue = readStringFromRegistry("Memory Serial");
    displayWithRegistryCheck(registryValue, memory_serial, "\n[Memory Serial]\n\n");
}

void serials::getTPM()
{
    print::set_text("\n[TPM module]\n", LightRed);
    std::vector<BYTE> ekData = Utils::GetEK();
    if (ekData.empty())
    {
        error = "Failed to retrieve EK\n";
        std::string registryValue = readStringFromRegistry("TPM Error");
        displayWithRegistryCheck(registryValue, error, "\n");
    }
    else
    {
        MD5 = Utils::GetKeyHash(ekData, CALG_MD5);
        SHA1 = Utils::GetKeyHash(ekData, CALG_SHA1);
        SHA256 = Utils::GetKeyHash(ekData, CALG_SHA_256);

        displayWithRegistryCheck(readStringFromRegistry("TPM MD5"), MD5, "\n< MD5 >\n");
        displayWithRegistryCheck(readStringFromRegistry("TPM SHA1"), SHA1, "\n< SHA1 >\n");
        displayWithRegistryCheck(readStringFromRegistry("TPM SHA256"), SHA256, "\n< SHA256 >\n");
    }
}

void serials::getGPU()
{
    gpu_uuid = exec("nvidia-smi -L 2>&1");
    if (gpu_uuid.empty() || gpu_uuid.find("NVIDIA") == std::string::npos) {
        print::set_text("Please install NVIDIA drivers ......... r u using radeon? fuck u man\n", LightGreen);
    }
    else {
        std::string toErase = "GPU 0: ";
        size_t pos = std::string::npos;
        while ((pos = gpu_uuid.find(toErase)) != std::string::npos) {
            gpu_uuid.erase(pos, toErase.length());
        }
        std::string registryValue = readStringFromRegistry("GPU UUID");
        displayWithRegistryCheck(registryValue, gpu_uuid, "\n[GPU UUID]\n\n");
    }
}

void serials::getMac()
{
    mac_address = exec("powershell -command \"(Get-NetAdapter | Where-Object { $_.Status -eq 'Up' }).MacAddress\"");
    std::string registryValue = readStringFromRegistry("MAC Address");
    displayWithRegistryCheck(registryValue, mac_address, "\n[Mac address]\n\n");
}

std::string generateFilenameWithTimestamp()
{
    auto t = std::time(nullptr);
    std::tm tm;
    localtime_s(&tm, &t);

    std::ostringstream oss;
    oss << "HWIDInfo-"
        << std::put_time(&tm, "%m-%d-%H-%M-%S") 
        << ".txt";

    return oss.str();
}

void serials::writeSerialsToFile()
{
    std::string filename = generateFilenameWithTimestamp();
    std::ofstream file(filename);
    if (file.is_open()) {
        if (disk_serial_number.empty()) {
            disk_serial_number = "Null";
        }
        file << "[Disk Serial Number]\n\n" << disk_serial_number << "\n";
        file << "[Disk Volume ID]\n\n" << disk_volume_id << "\n";
        file << "[SMBIOS Serial]\n\n" << smbios_serial << "\n";
        file << "[Motherboard Serial]\n\n" << motherboard_serial << "\n";
        file << "[Motherboard UUID]\n\n" << motherboard_uuid << "\n";
        file << "[Motherboard Identifying Number]\n\n" << motherboard_identifying_number << "\n";
        file << "[CPU Serial]\n\n" << cpu_serial << "\n";
        file << "[CPU Processor ID]\n\n" << cpu_processor_id << "\n";
        file << "[Memory Serial]\n\n" << memory_serial << "\n";
        file << "[GPU UUID]\n\n" << gpu_uuid << "\n";
        file << "[TPM module]\n\n";
        std::vector<BYTE> ekData = Utils::GetEK();
        if (ekData.empty())
        {
            file << error << "\n\n";
        }
        else
        {
            file << "< MD5 >\n\n" << MD5 << "\n";
            file << "< SHA1 >\n\n" << SHA1 << "\n";
            file << "< SHA256 >\n\n" << SHA256 << "\n";
        }
        file << "[MAC Address]\n\n" << mac_address << "\n";
        file.close();
    }

}

void serials::writeSerialsToRegistry()
{
    HKEY hKey;
    if (RegCreateKeyExA(HKEY_CURRENT_USER, registryPath.c_str(), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL) == ERROR_SUCCESS) {

        auto writeStringToRegistry = [&](const std::string& name, const std::string& value) {
            RegSetValueExA(hKey, name.c_str(), 0, REG_SZ, reinterpret_cast<const BYTE*>(value.c_str()), value.size() + 1);
            };

        writeStringToRegistry("Disk Serial Number", disk_serial_number);
        writeStringToRegistry("Disk Volume ID", disk_volume_id);
        writeStringToRegistry("SMBIOS Serial", smbios_serial);
        writeStringToRegistry("Motherboard Serial", motherboard_serial);
        writeStringToRegistry("Motherboard UUID", motherboard_uuid);
        writeStringToRegistry("Motherboard Identifying Number", motherboard_identifying_number);
        writeStringToRegistry("CPU Serial", cpu_serial);
        writeStringToRegistry("CPU Processor ID", cpu_processor_id);
        writeStringToRegistry("Memory Serial", memory_serial);
        writeStringToRegistry("GPU UUID", gpu_uuid);
        writeStringToRegistry("MAC Address", mac_address);

        if (!MD5.empty()) {
            writeStringToRegistry("TPM MD5", MD5);
            writeStringToRegistry("TPM SHA1", SHA1);
            writeStringToRegistry("TPM SHA256", SHA256);
        }
        else {
            writeStringToRegistry("TPM Error", error);
        }

        RegCloseKey(hKey);
    }
}