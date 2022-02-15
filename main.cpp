#include <iostream>
#include <windows.h>
#include <tlhelp32.h>
#include <tchar.h>
#include <vector>
#include <thread>
#include <ctime>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <stdio.h>
#include <comdef.h>
using namespace std;

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "advapi32.lib")


//used for getting memory info
BOOL SetPrivilege(HANDLE hToken, LPCTSTR lpszPrivilege, BOOL bEnablePrivilege)
{
	TOKEN_PRIVILEGES tp;
	LUID luid;

	if (!LookupPrivilegeValue(NULL, lpszPrivilege, &luid)) {
		printf("LookupPrivilegeValue error: %u\n", GetLastError() );
		return FALSE;
	}

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	if (bEnablePrivilege)
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	else
		tp.Privileges[0].Attributes = 0;

	if (!AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), (PTOKEN_PRIVILEGES)NULL, (PDWORD)NULL)) {
		//printf("AdjustTokenPrivileges error: %u\n", GetLastError() );
		return FALSE;
	}

	if (GetLastError() == ERROR_NOT_ALL_ASSIGNED) {
		//printf("The token does not have the specified privilege. \n");
		return FALSE;
	}

	return TRUE;
}

BOOL GetDebugPrivileges(void) {
	HANDLE hToken = NULL;
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken))
		return FALSE; //std::cout << "OpenProcessToken() failed, error\n>> " << GetLastError() << std::endl;
	//else std::cout << "OpenProcessToken() is OK, got the handle!" << std::endl;

	if (!SetPrivilege(hToken, SE_DEBUG_NAME, TRUE))
		return FALSE; //std::cout << "Failed to enable privilege, error:\n>> " << GetLastError() << std::endl;

	return TRUE;
}

string ReadMemory(HANDLE processHandle, int address) {
	string output = "";
	char ne = 'n';
	if (address == -1)
		return &ne;
	char buffer = !0;
	char* stringToRead = new char[128];
	SIZE_T NumberOfBytesToRead = sizeof(buffer);
	SIZE_T NumberOfBytesActuallyRead;
	int i = 0;
	do  {
		BOOL success = ReadProcessMemory(processHandle, (LPCVOID)address, &buffer, NumberOfBytesToRead, &NumberOfBytesActuallyRead);
		if (!success || NumberOfBytesActuallyRead != NumberOfBytesToRead)
			return &ne;
		stringToRead[i] = buffer;
		i++;
		address++;
		output+=buffer;
	}while (buffer != 0 && buffer != '\n' && i < 50);
	
	return output.substr(0,output.length()-2);
}

string readProcess(const char* process)
{
	string value = "N/A";
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (Process32First(snapshot, &entry) == TRUE)
	{
		
		while (Process32Next(snapshot, &entry) == TRUE)
		{
			
			_bstr_t b(entry.szExeFile);
			const char* c = b;
			
			if (_stricmp(c, process) == 0)
			{
				
				HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, entry.th32ProcessID);
				
				
				// Do stuff..
				GetDebugPrivileges();
				value = ReadMemory(hProcess, 0x82611C18);
				//cout << "memory value: " << value << "\n";
				
				CloseHandle(hProcess);
				break;
			}
		}
	}

	CloseHandle(snapshot);
	return value;
}



