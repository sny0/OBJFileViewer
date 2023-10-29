#include"DetailWindow.h"

DetailWindow::DetailWindow(HINSTANCE hi, LPCSTR windowName, ModelManager* mm, Camera* c, unsigned int x, unsigned int y, unsigned int width, unsigned int height) : BasicWindow(hi, windowName, mm, c, x, y, width, height) {
}

void DetailWindow::CreateEditBoxes() {
	DeleteChildWindow();

	int offsetX = 0;
	int offsetY = 0;

	if (_pId > 0) {
		_shouldRender = true;
		string modelName = _modelManager->_models[_pId - 1]->_name;

		_modelNameY = offsetY;
		offsetY += _baseHeight + _padding;
		if (_cId == 0) {
			CreateSRTEditBox(offsetY, true);
		}
		else {
			string subModelName = _modelManager->_models[_pId - 1]->_subModels[_cId - 1]->_materialName;

			_subModelNameY = offsetY;
			offsetY += _baseHeight + _padding;

			CreateSRTEditBox(offsetY, false);

			offsetY += _baseHeight;

			CreateMaterialEditBox(offsetY);

			offsetY += _baseHeight;

			_texY = offsetY;

		}
	}

	InvalidateRect(_hwnd, NULL, false);

}

void DetailWindow::CreateSRTEditBox(int& offsetY, bool isModel) {

	_srtY = offsetY;
	offsetY += _baseHeight;

	_posY = offsetY;
	if (isModel) {
		_hwndEditBoxes[0] = CreateEditBox(_width * (0 + 1) / 4, offsetY, _width / 4, _baseHeight, _spxId, to_string(_modelManager->_models[_pId - 1]->_transform.position.x));
		_hwndEditBoxes[1] = CreateEditBox(_width * (1 + 1) / 4, offsetY, _width / 4, _baseHeight, _spyId, to_string(_modelManager->_models[_pId - 1]->_transform.position.y));
		_hwndEditBoxes[2] = CreateEditBox(_width * (2 + 1) / 4, offsetY, _width / 4, _baseHeight, _spzId, to_string(_modelManager->_models[_pId - 1]->_transform.position.z));
	}
	else {
		_hwndEditBoxes[0] = CreateEditBox(_width * (0 + 1) / 4, offsetY, _width / 4, _baseHeight, _spxId, to_string(_modelManager->_models[_pId - 1]->_subModels[_cId - 1]->_transform.position.x));
		_hwndEditBoxes[1] = CreateEditBox(_width * (1 + 1) / 4, offsetY, _width / 4, _baseHeight, _spyId, to_string(_modelManager->_models[_pId - 1]->_subModels[_cId - 1]->_transform.position.y));
		_hwndEditBoxes[2] = CreateEditBox(_width * (2 + 1) / 4, offsetY, _width / 4, _baseHeight, _spzId, to_string(_modelManager->_models[_pId - 1]->_subModels[_cId - 1]->_transform.position.z));
	}
	offsetY += _baseHeight;
	offsetY += _padding;

	_rotY = offsetY;
	if (isModel) {
		_hwndEditBoxes[3] = CreateEditBox(_width * (0 + 1) / 4, offsetY, _width / 4, _baseHeight, _srxId, to_string(_modelManager->_models[_pId - 1]->_transform.rotation.x));
		_hwndEditBoxes[4] = CreateEditBox(_width * (1 + 1) / 4, offsetY, _width / 4, _baseHeight, _sryId, to_string(_modelManager->_models[_pId - 1]->_transform.rotation.y));
		_hwndEditBoxes[5] = CreateEditBox(_width * (2 + 1) / 4, offsetY, _width / 4, _baseHeight, _srzId, to_string(_modelManager->_models[_pId - 1]->_transform.rotation.z));
	}
	else {
		_hwndEditBoxes[3] = CreateEditBox(_width * (0 + 1) / 4, offsetY, _width / 4, _baseHeight, _srxId, to_string(_modelManager->_models[_pId - 1]->_subModels[_cId - 1]->_transform.rotation.x));
		_hwndEditBoxes[4] = CreateEditBox(_width * (1 + 1) / 4, offsetY, _width / 4, _baseHeight, _sryId, to_string(_modelManager->_models[_pId - 1]->_subModels[_cId - 1]->_transform.rotation.y));
		_hwndEditBoxes[5] = CreateEditBox(_width * (2 + 1) / 4, offsetY, _width / 4, _baseHeight, _srzId, to_string(_modelManager->_models[_pId - 1]->_subModels[_cId - 1]->_transform.rotation.z));
	}
	offsetY += _baseHeight;
	offsetY += _padding;

	_scaY = offsetY;
	if (isModel) {
		_hwndEditBoxes[6] = CreateEditBox(_width * (0 + 1) / 4, offsetY, _width / 4, _baseHeight, _ssxId, to_string(_modelManager->_models[_pId - 1]->_transform.scale.x));
		_hwndEditBoxes[7] = CreateEditBox(_width * (1 + 1) / 4, offsetY, _width / 4, _baseHeight, _ssyId, to_string(_modelManager->_models[_pId - 1]->_transform.scale.y));
		_hwndEditBoxes[8] = CreateEditBox(_width * (2 + 1) / 4, offsetY, _width / 4, _baseHeight, _sszId, to_string(_modelManager->_models[_pId - 1]->_transform.scale.z));
	}
	else {
		_hwndEditBoxes[6] = CreateEditBox(_width * (0 + 1) / 4, offsetY, _width / 4, _baseHeight, _ssxId, to_string(_modelManager->_models[_pId - 1]->_subModels[_cId - 1]->_transform.scale.x));
		_hwndEditBoxes[7] = CreateEditBox(_width * (1 + 1) / 4, offsetY, _width / 4, _baseHeight, _ssyId, to_string(_modelManager->_models[_pId - 1]->_subModels[_cId - 1]->_transform.scale.y));
		_hwndEditBoxes[8] = CreateEditBox(_width * (2 + 1) / 4, offsetY, _width / 4, _baseHeight, _sszId, to_string(_modelManager->_models[_pId - 1]->_subModels[_cId - 1]->_transform.scale.z));
	}
	offsetY += _baseHeight;
	offsetY += _padding;
}

