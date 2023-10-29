#pragma once
#include"BasicWindow.h"


class DetailWindow : public BasicWindow {
public:

	DetailWindow(HINSTANCE hi, LPCSTR windowName, ModelManager* mm, Camera* c, unsigned int x, unsigned int y, unsigned int width, unsigned int height);

	~DetailWindow();

	void CreateEditBoxes();

	void CreateSRTEditBox(int& offsetY, bool isModel);
	void CreateMaterialEditBox(int& offsetY);

	HWND CreateEditBox(int x, int y, int w, int h, int id, string n);

	void DeleteChildWindow();

	void SetComboBox();

	void SetSlectedModelID(int pId, int cId);

	LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) override;

private:

	HWND _hwndEditBoxes[20];


	HWND _hwndComboTex;


	string _editNameSRT[3] = { "Position", "Rotation", "Scale" };
	string _editNameMaterial[5] = { "Ambient", "Diffuse", "Specular", "d", "Ns" };
	string _comboNameTex = "Texture";
	string _xyz[3] = { "x", "y", "z" };
	string _rgb[3] = { "r", "g", "b" };
	const int _spxId = 0, _spyId = 1, _spzId = 2, _srxId = 3, _sryId = 4, _srzId = 5, _ssxId = 6, _ssyId = 7, _sszId = 8;
	const int _maxId = 9, _mayId = 10, _mazId = 11, _mdxId = 12, _mdyId = 13, _mdzId = 14, _msxId = 15, _msyId = 16, _mszId = 17, _mdId = 18, _mnsId = 19;
	const int _texId = 20;
	const int _srdId[3 * 3] = { 0, 1, 2, 3, 4, 5, 6, 7, 8 };
	const int _materialId[3 * 3 + 2] = { 9, 11, 12, 13, 14, 15, 16, 17, 18, 19 };
	int _baseHeight = 30;
	int _padding = 10;


	int _pId;
	int _cId;

	int _modelNameY;
	int _subModelNameY;
	int _srtY;
	int _posY;
	int _rotY;
	int _scaY;
	int _materialY;
	int _ambY;
	int _dfsY;
	int _spcY;
	int _dY;
	int _nsY;

	int _texY;
	int _texComboH = 1000;

	int _selectedTexId;
public:
	vector<string> _pngFiles;

	bool _shouldRender = false;

private:

	void ChangeModelInfo(int id, float v);
};