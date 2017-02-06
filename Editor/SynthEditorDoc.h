
// SynthEditorDoc.h : interface of the CSynthEditorDoc class
//


#pragma once


class CSynthEditorDoc : public CDocument
{
protected: // create from serialization only
	CSynthEditorDoc();
	DECLARE_DYNCREATE(CSynthEditorDoc)

public:
	static CSynthEditorDoc* Instance();

private:

// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// Implementation
public:
	virtual ~CSynthEditorDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// Helper function that sets search content for a Search Handler
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
};
