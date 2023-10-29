#pragma once
#include"BasicWindow.h"
#include"DetailWindow.h"

struct ModelSource {
	string name;
	HWND hwnd;
};

class ModelWindow : public BasicWindow {
public:
	ModelWindow(HINSTANCE hi, LPCSTR windowName, ModelManager* mm, Camera* c, unsigned int x, unsigned int y, unsigned int width, unsigned int height)
		: BasicWindow(hi, windowName, mm, c, x, y, width, height) {
	}

	void Init(DetailWindow* dw) {
		_detailWindow = dw;
	}

	~ModelWindow();

	void CreateModelTree();

	void DeleteTreeItem();

	void DeleteModel();

	void DuplicateModel();

	HTREEITEM InsertTreeItem(HTREEITEM hitemParent, LPTSTR lpszName, int pId, int cId);

	LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) override;

public:
	vector<ModelSource> _source;
	HWND _hwndTreeView;

	int _selectedModel = 0;

private:
	DetailWindow* _detailWindow;
};