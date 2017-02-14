
// SynthEditorDoc.cpp : implementation of the CSynthEditorDoc class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "SynthEditor.h"
#endif

#include "SynthEditorDoc.h"
#include "Serial.h"
#include "../Engine/Common.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CSynthEditorDoc

IMPLEMENT_DYNCREATE(CSynthEditorDoc, CDocument)

BEGIN_MESSAGE_MAP(CSynthEditorDoc, CDocument)
END_MESSAGE_MAP()


// CSynthEditorDoc construction/destruction

CSynthEditorDoc::CSynthEditorDoc()
{
	// TODO: add one-time construction code here

}

CSynthEditorDoc::~CSynthEditorDoc()
{
}

CSynthEditorDoc* CSynthEditorDoc::Instance()
{
	if (CFrameWnd* frame = static_cast<CFrameWnd*>(::AfxGetMainWnd()))
		if (CView* view = frame->GetActiveView())
			return static_cast<CSynthEditorDoc*>(view->GetDocument());
	return nullptr;
}

BOOL CSynthEditorDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}




// CSynthEditorDoc serialization

void CSynthEditorDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

#ifdef SHARED_HANDLERS

// Support for thumbnails
void CSynthEditorDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// Modify this code to draw the document's data
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// Support for Search Handlers
void CSynthEditorDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// Set search contents from document's data. 
	// The content parts should be separated by ";"

	// For example:  strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CSynthEditorDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CSynthEditorDoc diagnostics

#ifdef _DEBUG
void CSynthEditorDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CSynthEditorDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

const byte inputData0[] = 
{
	(byte)CommandType::StartGraph,
	(byte)CommandType::InitGraph, 3, 6, 2, 
	(byte)CommandType::AddMonoModule, (byte)ModuleType::MIDI, // 0			
	(byte)CommandType::AddPolyModule, (byte)ModuleType::UnsignedValue,	// 0 (attack)
	(byte)CommandType::AddPolyModule, (byte)ModuleType::UnsignedValue,	// 1 (decay)
	(byte)CommandType::AddPolyModule, (byte)ModuleType::UnsignedValue,	// 2 (sustain)
	(byte)CommandType::AddPolyModule, (byte)ModuleType::UnsignedValue,	// 3 (release)
	(byte)CommandType::AddPolyModule, (byte)ModuleType::Envelope,		// 4
	(byte)CommandType::AddPolyModule, (byte)ModuleType::Oscillator,		// 5
	(byte)CommandType::AddMonoModule, (byte)ModuleType::Mixer,	// 1
	(byte)CommandType::AddMonoModule, (byte)ModuleType::Target,	// 2

	(byte)CommandType::SetPolyUnsignedValue, 0, LOBYTE(100), HIBYTE(100), 
	(byte)CommandType::SetPolyUnsignedValue, 1, LOBYTE(1000), HIBYTE(1000), 
	(byte)CommandType::SetPolyUnsignedValue, 2, LOBYTE(0x8000), HIBYTE(0x8000), 
	(byte)CommandType::SetPolyUnsignedValue, 3, LOBYTE(1000), HIBYTE(1000), 

	(byte)CommandType::AddConnection, 
		(byte)InstanceType::Poly, (byte)PinType::Unsigned, 4, Pin::Envelope::UnsignedInput::Attack,
		(byte)InstanceType::Poly, (byte)PinType::Unsigned, 0, 0,
	(byte)CommandType::AddConnection, 
		(byte)InstanceType::Poly, (byte)PinType::Unsigned, 4, Pin::Envelope::UnsignedInput::Decay,
		(byte)InstanceType::Poly, (byte)PinType::Unsigned, 1, 0,
	(byte)CommandType::AddConnection, 
		(byte)InstanceType::Poly, (byte)PinType::Unsigned, 4, Pin::Envelope::UnsignedInput::Sustain,
		(byte)InstanceType::Poly, (byte)PinType::Unsigned, 2, 0,
	(byte)CommandType::AddConnection, 
		(byte)InstanceType::Poly, (byte)PinType::Unsigned, 4, Pin::Envelope::UnsignedInput::Release,
		(byte)InstanceType::Poly, (byte)PinType::Unsigned, 3, 0,

	(byte)CommandType::AddConnection, 
		(byte)InstanceType::Poly, (byte)PinType::Unsigned, 5, Pin::Oscillator::UnsignedInput::Level,
		(byte)InstanceType::Poly, (byte)PinType::Unsigned, 4, Pin::Envelope::UnsignedOutput::Level,
	(byte)CommandType::AddConnection, 
		(byte)InstanceType::Poly, (byte)PinType::Unsigned, 4, Pin::Envelope::UnsignedInput::Gate,
		(byte)InstanceType::Mono, (byte)PinType::Unsigned, 0, Pin::MIDI::UnsignedPolyOutput::Gate,
	(byte)CommandType::AddConnection, 
		(byte)InstanceType::Poly, (byte)PinType::Unsigned, 5, Pin::Oscillator::UnsignedInput::Pitch,
		(byte)InstanceType::Mono, (byte)PinType::Unsigned, 0, Pin::MIDI::UnsignedPolyOutput::Pitch,

	(byte)CommandType::AddConnection, 
		(byte)InstanceType::Mono, (byte)PinType::Signed, 1, Pin::PolyMixer::SignedPolyInput::Signal,
		(byte)InstanceType::Poly, (byte)PinType::Signed, 5, Pin::Oscillator::SignedOutput::Signal,
	(byte)CommandType::AddConnection, 
		(byte)InstanceType::Mono, (byte)PinType::Signed, 2, Pin::Target::SignedInput::Signal,
		(byte)InstanceType::Mono, (byte)PinType::Signed, 1, Pin::PolyMixer::SignedOutput::Signal,

	(byte)CommandType::EndGraph,
};

