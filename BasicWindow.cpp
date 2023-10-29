#include"BasicWindow.h"
#include"MainWindow.h"
#include"OBJFVLib.h"
#include"Application.h"
#include"Model.h"
#include<Windows.h>
#include<fstream>
#include<iostream>

using namespace std;

// ウィンドウプロシージャ
// 参考:https://suzulang.com/win32api%E3%81%A7%E3%82%A6%E3%82%A3%E3%83%B3%E3%83%89%E3%82%A6%E3%82%92%E3%82%AB%E3%83%97%E3%82%BB%E3%83%AB%E5%8C%96/
LRESULT CALLBACK BasicWindow::BasicWindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {

	BasicWindow* This = (BasicWindow*)GetWindowLongPtr(hwnd, GWLP_USERDATA); //ウィンドウに紐づけられているインスタンスポインタの取得

	if (!This) { //取得ができない -> 紐づけられていない
		if (msg == WM_CREATE) { //ウィンドウ生成時に、クラスインスタンスとウィンドウハンドルを紐づける
			This = (BasicWindow*)((LPCREATESTRUCT)lparam)->lpCreateParams;
			if (This) {
				SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)This); //ウィンドウのユーザデータにインスタンスポインタをセットする
				return This->WindowProcedure(hwnd, msg, wparam, lparam);
			}
			else { //lpCreateParamsが0 -> CreateWindowが失敗しており、ウィンドウプロシージャが機能しなくなるのでプログラムを終了させる
				PostQuitMessage(0);
			}
		}
	}
	else { //取得ができた -> 紐づけができている
		return This->WindowProcedure(hwnd, msg, wparam, lparam);
	}

	return DefWindowProc(hwnd, msg, wparam, lparam); //何にもないときはデフォルトのウィンドウプロシージャへ
}

// 引き数を受け取り、メンバ変数を初期化
BasicWindow::BasicWindow(HINSTANCE hi, LPCSTR windowName, ModelManager* mm, Camera* c, unsigned int x, unsigned int y, unsigned int width, unsigned int height) : _hInstance(hi), _windowName(windowName), _modelManager(mm), _camera(c), _x(x), _y(y), _width(width), _height(height) {

}

BasicWindow::~BasicWindow() {

}

// Windowを生成
void BasicWindow::CreateNewWindow() {
	// ウィンドウクラスを生成し、登録
	_windowClass.cbSize = sizeof(WNDCLASSEX);
	_windowClass.style = CS_HREDRAW | CS_VREDRAW;
	_windowClass.lpfnWndProc = BasicWindow::BasicWindowProcedure; // コールバック関数の指定
	_windowClass.cbClsExtra = 0;
	_windowClass.cbWndExtra = 0;
	_windowClass.lpszClassName = _T("OBJ File Viewer");// アプリケーションクラス名の指定
	_windowClass.hInstance = _hInstance; // ハンドルを取得し登録
	_windowClass.hIcon = NULL;
	_windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	_windowClass.hbrBackground = NULL;
	_windowClass.lpszMenuName = NULL;
	_windowClass.hIconSm = NULL;

	RegisterClassEx(&_windowClass); // ウィンドウクラスを登録

	// ウィンドウの大きさを定義
	RECT wrc = { 0, 0, _width, _height }; // ウィンドウの大きさを指定
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false); // ウィンドウを調整

	// ウィンドウオブジェクトの生成
	_hwnd = CreateWindow(
		_windowClass.lpszClassName, // クラス名
		_windowName, // タイトルバー名
		WS_OVERLAPPED,
		//WS_OVERLAPPEDWINDOW, // ウィンドウスタイル
		//CW_USEDEFAULT, // 表示x座標はOSにお任せ
		//CW_USEDEFAULT, // 表示y座標はOSにお任せ
		_x,
		_y,
		wrc.right - wrc.left, // ウィンドウの幅
		wrc.bottom - wrc.top, // ウィンドウの高さ
		NULL, // 親ウィンドウハンドル
		NULL, // メニューハンドル
		_windowClass.hInstance, //呼び出しアプリケーションハンドル
		this // 追加パラメータ
	);
}

// string型からfloat型に変換できるか調べる関数
bool BasicWindow::IsConvertibleStringToFloat(string s) {
	int decimalPointNum = 0;
	for (int i = 0; i < s.size(); i++) {
		if (i == 0 && s[i] == '-');
		else if (s[i] == '.') {
			decimalPointNum++;
			if (i == 0 || decimalPointNum > 1) return false;
		}
		else {
			int num = s[i] - '0';
			if (num < 0 || num > 9) return false;
		}
	}
	return true;
}

// Buttonを生成する関数
HWND BasicWindow::CreateButton(int x, int y, int w, int h, int id, string text) {
	return CreateWindow(
		"BUTTON",
		const_cast<LPCSTR>(text.c_str()), // button text
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, // style
		x, // x座標
		y, // y座標
		w, // 幅
		h, // 高さ
		_hwnd, // 親のHWND
		(HMENU)id, // MENU
		_hInstance,
		NULL
	);
}

// EditBoxを生成する関数
HWND BasicWindow::CreateEditBox(int x, int y, int w, int h, int id, string text) {
	return CreateWindowEx(
		0,
		"EDIT",
		const_cast<LPCSTR>(text.c_str()), // text
		WS_CHILD | WS_VISIBLE | WS_BORDER, // style
		x, // x座標
		y, // y座標
		w, // 幅
		h, // 高さ
		_hwnd, // 親のHWND
		(HMENU)id, // MENU
		_hInstance,
		NULL
	);
}