void DetailWindow::CreateMaterialEditBox(int& offsetY) {
	_materialY = offsetY;

	offsetY += _baseHeight;

	_ambY = offsetY;

	_hwndEditBoxes[9] = CreateEditBox(_width * (0 + 1) / 4, offsetY, _width / 4, _baseHeight, _maxId, to_string(_modelManager->_models[_pId - 1]->_subModels[_cId - 1]->_material.ambient.x));
	_hwndEditBoxes[10] = CreateEditBox(_width * (1 + 1) / 4, offsetY, _width / 4, _baseHeight, _mayId, to_string(_modelManager->_models[_pId - 1]->_subModels[_cId - 1]->_material.ambient.y));
	_hwndEditBoxes[11] = CreateEditBox(_width * (2 + 1) / 4, offsetY, _width / 4, _baseHeight, _mazId, to_string(_modelManager->_models[_pId - 1]->_subModels[_cId - 1]->_material.ambient.z));


	offsetY += _baseHeight;
	offsetY += _padding;

	_dfsY = offsetY;

	_hwndEditBoxes[12] = CreateEditBox(_width * (0 + 1) / 4, offsetY, _width / 4, _baseHeight, _mdxId, to_string(_modelManager->_models[_pId - 1]->_subModels[_cId - 1]->_material.diffuse.x));
	_hwndEditBoxes[13] = CreateEditBox(_width * (1 + 1) / 4, offsetY, _width / 4, _baseHeight, _mdyId, to_string(_modelManager->_models[_pId - 1]->_subModels[_cId - 1]->_material.diffuse.y));
	_hwndEditBoxes[14] = CreateEditBox(_width * (2 + 1) / 4, offsetY, _width / 4, _baseHeight, _mdzId, to_string(_modelManager->_models[_pId - 1]->_subModels[_cId - 1]->_material.diffuse.z));

	offsetY += _baseHeight;
	offsetY += _padding;

	_spcY = offsetY;

	_hwndEditBoxes[15] = CreateEditBox(_width * (0 + 1) / 4, offsetY, _width / 4, _baseHeight, _msxId, to_string(_modelManager->_models[_pId - 1]->_subModels[_cId - 1]->_material.specular.x));
	_hwndEditBoxes[16] = CreateEditBox(_width * (1 + 1) / 4, offsetY, _width / 4, _baseHeight, _msyId, to_string(_modelManager->_models[_pId - 1]->_subModels[_cId - 1]->_material.specular.y));
	_hwndEditBoxes[17] = CreateEditBox(_width * (2 + 1) / 4, offsetY, _width / 4, _baseHeight, _mszId, to_string(_modelManager->_models[_pId - 1]->_subModels[_cId - 1]->_material.specular.z));


	offsetY += _baseHeight;
	offsetY += _padding;

	_dY = offsetY;
	_hwndEditBoxes[18] = CreateEditBox(_width / 2, offsetY, _width / 2, _baseHeight, _mdId, to_string(_modelManager->_models[_pId - 1]->_subModels[_cId - 1]->_material.dissolve));
	offsetY += _baseHeight;
	offsetY += _padding;

	_nsY = offsetY;
	_hwndEditBoxes[19] = CreateEditBox(_width / 2, offsetY, _width / 2, _baseHeight, _mnsId, to_string(_modelManager->_models[_pId - 1]->_subModels[_cId - 1]->_material.specularity));
	offsetY += _baseHeight;
	offsetY += _padding;
}

