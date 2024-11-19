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

**bool GetKeyTrigger(WPARAM keyType, unsigned int frame)**
>第一引数 取得したいキータイプ
>第二引数 取得フレーム
## コントローラー入力

You can save any file of the workspace to **Google Drive**, **Dropbox** or **GitHub** by opening the **Synchronize** sub-menu and clicking **Save on**. Even if a file in the workspace is already synced, you can save it to another location. StackEdit can sync one file with multiple locations and accounts.



## SmartyPants


|                |ASCII                          |HTML                         |
|----------------|-------------------------------|-----------------------------|
|Single backticks|`'Isn't this fun?'`            |'Isn't this fun?'            |
|Quotes          |`"Isn't this fun?"`            |"Isn't this fun?"            |
|Dashes          |`-- is en-dash, --- is em-dash`|-- is en-dash, --- is em-dash|


