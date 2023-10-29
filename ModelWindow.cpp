#include"ModelWindow.h"


// TreeView�̒��g�𐶐�����֐�
void ModelWindow::CreateModelTree() {
	_source.clear();
	TreeView_DeleteAllItems(_hwndTreeView);

	for (int i = 0; i < _modelManager->_models.size(); i++) {
		string modelName = _modelManager->_models[i]->_name;
		HTREEITEM hitemModel = InsertTreeItem(TVI_ROOT, const_cast<LPTSTR>(modelName.c_str()), i + 1, 0);
		for (int j = 0; j < _modelManager->_models[i]->_subModels.size(); j++) {
			string subModelName = _modelManager->_models[i]->_subModels[j]->_materialName;
			InsertTreeItem(hitemModel, const_cast<LPTSTR>(subModelName.c_str()), i + 1, j + 1);
		}
	}
}

ModelWindow::~ModelWindow() {

}

// �E�B���h�E�v���V�[�W���֐�
LRESULT ModelWindow::WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	switch (msg) {
	case WM_CREATE: {
		HTREEITEM            hitemRoot, hitemParent;
		INITCOMMONCONTROLSEX ic;

		_hwndTreeView = CreateWindowEx(0, WC_TREEVIEW, TEXT(""), WS_CHILD | WS_VISIBLE | TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT, 0, 0, 0, 0, hwnd, (HMENU)1, ((LPCREATESTRUCT)lparam)->hInstance, NULL);

	}

	case WM_NOTIFY: {
		if (_selectedModel != -1) {
			LPNMHDR nmhdr = (LPNMHDR)lparam;
			if (nmhdr->code == TVN_SELCHANGED) {  // �c���[�A�C�e�����I�����ꂽ�ꍇ
				LPNMTREEVIEW pNMTV = (LPNMTREEVIEW)lparam;
				HTREEITEM hSelectedItem = pNMTV->itemNew.hItem;

				// �J�X�^���f�[�^���擾
				TVITEMEX tvItem;
				tvItem.mask = TVIF_PARAM;
				tvItem.hItem = hSelectedItem;
				TreeView_GetItem(_hwndTreeView, &tvItem);

				int itemIndex = static_cast<int>(tvItem.lParam); // �J�X�^���f�[�^���A�C�e���̈ʒu
				cout << "lparam : " << itemIndex << endl;

				int pId = static_cast<int>((tvItem.lParam >> 0) & 0xFFFF);
				int cId = static_cast<int>((tvItem.lParam >> 16) & 0xFFFF);

				cout << "PID : " << pId << endl;
				cout << "CID : " << cId << endl;
				_detailWindow->_shouldRender = true;
				_detailWindow->SetSlectedModelID(pId, cId);
				_detailWindow->CreateEditBoxes();
				_selectedModel = pId;
			}
		}
	}

	case WM_SIZE:
		MoveWindow(_hwndTreeView, 0, 0, LOWORD(lparam), HIWORD(lparam), TRUE);
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	default:
		break;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

// TreeView�ɗv�f��ǉ�����֐�
HTREEITEM ModelWindow::InsertTreeItem(HTREEITEM hitemParent, LPTSTR lpszName, int pId, int cId) {

	TVINSERTSTRUCT is;
	is.hParent = hitemParent;
	is.item.mask = TVIF_TEXT | TVIF_PARAM;
	is.item.pszText = lpszName;

	LPARAM lparam = 0;
	lparam |= (pId & 0xFFFF) << 0; // ���f���C���f�N�X������16�r�b�g�Ɋi�[
	lparam |= (cId & 0xFFFF) << 16; // �T�u���f���C���f�b�N�X�����16�r�b�g�Ɋi�[

	is.item.lParam = lparam;

	return TreeView_InsertItem(_hwndTreeView, &is);
}

// TreeView�̗v�f���폜����֐�
void ModelWindow::DeleteTreeItem() {
	int nodeId = _selectedModel;

	if (_selectedModel != -1) {
		_selectedModel = -1;
		HTREEITEM hitem = TreeView_GetRoot(_hwndTreeView);
		for (int i = 0; i < nodeId - 1; i++) {
			hitem = TreeView_GetNextItem(_hwndTreeView, hitem, TVGN_NEXT);
		}
		TreeView_DeleteItem(_hwndTreeView, hitem);
		_detailWindow->_shouldRender = false;
		_detailWindow->SetSlectedModelID(0, 0);
		_detailWindow->CreateEditBoxes();
	}
	_modelManager->DeleteModel(nodeId - 1);

}

// Model���폜����֐�
void ModelWindow::DeleteModel() {
	int tmpSelectedModel = _selectedModel;
	std::cout << "selected Model:" << tmpSelectedModel << std::endl;
	_selectedModel = -1;
	if (tmpSelectedModel > 0) {
		_detailWindow->_shouldRender = false;
		_detailWindow->SetSlectedModelID(0, 0);
		_detailWindow->CreateEditBoxes();
		_modelManager->DeleteModel(tmpSelectedModel-1);
		CreateModelTree();
	}
	_selectedModel = 0;
}

// Model�𕡐�����֐�
void ModelWindow::DuplicateModel() {
	if (_selectedModel > 0) {
		_modelManager->DuplicateModel(_selectedModel - 1);
		CreateModelTree();
	}
}