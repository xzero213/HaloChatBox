#ifndef D3DHOOK_H
#define D3DHOOK_H
//-------------------------------------------
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "detours.lib")
#pragma warning( disable : 4996 )
//-------------------------------------------

#include <d3d9.h>
#include <d3dx9.h>
//#include <fstream>
#include <tchar.h>
#include <detours.h>
#include "structs.h"
#include <stdint.h>
#include <string>




__inline bool StartsWith(const std::string& text,const std::string& token)
{
	if(text.length() < token.length())
		return false;
	return (text.compare(0, token.length(), token) == 0);
}

__inline char* StringToChar(std::string str)
{
	char *cstr = new char[str.length() + 1];
	strcpy(cstr, str.c_str());
	return cstr;
}

__inline char* IntToChar(int str)
{
	std::string temp = std::to_string(str);
	return StringToChar(temp);
}


typedef void (*pTextOut)(char *pString, const float fColor[4]);
pTextOut oTextOut; 
void hkTextOut( char * pString, const float fColor[4] )
{
	__asm MOV EAX,fColor
	__asm PUSH pString
	//__asm PUSH EDI; //Added
	//__asm MOV EDI,EAX; //Added
	__asm CALL DWORD PTR DS:[oTextOut]
	__asm ADD ESP, 04h
} 
DWORD __stdcall hkDrawText(char *pString, const float fColor[4])
{
	DWORD dwOldProtect = 0;
	//DWORD dwTextOut = 0x00496B50;
	BYTE bTextOutOrig[6] = {0x83, 0xEC, 0x10, 0x57, 0x8B, 0xF8};

	VirtualProtect((void*)dwTextOut, 10, PAGE_EXECUTE_READWRITE, &dwOldProtect);
	oTextOut = (pTextOut)DetourFunc((PBYTE)dwTextOut,(PBYTE)&hkTextOut, 6);

	hkTextOut(pString, fColor);

	memcpy((void*)dwTextOut, (void*)bTextOutOrig, 6);
	VirtualProtect((void*)dwTextOut, 10, dwOldProtect, &dwOldProtect);

	return 0;
}
void *DetourFunc(BYTE *src, const BYTE *dst, const int len)
{
	BYTE *jmp = (BYTE*)malloc(len+5);
	DWORD dwback;

	VirtualProtect(src, len, PAGE_READWRITE, &dwback);

	memcpy(jmp, src, len);    jmp += len;

	jmp[0] = 0xE9;
	*(DWORD*)(jmp+1) = (DWORD)(src+len - jmp) - 5;

	src[0] = 0xE9;
	*(DWORD*)(src+1) = (DWORD)(dst-src) - 5;

	VirtualProtect(src, len, dwback, &dwback);

	return (jmp-len);
} 

void hkTextSend(char * cstr)
{
	float fGreen[4] = {1.0f, 0.0f, 1.0f, 0.0f};
	hkDrawText(cstr,fGreen);
}

//------------------------------------------------------------------------------
bool bMenu = false;
//------------------------------------------------------------------------------
void BorderedText( LPD3DXFONT Font, char* pString, int x, int y, D3DCOLOR Color, D3DCOLOR Border )
{
	RECT rect0 = { x + 1, y    , x + 1000, y + 200 };
	RECT rect1 = { x - 1, y    , x + 1000, y + 200 };
	RECT rect2 = { x    , y + 1, x + 1000, y + 200 };
	RECT rect3 = { x    , y - 1, x + 1000, y + 200 };
	RECT rect4 = { x    , y    , x + 1000, y + 200 };

	Font->DrawTextA(NULL, pString, -1, &rect0, 0, Border);
	Font->DrawTextA(NULL, pString, -1, &rect1, 0, Border);
	Font->DrawTextA(NULL, pString, -1, &rect2, 0, Border);
	Font->DrawTextA(NULL, pString, -1, &rect3, 0, Border);
	Font->DrawTextA(NULL, pString, -1, &rect4, 0, Color);
}

