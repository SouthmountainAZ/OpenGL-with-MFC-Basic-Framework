#include "stdafx.h"
#include "OpenGLControl.h"

OpenGLControl::OpenGLControl()
{
	_fPosX = 0.0f;
	_fPosY = 0.0f;
	_fZoom = 10.0f;
	_fRotX = 0.0f;
	_fRotY = 0.0f;
	_isMaximized = false;
}

//set up basic window variables and function calls
void OpenGLControl::oglCreate(CRect rect, CWnd *parent)
{
	CString className = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW | CS_OWNDC, NULL, (HBRUSH)GetStockObject(BLACK_BRUSH), NULL);
	CreateEx(0, className, _T("OpenGL"), WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, rect, parent, 0);
	_oldWindow = rect;
	_originalRect = rect;
	hWnd = parent;
}



OpenGLControl::~OpenGLControl()
{
}
BEGIN_MESSAGE_MAP(OpenGLControl, CWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_TIMER()
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()


void OpenGLControl::OnPaint()
{
	//CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CWnd::OnPaint() for painting messages
	ValidateRect(NULL);
}


int OpenGLControl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	oglInitialize();

	return 0;
}


void OpenGLControl::oglInitialize(void)
{
	static PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		32,	//bit depth
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		16,	// z-buffer depth
		0, 0, 0, 0, 0, 0, 0
	};

	hdc = GetDC()->m_hDC;

	//Pixel Format
	_pixelFormat = ChoosePixelFormat(hdc, &pfd);
	SetPixelFormat(hdc, _pixelFormat, &pfd);

	//Create the OpenGL Rendering Context
	hrc = wglCreateContext(hdc);
	wglMakeCurrent(hdc, hrc);

	//Set color to use when clearing the background
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);

	//Turn on backface culling
	glFrontFace(GL_CCW);
	//glCullFace(GL_BACK);
	glDisable(GL_CULL_FACE);

	//Turn on depth testing
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	//Send translate request
	OnTranslate(NULL);	//manually added
}

void OpenGLControl::OnTranslate(CDC *pDC)	//manually added
{
	//TODO: Camera controlls
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -_fZoom);
	glTranslatef(_fPosX, _fPosY, 0.0f);
	glRotatef(_fRotX, 1.0f, 0.0f, 0.0f);
	glRotatef(_fRotY, 0.0f, 1.0f, 0.0f);
}



void OpenGLControl::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	switch (nIDEvent)
	{
		case 1:
		{
			//clear color and depth buffer bits
			glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

			//Draw a cube
			oglDrawCube();
		
			//Swap buffer
			SwapBuffers(hdc);

			break;
		}

		default:
			break;
	}

	CWnd::OnTimer(nIDEvent);
}


void OpenGLControl::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if (cx <= 0 || cy <= 0 || nType == SIZE_MINIMIZED)
		return;

	//map the opengl coordinates
	glViewport(0, 0, cx, cy);

	//projection view
	glMatrixMode(GL_PROJECTION);
	
	glLoadIdentity();

	//set our current view perspective
	gluPerspective(35.0f, (float)cx/(float)cy, 0.01f, 2000.0f);

	//Model View
	glMatrixMode(GL_MODELVIEW);

	switch (nType)
	{
		case SIZE_MAXIMIZED:
		{
			MoveWindow(0, 0, cx, cy);
			GetWindowRect(_rect);
			_oldWindow = _rect;

			break;
		}

		case SIZE_RESTORED:
		{
			if (_isMaximized)
			{
				MoveWindow(0, 0, cx, cy);	//execute 9 times (reason unknown)
				GetWindowRect(_rect);
				_oldWindow = _rect;
			}

			break;
		}

		case SIZE_RESIZE:
		{
			MoveWindow(0, 0, cx, cy);
			GetWindowRect(_rect);
			_oldWindow = _rect;
		}

		default:
			break;
	}

}


void OpenGLControl::oglDrawCube(void)
{
	// Wireframe Mode
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glBegin(GL_QUADS);

	glColor3f(1.0f, 1.0f, 1.0f);

	// Front Side
	glVertex3f(1.0f, 1.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);

	// Back Side
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f);
	glVertex3f(1.0f, 1.0f, -1.0f);
	glVertex3f(1.0f, -1.0f, -1.0f);

	// Top Side
	glVertex3f(1.0f, 1.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, -1.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f);

	// Bottom Side
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glVertex3f(1.0f, -1.0f, -1.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);

	// Right Side
	glVertex3f(1.0f, 1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, -1.0f);
	glVertex3f(1.0f, 1.0f, -1.0f);

	// Left Side
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f);
	glEnd();
}

void OpenGLControl::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	int diffX = (int)(point.x - _fLastX);
	int diffY = (int)(point.y - _fLastY);
	_fLastX = (float)point.x;
	_fLastY = (float)point.y;

	// Left mouse button
	if (nFlags & MK_LBUTTON)
	{
		_fRotX += (float)0.5f * diffY;

		if ((_fRotX > 360.0f) || (_fRotX < -360.0f))
		{
			_fRotX = 0.0f;
		}

		_fRotY += (float)0.5f * diffX;

		if ((_fRotY > 360.0f) || (_fRotY < -360.0f))
		{
			_fRotY = 0.0f;
		}
	}

	// Right mouse button
	else if (nFlags & MK_RBUTTON)
	{
		_fZoom -= (float)0.1f * diffY;
	}

	// Middle mouse button
	else if (nFlags & MK_MBUTTON)
	{
		_fPosX += (float)0.015f * diffX;
		_fPosY -= (float)0.015f * diffY;
	}

	OnTranslate(NULL);	//manually added

	CWnd::OnMouseMove(nFlags, point);
}