HWND DetailWindow::CreateEditBox(int x, int y, int w, int h, int id, string n) {
	return CreateWindowEx(0, "EDIT", const_cast<LPCSTR>(n.c_str()), WS_CHILD | WS_VISIBLE | WS_BORDER, x, y, w, h, _hwnd, (HMENU)id, _hInstance, NULL);
}

DetailWindow::~DetailWindow() {}

void DetailWindow::DeleteChildWindow() {
	for (int i = 0; i < 20; i++) {
		DestroyWindow(_hwndEditBoxes[i]);
	}
}

void DetailWindow::SetComboBox() {
	SendMessage(_hwndComboTex, CB_RESETCONTENT, 0, 0);
	for (int i = 0; i < _pngFiles.size(); i++) {
		SendMessage(_hwndComboTex, CB_ADDSTRING, 0, (LPARAM)_pngFiles[i].c_str());
	}
	if (_pId > 0) {
		for (int i = 0; i < _pngFiles.size(); i++) {
			if (_pngFiles[i] == _modelManager->_models[_pId - 1]->_subModels[_cId - 1]->_textureName) {
				SendMessage(_hwndComboTex, CB_SETCURSEL, (WPARAM)i, 0);
				_selectedTexId = i;
				return;
			}
		}
	}
	SendMessage(_hwndComboTex, CB_SETCURSEL, (WPARAM)-1, 0);
}

