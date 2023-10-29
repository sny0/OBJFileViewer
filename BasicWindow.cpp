#include"BasicWindow.h"
#include"MainWindow.h"
#include"OBJFVLib.h"
#include"Application.h"
#include"Model.h"
#include<Windows.h>
#include<fstream>
#include<iostream>

using namespace std;

// �E�B���h�E�v���V�[�W��
// �Q�l:https://suzulang.com/win32api%E3%81%A7%E3%82%A6%E3%82%A3%E3%83%B3%E3%83%89%E3%82%A6%E3%82%92%E3%82%AB%E3%83%97%E3%82%BB%E3%83%AB%E5%8C%96/
LRESULT CALLBACK BasicWindow::BasicWindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {

	BasicWindow* This = (BasicWindow*)GetWindowLongPtr(hwnd, GWLP_USERDATA); //�E�B���h�E�ɕR�Â����Ă���C���X�^���X�|�C���^�̎擾

	if (!This) { //�擾���ł��Ȃ� -> �R�Â����Ă��Ȃ�
		if (msg == WM_CREATE) { //�E�B���h�E�������ɁA�N���X�C���X�^���X�ƃE�B���h�E�n���h����R�Â���
			This = (BasicWindow*)((LPCREATESTRUCT)lparam)->lpCreateParams;
			if (This) {
				SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)This); //�E�B���h�E�̃��[�U�f�[�^�ɃC���X�^���X�|�C���^���Z�b�g����
				return This->WindowProcedure(hwnd, msg, wparam, lparam);
			}
			else { //lpCreateParams��0 -> CreateWindow�����s���Ă���A�E�B���h�E�v���V�[�W�����@�\���Ȃ��Ȃ�̂Ńv���O�������I��������
				PostQuitMessage(0);
			}
		}
	}
	else { //�擾���ł��� -> �R�Â����ł��Ă���
		return This->WindowProcedure(hwnd, msg, wparam, lparam);
	}

	return DefWindowProc(hwnd, msg, wparam, lparam); //���ɂ��Ȃ��Ƃ��̓f�t�H���g�̃E�B���h�E�v���V�[�W����
}

// ���������󂯎��A�����o�ϐ���������
BasicWindow::BasicWindow(HINSTANCE hi, LPCSTR windowName, ModelManager* mm, Camera* c, unsigned int x, unsigned int y, unsigned int width, unsigned int height) : _hInstance(hi), _windowName(windowName), _modelManager(mm), _camera(c), _x(x), _y(y), _width(width), _height(height) {

}

BasicWindow::~BasicWindow() {

}

// Window�𐶐�
void BasicWindow::CreateNewWindow() {
	// �E�B���h�E�N���X�𐶐����A�o�^
	_windowClass.cbSize = sizeof(WNDCLASSEX);
	_windowClass.style = CS_HREDRAW | CS_VREDRAW;
	_windowClass.lpfnWndProc = BasicWindow::BasicWindowProcedure; // �R�[���o�b�N�֐��̎w��
	_windowClass.cbClsExtra = 0;
	_windowClass.cbWndExtra = 0;
	_windowClass.lpszClassName = _T("OBJ File Viewer");// �A�v���P�[�V�����N���X���̎w��
	_windowClass.hInstance = _hInstance; // �n���h�����擾���o�^
	_windowClass.hIcon = NULL;
	_windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	_windowClass.hbrBackground = NULL;
	_windowClass.lpszMenuName = NULL;
	_windowClass.hIconSm = NULL;

	RegisterClassEx(&_windowClass); // �E�B���h�E�N���X��o�^

	// �E�B���h�E�̑傫�����`
	RECT wrc = { 0, 0, _width, _height }; // �E�B���h�E�̑傫�����w��
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false); // �E�B���h�E�𒲐�

	// �E�B���h�E�I�u�W�F�N�g�̐���
	_hwnd = CreateWindow(
		_windowClass.lpszClassName, // �N���X��
		_windowName, // �^�C�g���o�[��
		WS_OVERLAPPED,
		//WS_OVERLAPPEDWINDOW, // �E�B���h�E�X�^�C��
		//CW_USEDEFAULT, // �\��x���W��OS�ɂ��C��
		//CW_USEDEFAULT, // �\��y���W��OS�ɂ��C��
		_x,
		_y,
		wrc.right - wrc.left, // �E�B���h�E�̕�
		wrc.bottom - wrc.top, // �E�B���h�E�̍���
		NULL, // �e�E�B���h�E�n���h��
		NULL, // ���j���[�n���h��
		_windowClass.hInstance, //�Ăяo���A�v���P�[�V�����n���h��
		this // �ǉ��p�����[�^
	);
}

// string�^����float�^�ɕϊ��ł��邩���ׂ�֐�
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

// Button�𐶐�����֐�
HWND BasicWindow::CreateButton(int x, int y, int w, int h, int id, string text) {
	return CreateWindow(
		"BUTTON",
		const_cast<LPCSTR>(text.c_str()), // button text
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, // style
		x, // x���W
		y, // y���W
		w, // ��
		h, // ����
		_hwnd, // �e��HWND
		(HMENU)id, // MENU
		_hInstance,
		NULL
	);
}

// EditBox�𐶐�����֐�
HWND BasicWindow::CreateEditBox(int x, int y, int w, int h, int id, string text) {
	return CreateWindowEx(
		0,
		"EDIT",
		const_cast<LPCSTR>(text.c_str()), // text
		WS_CHILD | WS_VISIBLE | WS_BORDER, // style
		x, // x���W
		y, // y���W
		w, // ��
		h, // ����
		_hwnd, // �e��HWND
		(HMENU)id, // MENU
		_hInstance,
		NULL
	);
}