void ColoredBorderText(LPD3DXFONT Font, std::vector<char*> pString, int x, int y, D3DCOLOR Color[], D3DCOLOR Border,int size)
{
	RECT r = { x,y, 0,0}; // starting point

	RECT rect0 = { x + 1, y , 0,0};
	RECT rect1 = { x - 1, y , 0,0};
	RECT rect2 = { x, y + 1 , 0,0};
	RECT rect3 = { x, y - 1 , 0,0};
	RECT rect4 = { x, y , 0,0};

	for(int i = 0;i < size;i++)
	{
		BigFont->DrawTextA(NULL, pString[i], -1, &rect0, DT_CALCRECT, 0);
		BigFont->DrawTextA(NULL, pString[i], -1, &rect1, DT_CALCRECT, 0);
		BigFont->DrawTextA(NULL, pString[i], -1, &rect2, DT_CALCRECT, 0);
		BigFont->DrawTextA(NULL, pString[i], -1, &rect3, DT_CALCRECT, 0);
		BigFont->DrawTextA(NULL, pString[i], -1, &rect4, DT_CALCRECT, 0);

		BigFont->DrawTextA(NULL, pString[i], -1, &rect0, DT_NOCLIP, Border);
		BigFont->DrawTextA(NULL, pString[i], -1, &rect1, DT_NOCLIP, Border);
		BigFont->DrawTextA(NULL, pString[i], -1, &rect2, DT_NOCLIP, Border);
		BigFont->DrawTextA(NULL, pString[i], -1, &rect3, DT_NOCLIP, Border);
		BigFont->DrawTextA(NULL, pString[i], -1, &rect4, DT_NOCLIP, Border);

		BigFont->DrawTextA(NULL, pString[i], -1, &r, DT_CALCRECT, 0);
		BigFont->DrawTextA(NULL, pString[i], -1, &r, DT_NOCLIP, Color[i]);


		r.left = r.right;
		rect0.left = rect0.right;
		rect1.left = rect1.right;
		rect2.left = rect2.right;
		rect3.left = rect3.right;
		rect4.left = rect4.right;
	}
}



//-----------------------------------------------------------------------------
void DrawText( LPD3DXFONT Font, char* pString, int x, int y, D3DCOLOR Color )
{
	RECT rect = { x, y, x + 1000, y + 200 };
	Font->DrawTextA(NULL, pString, -1, &rect, 0, Color);
}
//-----------------------------------------------------------------------------

typedef HRESULT ( WINAPI* tEndScene ) ( LPDIRECT3DDEVICE9 pDevice );
tEndScene oEndScene;


struct MenuItem
{
	bool on;
	char* name;
	int value;
	bool istitle;
};

struct ChatItem
{
	std::vector<char*> text;
	std::string FullMessage;


	int size;

	time_t start, end;
	double elapsed;

	bool hidden;

	bool KillMessage;
	bool ChatMessage;
	bool TeamMessage;


	char* PlayerName0;
	bool PlayerTeam0;

	char* PlayerName1;
	bool PlayerTeam1;

	D3DCOLOR Color[4];

};




class _ChatMenu
{
public:

