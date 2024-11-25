/*==============================================================================

   キー、コントローラー入力モジュール
														 Author	: 桜井優輝
														 Ver	: 1.0.1-20241126
--------------------------------------------------------------------------------

	main.cpp内のグローバル変数として以下を定義してください。
	KeyLogger* KeyLogger::instance = nullptr;

	ウィンドウプロシージャコールバック関数(WndProc)内の一番上に追加してください。
	KeyLogger::StoreKeyLog(uMsg, wParam);

	ゲームフレーム適応後のアップデート処理(WinMain下のUpdate関数内)に追加してください。
	KeyLogger::UpdateKeyLog();
==============================================================================*/

#ifndef KEYLOGGER_H_
#define KEYLOGGER_H_

#define _K_DEBUG //デバッグ用

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

	XINPUTBUTTON_LEFTBUTTON = 0x0100,
	XINPUTBUTTON_RIGHTBUTTON = 0x0200,	

	XINPUTBUTTON_A = 0x1000,
	XINPUTBUTTON_B = 0x2000,
	XINPUTBUTTON_X = 0x4000,
	XINPUTBUTTON_Y = 0x8000,
};

static enum XINPUTDIRECTION
{
	XINPUTDIRECTION_LEFT = 0x0400,
	XINPUTDIRECTION_RIGHT = 0x0800,
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

//スティックデッドゾーン
//値 0 ～ 32767
//デフォルト 2048
constexpr SHORT CONTROLLER_DEADZONE_STICK = 8192;

//トリガーデッドゾーン
//値 0 ～ 255
//デフォルト 16
constexpr BYTE CONTROLLER_DEADZONE_TRIGGER = 16;

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
		for (int frame = LOG_FRAME - 1; frame > 0; frame--)
		{
			memcpy_s(instance->controllerLog[frame], sizeof(XINPUT_STATE) * CONTROLLER_MAX, instance->controllerLog[frame - 1], sizeof(XINPUT_STATE) * CONTROLLER_MAX);
		}

		for (DWORD i = 0; i < CONTROLLER_MAX; i++)
		{
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
					if (GetButtonTrigger((XINPUTBUTTON)buttonID, i))
					{
						std::cout << b << " ";
					}

					buttonID <<= 1;
				}
				XMFLOAT2 stickAxis;
				stickAxis = ConvertRawStickAxis(GetStickAxis(XINPUTDIRECTION_LEFT, i));
				std::cout << "StickLeft X:" << stickAxis.x << " Y:" << stickAxis.y << " (" << GetStickAxis(XINPUTDIRECTION_LEFT, i) << ") ";
				stickAxis = ConvertRawStickAxis(GetStickAxis(XINPUTDIRECTION_RIGHT, i));
				std::cout << "StickRight X:" << stickAxis.x << " Y:" << stickAxis.y << " (" << GetStickAxis(XINPUTDIRECTION_RIGHT, i) << ") ";

				std::cout << "TriggerLeft " << (int)GetTriggerPressure(XINPUTDIRECTION_LEFT, i) << " ";
				std::cout << "TriggerRight " << (int)GetTriggerPressure(XINPUTDIRECTION_RIGHT, i) << " ";

				std::cout << "wParam(" << instance->controllerLog[0][i].Gamepad.wButtons << ")" << std::endl;
				std::cout << "  L Vector X:" << GetStickVector(XINPUTDIRECTION_LEFT, i).x << " Y: " << GetStickVector(XINPUTDIRECTION_LEFT, i).y << " Len: " << MagnitudeVector2(GetStickVector(XINPUTDIRECTION_LEFT)) << std::endl;
				std::cout << "  L VectorDirection X:" << GetStickVectorDirection(XINPUTDIRECTION_LEFT, i).x << " Y: " << GetStickVectorDirection(XINPUTDIRECTION_LEFT, i).y << std::endl;
				std::cout << "  L VectorRatio X:" << GetStickVectorRatio(XINPUTDIRECTION_LEFT, i).x << " Y: " << GetStickVectorRatio(XINPUTDIRECTION_LEFT, i).y << std::endl;
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

	static bool GetButtonTrigger(XINPUTBUTTON buttonType, unsigned int frame = 1, int controllerID = 0)
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

	static bool GetButtonDown(XINPUTBUTTON buttonType, int controllerID = 0)
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

	/*---------------------------------------------------------------------------------------------*/

	static LONG GetStickAxis(XINPUTDIRECTION stick, int controllerID = 0, SHORT deadzone = CONTROLLER_DEADZONE_STICK, int frame = 0)
	{
		unsigned short x, y = 0;

		switch (stick)
		{
			//対応したスティックの傾きを返す(上位16X 下位16Y)
		case XINPUTDIRECTION_LEFT:
			x = instance->controllerLog[frame][controllerID].Gamepad.sThumbLX;
			y = instance->controllerLog[frame][controllerID].Gamepad.sThumbLY;
			break;
		case XINPUTDIRECTION_RIGHT:
			x = instance->controllerLog[frame][controllerID].Gamepad.sThumbRX;
			y = instance->controllerLog[frame][controllerID].Gamepad.sThumbRY;
			break;
		}

		if (x < deadzone || x > 65535 - deadzone) { x = 0; }
		if (y < deadzone || y > 65535 - deadzone) { y = 0; }

		return x << 16 | (y);
	}

