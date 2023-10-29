#pragma once
#include"OBJFVLib.h"
#include"ModelManager.h"
#include"Model.h"
#include"Camera.h"
#include<Windows.h>
#include<commctrl.h>
#include<tchar.h>
#include<d3d12.h>
#include<iostream>
#include<dxgi1_6.h>
#include<DirectXMath.h>
#include<vector>
#include<map>
#include<d3dcompiler.h>
#include<DirectXTex.h>
#include<d3dx12.h>
#include<wrl.h>

#pragma comment(lib,"DirectXTex.lib")
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"d3dcompiler.lib")

using namespace std;
using namespace DirectX;


class BasicWindow {
public:
	void CreateNewWindow();

	BasicWindow(HINSTANCE hi, LPCSTR windowName, ModelManager* mm, Camera* c, unsigned int x, unsigned int y, unsigned int width, unsigned int height);

	~BasicWindow();


protected:
	unsigned int _width = 500;
	unsigned int _height = 500;
	unsigned int _x = 0;
	unsigned int _y = 0;

	HINSTANCE _hInstance;
	// ビューポート
	//CD3DX12_VIEWPORT _viewPort;

	// シザー矩形
	//CD3DX12_RECT _scissorRect;

	LPCSTR _windowName = "OBJFILEVIEWER";

	HWND test_button_hwnd;

	ModelManager* _modelManager;

	Camera* _camera;

	bool IsConvertibleStringToFloat(string s);

protected:
	HWND CreateButton(int x, int y, int w, int h, int id, string text);

	HWND CreateEditBox(int x, int y, int w, int h, int id, string text);


public:
	HWND _hwnd;
	WNDCLASSEX _windowClass;

public:
	static LRESULT CALLBACK BasicWindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	virtual LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) = 0;
};