	void AddItemToChat(std::string text,std::vector<char*> FormattedText,D3DCOLOR Colour[],int size, std::string PlayerName0,bool PlayerTeam0,std::string PlayerName1,bool PlayerTeam1,bool KillMessage,bool ChatMessage,bool TeamMessage)
		//void AddItemToChat(ChatMessages ChatMessage)
	{
		try
		{
			mi.push_back(ChatItem());
			ChatItem temp;

			temp.text = FormattedText;
			for(int i = 0;i < size;i++)
				temp.Color[i] = Colour[i];
			temp.PlayerName0 = StringToChar(PlayerName0);
			temp.PlayerName1 = StringToChar(PlayerName1);

			temp.PlayerTeam0 = PlayerTeam0;
			temp.PlayerTeam1 = PlayerTeam1;

			temp.KillMessage = KillMessage;


			temp.ChatMessage = ChatMessage;
			temp.TeamMessage = TeamMessage;
			temp.size = size;
			temp.FullMessage = text;

			/*temp.PlayerName0 = StringToChar(PlayerName0);
			temp.PlayerTeam0 = PlayerTeam0;

			temp.PlayerName1 = StringToChar(PlayerName1);
			temp.PlayerTeam1 = PlayerTeam1;

			temp.KillMessage = KillMessage;
			temp.ChatMessage = ChatMessage;
			temp.TeamMessage = TeamMessage;

			if(!ChatMessage && !KillMessage)
			temp.text.insert(temp.text.begin(),StringToChar(text)); //insert the chat message
			else if(ChatMessage){
			if(!TeamMessage) {

			text = text.erase(0,PlayerName0.size());
			temp.MessageSize = text.length();
			temp.text.insert(temp.text.begin(),StringToChar(text)); //insert the chat message
			temp.text.insert(temp.text.begin(),StringToChar(PlayerName0)); //insert the players name

			}else{
			temp.text.insert(temp.text.begin(),StringToChar(text));
			}
			} else if(KillMessage) {
			//Message = Message.erase(0,PlayerName0.size());
			//Message = Message.erase(15,PlayerName1.size());

			text = " was killed by ";

			temp.MessageSize = text.length();

			temp.text.insert(temp.text.begin(),StringToChar(" " + PlayerName1)); //insert the player killed?
			temp.text.insert(temp.text.begin(),StringToChar(text)); //insert the chat message
			temp.text.insert(temp.text.begin(),StringToChar(PlayerName0)); //insert the players name
			}*/

			temp.elapsed = 0;
			temp.hidden = false;
			time(&temp.start);
			if(TotalChatItems == 0)
				mi.insert(mi.begin(),temp); //insert the chat message at position 0
			else
				mi.insert(mi.begin() + TotalChatItems,temp); //insert the message at the last point

			if(TotalChatItems != MAX_CHAT_ITEMS)
				TotalChatItems++;
			else
				mi.erase(mi.begin()); //Delete the first message when all of the lines are used up
		}
		catch(const std::exception& e){
			hkTextSend("An error occurred while parsing this message:");
			hkTextSend(StringToChar(text));
		}
	}

	void AddItemToKills(char* text)
	{



	}

	void StartMenu(IDirect3DDevice9* pDevice)
	{
		static bool inited = false;
		if(!inited)
		{
			inited = true;
		}

		DrawChat(pDevice);
	} 

private:
	//MenuItem mi[MAX_MENU_ITEMS];


	std::vector<ChatItem> mi;

	std::vector<ChatItem> vKills;

	HANDLE hand;

	void DrawFillRect(IDirect3DDevice9* dev, int x, int y, int w, int h,unsigned char r, unsigned char g, unsigned char b)
	{
		D3DCOLOR rectColor = D3DCOLOR_XRGB(r,g,b);
		D3DRECT BarRect = { x, y, x + w, y + h }; 
		dev->Clear(1,&BarRect,  D3DCLEAR_TARGET | D3DCLEAR_TARGET ,rectColor,0,0);
	}

	void DrawChat(IDirect3DDevice9* pDevice)
	{

		if(StaticPlayerHeader->IsInMainMenu == 1 && TotalChatItems > 0){
			for(unsigned int i = 0;i < mi.size();i++)
				mi.erase(mi.begin());
			TotalChatItems = 0;
			return;
		}
		if(GetAsyncKeyState(VK_F1))
			return;

		//if(TextBoxOpen[0] == 0)
		//	DrawFillRect(pDevice, MenuPosX, MenuPosY, 400, TotalMenuItems*12 + 20, 15,15,15); //250
		for( int x = 0; x < TotalChatItems; ++x)
		{

			//char optname[256];
			time(&mi[x].end);
			mi[x].elapsed = difftime(mi[x].end,mi[x].start);

			if(mi[x].elapsed > ChatMessageTime)
				mi[x].hidden = true;





			if(!mi[x].hidden) {
				if(mi[x].text.size() != 0){
					for (int i = 0; i < mi[x].text.size(); i++)
					{
						//if(!mi[x].KillMessage)
						ColoredBorderText(BigFont,mi[x].text,ChatPosX + 15,ChatPosY + 12*x,mi[x].Color,tBlack,mi[x].size);
						//else
						//	ColoredBorderText(BigFont,mi[x].text,KillsPosX + 15,KillsPosY + 12*x,mi[x].Color,tBlack,mi[x].size);
					}
				}
				else{
					BorderedText(BigFont,StringToChar(mi[x].FullMessage),ChatPosX + 15, ChatPosY + 12*x, tWhite,tBlack);
				}
			}

			if(mi[x].hidden && TextBoxOpen[0] == 0) {
				if(mi[x].text.size() != 0){
					for (int i = 0; i < mi[x].text.size(); i++)
					{

						//if(!mi[x].KillMessage)
						ColoredBorderText(BigFont,mi[x].text,ChatPosX + 15,ChatPosY + 12*x,mi[x].Color,tBlack,mi[x].size);
						//else
						//	ColoredBorderText(BigFont,mi[x].text,KillsPosX + 15,KillsPosY + 12*x,mi[x].Color,tBlack,mi[x].size);

					}
				}
				else{
					BorderedText(BigFont,StringToChar(mi[x].FullMessage),ChatPosX + 15, ChatPosY + 12*x, tWhite,tBlack);
				}
			}



		}
	}