const byte inputData1[] = 
{
	(byte)CommandType::StartGraph,
	(byte)CommandType::InitGraph, 3, 6, 4, 
	(byte)CommandType::AddMonoModule, (byte)ModuleType::MIDI, // 0			
	(byte)CommandType::AddPolyModule, (byte)ModuleType::UnsignedValue,	// 0 (attack)
	(byte)CommandType::AddPolyModule, (byte)ModuleType::UnsignedValue,	// 1 (decay)
	(byte)CommandType::AddPolyModule, (byte)ModuleType::UnsignedValue,	// 2 (sustain)
	(byte)CommandType::AddPolyModule, (byte)ModuleType::UnsignedValue,	// 3 (release)
	(byte)CommandType::AddPolyModule, (byte)ModuleType::Envelope,		// 4
	(byte)CommandType::AddPolyModule, (byte)ModuleType::Oscillator,		// 5
	(byte)CommandType::AddMonoModule, (byte)ModuleType::Mixer,	// 1
	(byte)CommandType::AddMonoModule, (byte)ModuleType::Target,	// 2

	(byte)CommandType::SetPolyUnsignedValue, 0, LOBYTE(1000), HIBYTE(1000), 
	(byte)CommandType::SetPolyUnsignedValue, 1, LOBYTE(0), HIBYTE(0), 
	(byte)CommandType::SetPolyUnsignedValue, 2, LOBYTE(0xffff), HIBYTE(0xffff), 
	(byte)CommandType::SetPolyUnsignedValue, 3, LOBYTE(100), HIBYTE(100), 

	(byte)CommandType::AddConnection, 
		(byte)InstanceType::Poly, (byte)PinType::Unsigned, 4, Pin::Envelope::UnsignedInput::Attack,
		(byte)InstanceType::Poly, (byte)PinType::Unsigned, 0, 0,
	(byte)CommandType::AddConnection, 
		(byte)InstanceType::Poly, (byte)PinType::Unsigned, 4, Pin::Envelope::UnsignedInput::Decay,
		(byte)InstanceType::Poly, (byte)PinType::Unsigned, 1, 0,
	(byte)CommandType::AddConnection, 
		(byte)InstanceType::Poly, (byte)PinType::Unsigned, 4, Pin::Envelope::UnsignedInput::Sustain,
		(byte)InstanceType::Poly, (byte)PinType::Unsigned, 2, 0,
	(byte)CommandType::AddConnection, 
		(byte)InstanceType::Poly, (byte)PinType::Unsigned, 4, Pin::Envelope::UnsignedInput::Release,
		(byte)InstanceType::Poly, (byte)PinType::Unsigned, 3, 0,

	(byte)CommandType::AddConnection, 
		(byte)InstanceType::Poly, (byte)PinType::Unsigned, 5, Pin::Oscillator::UnsignedInput::Level,
		(byte)InstanceType::Poly, (byte)PinType::Unsigned, 4, Pin::Envelope::UnsignedOutput::Level,
	(byte)CommandType::AddConnection, 
		(byte)InstanceType::Poly, (byte)PinType::Unsigned, 4, Pin::Envelope::UnsignedInput::Gate,
		(byte)InstanceType::Mono, (byte)PinType::Unsigned, 0, Pin::MIDI::UnsignedPolyOutput::Gate,
	(byte)CommandType::AddConnection, 
		(byte)InstanceType::Poly, (byte)PinType::Unsigned, 5, Pin::Oscillator::UnsignedInput::Pitch,
		(byte)InstanceType::Mono, (byte)PinType::Unsigned, 0, Pin::MIDI::UnsignedPolyOutput::Pitch,

	(byte)CommandType::AddConnection, 
		(byte)InstanceType::Mono, (byte)PinType::Signed, 1, Pin::PolyMixer::SignedPolyInput::Signal,
		(byte)InstanceType::Poly, (byte)PinType::Signed, 5, Pin::Oscillator::SignedOutput::Signal,
	(byte)CommandType::AddConnection, 
		(byte)InstanceType::Mono, (byte)PinType::Signed, 2, Pin::Target::SignedInput::Signal,
		(byte)InstanceType::Mono, (byte)PinType::Signed, 1, Pin::PolyMixer::SignedOutput::Signal,

	(byte)CommandType::EndGraph,
};

const byte* patches[] = { inputData0, inputData1 };
const DWORD patchSizes[] = { sizeof inputData0, sizeof inputData1 };

void CSynthEditorDoc::SetPatchIndex(int index)
{
	_patchIndex = index;
}

void CSynthEditorDoc::Upload() const
{
	Serial serial;
	if (serial.Open())
	{
		serial.Write(patches[_patchIndex], patchSizes[_patchIndex]);
		serial.Close();
	}
}
