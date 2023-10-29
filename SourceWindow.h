#pragma once
#include "BasicWindow.h"
#include "ModelWindow.h"

struct Source {
	string name;
	bool isMTLExist;
	HWND hwnd;
};

class SourceWindow : public BasicWindow {
public:
	SourceWindow(HINSTANCE hi, LPCSTR windowName, ModelManager* mm, Camera* c, unsigned int width, unsigned int x, unsigned int y, unsigned int height);

	void Init(ModelWindow* mw);

	~SourceWindow();

	void CreateSourceButtons(map<string, bool> objFiles);

	void CreateCameraEditBox();

	vector<Source> _sources;

private:
	int _btnWidth = 500;
	int _btnHeight = 50;

	int _baseHeight = 30;
	int _padding = 10;

	ModelWindow* _modelWindow = nullptr;

	int _offsetSrcId = 30;


	HWND _hwndQuitBtn, _hwndDuplBtn, _hwndDeleBtn;
	int _quitId = 20, _duplicateId = 21, _deleteId = 22;

	HWND _hwndComboSrc;
	int _srcId = 30;
	int _srcY;
	int _srccbY = 50;

	HWND _hwnds[20];

	string _xyz[3] = { "x", "y", "z" };
	string _rgb[3] = { "r", "g", "b" };
	string _editNameSRT[3] = { "Position", "Rotation", "Scale" };

	int _cpxId = 0, _cpyId = 1, _cpzId = 2, _crxId = 3, _cryId = 4, _crzId = 5, _cfId = 6, _cnz = 7, _cfz = 8, _lvxId = 11, _lvyId = 12, _lvzId = 13, _lcxId = 14, _lcyId = 15, _lczId = 16, _bcxId = 17, _bcyId = 18, _bczId = 19;

	int _cameraY;
	int _cpY;
	int _crY;
	int _cfovY;
	int _ccpY;
	int _dirLigY;
	int _dlvY;
	int _dlcY;
	int _backgroundY;
	int _bgcY;

	void ChangeModelInfo(int id, float v);
public:

	LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) override;
};
