
#if !defined(AFX_TREE_H__FA28BD4D_8FA6_4B9C_B12B_8D8890F76594__INCLUDED_)
#define AFX_TREE_H__FA28BD4D_8FA6_4B9C_B12B_8D8890F76594__INCLUDED_

extern HWND ghwndTree;

void CreateTreeCtrl(HINSTANCE ahInst,HWND hWndParent);
void DestroyTreeCtrl();
void RebuildTree(void* apData, unsigned int auDataLength);

#endif