
// SynthEditorView.h : interface of the CSynthEditorView class
//

#pragma once


class CSynthEditorView : public CView
{
protected: // create from serialization only
	CSynthEditorView();
	DECLARE_DYNCREATE(CSynthEditorView)

// Attributes
public:
	CSynthEditorDoc* GetDocument() const;

	static CSynthEditorView* Instance();

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CSynthEditorView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnFileUpload();
};

#ifndef _DEBUG  // debug version in SynthEditorView.cpp
inline CSynthEditorDoc* CSynthEditorView::GetDocument() const
   { return reinterpret_cast<CSynthEditorDoc*>(m_pDocument); }
#endif
