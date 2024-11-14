/*==============================================================================

   キー、コントローラー入力モジュール
														 Author	: 桜井優輝
														 Ver	: 0.1.3-20241114
--------------------------------------------------------------------------------

	main.cpp内のグローバル変数として以下を定義してください
	KeyLogger* KeyLogger::instance = nullptr;

	ウィンドウプロシージャコールバック関数(WndProc)内の一番上に追加してください
	KeyLogger::StoreKeyLog(uMsg, wParam);

	ゲームフレーム適応後のアップデート処理(WinMain下のUpdate関数内)に追加してください。
	KeyLogger::UpdateKeyLog();


==============================================================================*/

#ifndef KEYLOGGER_H_
#define KEYLOGGER_H_

//#define _K_DEBUG //デバッグ用

#pragma comment (lib, "Xinput.lib")

#include <WinUser.h>
#include <Xinput.h>

#ifdef _K_DEBUG
#include <iostream>
#endif // _K_DEBUG

constexpr int MAX_INPUT = 256;

static enum XINPUTBUTTON
{
	XINPUTBUTTON_UP = 0x0001,
	XINPUTBUTTON_DOWN = 0x0002,
	XINPUTBUTTON_LEFT = 0x0004,
	XINPUTBUTTON_RIGHT = 0x0008,

	XINPUTBUTTON_START = 0x0010,
	XINPUTBUTTON_BACK = 0x0020,
	XINPUTBUTTON_LEFTSTICKBUTTON = 0x0040,
	XINPUTBUTTON_RIGHTSTICKBUTTON = 0x0080,

	XINPUTBUTTON_LEFTTRIGGER = 0x0100,		//トリガー2つの組み合わせ
	XINPUTBUTTON_RIGHTTRIGGER = 0x0200,		
	XINPUTBUTTON_A = 0x1000,
	XINPUTBUTTON_B = 0x2000,

	XINPUTBUTTON_X = 0x4000,
	XINPUTBUTTON_Y = 0x8000,
};

/*-----------------------------------変更可能部位-----------------------------*/

//保存するフレーム 
//値 2 ～ 2147483647
//デフォルト 10
constexpr int LOG_FRAME = 10; 

//接続可能コントローラ最大数
//値 0 ～ 4
//デフォルト 4
constexpr int CONTROLLER_MAX = 4;

/*----------------------------------------------------------------------------*/

class KeyLogger
{
protected:
	KeyLogger() {};

	bool pressedKey[MAX_INPUT];
	bool keyLog[LOG_FRAME][MAX_INPUT];
	XINPUT_STATE controllerLog[LOG_FRAME][CONTROLLER_MAX];

public:
	void operator=(const KeyLogger& obj) = delete;
	KeyLogger(const KeyLogger& obj) = delete;

	~KeyLogger() { delete instance; }
	static KeyLogger* instance;

	static void Initialize()
	{
		if (!instance)
		{
			instance = new KeyLogger;
#ifdef _K_DEBUG
			AllocConsole(); // コンソールを作成
			freopen("CONOUT$", "w", stdout); // stdoutをコンソールにリダイレクト

			std::setvbuf(stdout, NULL, _IOLBF, 8192); //コンソール用のバッファ確保(8Kbyte)
#endif // _K_DEBUG
		}
	}

	static void StoreKeyLog(UINT uMsg, WPARAM keys)
	{
		switch (uMsg)
		{
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
			instance->pressedKey[keys] = true;
			break;

		case WM_KEYUP:
		case WM_SYSKEYUP:
			instance->pressedKey[keys] = false;
			break;
		}
	}

