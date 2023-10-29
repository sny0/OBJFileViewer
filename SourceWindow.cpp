#include "SourceWindow.h"

SourceWindow::SourceWindow(HINSTANCE hi, LPCSTR windowName, ModelManager* mm, Camera* c, unsigned int x, unsigned int y, unsigned int width, unsigned int height)
	: BasicWindow(hi, windowName, mm, c, x, y, width, height) {

}

void SourceWindow::Init(ModelWindow* mw) {
	_modelWindow = mw;
	_hwndQuitBtn = CreateButton(0 + 50, 0, _width / 4 - 100, _btnHeight, _quitId, "Quit App");
	_hwndDuplBtn = CreateButton(0 + 50, _btnHeight * 1.5, _width / 4 - 100, _btnHeight, _duplicateId, "Duplicate Model");
	_hwndDeleBtn = CreateButton(0 + 50, _btnHeight * 3, _width / 4 - 100, _btnHeight, _deleteId, "Delete Model");
}

SourceWindow::~SourceWindow() {

}

// CameraやLightのEditBoxを生成する関数
void SourceWindow::CreateCameraEditBox() {
	int offsetY = 0;
	_cameraY = offsetY;
	offsetY += _baseHeight;

	_cpY = offsetY;
	_hwnds[0] = CreateEditBox((_width * 2 / 4) + (_width / 4) * (0 + 1) / 4, offsetY, _width / 4 / 4, _baseHeight, _cpxId, to_string(_camera->_transform.position.x));
	_hwnds[1] = CreateEditBox((_width * 2 / 4) + (_width / 4) * (1 + 1) / 4, offsetY, _width / 4 / 4, _baseHeight, _cpyId, to_string(_camera->_transform.position.y));
	_hwnds[2] = CreateEditBox((_width * 2 / 4) + (_width / 4) * (2 + 1) / 4, offsetY, _width / 4 / 4, _baseHeight, _cpzId, to_string(_camera->_transform.position.z));
	offsetY += _baseHeight + _padding;

	_crY = offsetY;
	_hwnds[3] = CreateEditBox((_width * 2 / 4) + (_width / 4) * (0 + 1) / 4, offsetY, _width / 4 / 4, _baseHeight, _crxId, to_string(_camera->_transform.rotation.x));
	_hwnds[4] = CreateEditBox((_width * 2 / 4) + (_width / 4) * (1 + 1) / 4, offsetY, _width / 4 / 4, _baseHeight, _cryId, to_string(_camera->_transform.rotation.y));
	_hwnds[5] = CreateEditBox((_width * 2 / 4) + (_width / 4) * (2 + 1) / 4, offsetY, _width / 4 / 4, _baseHeight, _crzId, to_string(_camera->_transform.rotation.z));
	offsetY += _baseHeight + _padding;

	_cfovY = offsetY;
	_hwnds[6] = CreateEditBox((_width * 2 / 4) + (_width / 4) * (0 + 1) / 2, offsetY, _width / 4 / 2, _baseHeight, _cfId, to_string(_camera->_fov));
	offsetY += _baseHeight + _padding;

	_ccpY = offsetY;
	_hwnds[7] = CreateEditBox((_width * 2 / 4) + (_width / 4) * (1 + 1) / 4, offsetY, _width / 4 / 4, _baseHeight, _cnz, to_string(_camera->_clippingPlanes.x));
	_hwnds[8] = CreateEditBox((_width * 2 / 4) + (_width / 4) * (2 + 1) / 4, offsetY, _width / 4 / 4, _baseHeight, _cfz, to_string(_camera->_clippingPlanes.y));

	offsetY = 0;
	_dirLigY = offsetY;
	offsetY += _baseHeight;
	_dlvY = offsetY;
	_hwnds[11] = CreateEditBox((_width * 3 / 4) + (_width / 4) * (0 + 1) / 4, offsetY, _width / 4 / 4, _baseHeight, _lvxId, to_string(_camera->_sceneData.ligVec.x));
	_hwnds[12] = CreateEditBox((_width * 3 / 4) + (_width / 4) * (1 + 1) / 4, offsetY, _width / 4 / 4, _baseHeight, _lvyId, to_string(_camera->_sceneData.ligVec.y));
	_hwnds[13] = CreateEditBox((_width * 3 / 4) + (_width / 4) * (2 + 1) / 4, offsetY, _width / 4 / 4, _baseHeight, _lvzId, to_string(_camera->_sceneData.ligVec.z));
	offsetY += _baseHeight + _padding;

	_dlcY = offsetY;
	_hwnds[14] = CreateEditBox((_width * 3 / 4) + (_width / 4) * (0 + 1) / 4, offsetY, _width / 4 / 4, _baseHeight, _lcxId, to_string(_camera->_sceneData.ligCol.x));
	_hwnds[15] = CreateEditBox((_width * 3 / 4) + (_width / 4) * (1 + 1) / 4, offsetY, _width / 4 / 4, _baseHeight, _lcyId, to_string(_camera->_sceneData.ligCol.y));
	_hwnds[16] = CreateEditBox((_width * 3 / 4) + (_width / 4) * (2 + 1) / 4, offsetY, _width / 4 / 4, _baseHeight, _lczId, to_string(_camera->_sceneData.ligCol.x));
	offsetY += _baseHeight + _padding;

	_backgroundY = offsetY;
	offsetY += _baseHeight + _padding;

	_bgcY = offsetY;
	_hwnds[17] = CreateEditBox((_width * 3 / 4) + (_width / 4) * (0 + 1) / 4, offsetY, _width / 4 / 4, _baseHeight, _bcxId, to_string(_camera->_background.x));
	_hwnds[18] = CreateEditBox((_width * 3 / 4) + (_width / 4) * (1 + 1) / 4, offsetY, _width / 4 / 4, _baseHeight, _bcyId, to_string(_camera->_background.y));
	_hwnds[19] = CreateEditBox((_width * 3 / 4) + (_width / 4) * (2 + 1) / 4, offsetY, _width / 4 / 4, _baseHeight, _bczId, to_string(_camera->_background.z));
}