	void DrawKillsWindow(IDirect3DDevice9* pDevice)
	{
		int* TextBoxOpen = (int*)0x686A98;
		//if(TextBoxOpen[0] == 0)
		//	DrawFillRect(pDevice, MenuPosX, MenuPosY, 400, TotalMenuItems*12 + 20, 15,15,15); //250

		/*for( int x = 0; x < TotalChatItems; ++x)
		{
		//char optname[256];
		time(&vKills[x].end);
		vKills[x].elapsed = difftime(vKills[x].end,vKills[x].start);

		if(vKills[x].elapsed > 5)
		vKills[x].hidden = true;

		if(!vKills[x].hidden)
		BorderedText(BigFont,vKills[x].text[0],MenuPosX + 15, MenuPosY + 12*x, tWhite,tBlack);

		if(vKills[x].hidden && TextBoxOpen[0] == 0)
		BorderedText(BigFont,(vKills[x].text[0]),MenuPosX + 15, MenuPosY + 12*x, tWhite,tBlack);

		}
		*/
	}

	void WriteMemory(int Addr,byte buffer[],int size)
	{
		DWORD fOld = 0;
		hand = GetCurrentProcess();
		VirtualProtectEx(hand, (void*)Addr, size,PAGE_EXECUTE_READWRITE,&fOld);
		WriteProcessMemory(hand, (void*)Addr,buffer,size,nullptr);
		VirtualProtectEx(hand, (void*)Addr, size,fOld,&fOld);
	}



};
_ChatMenu ChatMenu;

class _Menu
{

public:

	bool GetPlayerByIndex(unsigned int index)
	{
		if(index == -1)
			return false;
		StaticPlayer = 0, ObjectTableArray = 0, Masterchief = 0;
		StaticPlayer = (Static_Player*)(StaticPlayerHeader->FirstPlayer + (index * StaticPlayerHeader->SlotSize));
		if(StaticPlayer->ObjectID != 65535 && StaticPlayer->ObjectID != 0)
		{
			if(StaticPlayer->ObjectIndex != Local->ObjectIndex && StaticPlayer->ObjectID != Local->ObjectID)
			{
				ObjectTableArray = (Object_Table_Array*)(ObjectTableHeader->FirstObject + (StaticPlayer->ObjectIndex * ObjectTableHeader->Size));
				Masterchief = (AMasterchief*)ObjectTableArray->Offset;
				if(Masterchief == NULL)
					return false;
				return true;
			}
		}
		return false;
	}
	//------------------------------------------------------------------------------
	bool GetLocalPlayer(unsigned int index)
	{
		if(index == -1 || index == 65535)
			return false;
		LocalPlayer = 0, ObjectTableArray = 0, LocalMC = 0;
		LocalPlayer = (Static_Player*)(StaticPlayerHeader->FirstPlayer + (index * StaticPlayerHeader->SlotSize));
		if(LocalPlayer->ObjectID != 65535 && LocalPlayer->ObjectID != 0)
		{
			ObjectTableArray = (Object_Table_Array*)(ObjectTableHeader->FirstObject + (LocalPlayer->ObjectIndex * ObjectTableHeader->Size));
			LocalMC = (AMasterchief*)ObjectTableArray->Offset;
			return true;
		}
		return false;
	}



