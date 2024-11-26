# CPPKeyLogger

**C++用のキーボード、コントローラー入力を取得するヘッダーライブラリ**
※Xinputのみ対応

# インストール

ヘッダファイルを[ダウンロード](https://github.com/isicoro8372/CPPKeyLogger/raw/refs/heads/main/keylogger.h)し、エントリーポイント(Main関数が置かれているファイル)にグローバル変数として以下を定義してください。
		
	KeyLogger* KeyLogger::instance = nullptr;

ウィンドウプロシージャコールバック関数(WndProc)内の一番上に追加してください。
	
	KeyLogger::StoreKeyLog(uMsg, wParam);

ゲームフレーム適応後のアップデート処理(WinMain下のUpdate関数内)に追加してください。
	
	KeyLogger::UpdateKeyLog();


# 使用方法

キーボード、コントローラーの入力を取得、使用したいファイルにインクルード。
取得したい入力に対応する関数を実行する。

# 関数一覧



## キーボード入力

***bool*** <ins>**GetKeyTrigger**</ins> **( *WPARAM*** keyType, ***unsigned int*** frame = 1 **)**
>|**引数**|WPARAM| unsigned int||**戻り値**
>|-|-|-|-|-|
>|**説明**|`取得したいキータイプ`|`遅延フレーム`||`結果`
>|**取れる値**|'(KeyChar)' or VK_| 1 ~ LOG_FRAME||bool
>
>遅延フレーム分キーを押し続けられた後、放されるまでtrueを返します

***bool*** <ins>**GetKeyDown**</ins> **( *WPARAM*** keyType **)**
>|**引数**|WPARAM||**戻り値**
>|-|-|-|-|
>|**説明**|`取得したいキータイプ`||`結果`
>|**取れる値**|'(KeyChar)' or VK_||bool
>
>現在フレームでキーボードを押された場合にtrueを返します


***bool*** <ins>**GetKeyUp**</ins> **( *WPARAM*** keyType **)**
>|**引数**|WPARAM||**戻り値**
>|-|-|-|-|
>|**説明**|`取得したいキータイプ`||`結果`
>|**取れる値**|'(KeyChar)' or VK_||bool
>
>現在フレームでキーボードから離された場合にtrueを返します



## コントローラー入力
***bool*** <ins>**GetButtonTrigger**</ins>  
**( *XINPUTBUTTON*** buttonType, ***unsigned int*** frame = 1, ***int*** controllerID = 0 **)**
>|**引数**|XINPUTBUTTON| unsigned int|int||**戻り値**
>|-|-|-|-|-|-|
>|**説明**|`取得したいボタンタイプ`|`遅延フレーム`|`コントローラーのID`||`結果`
>|**取れる値**|-| 1 ~ LOG_FRAME|0 ~ CONTROLELR_MAX||bool
>
>遅延フレーム分押し続けられた後、放されるまでtrueを返します

***bool*** <ins>**GetButtonDown**</ins> **( *XINPUTBUTTON*** buttonType, ***int*** controllerID = 0 **)**
>|**引数**|XINPUTBUTTON|int||**戻り値**
>|-|-|-|-|-|
>|**説明**|`取得したいボタンタイプ`|`コントローラーのID`|||`結果`
>|**取れる値**|-|0 ~ CONTROLELR_MAX||bool
>
>現在フレームで押された場合にtrueを返します


***bool*** <ins>**GetButtonUp**</ins> **( *XINPUTBUTTON*** buttonType, ***int*** controllerID = 0 **)**
>|**引数**|XINPUTBUTTON|int||**戻り値**
>|-|-|-|-|-|
>|**説明**|`取得したいボタンタイプ`|`コントローラーのID`|||`結果`
>|**取れる値**|-|0 ~ CONTROLELR_MAX||bool
>
>現在フレームで離された場合にtrueを返します

***LONG*** <ins>**GetStickAxis**</ins>  
**( *XINPUTDIRECTION*** stick, ***int*** controllerID = 0, ***SHORT*** deadzone = CONTROLLER_DEADZONE_STICK, ***int*** frame = 0 **)**
>|**引数**|XINPUTDIRECTION| int|SHORT |int ||**戻り値**
>|-|-|-|-|-|-|-|
>|**説明**|`取得したいアナログスティック`|`コントローラーのID`|`デッドゾーン`|`対象フレーム`||`座標`
>|**取れる値**|-| 0 ~ CONTROLELR_MAX|0 ~ 32,767|0 ~ LOG_FRAME||-2,147,483,648 ~ 2,147,483,647|
>
>上位16ビットがX座標、下位16ビットがY座標として返ってきます

***BYTE*** <ins>**GetTriggerPressure**</ins>  
**( *XINPUTDIRECTION*** trigger, ***int*** controllerID = 0, ***SHORT*** deadzone = CONTROLLER_DEADZONE_TRIGGER, ***int*** frame = 0 **)**
>|**引数**|XINPUTDIRECTION| int|SHORT |int ||**戻り値**
>|-|-|-|-|-|-|-|
>|**説明**|`取得したいトリガー`|`コントローラーのID`|`デッドゾーン`|`対象フレーム`||`押下圧`
>|**取れる値**|-| 0 ~ CONTROLELR_MAX|0 ~ 32,767|0 ~ LOG_FRAME||0 ~ 255|
>|||||||
>
>トリガーの押下圧を返します

***XINPUT_STATE*** <ins>**GetControllerState**</ins> **( *int*** index **)**
>|**引数**|int||**戻り値**
>|-|-|-|-|
>|**説明**|`コントローラーのID`|||`XINPUT_STATEのポインタ`
>|**取れる値**|0 ~ CONTROLELR_MAX||XINPUT_STATE
>
>現在フレームで押された場合にtrueを返します

## DirectX連携関数
***XMFLOAT2*** <ins>**ConvertRawStickAxis**</ins> **( *LONG*** rawValue **)**
>|**引数**|LONG||**戻り値**
>|-|-|-|-|
>|**説明**|`スティックの傾きの生の値`||`XMFLOAT2に変換されたスティック座標`
>|**取れる値**|-2,147,483,648 ~ 2,147,483,647| |{ -32768 ~ 32767, -32768 ~ 32767 }|
>|||||||
>
><ins><B>GetStickAxis()</B></ins>で取得したスティックの傾きをXMFLOAT2型にして返します

***XMFLOAT2*** <ins>**GetStickVector**</ins>  
**( *XINPUTDIRECTION*** direction, ***int*** controllerID = 0, ***int*** retroactiveFrames = 1, ***int*** startingFrame = 0 **)**
>|**引数**|XINPUTDIRECTION| int | int | int ||**戻り値**
>|-|-|-|-|-|-|-|
>|**説明**|`取得したいスティック`|`コントローラーのID`|`起点フレーム`|`終点フレーム`||`単位ベクトル`
>|**取れる値**|-| 0 ~ CONTROLELR_MAX|0 ~ LOG_FRAME|0 ~ LOG_FRAME||{ -1 ~ 1, -1 ~ 1 }|
>|||||||
>
>対象スティックの起点フレームから終点フレームまでの間に動かされたスティックのベクトルをXMFLOAT2型で返します

***XMFLOAT2*** <ins>**GetStickVectorDirection**</ins>  
**( *XINPUTDIRECTION*** direction, ***int*** controllerID = 0, ***int*** retroactiveFrames = 1, ***int*** startingFrame = 0 **)**
>|**引数**|XINPUTDIRECTION| int | int | int ||**戻り値**
>|-|-|-|-|-|-|-|
>|**説明**|`取得したいスティック`|`コントローラーのID`|`起点フレーム`|`終点フレーム`||`単位ベクトル`
>|**取れる値**|-| 0 ~ CONTROLELR_MAX|0 ~ LOG_FRAME|0 ~ LOG_FRAME||{ -32768 ~ 32767, -32768 ~ 32767 }|
>|||||||
>
>対象スティックの起点フレームから終点フレームまでの間に動かされたスティックのベクトルを正規化してXMFLOAT2型で返します

***XMFLOAT2*** <ins>**GetStickVectorRatio**</ins>  
**( *XINPUTDIRECTION*** direction, ***int*** controllerID = 0, ***int*** retroactiveFrames = 1, ***int*** startingFrame = 0 **)**
>|**引数**|XINPUTDIRECTION| int | int | int ||**戻り値**
>|-|-|-|-|-|-|-|
>|**説明**|`取得したいスティック`|`コントローラーのID`|`起点フレーム`|`終点フレーム`||`割合ベクトル`
>|**取れる値**|-| 0 ~ CONTROLELR_MAX|0 ~ LOG_FRAME|0 ~ LOG_FRAME||{ -1 ~ 1, -1 ~ 1 }|
>|||||||
>
>対象スティックの起点フレームから終点フレームまでの間に動かされたスティックのベクトルの割合をXMFLOAT2型で返します