// Source(objファイル)を表示するComboBoxを生成する関数
void SourceWindow::CreateSourceButtons(map<string, bool> objFiles) {
	_sources.clear();
	for (auto f : objFiles) {
		Source tmpSrc;
		tmpSrc.name = f.first;
		tmpSrc.isMTLExist = f.second;
		_sources.push_back(tmpSrc);
	}

	_hwndComboSrc = CreateWindowEx(0, "COMBOBOX", "", WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST, _width / 4, _srccbY, _width / 4, 1000, _hwnd, (HMENU)_srcId, _hInstance, NULL);
	for (int i = 0; i < _sources.size(); i++) {
		string btnNameStr = _sources[i].name + "(OBJ";
		if (_sources[i].isMTLExist) {
			btnNameStr += ", MTL";
		}
		btnNameStr += ")";
		SendMessage(_hwndComboSrc, CB_ADDSTRING, 0, (LPARAM)btnNameStr.c_str());
	}

	CreateCameraEditBox();

}

// ウィンドウプロシージャ関数
LRESULT SourceWindow::WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	//std::cout << "sourceWindowPro" << endl;
	switch (msg) {
	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);
		// 赤色でクライアント領域を塗りつぶす
		HBRUSH hRedBrush = CreateSolidBrush(RGB(0xFF, 0xFF, 0xFF));
		FillRect(hdc, &ps.rcPaint, hRedBrush);

		DeleteObject(hRedBrush);

		TextOut(hdc, _width / 4, _srcY, "Sources", 7);

		TextOut(hdc, _width * 2 / 4, _cameraY, const_cast<LPCSTR>(TEXT("Camera")), 6);
		for (int i = 0; i < 3; i++) {
			TextOut(hdc, _width * 2 / 4 + _width * (1 + i) / 4 / 4, _cameraY, const_cast<LPCSTR>(_xyz[i].c_str()), _xyz[i].size());
		}

		TextOut(hdc, _width * 2 / 4, _cpY, const_cast<LPCSTR>(_editNameSRT[0].c_str()), _editNameSRT[0].size());
		TextOut(hdc, _width * 2 / 4, _crY, const_cast<LPCSTR>(_editNameSRT[1].c_str()), _editNameSRT[1].size());
		TextOut(hdc, _width * 2 / 4, _cfovY, const_cast<LPCSTR>(TEXT("Fov[Vertical]")), 13);
		TextOut(hdc, _width * 2 / 4, _ccpY, const_cast<LPCSTR>(TEXT("Clipping Planes[Near, Far]")), 26);


		TextOut(hdc, _width * 3 / 4, _dirLigY, const_cast<LPCSTR>(TEXT("Dir Light")), 9);
		for (int i = 0; i < 3; i++) {
			TextOut(hdc, _width * 3 / 4 + _width * (1 + i) / 4 / 4, _dirLigY, const_cast<LPCSTR>(_xyz[i].c_str()), _xyz[i].size());
		}
		TextOut(hdc, _width * 3 / 4, _dlvY, const_cast<LPCSTR>(TEXT("vector")), 6);
		TextOut(hdc, _width * 3 / 4, _dlcY, const_cast<LPCSTR>(TEXT("color")), 5);

		TextOut(hdc, _width * 3 / 4, _backgroundY, const_cast<LPCSTR>(TEXT("Background")), 10);
		for (int i = 0; i < 3; i++) {
			TextOut(hdc, _width * 3 / 4 + _width * (1 + i) / 4 / 4, _backgroundY, const_cast<LPCSTR>(_rgb[i].c_str()), _rgb[i].size());
		}
		TextOut(hdc, _width * 3 / 4, _bgcY, const_cast<LPCSTR>(TEXT("color")), 5);
		EndPaint(hwnd, &ps);
		return DefWindowProc(hwnd, msg, wparam, lparam);
	}
	case WM_COMMAND:
		int wmId = LOWORD(wparam);
		int wmEvent = HIWORD(wparam);

		if (wmId == _quitId) {
			MessageBox(hwnd, "Quite", "", MB_OK);
			PostQuitMessage(0);
		}
		else if (wmId == _duplicateId) {
			_modelWindow->DuplicateModel();
		}
		else if (wmId == _deleteId) {
			_modelWindow->DeleteModel();
		}

		else if (wmId >= 0 && wmId <= 20) {
			if (wmEvent == EN_CHANGE) {
				char buffer[128];
				GetWindowText(_hwnds[wmId], buffer, sizeof(buffer));
				string bufferS(buffer);
				if (bufferS == "") break;
				if (bufferS == "-") break;
				if (bufferS[0] == '.') bufferS = "0" + bufferS;
				cout << "buffer" << bufferS << endl;

				if (IsConvertibleStringToFloat(bufferS)) {
					float value = stof(bufferS);
					if (wmId == _cfz && value < 1) {
						value = 1.0f;
						SetWindowText(_hwnds[wmId], "1");
						MessageBox(hwnd, "Clipping Planes[Far]は1以上にしてください。", "入力エラー", MB_OK);
					}
					if (wmId == _cfz && value <= _camera->_clippingPlanes.x) {
						value = _camera->_clippingPlanes.x + 1;
						SetWindowText(_hwnds[wmId], to_string(value).c_str());
						MessageBox(hwnd, "Clipping Planesは、near < far である必要があります。", "入力エラー", MB_OK);
					}
					if (wmId == _cnz && value >= _camera->_clippingPlanes.y) {
						value = _camera->_clippingPlanes.y - 0.1;
						SetWindowText(_hwnds[wmId], to_string(value).c_str());
						MessageBox(hwnd, "Clipping Planesは、near < far である必要があります。", "入力エラー", MB_OK);
					}
					if (wmId == _cfId || wmId == _cnz || wmId == _cfz) {
						if (value == 0) {
							value = 0.1;
							MessageBox(hwnd, "ここには0を入力することはできません。", "入力エラー", MB_OK);
							SetWindowText(_hwnds[wmId], "0.1");
						}
					}
					ChangeModelInfo(wmId, value);
				}
				else {
					MessageBox(hwnd, "有効な数値を入力してください。", "入力エラー", MB_OK);
				}
			}
		}
		else if (wmEvent == CBN_SELCHANGE) {
			int comboBoxIndex;
			char selectedTexName[256];
			comboBoxIndex = SendMessage(_hwndComboSrc, CB_GETCURSEL, 0, 0);
			if (comboBoxIndex != -1) {
				SendMessage(_hwndComboSrc, CB_GETLBTEXT, comboBoxIndex, (LPARAM)selectedTexName);
				string message(selectedTexName);
				_modelManager->CreateModel(_sources[comboBoxIndex].name, _sources[comboBoxIndex].isMTLExist);
				_modelWindow->CreateModelTree();
				message += "が選択されました。";
				MessageBox(hwnd, message.c_str(), "OK", MB_OK);
				SendMessage(_hwndComboSrc, CB_SETCURSEL, (WPARAM)-1, 0);
			}
		}
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