uint64_t timeSinceEpochMillisec() {
	using namespace std::chrono;
	return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

void LeftClick()
{
	INPUT    Input = { 0 };
	// left down 
	Input.type = INPUT_MOUSE;
	Input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
	::SendInput(1, &Input, sizeof(INPUT));

	// left up
	::ZeroMemory(&Input, sizeof(INPUT));
	Input.type = INPUT_MOUSE;
	Input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
	::SendInput(1, &Input, sizeof(INPUT));
}

void RightClick()
{
	INPUT    Input = { 0 };
	// right down 
	Input.type = INPUT_MOUSE;
	Input.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
	::SendInput(1, &Input, sizeof(INPUT));

	// right up
	::ZeroMemory(&Input, sizeof(INPUT));
	Input.type = INPUT_MOUSE;
	Input.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
	::SendInput(1, &Input, sizeof(INPUT));
}

void pressKey(int key)
{
	keybd_event(key, 0, 0, 0); //Press down the Key
	keybd_event(key, 0, KEYEVENTF_KEYUP, 0); //Release the Key
}

void pressDoubleKey(int key1, int key2)
{
	keybd_event(key1, 0, 0, 0); //Press down the Key
	keybd_event(key2, 0, 0, 0); //Press down the Key
	keybd_event(key1, 0, KEYEVENTF_KEYUP, 0); //Release the Keyv
	keybd_event(key2, 0, KEYEVENTF_KEYUP, 0); //Release the Key
}


const vector<int> keyCode = { 0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4A,0x4B,0x4C,0x4D,0x4E,0x4F,0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5A,VK_SPACE,VK_F20,VK_DECIMAL};
const vector<char> keys =   { '0' ,'1' ,'2' ,'3' ,'4' ,'5' ,'6' ,'7' ,'8' ,'9' ,'a' ,'b' ,'c' ,'d' ,'e' ,'f' ,'g' ,'h' ,'i' ,'j' ,'k' ,'l' ,'m' ,'n' ,'o' ,'p' ,'q' ,'r' ,'s' ,'t' ,'u' ,'v' ,'w' ,'x' ,'y' ,'z',' ','_','.'};
namespace key{
	const int enter = VK_F24; 
	const int slash = VK_F23;
	const int at = VK_F22;
	const int shiftClick = VK_F21;
	const int underLine = VK_F20;
	const int escape = VK_F18;
	const int menu = VK_F17;
	const int shiftQ = VK_F14;

}	
/*
	0x30 	0 key
	0x31 	1 key
	0x32 	2 key
	0x33 	3 key
	0x34 	4 key
	0x35 	5 key
	0x36 	6 key
	0x37 	7 key
	0x38 	8 key
	0x39 	9 key
	0x3A-40 	Undefined
	0x41 	A key
	0x42 	B key
	0x43 	C key
	0x44 	D key
	0x45 	E key
	0x46 	F key
	0x47 	G key
	0x48 	H key
	0x49 	I key
	0x4A 	J key
	0x4B 	K key
	0x4C 	L key
	0x4D 	M key
	0x4E 	N key
	0x4F 	O key
	0x50 	P key
	0x51 	Q key
	0x52 	R key
	0x53 	S key
	0x54 	T key
	0x55 	U key
	0x56 	V key
	0x57 	W key
	0x58 	X key
	0x59 	Y key
	0x5A 	Z key


*/

void sleep(long long duration)
{
	long long curTime = timeSinceEpochMillisec();
	while (timeSinceEpochMillisec() - curTime < duration)
	{
	}
}

void typeStr(string str,int delay = 10)
{
	
	for (int i = 0; i < str.length(); i++)
	{
		//find key
		for (int index = 0; index < keys.size();index++)
		{
			if (str[i] == keys[index])
			{ 
				//key found
				pressKey(keyCode[index]);
				sleep(delay);
				break;
			}else if (index==keys.size()-1)
			{
				//no key found
				return;
			}
		}
		
	}
}



namespace mc
{

	void sendCommand(string cmd, int prefixKey = key::slash)
	{
		
		cout << "\nRunning Command: " << cmd << "..." << endl; 
		pressKey(key::enter);
		sleep(100);
		pressKey(prefixKey);
		sleep(100);
		typeStr(cmd);
		pressKey(key::enter);
		cout << "Done.";
	}

	void craft(string item, int amount = 1, bool noTable = false)
	{
		/*
		type
		909 520

		select
		861 592

		output pixel
		1653 593


		*/
		SetCursorPos(909, 520);
		sleep(10);
		
		LeftClick();
		sleep(10);
		typeStr(item);
		sleep(10);
		SetCursorPos(861, 592);
		sleep(10);
		if (amount == -1)
		{
			pressKey(key::shiftClick);
			sleep(10);
		}
		for (int i = 0; i < amount; i++)
		{
			LeftClick();
			sleep(10);
		}
		if (noTable)
		{
			SetCursorPos(1736, 573);
		}else
		{
			SetCursorPos(1653, 593);
		}
		
		sleep(10);
		pressKey(key::shiftClick);
		


	}

}
void ahkThread()
{
	system("C:\\Users\\name\\source\\repos\\MinecraftBot\\ahk_rebinds\\rebinds.ahk");

}


void craftFurnace()
{
	
		/*
		type
		909 520furnace

		select
		861 592

		output pixel1
		1653 593


		*/
		sleep(100);
		pressKey(key::escape);
		sleep(100);
		pressKey(key::menu);
		sleep(100);
		SetCursorPos(1288, 917);
		sleep(10);
		pressKey(key::shiftClick);

		sleep(100);
		pressKey(key::escape);
		sleep(100);
		LeftClick();
		sleep(100);

		SetCursorPos(909, 520);
		sleep(10);

		LeftClick();
		sleep(10);
		typeStr("furnace");
		sleep(10);
		SetCursorPos(861, 592);
		sleep(10);
		

		LeftClick();
		sleep(10);
		SetCursorPos(1653, 893);
		sleep(10);
		LeftClick();
		sleep(20);
		SetCursorPos(1653, 593);

		sleep(20);
		pressKey(VK_F16);





}
void placeFurnace()
{
	
	pressKey(VK_F15);
}
void start()
{
	cout << "Mining logs";
	mc::sendCommand("mine 30 oak_log birch_log jungle_log spruce_log acacia_log dark_oak_log",key::at);
	sleep(100);
	while (readProcess("javaw.exe").find("[Baritone] Have") == string::npos)
	{
		//cout << "\"" << readProcess("javaw.exe") << "\"\n";
		cout << ".";
		sleep(1000);

	}
	cout << "\nDone.\n";
}

void craftStoneTools()
{
	mc::sendCommand("mine 30 stone", key::at);
	cout << "Mining stone";
	while (readProcess("javaw.exe").find("[Baritone] Have") == string::npos)
	{
		//cout << "\"" << readProcess("javaw.exe") << "\"\n";
		cout << ".";
		sleep(1000);
	}
	cout << "Done.";
	
	cout << "Pathing to table";
	sleep(100);
	mc::sendCommand("goto crafting_table", key::at);
	sleep(100);
	while (readProcess("javaw.exe").find("]: net.minecraft.class") == string::npos)
	{
		cout << ".";
		sleep(1000);
	}
	
	cout << "\nCrafting Stone Items.\n";

	cout << "Crafting Pickaxe...\n";

	mc::craft("stone pick", 3, false);

	//reopen inv
	sleep(100);
	pressKey(key::escape);
	sleep(100);
	LeftClick();
	sleep(100);

	cout << "Crafting Furnace...\n";
	craftFurnace();
	cout << "Done.\n";
	//reopen inv
	sleep(100);
	pressKey(key::escape);
	sleep(100);
	

	
	cout << "Done crafting stone items!\n";

}
//1297 920
//1352 920
//1296 860

void craftBasicTools()
{
	cout << "Crafting basic tools...\n";
	cout << "Placing crafting table...\n";
	mc::sendCommand("build craft.schem", key::at);
	while (readProcess("javaw.exe").find("[Baritone] Done") == string::npos)
	{
		cout << ".";
		sleep(1000);
	}
	cout << "Done.";
	cout << "Pathing to table";
	sleep(100);
	mc::sendCommand("goto crafting_table", key::at);
	sleep(100);
	while (readProcess("javaw.exe").find("]: net.minecraft.class") == string::npos)
	{
		cout << ".";
		sleep(1000);
	}
	cout << "Done.";
	sleep(100);
	cout << "Crafting wood pickaxe";
	mc::craft("wooden pick", 1);
	cout << "Done.\n";
	sleep(100);
	pressKey(key::escape);
	sleep(100);
	cout << "Done crafting.\n";
}

void craftBasics()
{
	cout << "Crafting Planks...\n";
	pressKey(key::menu);
	sleep(100);
	SetCursorPos(1356, 675);
	sleep(10);
	LeftClick();
	sleep(100);

	mc::craft("planks", -1, true);

	//reopen inv
	sleep(100);
	pressKey(key::escape);
	sleep(100);
	pressKey(key::menu);
	sleep(100);

	cout << "Crafting Sticks...\n";
	mc::craft("stick", 2, true);
	cout << "Done.\n";
	//reopen inv
	sleep(100);
	pressKey(key::escape);
	sleep(100);
	pressKey(key::menu);
	sleep(100);

	cout << "Crafting crafting table...\n";
	mc::craft("crafting", 1, true);
	cout << "Done.\n";
	sleep(100);
	pressKey(key::escape);
	sleep(100);
	cout << "Done crafting basics!\n";

}


//7 sticks
//34 iron
//6 planks

void multiCraft(vector<string> items)
{
	
	for (int i = 0; i < items.size(); i++)
	{

		mc::craft(items[i]);
		sleep(100);
		pressKey(key::escape);
		sleep(100);
		LeftClick();
		sleep(100);

	}

	cout << "Done crafting items.\n";


}

void craftIronItems2()
{
	//craft items
	pressKey(key::menu);
	sleep(100);
	cout << "Crafting crafting table...\n";
	mc::craft("crafting", 1, true);
	cout << "Done.\n";
	sleep(100);
	pressKey(key::escape);
	sleep(100);

	cout << "Placing crafting table...\n";
	mc::sendCommand("build craft.schem", key::at);
	while (readProcess("javaw.exe").find("[Baritone] Done") == string::npos)
	{
		cout << ".";
		sleep(1000);
	}
	cout << "Done.";
	cout << "Pathing to table";
	sleep(100);
	mc::sendCommand("goto crafting_table", key::at);
	sleep(100);
	while (readProcess("javaw.exe").find("]: net.minecraft.class") == string::npos)
	{
		cout << ".";
		sleep(1000);
	}
	cout << "\nDone.";

	vector<string> items = {"iron chest", "iron legg", "iron boot", "iron sword", "iron hel", "iron shov", "iron pick", "shield", "bucket", "iron axe"};
	multiCraft(items);
	sleep(100);
	pressKey(key::escape);
	sleep(100);
	mc::sendCommand("mine 1 crafting_table", key::at);
	sleep(1000);
	while (readProcess("javaw.exe").find("[Baritone] Have") == string::npos)
	{
		//cout << "\"" << readProcess("javaw.exe") << "\"\n";
		cout << ".";
		sleep(1000);
	}

}

void craftIronItems()
{
	cout << "Getting iron";
	mc::sendCommand("mine 40 iron_ore", key::at);
	sleep(1000);
	while (readProcess("javaw.exe").find("[Baritone] Have") == string::npos)
	{
		//cout << "\"" << readProcess("javaw.exe") << "\"\n";
		cout << ".";
		sleep(1000);

	}
	cout << "\nDone.\n";
	cout << "Getting coal";
	mc::sendCommand("mine 40 coal_ore", key::at);
	sleep(1000);
	while (readProcess("javaw.exe").find("[Baritone] Have") == string::npos)
	{
		//cout << "\"" << readProcess("javaw.exe") << "\"\n";
		cout << ".";
		sleep(1000);

	}
	cout << "\nDone.\n";
	cout << "Placing Furnace...";
	sleep(1000);
	placeFurnace();
	sleep(10000);
	LeftClick();
	cout << "Done.\n";

	

}

void diamondMode()
{
	mc::sendCommand("mine 64 diamond_ore", key::at);
	while (readProcess("javaw.exe").find("[Baritone] Done") == string::npos)
	{
		cout << ".";
		sleep(1000);
	}
	cout << "Done.";
}

void playMinecraft()
{
	cout << "Running minecraft bot...";
	int step = 1;
	start();
	craftBasics();
	craftBasicTools();
	craftStoneTools();
	craftIronItems();
	return;
	craftIronItems2();
	diamondMode();
}

void getCreativeItem(string item, int slot = 0)
{
	pressKey(key::menu);
	sleep(10);
	SetCursorPos(1536, 471);
	sleep(10);
	LeftClick();
	sleep(10);
	typeStr(item, 4);
	sleep(10);
	SetCursorPos(1038, 586);
	sleep(30);
	pressKey(keyCode[slot]);
	sleep(20);
	pressKey(key::escape);
	
}

int main()
{
	cout << "Minecraft Bot v2.4\nMade by Vetronus#0171\n";
	POINT mousePos;
	cout << "Running ahk rebind script...\n";
	thread ahk(ahkThread);
	sleep(100);
	cout << "Done.\n";
	


	while (true)
	{
		if (GetAsyncKeyState(VK_UP))
		{
			
		}
		else if (GetAsyncKeyState(VK_LBUTTON))
		{
			GetCursorPos(&mousePos);
			cout << mousePos.x << " " << mousePos.y << endl;
			while (GetAsyncKeyState(VK_LBUTTON))
			{
			
			}
		}else if (GetAsyncKeyState(VK_F1))
		{
			//typeStr("test");
			mc::sendCommand("gamemode creative");
			while (GetAsyncKeyState(VK_F1))
			{
			
			}
		}else if (GetAsyncKeyState(0x5A))
		{
			
			while (GetAsyncKeyState(0x5A))
			{
				
			}
		}else if (GetAsyncKeyState(VK_END))
		{
			return 0;
		}else if (GetAsyncKeyState(VK_ADD))
		{
			//creative speedrun

			//first open inv and set to search mode'
			getCreativeItem("block of diamond",1);
			sleep(10);
			getCreativeItem("block of diamond",2);
			while (GetAsyncKeyState(VK_ADD))
			{

			}
		}
		
	}

	

	system("pause");
}
