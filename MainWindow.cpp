#include "MainWindow.h"

MainWindow::MainWindow(HINSTANCE hi, LPCSTR windowName, ModelManager* mm, Camera* c, unsigned int x, unsigned int y, unsigned int width, unsigned int height)
	: BasicWindow(hi, windowName, mm, c, x, y, width, height) {

}

MainWindow::~MainWindow() {

}

// ウィンドウプロシージャ関数
LRESULT MainWindow::WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {

	if (msg == WM_DESTROY) {
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, msg, wparam, lparam);
}