	static void UpdateKeyLog()
	{
		//キーボード入力
		for (int frame = LOG_FRAME - 1; frame > 0; frame--)
		{
			memcpy_s(instance->keyLog[frame], MAX_INPUT, instance->keyLog[frame - 1], sizeof(bool) * MAX_INPUT);
		}
		memcpy_s(instance->keyLog[0], MAX_INPUT, instance->pressedKey, sizeof(bool) * MAX_INPUT);

		//system("cls");

		//コントローラー入力
		for (DWORD i = 0; i < CONTROLLER_MAX; i++)
		{
			for (int frame = LOG_FRAME - 1; frame > 0; frame--)
			{
				memcpy_s(instance->controllerLog[frame], MAX_INPUT, instance->controllerLog[frame - 1], sizeof(XINPUT_STATE) * CONTROLLER_MAX);
			}

			DWORD result = XInputGetState(i, &instance->controllerLog[0][i]);
#ifdef _K_DEBUG
			if (result == ERROR_DEVICE_NOT_CONNECTED)
			{
				std::cout << "Device " << i + 1 << " is not connected\n";
			}
			else
			{
				WORD buttonID = XINPUTBUTTON_UP;
				std::cout << "Device " << i + 1 << " Pressed ";
				for (int b = 0; b < 16; b++)
				{
					if (GetButtonDown((XINPUTBUTTON)buttonID, i))
					{
						std::cout << b << " ";
					}

					buttonID <<= 1;
				}
				std::cout << "wParam(" << instance->controllerLog[0][i].Gamepad.wButtons << ")" << std::endl;
			}
#endif // _K_DEBUG
		}
	}

	/*---------------------------------------------------------------------------------------------*/

	static bool GetKeyTrigger(WPARAM keyType, unsigned int frame = 1)
	{		
		//遡れるフレームより値が大きいなら最大値に
		if (frame > LOG_FRAME - 1)
		{
			frame = LOG_FRAME - 1;
		}

		//任意フレーム前に押されているか
		for (int i = 0; i < frame + 1; i++)
		{
			if (instance->keyLog[i][keyType] == true)
			{
				return true;
			}
		}

		return false;

	}

	static bool GetKeyUp(WPARAM keyType)
	{
		//前フレームに押されているか
		if (instance->keyLog[1][keyType] == true)
		{
			//現在のフレームで押されているか
			if (instance->keyLog[0][keyType] == true)
			{
				return false;
			}
			else
			{
				return true;
			}
		}
		else
		{
			return false;
		}
	}

	static bool GetKeyDown(WPARAM keyType)
	{
		//前フレームに押されているか
		if (instance->keyLog[1][keyType] == true)
		{
			return false;
		}
		//押されていなかった場合に現在フレームに押されているか
		else if (instance->keyLog[0][keyType] == true)
		{
			return true;
		}

		return false;
	}

	/*---------------------------------------------------------------------------------------------*/

	static XINPUT_STATE GetControllerState(int index)
	{
		return instance->controllerLog[0][index];
	}

	static bool GetButtonTrigger(XINPUTBUTTON buttonType, int controllerID = 0)
	{
		//前フレームに押されているか
		if (instance->controllerLog[1][controllerID].Gamepad.wButtons & buttonType)
		{
			return false;
		}
		//押されていなかった場合に現在フレームに押されているか
		else if (instance->controllerLog[0][controllerID].Gamepad.wButtons & buttonType)
		{
			return true;
		}

		return false;
	}

	static bool GetButtonUp(XINPUTBUTTON buttonType, int controllerID = 0)
	{
		//前フレームに押されているか
		if (instance->controllerLog[1][controllerID].Gamepad.wButtons & buttonType)
		{
			//現在のフレームで押されているか
			if (instance->controllerLog[0][controllerID].Gamepad.wButtons & buttonType)
			{
				return false;
			}
			else
			{
				return true;
			}
		}
		else
		{
			return false;
		}
	}

	static bool GetButtonDown(XINPUTBUTTON buttonType, unsigned int frame = 1, int controllerID = 0)
	{
		//遡れるフレームより値が大きいなら最大値に
		if (frame > LOG_FRAME - 1)
		{
			frame = LOG_FRAME - 1;
		}

		//任意フレーム前に押されているか
		for (int i = 0; i < frame + 1; i++)
		{
			if (instance->controllerLog[i][controllerID].Gamepad.wButtons & buttonType)
			{
				return true;
			}
		}

		//押されていたらtrueを返す
		return false;
	}

};

#endif