LRESULT DetailWindow::WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	switch (msg) {

	case WM_CREATE: {
		_hwndComboTex = CreateWindowEx(0, "COMBOBOX", "", WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST, 0, 0, _width, _texComboH, hwnd, (HMENU)_texId, _hInstance, NULL);
		for (int i = 0; i < _pngFiles.size(); i++) {
			SendMessage(_hwndComboTex, CB_ADDSTRING, 0, (LPARAM)_pngFiles[i].c_str());
		}
		ShowWindow(GetDlgItem(_hwnd, _texId), SW_HIDE);
	}

	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);
		// 白色でクライアント領域を塗りつぶす
		HBRUSH hRedBrush = CreateSolidBrush(RGB(0xFF, 0xFF, 0xFF));
		FillRect(hdc, &ps.rcPaint, hRedBrush);

		ShowWindow(GetDlgItem(_hwnd, _texId), SW_HIDE);

		DeleteObject(hRedBrush);

		if (_shouldRender) {
			if (_pId > 0) {
				string modelName = _modelManager->_models[_pId - 1]->_name;
				TextOut(hdc, 0, _modelNameY, const_cast<LPCSTR>(("Model : " + modelName).c_str()), modelName.size() + 8);
				if (_cId > 0) {
					string subModelName = _modelManager->_models[_pId - 1]->_subModels[_cId - 1]->_materialName;
					TextOut(hdc, 0, _subModelNameY, const_cast<LPCSTR>(("SubModel : " + subModelName).c_str()), subModelName.size() + 11);

					TextOut(hdc, 0, _materialY, const_cast<LPCSTR>(TEXT("Material")), 8);
					for (int i = 0; i < 3; i++) {
						TextOut(hdc, _width * (i + 1) / 4, _materialY, const_cast<LPCSTR>(_rgb[i].c_str()), _rgb[i].size());
					}
					TextOut(hdc, 0, _ambY, const_cast<LPCSTR>(_editNameMaterial[0].c_str()), _editNameMaterial[0].size());
					TextOut(hdc, 0, _dfsY, const_cast<LPCSTR>(_editNameMaterial[1].c_str()), _editNameMaterial[1].size());
					TextOut(hdc, 0, _spcY, const_cast<LPCSTR>(_editNameMaterial[2].c_str()), _editNameMaterial[2].size());
					TextOut(hdc, 0, _dY, const_cast<LPCSTR>(_editNameMaterial[3].c_str()), _editNameMaterial[3].size());
					TextOut(hdc, 0, _nsY, const_cast<LPCSTR>(_editNameMaterial[4].c_str()), _editNameMaterial[4].size());
					SetWindowPos(_hwndComboTex, NULL, 0, _texY + _baseHeight, _width, _texComboH, SWP_NOZORDER);
					SetComboBox();
					ShowWindow(GetDlgItem(_hwnd, _texId), SW_SHOW);
					TextOut(hdc, 0, _texY, const_cast<LPCSTR>(_comboNameTex.c_str()), _comboNameTex.size());
				}

				TextOut(hdc, 0, _srtY, const_cast<LPCSTR>(TEXT("SRT")), 3);
				for (int i = 0; i < 3; i++) {
					TextOut(hdc, _width * (i + 1) / 4, _srtY, const_cast<LPCSTR>(_xyz[i].c_str()), _xyz[i].size());
				}
				TextOut(hdc, 0, _posY, const_cast<LPCSTR>(_editNameSRT[0].c_str()), _editNameSRT[0].size());
				TextOut(hdc, 0, _rotY, const_cast<LPCSTR>(_editNameSRT[1].c_str()), _editNameSRT[1].size());
				TextOut(hdc, 0, _scaY, const_cast<LPCSTR>(_editNameSRT[2].c_str()), _editNameSRT[2].size());

			}
		}
		EndPaint(hwnd, &ps);
		return DefWindowProc(hwnd, msg, wparam, lparam);
	}
	case WM_COMMAND: {
		int wmId = LOWORD(wparam);
		int wmEvent = HIWORD(wparam);

		cout << "Edit Box id : " << wmId << endl;
		if (wmEvent == EN_CHANGE) {
			char buffer[128];
			GetWindowText(_hwndEditBoxes[wmId], buffer, sizeof(buffer));
			string bufferS(buffer);
			if (bufferS == "") break;
			if (bufferS == "-") break;
			if (bufferS[0] == '.') bufferS = "0" + bufferS;
			cout << "buffer" << bufferS << endl;
			if (IsConvertibleStringToFloat(bufferS)) {
				float value = stof(bufferS);
				ChangeModelInfo(wmId, value);
			}
			else {
				MessageBox(hwnd, "有効な数値を入力してください。", "入力エラー", MB_OK);
			}
		}
		else if (wmEvent == CBN_SELCHANGE) {
			int comboBoxIndex;
			char selectedTexName[256];
			comboBoxIndex = SendMessage(_hwndComboTex, CB_GETCURSEL, 0, 0);
			if (comboBoxIndex != _selectedTexId) {
				SendMessage(_hwndComboTex, CB_GETLBTEXT, comboBoxIndex, (LPARAM)selectedTexName);
				string message(selectedTexName);
				if (!_modelManager->ChangeTexture(_pId - 1, _cId - 1, message)) {
					MessageBox(hwnd, "Textureを変更できませんでした。", "OK", MB_OK);
				}
				_selectedTexId = comboBoxIndex;
				message += "が選択されました。";
				MessageBox(hwnd, message.c_str(), "OK", MB_OK);
			}
		}
		return DefWindowProc(hwnd, msg, wparam, lparam);
	}
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);
}