	static BYTE GetTriggerPressure(XINPUTDIRECTION trigger, int controllerID = 0, SHORT deadzone = CONTROLLER_DEADZONE_TRIGGER, int frame = 0)
	{
		BYTE returnValue = 0;

		//対応したトリガーの押下圧を取得
		switch (trigger)
		{
		case XINPUTDIRECTION_LEFT:
			returnValue = instance->controllerLog[frame][controllerID].Gamepad.bLeftTrigger;
			break;
		case XINPUTDIRECTION_RIGHT:
			returnValue = instance->controllerLog[frame][controllerID].Gamepad.bRightTrigger;
			break;
		}

		if (returnValue < deadzone)
		{
			return 0;
		}

		return returnValue;
	}

#ifdef DIRECTX_MATH_VERSION
	static XMFLOAT2 ConvertRawStickAxis(LONG rawValue)
	{
		XMFLOAT2 returnValue;
		returnValue.x = rawValue >> 16;
		rawValue <<= 16;
		returnValue.y = rawValue >> 16;

		return returnValue;
	}

	//スティックのベクトル取得
	static XMFLOAT2 GetStickVector(XINPUTDIRECTION direction, int controllerID = 0, int retroactiveFrames = 1, int startingFrame = 0)
	{
		//フレーム数が1未満の場合に0,0を返す
		if (retroactiveFrames < 1)
		{
			return { 0.0f,0.0f };
		}

		switch (direction)
		{
		case XINPUTDIRECTION_LEFT:
		return { (float)(instance->controllerLog[startingFrame][controllerID].Gamepad.sThumbLX - instance->controllerLog[startingFrame + retroactiveFrames][controllerID].Gamepad.sThumbLX),
					 (float)(instance->controllerLog[startingFrame][controllerID].Gamepad.sThumbLY - instance->controllerLog[startingFrame + retroactiveFrames][controllerID].Gamepad.sThumbLY) };
			break;

		case XINPUTDIRECTION_RIGHT:
		return { (float)(instance->controllerLog[startingFrame][controllerID].Gamepad.sThumbRX - instance->controllerLog[startingFrame + retroactiveFrames][controllerID].Gamepad.sThumbRX),
					 (float)(instance->controllerLog[startingFrame][controllerID].Gamepad.sThumbRY - instance->controllerLog[startingFrame + retroactiveFrames][controllerID].Gamepad.sThumbRY) };
			break;

		default:
			return { 0.0f,0.0f };
		}
	}

	//スティックの単位ベクトル取得(正規化後)
	static XMFLOAT2 GetStickVectorDirection(XINPUTDIRECTION direction, int controllerID = 0, int retroactiveFrames = 1, int startingFrame = 0)
	{
		//フレーム数が1未満の場合に0,0を返す
		if (retroactiveFrames < 1)
		{
			return { 0.0f,0.0f };
		}

		XMFLOAT2 returnVector = { 0.0f, 0.0f };

		switch (direction)
		{
		case XINPUTDIRECTION_LEFT:
			returnVector = { (float)(instance->controllerLog[startingFrame][controllerID].Gamepad.sThumbLX - instance->controllerLog[startingFrame + retroactiveFrames][controllerID].Gamepad.sThumbLX),
						 (float)(instance->controllerLog[startingFrame][controllerID].Gamepad.sThumbLY - instance->controllerLog[startingFrame + retroactiveFrames][controllerID].Gamepad.sThumbLY) };
			break;
		case XINPUTDIRECTION_RIGHT:
			returnVector = { (float)(instance->controllerLog[startingFrame][controllerID].Gamepad.sThumbRX - instance->controllerLog[startingFrame + retroactiveFrames][controllerID].Gamepad.sThumbRX),
						 (float)(instance->controllerLog[startingFrame][controllerID].Gamepad.sThumbRY - instance->controllerLog[startingFrame + retroactiveFrames][controllerID].Gamepad.sThumbRY) };
			break;
		}

		return NormalizeVector2(returnVector, MagnitudeVector2(returnVector));
	}

	//スティックの割合ベクトル取得
	static XMFLOAT2 GetStickVectorRatio(XINPUTDIRECTION direction, int controllerID = 0, int retroactiveFrames = 1, int startingFrame = 0)
	{
		//フレーム数が1未満の場合に0,0を返す
		if (retroactiveFrames < 1)
		{
			return { 0.0f,0.0f };
		}

		XMFLOAT2 returnVector = { 0.0f, 0.0f };

		switch (direction)
		{
		case XINPUTDIRECTION_LEFT:
			returnVector = { (float)(instance->controllerLog[startingFrame][controllerID].Gamepad.sThumbLX - instance->controllerLog[startingFrame + retroactiveFrames][controllerID].Gamepad.sThumbLX),
						 (float)(instance->controllerLog[startingFrame][controllerID].Gamepad.sThumbLY - instance->controllerLog[startingFrame + retroactiveFrames][controllerID].Gamepad.sThumbLY) };
			break;
		case XINPUTDIRECTION_RIGHT:
			returnVector = { (float)(instance->controllerLog[startingFrame][controllerID].Gamepad.sThumbRX - instance->controllerLog[startingFrame + retroactiveFrames][controllerID].Gamepad.sThumbRX),
						 (float)(instance->controllerLog[startingFrame][controllerID].Gamepad.sThumbRY - instance->controllerLog[startingFrame + retroactiveFrames][controllerID].Gamepad.sThumbRY) };
			break;
		}

		return { returnVector.x / 65535, returnVector.y / 65535 };
	}

	//2次元ベクトル長さ取得関数
	static inline float MagnitudeVector2(const XMFLOAT2& vector2)
	{
		if (vector2.x == 0 && vector2.y == 0) { return -1.0f; }
		return (float)sqrt(vector2.x * vector2.x + vector2.y * vector2.y);
	}

	//2次元ベクトル正規化関数
	static inline XMFLOAT2 NormalizeVector2(const XMFLOAT2& vector2, float magnitude)
	{
		return { vector2.x / magnitude, vector2.y / magnitude };
	}


#endif // DIRECTX_MATH_VERSION

};

#endif