	MenuItem mi[MAX_MUTE_ITEMS];


	void KeyBoardInput()
	{
		if(CurrentMenuItem >= TotalMenuItems)
		{
			CurrentMenuItem = 0; //so you can loop up and down heheh
		}
		else if(CurrentMenuItem < 0)
		{
			CurrentMenuItem = (TotalMenuItems-1); //it starts at 0, remember
		}

		//KEYBOARD
		if(GetAsyncKeyState(VK_DOWN)&1)
		{
			CurrentMenuItem ++;// nothing to explain really, same below..
			if(mi[CurrentMenuItem].istitle)
				CurrentMenuItem++;
		}
		if(GetAsyncKeyState(VK_UP)&1)
		{
			CurrentMenuItem --;
			if(mi[CurrentMenuItem].istitle)
				CurrentMenuItem--;
		}
		//Turns on and Off.


		if(GetAsyncKeyState(VK_LEFT) &1 || GetAsyncKeyState(VK_RIGHT) &1)//maybe you want another key :P
		{
			mi[CurrentMenuItem].on = !mi[CurrentMenuItem].on; // a little toggle
		}


	}


	void StartMenu(IDirect3DDevice9* pDevice)
	{
		static bool inited = false;
		if(!inited)
		{
			inited = true;
		}

		DrawMuted(pDevice);
		KeyBoardInput();
	} 

private:

	



	int CurrentMenuItem;

	void DrawFillRect(IDirect3DDevice9* dev, int x, int y, int w, int h,unsigned char r, unsigned char g, unsigned char b)
	{
		D3DCOLOR rectColor = D3DCOLOR_XRGB(r,g,b);
		D3DRECT BarRect = { x, y, x + w, y + h }; 
		dev->Clear(1,&BarRect,  D3DCLEAR_TARGET | D3DCLEAR_TARGET ,rectColor,0,0);
	}

	void DrawMuted(IDirect3DDevice9* pDevice)
	{

		//DrawFillRect(pDevice, MenuPosX, MenuPosY - 20, 200, 16*12 + 20, 15,15,15); //250

		DrawFillRect(pDevice, MutePosX, MutePosY - 20, 200, 16*12 + 50, 15, 15, 15); //250
		DrawText(BigFont, "Mute players" ,MutePosX, MutePosY - 20,tRed);

		for( unsigned int x = 0; x < 16; ++x)
		{
			if( !GetLocalPlayer ( Local->PlayerIndex ) ){ //if local player then set to false
				mi[x].on = false;
				continue;
			}
			if ( !GetPlayerByIndex ( x ) ){ //not set to false here because on death it breaks itself
				continue;
			}
			if ( StaticPlayer->PlayerID == 0 ){ //if invalid player ID set to false
				mi[x].on = false;
				continue;
			}

			char optname[35];
			sprintf(optname, "[%s]" , mi[x].on ? "Muted" : "Unmuted");
			char PlayerBuf[15];
			sprintf(PlayerBuf,"%S", StaticPlayer->PlayerName1);

			DrawText(mi[x].istitle ? BigFont : SmallFont, PlayerBuf ,MutePosX + 15, MutePosY + 12*x,tWhite); //draws name of muted
			DrawText(SmallFont, optname ,MutePosX + 140, MutePosY + 12*x,mi[x].istitle ? tBlue : ( mi[x].on ? tGreen : tRed)); //draws value
		}

		DrawText(Menu,"-->",MutePosX,MutePosY + (12*CurrentMenuItem),tWhite);
	}
};
_Menu MutedMenu;






bool bRunOnce = true;