void DetailWindow::ChangeModelInfo(int id, float v) {
	if (_cId == 0) {
		if (id == _spxId) _modelManager->_models[_pId - 1]->_transform.position.x = v;
		else if (id == _spyId) _modelManager->_models[_pId - 1]->_transform.position.y = v;
		else if (id == _spzId) _modelManager->_models[_pId - 1]->_transform.position.z = v;
		else if (id == _srxId) _modelManager->_models[_pId - 1]->_transform.rotation.x = v;
		else if (id == _sryId) _modelManager->_models[_pId - 1]->_transform.rotation.y = v;
		else if (id == _srzId) _modelManager->_models[_pId - 1]->_transform.rotation.z = v;
		else if (id == _ssxId) _modelManager->_models[_pId - 1]->_transform.scale.x = v;
		else if (id == _ssyId) _modelManager->_models[_pId - 1]->_transform.scale.y = v;
		else if (id == _sszId) _modelManager->_models[_pId - 1]->_transform.scale.z = v;
		_modelManager->_models[_pId - 1]->Update();
	}
	else {
		if (id == _spxId) _modelManager->_models[_pId - 1]->_subModels[_cId - 1]->_transform.position.x = v;
		else if (id == _spyId) _modelManager->_models[_pId - 1]->_subModels[_cId - 1]->_transform.position.y = v;
		else if (id == _spzId) _modelManager->_models[_pId - 1]->_subModels[_cId - 1]->_transform.position.z = v;
		else if (id == _srxId) _modelManager->_models[_pId - 1]->_subModels[_cId - 1]->_transform.rotation.x = v;
		else if (id == _sryId) _modelManager->_models[_pId - 1]->_subModels[_cId - 1]->_transform.rotation.y = v;
		else if (id == _srzId) _modelManager->_models[_pId - 1]->_subModels[_cId - 1]->_transform.rotation.z = v;
		else if (id == _ssxId) _modelManager->_models[_pId - 1]->_subModels[_cId - 1]->_transform.scale.x = v;
		else if (id == _ssyId) _modelManager->_models[_pId - 1]->_subModels[_cId - 1]->_transform.scale.y = v;
		else if (id == _sszId) _modelManager->_models[_pId - 1]->_subModels[_cId - 1]->_transform.scale.z = v;

		else if (id == _maxId) _modelManager->_models[_pId - 1]->_subModels[_cId - 1]->_material.ambient.x = v;
		else if (id == _mayId) _modelManager->_models[_pId - 1]->_subModels[_cId - 1]->_material.ambient.y = v;
		else if (id == _mazId) _modelManager->_models[_pId - 1]->_subModels[_cId - 1]->_material.ambient.z = v;
		else if (id == _mdxId) _modelManager->_models[_pId - 1]->_subModels[_cId - 1]->_material.diffuse.x = v;
		else if (id == _mdyId) _modelManager->_models[_pId - 1]->_subModels[_cId - 1]->_material.diffuse.y = v;
		else if (id == _mdzId) _modelManager->_models[_pId - 1]->_subModels[_cId - 1]->_material.diffuse.z = v;
		else if (id == _msxId) _modelManager->_models[_pId - 1]->_subModels[_cId - 1]->_material.specular.x = v;
		else if (id == _msyId) _modelManager->_models[_pId - 1]->_subModels[_cId - 1]->_material.specular.y = v;
		else if (id == _mszId) _modelManager->_models[_pId - 1]->_subModels[_cId - 1]->_material.specular.z = v;
		else if (id == _mdId) _modelManager->_models[_pId - 1]->_subModels[_cId - 1]->_material.dissolve = v;
		else if (id == _mnsId) _modelManager->_models[_pId - 1]->_subModels[_cId - 1]->_material.specularity = v;
		_modelManager->_models[_pId - 1]->_subModels[_cId - 1]->Update();
	}
}

void DetailWindow::SetSlectedModelID(int pId, int cId) {
	_pId = pId;
	_cId = cId;
}