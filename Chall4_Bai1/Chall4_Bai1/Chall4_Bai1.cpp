

#include <windows.h>
#include <iostream>

using namespace std;

int main()
{
    HKEY hKey;
    LPCTSTR sk = TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Run");
    LONG lError = RegOpenKeyEx(HKEY_CURRENT_USER, sk, NULL, KEY_ALL_ACCESS, &hKey);
    if (ERROR_FILE_NOT_FOUND == lError)
    {
        //Create a key
        lError = RegCreateKeyEx(HKEY_CURRENT_USER, sk, NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, NULL);
    }
    if (lError != ERROR_SUCCESS)
    {
        std::cout << "\nSomething is wrong. Could not Open or Create the Key";
    }
    LPCTSTR value = TEXT("MathClient");
    WCHAR path[100] = TEXT("C:\\Users\\duyhu\\source\\repos\\MathClient\\x64\\Debug\\MathClient.exe");

    LONG setRes = RegSetValueEx(hKey, value, 0, REG_SZ, (LPBYTE)path, sizeof(path));

    if (setRes == ERROR_SUCCESS) {
        printf("Success writing to Registry.");
    }
    else {
        printf("Error writing to Registry.");
    }
    RegCloseKey(hKey);
    system("pause");
    return 0;
}

