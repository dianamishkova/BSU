﻿#include <Windows.h>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <fstream>

int main(int argc, char* argv[])
{
    if (argc == 2) {
        setlocale(LC_ALL, "Russian");
        HANDLE mutex = OpenMutexA(SYNCHRONIZE, FALSE, "SenderWriteMutex");
        HANDLE busyEvent = OpenEventA(EVENT_MODIFY_STATE, FALSE, argv[1]);
        HANDLE startupEvent = OpenEventA(SYNCHRONIZE, FALSE, "SenderStartupEvent");
        std::ofstream fout(argv[0], std::ios_base::binary | std::ios_base::out | std::ios_base::app);
        std::string message;
        SignalObjectAndWait(busyEvent, startupEvent, INFINITE, FALSE);
        while (true) {
            std::cout << "Введите сообщение для передачи или введите 0 для прекращения работы.\n";
            std::cin >> message;
            if (message.compare("0") == 0)
                break; 
            WaitForSingleObject(mutex, INFINITE);
            fout << message << std::endl;  
            if(WaitForSingleObject(startupEvent, 0) == WAIT_TIMEOUT)
                SetEvent(busyEvent);
            ReleaseMutex(mutex);
        }
        CloseHandle(startupEvent);
        CloseHandle(mutex);
        fout.close();
        CloseHandle(busyEvent);
    }
    return 0;
}
