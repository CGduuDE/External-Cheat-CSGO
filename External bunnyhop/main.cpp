#include <iostream>
#include <Windows.h>
#include <C:\Users\cgduude\Desktop\mem class\MemMan.h>



MemMan memory;


struct gameOffsets
{
    DWORD oLocalPlayer = 0xD8A2DC;
    DWORD iCrosshairId = 0xB3E8;
    DWORD fJump = 0x524CFDC;
    DWORD flags = 0x104;
    DWORD iTeamNum = 0xF4;
    DWORD dwEntityList = 0x4DA215C;
    DWORD iHealth = 0x100;
    DWORD iGlowIndex = 0xA438;
    DWORD GlowObjectManager = 0x52EA5D0;
    DWORD bSpotted = 0x93D;
    DWORD bDormant = 0xED;
    DWORD flFlashDuration = 0xA420;
    DWORD dwbSendPackets = 0xD76DA;
    DWORD iObserverMode = 0x3378;
}offsets;

struct values
{
    DWORD localPlayer;
    DWORD gameModule;
    DWORD process;
    BYTE flag;
    int PlayerTeam;
    int CrosshairId;
    DWORD EnemyTeam;
    bool pause = false;
}val;

void Bunny()
{
    val.localPlayer = memory.readMem<DWORD>(val.gameModule + offsets.oLocalPlayer);
    val.flag = memory.readMem<BYTE>(val.localPlayer + offsets.flags);
    if (GetAsyncKeyState(VK_SPACE) && val.flag & (1 << 0)) {
        memory.writeMem<DWORD>(val.gameModule + offsets.fJump, 6);
    }

}

void Trigger()
{
    val.localPlayer = memory.readMem<DWORD>(val.gameModule + offsets.oLocalPlayer);
    val.PlayerTeam = memory.readMem<int>(val.localPlayer + offsets.iTeamNum);
    val.CrosshairId = memory.readMem<int>(val.localPlayer + offsets.iCrosshairId);

    if (val.CrosshairId > 0 && val.CrosshairId <= 64) {
        int Target = memory.readMem<int>(val.gameModule + offsets.dwEntityList + (val.CrosshairId - 1) * 0x10);
        int TargetTeam = memory.readMem<int>(Target + offsets.iTeamNum);

        if (TargetTeam != val.PlayerTeam)//&& GetAsyncKeyState(VK_MENU)
        {
            mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
            Sleep(1);
            mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
        }
    }


}

void noflash() {
    val.localPlayer = memory.readMem<DWORD>(val.gameModule + offsets.oLocalPlayer);
    int flashDur = 0;
    flashDur = memory.readMem<DWORD>(val.localPlayer + offsets.oLocalPlayer);
    if (flashDur > 0) {
        memory.writeMem(val.localPlayer + offsets.flFlashDuration, 0);
    }
}

void WH()
{
    int PlayerTeam = memory.readMem<int>(val.localPlayer + offsets.iTeamNum);
    for (int i = 0; i < 64; i++) {

        int EntityList = memory.readMem<int>(val.gameModule + offsets.dwEntityList + i * 0x10);

        int GlowIndex = memory.readMem<int>(EntityList + offsets.iGlowIndex);
        int EntityTeam = memory.readMem<int>(EntityList + offsets.iTeamNum);
        int GlowObject = memory.readMem<int>(val.gameModule + offsets.GlowObjectManager);
        if (EntityTeam != 0 && EntityTeam != PlayerTeam) {
            memory.writeMem(GlowObject + (GlowIndex * 0x38) + 0x4, 0.f);
            memory.writeMem(GlowObject + (GlowIndex * 0x38) + 0x8, 1.f);
            memory.writeMem(GlowObject + (GlowIndex * 0x38) + 0xC, 1.f);
            memory.writeMem(GlowObject + (GlowIndex * 0x38) + 0x10, 1.7f);

        }
        memory.writeMem(GlowObject + (GlowIndex * 0x38) + 0x24, true);
        memory.writeMem(GlowObject + (GlowIndex * 0x38) + 0x25, false);


    }
}

void radarhack()
{

    for (int x = 0; x < 64; x++) {
        int Entity = memory.readMem<int>(val.gameModule + offsets.dwEntityList + x * 0x10);
        int EntityTeam = memory.readMem<int>(Entity + offsets.iTeamNum);
        int EntityDormant = memory.readMem<int>(Entity + offsets.bDormant);
        if (EntityTeam != val.PlayerTeam && !EntityDormant) {
            memory.writeMem<bool>(Entity + offsets.bSpotted, true);
        }
    }
}

void thirdperson()
{
    memory.writeMem(val.localPlayer + offsets.iObserverMode, val.pause);
}

 
void FakeLag()
{
    for (;;)
    {
        memory.writeMem(val.gameModule + offsets.dwbSendPackets, false);
        Sleep(16 * (1 / 64)); //16 фактор, ставьте любой от 0 до 16
        memory.writeMem(val.gameModule + offsets.dwbSendPackets, true);
        Sleep(1 * (1 / 64)); // отправляем 1 пакет чтобы не тайм аутнулись
    }
}

int main()
{

    
    val.process = memory.getProcess("csgo.exe");
    val.gameModule = memory.getModule(val.process, "client.dll");
    if (val.localPlayer == NULL)
        val.localPlayer = memory.readMem<DWORD>(val.gameModule + offsets.oLocalPlayer);


    for (;;) {
        if (GetAsyncKeyState(VK_INSERT) && val.pause == true) val.pause = false; Sleep(0.5);
       if (GetAsyncKeyState(VK_INSERT) && val.pause == false) val.pause = true; Sleep(0.5);
        if (!val.pause) {
            //thirdperson();
            WH();
            if (GetAsyncKeyState(VK_MENU)) Trigger();
            Bunny();
            noflash();
            radarhack();
            //FakeLag();
        }
    }



    return 0;
}