// CameraやLightなどの情報を更新する関数
void SourceWindow::ChangeModelInfo(int id, float v) {
	if (id == _cpxId) _camera->_transform.position.x = v;
	else if (id == _cpyId) _camera->_transform.position.y = v;
	else if (id == _cpzId) _camera->_transform.position.z = v;
	else if (id == _crxId) _camera->_transform.rotation.x = v;
	else if (id == _cryId) _camera->_transform.rotation.y = v;
	else if (id == _crzId) _camera->_transform.rotation.z = v;
	else if (id == _cfId) _camera->_fov = v;
	else if (id == _cnz) _camera->_clippingPlanes.x = v;
	else if (id == _cfz) _camera->_clippingPlanes.y = v;
	else if (id == _lvxId) _camera->_sceneData.ligVec.x = v;
	else if (id == _lvyId) _camera->_sceneData.ligVec.y = v;
	else if (id == _lvzId) _camera->_sceneData.ligVec.z = v;
	else if (id == _lcxId) _camera->_sceneData.ligCol.x = v;
	else if (id == _lcyId) _camera->_sceneData.ligCol.y = v;
	else if (id == _lczId) _camera->_sceneData.ligCol.z = v;
	else if (id == _bcxId) _camera->_background.x = v;
	else if (id == _bcyId) _camera->_background.y = v;
	else if (id == _bczId) _camera->_background.z = v;
	_camera->Update();
}