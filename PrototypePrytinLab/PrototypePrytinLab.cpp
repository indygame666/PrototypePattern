// PrototypePrytinLab.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include "pch.h"
#include <iostream> 
#include <vector> 
#include <stdio.h> 
#include <Windows.h> 
#include <winternl.h> 
#include "TCHAR.h" 
#include "pdh.h" 


using namespace std;

#pragma comment(lib,"ntdll.lib") 

static ULARGE_INTEGER lastCPU, lastSysCPU, lastUserCPU;
static int numProcessors;
static HANDLE self;

typedef struct _SYSTEM_PROCESS_INFO
{
	ULONG NextEntryOffset;
	ULONG NumberOfThreads;
	LARGE_INTEGER Reserved[3];
	LARGE_INTEGER CreateTime;
	LARGE_INTEGER UserTime;
	LARGE_INTEGER KernelTime;
	UNICODE_STRING ImageName;
	ULONG BasePriority;
	HANDLE ProcessId;
	HANDLE InheritedFromProcessId;
}SYSTEM_PROCESS_INFO, *PSYSTEM_PROCESS_INFO;

///////////////////////////////////////////////////////////////////////

class Prototype
{
public:
	int id;
	Prototype() {};
	virtual Prototype* clone() = 0;
	virtual int printStatus() = 0;
	virtual ~Prototype() {}
	Prototype(int _id)
	{
		id = _id;
	}
};

class ProcessStatus : public Prototype
{
public:
	int id;
	ProcessStatus() {};
	ProcessStatus(int _id) { id = _id; };

	Prototype* clone() {
		return new ProcessStatus(id);
	}

	int printStatus()
	{
		NTSTATUS status;
		PVOID buffer;
		PSYSTEM_PROCESS_INFO spi;

		buffer = VirtualAlloc(NULL, 1024 * 1024, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE); // We need to allocate a large buffer because the process list can be large. 

		if (!buffer)
		{
			printf("\nError: Unable to allocate memory for process list (%d)\n", GetLastError());
			return -1;
		}

		printf("\nProcess list allocated at address %#x\n", buffer);
		spi = (PSYSTEM_PROCESS_INFO)buffer;

		if (!NT_SUCCESS(status = NtQuerySystemInformation(SystemProcessInformation, spi, 1024 * 1024, NULL)))
		{
			printf("\nError: Unable to query process list (%#x)\n", status);

			VirtualFree(buffer, 0, MEM_RELEASE);
			return -1;
		}

		while (spi->NextEntryOffset) // Loop over the list until we reach the last entry. 
		{
			printf("\nProcess name: %ws | Process ID: %d\n", spi->ImageName.Buffer, spi->ProcessId); // Display process information. 
			spi = (PSYSTEM_PROCESS_INFO)((LPBYTE)spi + spi->NextEntryOffset); // Calculate the address of the next entry. 
		}

		printf("\nPress any key to continue.\n");
		getchar();

		VirtualFree(buffer, 0, MEM_RELEASE); // Free the allocated buffer. 
	}
};


int main()
{	Prototype* SampleExample;	SampleExample = new ProcessStatus(1);	SampleExample->printStatus();
	system("pause");

}