HRESULT WINAPI hkEndScene(LPDIRECT3DDEVICE9 pDevice)
{
	if(GetAsyncKeyState(VK_F3)&1)
		bMenu=!bMenu;

	if(bRunOnce)
	{
		pDevice->GetViewport(&pViewport);
		//---------------------------------------------	
		// Create our font
		//---------------------------------------------
		D3DXCreateFont(pDevice, IniSettings.BigFontSize, 0, FW_BOLD, 1, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, IniSettings.Font, &Menu);
		D3DXCreateFont(pDevice, IniSettings.SmallFontSize, 0, FW_BOLD, 1, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, IniSettings.Font, &SmallFont);
		D3DXCreateFont(pDevice, IniSettings.BigFontSize, 0, FW_BOLD, 1, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, IniSettings.Font, &BigFont);

		/*D3DXCreateFont(pDevice, 15, 0, FW_BOLD, 1, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, _T("Ariel"), &Menu);
		D3DXCreateFont(pDevice, 13, 0, FW_BOLD, 1, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, _T("Ariel"), &SmallFont);
		D3DXCreateFont(pDevice, 15, 0, FW_BOLD, 1, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, _T("Ariel"), &BigFont);*/


		bRunOnce = false;
	}
	ChatMenu.StartMenu(pDevice);
	if(bMenu)
		MutedMenu.StartMenu(pDevice);

	return oEndScene(pDevice);
}
typedef HRESULT ( WINAPI* tSetViewport ) ( LPDIRECT3DDEVICE9 pDevice, CONST D3DVIEWPORT9* pViewport );
tSetViewport oSetViewport;
//------------------------------------------------------------------------------
HRESULT WINAPI hkSetViewport(LPDIRECT3DDEVICE9 pDevice,CONST D3DVIEWPORT9* pViewport)
{ 
	g_fxCenter = ( float )(pViewport->Width/2);
	g_fyCenter = ( float )(pViewport->Height/2);

	return oSetViewport(pDevice,pViewport);
}
//------------------------------------------------------------------------------
typedef HRESULT ( WINAPI* tReset ) ( LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters );
tReset oReset;
//------------------------------------------------------------------------------
HRESULT WINAPI hkReset(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters)
{ 
	__asm PUSHAD;

	if ( g_pFont1 )
		g_pFont1->OnLostDevice( );

	if ( g_pFont2 )
		g_pFont2->OnLostDevice( );

	if ( g_pLine1 )
		g_pLine1->OnLostDevice( );

	if( Menu )
		Menu->OnLostDevice( );

	HRESULT hResult = oReset( pDevice, pPresentationParameters );

	if ( g_pFont1 )
		g_pFont1->OnResetDevice( );

	if ( g_pFont2 )
		g_pFont2->OnResetDevice( );

	if ( g_pLine1 )
		g_pLine1->OnResetDevice( );

	if( Menu )
		Menu->OnResetDevice( );


	__asm POPAD;
	return hResult;
}
//------------------------------------------------------------------------------
LRESULT CALLBACK MsgProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
} 
//------------------------------------------------------------------------------
void DX_Init(DWORD* table)
{
	WNDCLASSEX wc = { sizeof(WNDCLASSEX),CS_CLASSDC,MsgProc,0L,0L,GetModuleHandle(NULL),NULL,NULL,NULL,NULL,_T("DX"),NULL};
	RegisterClassEx(&wc);
	HWND hWnd = CreateWindow(_T("DX"),NULL,WS_OVERLAPPEDWINDOW,100,100,300,300,GetDesktopWindow(),NULL,wc.hInstance,NULL);
	LPDIRECT3D9 pD3D = Direct3DCreate9( D3D_SDK_VERSION );
	D3DPRESENT_PARAMETERS d3dpp; 
	ZeroMemory( &d3dpp, sizeof(d3dpp) );
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	LPDIRECT3DDEVICE9 pd3dDevice;
	pD3D->CreateDevice(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,hWnd,D3DCREATE_SOFTWARE_VERTEXPROCESSING,&d3dpp,&pd3dDevice);
	DWORD* pVTable = (DWORD*)pd3dDevice;
	pVTable = (DWORD*)pVTable[0];

	table[ES]	= pVTable[42];
	table[RS]	= pVTable[16];
	table[SV]	= pVTable[47];

	table[DIP]	= pVTable[82];
	table[SSS]	= pVTable[100];


	DestroyWindow(hWnd);
}
#endif /* D3DHOOK_H */