/* *	File:		QTMusic Sample Keyboards.c *//*--------------------------	Inclusions--------------------------*/#include <QuickDraw.h>#include <Memory.h>#include <Windows.h>#include <Controls.h>#include <QuickTimeComponents.h>#include <BDC.h>#include "EasyKeyboardDiagram.h"#include "BigEasy2.h"#include "BigEasyUtils.h"#include "BigEasyGrafish.h"/*--------------------------	Limits and Konstants--------------------------*/enum	{	mFirstDocOnly = 100,	mClose,	mLastDocOnly,	mOpen	};typedef struct	{	KeyboardDiagram kd;	NoteRequest nr;	NoteChannel nc;	short panelTop;	ControlHandle instrumentButton;	Rect statR;	} QTMPanel;#define kPanelCount 3typedef struct	{	WindowPtr w;	NoteAllocator na;	QTMPanel panel[kPanelCount];	} Globals;Globals *g = nil;/*--------------------------	Formatting--------------------------*/#define kOctaveCount 5#define kHeaderHeight 17#define kDocMargin 8#define kKeyboardWidth (kOctaveCount * kKeyboardOctaveWidth + 1)#define kInstrumentButtonHeight 20#define kInstrumentButtonWidth 120#define kStatHeight 32#define kTextAllowance 13#define kStatWidth 160#define kPanelHeight (3*kDocMargin + kKeyboardHeight + kStatHeight)#define kPanelTop(x) (x * kPanelHeight)#define kKeyboardLeft kDocMargin#define kKeyboardRight (kKeyboardLeft + kKeyboardWidth)#define kKeyboardTop(x) (kPanelTop(x)+2*kDocMargin+kStatHeight)#define kStatLeft kDocMargin#define kStatRight (kStatLeft + kStatWidth)#define kStatTop(x) (kPanelTop(x)+kDocMargin)#define kInstrumentButtonLeft (kStatRight + kDocMargin)#define kInstrumentButtonRight (kInstrumentButtonLeft + kInstrumentButtonWidth)#define kInstrumentButtonTop(x) (kStatTop(x) + (kStatHeight-kInstrumentButtonHeight))#define kWindowWidth (kKeyboardWidth+2*kDocMargin)#define kWindowHeight (kPanelCount * kPanelHeight)/*--------------------------	Prototypes--------------------------*/static void DrawDoc(short n);static void ClickDoc(short n,Point p,short mods);static void KeyDoc(short n,short key,short code,short mods);static void IdleDoc(short n, Boolean front);static void ActivateDoc(short n);static void DeactivateDoc(short n);static void LetsQuit(short n,short menuItem,short menuRef);static void OpenWindow();static void InitVars(void);static void TrackKeyboardClick(KeyboardDiagram *kd,NoteAllocator na,NoteChannel nc,Point p);static void DrawNum(long n);static void DrawStat(short i);/*--------------------------	Computer Programs--------------------------*/void DrawDoc(short n)/* * Draws the window. */	{	Rect r;	QTMPanel *p;	short i;	GoBW();	RGBFore(50000,50000,65535);	r = qd.thePort->portRect;	PaintRect(&r);	GoBW();	DrawControls(g->w);	for(i = 0; i < kPanelCount; i++)		{		DrawStat(i);		p = &g->panel[i];		DrawKeyboard(&p->kd);		if(i)			{			MoveTo(0,p->panelTop);			Line(kWindowWidth,0);			}		}	}void DrawStat(short i)	{	Rect r;	QTMPanel *panel;	Str31 s;	GoBW();	TextSize(9);	panel = &g->panel[i];	r = panel->statR;	FrameRect(&r);	MoveTo(r.left + 3,r.bottom);	LineTo(r.right,r.bottom);	LineTo(r.right,r.top + 3);	InsetRect(&r,1,1);	EraseRect(&r);	MoveTo(r.left + 3,r.top + kTextAllowance);	DrawString(panel->nr.tone.synthesizerName);	if(panel->nr.tone.synthesizerType)		{		DrawString("\p (");		s[0] = 4;		s[1] = panel->nr.tone.synthesizerType >> 24;		s[2] = panel->nr.tone.synthesizerType >> 16;		s[3] = panel->nr.tone.synthesizerType >> 8;		s[4] = panel->nr.tone.synthesizerType;		DrawString(s);		DrawString("\p)");		}	MoveTo(r.left + 3,r.top + 2*kTextAllowance);	DrawString(panel->nr.tone.instrumentName);	DrawString("\p (");	DrawNum(panel->nr.tone.instrumentNumber);	DrawString("\p / ");	DrawNum(panel->nr.tone.gmNumber);	DrawString("\p)");	}void DrawNum(long n)/*  * Draw number n as a signed long, in decimal  */	{	Str31 c;	NumToString(n,c);	DrawString(c);	}void ClickDoc(short n,Point p,short mods)/* * Come here for a click in the window. */	{	QTMPanel *panel;	short i;	ControlHandle cH;	short part;	part = FindControl(p,g->w,&cH);	if(cH)		{		part = TrackControl(cH,p,nil);		if(part)			{			ComponentResult result;			i = GetControlReference(cH);			panel = &g->panel[i];			result = NAPickInstrument(g->na, nil, "\pNew Instrument:", &panel->nr.tone,					0, 0, nil, nil);			if(!result)				{				NADisposeNoteChannel(g->na,panel->nc);				panel->nc = 0;				NANewNoteChannel(g->na,&panel->nr,&panel->nc);				DrawStat(i);				}			}		goto doneClick;		}	for(i = 0; i < kPanelCount; i++)		{		panel = &g->panel[i];		if(PtInRect(p,&panel->kd.r))		/* clicked in keyboard picture? */			{			TrackKeyboardClick(&panel->kd,g->na,panel->nc,p);	/* play musical notes */			goto doneClick;			}		}doneClick:;	}void TrackKeyboardClick(KeyboardDiagram *kd,NoteAllocator na,NoteChannel nc,Point p)	{	short pitch,lastPitch;	long velocity;	short mods;	lastPitch = 0;	do		{		pitch = GetKeyboardKey(kd,p);		if(pitch < 0)			pitch = 0;		GetMouse(&p);		if(pitch != lastPitch)			{			NAPlayNote(na,nc,lastPitch,0);			if(pitch >= 0)				{				mods = GetModKeys();				velocity = 64 + (mods & optionKey ? 20 : 0) + (mods & cmdKey ? 32 : 0);				NAPlayNote(na,nc,pitch,velocity);				}			PaintKeyboardKey(kd,lastPitch,0);			PaintKeyboardKey(kd,pitch,255);			lastPitch = pitch;			}		} while(StillDown());	PaintKeyboardKey(kd,lastPitch,0);	NAPlayNote(na,nc,lastPitch,0);	}void KeyDoc(short n,short key,short code,short mods)	{	#pragma unused (n,key,code,mods)	}void IdleDoc(short n, Boolean front)	{	#pragma unused (n,front)	}void ActivateDoc(short n)	{	#pragma unused (n)	SetMenuItemRange(mFirstDocOnly,mLastDocOnly,1,0);	SetMenuItem(mOpen,-1,0,0,nil);				/* disable "Open" menu item		*/	}void DeactivateDoc(short n)	{	#pragma unused (n)	SetMenuItemRange(mFirstDocOnly,mLastDocOnly,-1,0);	SetMenuItem(mOpen,1,0,0,nil);				/* enable "Open" menu item		*/	}void LetsQuit(short n,short menuItem,short menuRef)	{	#pragma unused (n,menuItem,menuRef)	gQuitApp++;	}void OpenWindow(void)	{	Rect r;	SetRect(&r,0,0,kWindowWidth,kWindowHeight);	OffsetRect(&r,30,50);	g->w = InstallWindow(1,(StringPtr)0x910,&r,0,wCopyDraw + wPrintDraw,			DrawDoc,ClickDoc,KeyDoc,nil,			ActivateDoc,DeactivateDoc,IdleDoc);	}void InitVars()/* * Called once at startup: yes, it * inits the vars. */	{	short i;	Rect r;	QTMPanel *p;	g = (Globals *)NewPtrClear(sizeof(Globals));	OpenWindow();	g->na = OpenDefaultComponent('nota',0);	for(i = 0; i < kPanelCount; i++)	/*	 * For each leetle keyboard picture, startup	 * the keyboard diagram, and allocate a "NoteChannel"	 * upon which we shall play musical notes.	 *	 * Start each keyboard with the sound of General MIDI instrument 1.	 */		{		p = &g->panel[i];		r.left = kKeyboardLeft;		r.right = kKeyboardRight;		r.top = kKeyboardTop(i);		r.bottom = r.top + kKeyboardHeight;		InitializeKeyboard(&p->kd,&r,kOctaveCount,36);		r.left = kInstrumentButtonLeft;		r.right = kInstrumentButtonRight;		r.top = kInstrumentButtonTop(i);		r.bottom = r.top + kInstrumentButtonHeight;		p->instrumentButton = NewControl(g->w, &r,"\pInstrumentÉ", true, 0,0,1,				pushButProc, i);		p->panelTop = kPanelTop(i);		p->statR.left = kStatLeft;		p->statR.right = kStatRight;		p->statR.top = kStatTop(i);		p->statR.bottom = p->statR.top + kStatHeight;		p->nr.polyphony = 2;		p->nr.typicalPolyphony = 0x00010000;		NAStuffToneDescription(g->na,1,&p->nr.tone);		NANewNoteChannel(g->na,&p->nr,&p->nc);		}	}void Bootstrap()	{	InitVars();/*** File Menu ***/	InstallMenu("\pFile",nil,0);	InstallPrintItems(nil,nil);	InstallMenuItem("\p(-",nil,0);	InstallQuitItem(LetsQuit,0);/*** Edit Menu ***/	InstallEditMenu(nil,nil,nil,nil,nil);	}void Hatstrap()/*  * clean up  */	{	}