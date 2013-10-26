// glinfo.cpp
// ==========
// display current OpenGL version, information, and extensions that the video
// card supports. (similar to "glxinfo" on Unix/Linux)
//
// 1.04: Ported to Dev-C++
// 1.03: Modified glPixelFormat struct
// 1.02: Fixed bugs in extension list
//       Added sorting extensions
// 1.01: Added listing of supported pixel formats
//       Added logging to a file
// 1.00: Released the first revision
//
//  AUTHOR: Song Ho Ahn (song.ahn@gmail.com)
// CREATED: 2002-02-21
// UPDATED: 2005-10-31
///////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <GL/gl.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include <algorithm>
using std::string;
using std::stringstream;
using std::ofstream;
using std::vector;
using std::cout;
using std::endl;


// struct variables ///////////////////////////////////////////////////////////
struct glPixelFormat
{
    int  index;         // pixel format index
    int  drawTo;        // to window(1) or to bitmap(2)
    int  type;          // pixel type: RGBA(1) or INDEX(2)
    int  colorBits;     // total number of color bits
    int  redBits;       // number of red color bits, for only RGBA type
    int  greenBits;     // number of green color bits, for only RGBA type
    int  blueBits;      // number of blue color bits, for only RGBA type
    int  alphaBits;     // number of alpha color bits, for only RGBA type
    int  depthBits;     // total number of depth bits
    int  stencilBits;   // total number of stencil bits
    bool doubleBuffer;  // support double buffer?
    bool stereoBuffer;  // support stereo?
    int  auxBuffers;    // total number of auxiliary buffers
};

struct glInfo
{
    string vendor;
    string renderer;
    string version;
    vector<string> extensions;
    int colorBits[4];   // (r,g,b,a)
    int depthBits;
    int stencilBits;
    int maxTextureSize;
    int maxLights;
    int maxAttribStacks;
    int maxModelViewStacks;
    int maxProjectionStacks;
    int maxClipPlanes;
    int maxTextureStacks;
    int pixelFormat;                    // current pixel format mode
    vector<glPixelFormat> pixelFormats; // list of all OpenGL pixel formats
};


// constants //////////////////////////////////////////////////////////////////
const char* COPYRIGHT = "glinfo.exe v1.04  Copyright (c) 2002-2005  Song Ho Ahn  2005-06-23";
const char* LOG_FILE = "glinfo.txt";

// function prototypes ////////////////////////////////////////////////////////
// CALLBACK functions
void displayCB(void);
void reshapeCB(int w, int h);
void keyboardCB(unsigned char key, int x, int y);
LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

bool registerWindowClass(const char* className, HINSTANCE instance);
HWND createWindow(const char* title, const char* className, HINSTANCE instance);
bool setPixelFormat(HDC hdc, int colorbits, unsigned int flags=0);
int  findPixelFormat(HDC hdc, int colorbits);
void getGLInfo(HDC hdc, glInfo& info);
void printGLInfo(glInfo& info);
void drawString(char *str, int x, int y, void *font);



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
int main(int argc, char** argv)
{
    const char* CLASS_NAME = "glinfo";

    HWND    hwnd;                               // handle to window
    HDC     hdc;                                // handle to display device context
    HGLRC   hglrc;                              // handle to OpenGL rendering context

    // register window class
    if(!registerWindowClass(CLASS_NAME, GetModuleHandle(0)))
    {
        MessageBox(0, "Window Class Registration Failed.", "Error", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // create window
    hwnd = createWindow("glinfo", CLASS_NAME, GetModuleHandle(0));
    if(!hwnd)
    {
        MessageBox(0, "Window Creation Failed.", "Error", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // retrieve a handle to a display device context
    hdc = GetDC(hwnd);

    // set pixel format
    if(!setPixelFormat(hdc, 32))
    {
        MessageBox(0, "Cannot set a suitable pixel format.", "Error", MB_ICONEXCLAMATION | MB_OK);
        ReleaseDC(hwnd, hdc);                   // remove device context
        return 0;
    }

    // create a new OpenGL rendering context
    hglrc = wglCreateContext(hdc);
    wglMakeCurrent(hdc, hglrc);

    // get openGL info
    glInfo info;
    getGLInfo(hdc, info);

    // report OpenGL info
    printGLInfo(info);

    // we don't need to show window. We only need OpenGL info.
    ShowWindow(hwnd, SW_HIDE);

    // we are done collecting GL info, quit the application
    PostQuitMessage(0);

    // main message loop
    MSG  msg;
    while(GetMessage(&msg, 0, 0, 0) > 0) 
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // delete DC and RC
    wglMakeCurrent(0, 0);
    wglDeleteContext(hglrc);
    ReleaseDC(hwnd, hdc);

    // destroy the window explicitly
    DestroyWindow(hwnd);

    return msg.wParam;
}
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@



///////////////////////////////////////////////////////////////////////////////
// initialize WNDCLASSEX and register it
///////////////////////////////////////////////////////////////////////////////
bool registerWindowClass(const char* className, HINSTANCE instance)
{
    WNDCLASSEX winClass;                        // window class information

    // fill all fields of WNDCLASSEX
    winClass.cbSize        = sizeof(WNDCLASSEX);
    winClass.style         = CS_OWNDC;
    winClass.lpfnWndProc   = WindowProcedure;           // window procedure
    winClass.cbClsExtra    = 0;
    winClass.cbWndExtra    = 0;
    winClass.hInstance     = instance;                  // owner of this class
    winClass.hIcon         = 0;
    winClass.hCursor       = LoadCursor(0, IDC_ARROW);  // arrow cursor
    winClass.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);  // the value 1 must be added to the chosen color
    winClass.lpszMenuName  = 0;
    winClass.lpszClassName = className;
    winClass.hIconSm       = 0;                         // small icon

    // register it and return the result, too
    ATOM returnValue = RegisterClassEx(&winClass);
    if(returnValue)
        return true;
    else
        return false;
}



///////////////////////////////////////////////////////////////////////////////
// create a window
///////////////////////////////////////////////////////////////////////////////
HWND createWindow(const char* title, const char* className, HINSTANCE instance)
{
    HWND hwnd;
    hwnd = CreateWindowEx(WS_EX_CLIENTEDGE,     // window border with a sunken edge
                          className,            // name of a registered window class
                          title,                // caption of window
                          WS_OVERLAPPEDWINDOW,  // window style
                          CW_USEDEFAULT,        // x position
                          CW_USEDEFAULT,        // y position
                          CW_USEDEFAULT,        // witdh
                          CW_USEDEFAULT,        // height
                          0,                    // handle to parent window
                          0,                    // handle to menu
                          instance,             // application instance
                          0);                   // window creation data

    return hwnd;
}



///////////////////////////////////////////////////////////////////////////////
// choose pixel format
// By default, pdf.dwFlags is set PFD_DRAW_TO_WINDOW, PFD_DOUBLEBUFFER and PFD_SUPPORT_OPENGL.
// Other bit flags that can be combined with are;
//  PFD_STEREO
//  PFD_GENERIC_ACCELERATED
//  PFD_GENERIC_FORMAT
//  PFD_DEPTH_DONTCARE
//  PFD_DOUBLEBUFFER_DONTCARE
//  PFD_STEREO_DONTCARE
//
// For more flags, see PIXELFORMATDESCRIPTOR document from MS.
///////////////////////////////////////////////////////////////////////////////
bool setPixelFormat(HDC hdc, int colorbits, unsigned int flags)
{
    PIXELFORMATDESCRIPTOR pfd;

    // fill required PIXELFORMATDESCRIPTOR
    memset(&pfd, 0, sizeof(pfd));
    pfd.nSize        = sizeof(pfd);
    pfd.nVersion     = 1;
    pfd.dwFlags      = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER | flags;
    pfd.iPixelType   = PFD_TYPE_RGBA;
    pfd.cColorBits   = colorbits;

    // find best pixel format
    int pixelFormat = ChoosePixelFormat(hdc, &pfd);
    if(!pixelFormat)
        return false;

    // set the fixel format
    if(!SetPixelFormat(hdc, pixelFormat, &pfd))
        return false;

    DescribePixelFormat(hdc, pixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd);
    return true;
}



///////////////////////////////////////////////////////////////////////////////
// find the best pixel format
///////////////////////////////////////////////////////////////////////////////
int findPixelFormat(HDC hdc, int colorbits)
{
    int currMode;                               // pixel format mode ID
    int bestMode = 0;                           // return value, best pixel format
    int currScore = 0;                          // points of current mode
    int bestScore = 0;                          // points of best candidate
    PIXELFORMATDESCRIPTOR pfd;

    // search the available formats for the best mode
    bestMode = 0;
    bestScore = 0;
    for(currMode = 1; DescribePixelFormat(hdc, currMode, sizeof(pfd), &pfd) > 0; ++currMode)
    {
        // ignore if cannot support opengl
        if(!(pfd.dwFlags & PFD_SUPPORT_OPENGL))
            continue;

        // ignore if cannot render into a window
        if(!(pfd.dwFlags & PFD_DRAW_TO_WINDOW))
            continue;

        // ignore if cannot support rgba mode
        if((pfd.iPixelType != PFD_TYPE_RGBA) || (pfd.dwFlags & PFD_NEED_PALETTE))
            continue;

        // ignore software only devices
        if(pfd.dwFlags & PFD_GENERIC_FORMAT)
            continue;

        // try to find best candidate
        currScore = 0;

        // colour bits
        if(pfd.cColorBits >= colorbits) ++currScore;
        if(pfd.cColorBits == colorbits) ++currScore;

        // stencil bits
        if(pfd.cStencilBits > 0) ++currScore;

        // alpha bits
        if(pfd.cAlphaBits > 0) ++currScore;

        // check if it is best mode so far
        if(currScore > bestScore)
        {
            bestScore = currScore;
            bestMode = currMode;
        }
    }

    return bestMode;
}



///////////////////////////////////////////////////////////////////////////////
// Window Procedure called by Windows system with messages
///////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;           // return zero when processed

    // provide default processing for any window messages that this cannot process here
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam); 
    }
}



///////////////////////////////////////////////////////////////////////////////
// extract openGL info
// This function must be called after GL rendering context opened.
///////////////////////////////////////////////////////////////////////////////
void getGLInfo(HDC hdc, glInfo& info)
{
    char* str = 0;
    char* tok = 0;

    // get vendor string
    str = (char*)glGetString(GL_VENDOR);
    if(str) info.vendor = str;                  // check NULL return value

    // get renderer string
    str = (char*)glGetString(GL_RENDERER);
    if(str) info.renderer = str;                // check NULL return value

    // get version string
    str = (char*)glGetString(GL_VERSION);
    if(str) info.version = str;                 // check NULL return value


    // get all extensions as a string
    str = (char*)glGetString(GL_EXTENSIONS);

    // split extensions
    if(str)
    {
        tok = strtok((char*)str, " ");
        while(tok)
        {
            info.extensions.push_back(tok);     // put a extension into struct
            tok = strtok(0, " ");               // next token
        }
    }

    // sort extension by alphabetical order
    std::sort(info.extensions.begin(), info.extensions.end());

    // get number of color bits
    glGetIntegerv(GL_RED_BITS, &info.colorBits[0]);
    glGetIntegerv(GL_GREEN_BITS, &info.colorBits[1]);
    glGetIntegerv(GL_BLUE_BITS, &info.colorBits[2]);
    glGetIntegerv(GL_ALPHA_BITS, &info.colorBits[3]);

    // get depth bits
    glGetIntegerv(GL_DEPTH_BITS, &info.depthBits);

    // get stecil bits
    glGetIntegerv(GL_STENCIL_BITS, &info.stencilBits);

    // get max number of lights allowed
    glGetIntegerv(GL_MAX_LIGHTS, &info.maxLights);

    // get max texture resolution
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &info.maxTextureSize);

    // get max number of clipping planes
    glGetIntegerv(GL_MAX_CLIP_PLANES, &info.maxClipPlanes);

    // get max modelview and projection matrix stacks
    glGetIntegerv(GL_MAX_MODELVIEW_STACK_DEPTH, &info.maxModelViewStacks);
    glGetIntegerv(GL_MAX_PROJECTION_STACK_DEPTH, &info.maxProjectionStacks);
    glGetIntegerv(GL_MAX_ATTRIB_STACK_DEPTH, &info.maxAttribStacks);

    // get max texture stacks
    glGetIntegerv(GL_MAX_TEXTURE_STACK_DEPTH, &info.maxTextureStacks);

    // get the current pixel format
    info.pixelFormat = GetPixelFormat(hdc);

    // get info of all pixel formats //////////////////////
    int pfCount;                                // number of pixel formats
    PIXELFORMATDESCRIPTOR pfd;
    glPixelFormat pf;

    // get number of pixel format
    pfCount = DescribePixelFormat(hdc, 0, 0, 0);

    for(int i=1; i <= pfCount; ++i)
    {
        DescribePixelFormat(hdc, i, sizeof(PIXELFORMATDESCRIPTOR), &pfd);

        // skip if not support OpenGL
        if(!(pfd.dwFlags & PFD_SUPPORT_OPENGL))
            continue;

        pf.index = i;                           // index

        // draw to ?
        if(pfd.dwFlags & PFD_DRAW_TO_WINDOW)
            pf.drawTo = 1;
        else if(pfd.dwFlags & PFD_DRAW_TO_BITMAP)
            pf.drawTo = 2;
        else
            pf.drawTo = 0;  // unknown

        // pixel type
        if(pfd.iPixelType == PFD_TYPE_RGBA)
            pf.type = 1;
        else if(pfd.iPixelType == PFD_TYPE_COLORINDEX)
            pf.type = 2;
        else
            pf.type = 0;    // unknown

        pf.colorBits = pfd.cColorBits;          // color bits

        // r, g, b, a bits for RGBA type
        if(pfd.iPixelType == PFD_TYPE_RGBA)
        {
            pf.redBits = pfd.cRedBits;
            pf.greenBits = pfd.cGreenBits;
            pf.blueBits = pfd.cBlueBits;
            pf.alphaBits = pfd.cAlphaBits;
        }
        else
        {
            pf.redBits = 0;
            pf.greenBits = 0;
            pf.blueBits = 0;
            pf.alphaBits = 0;
        }

        pf.depthBits = pfd.cDepthBits;          // depth bits
        pf.stencilBits = pfd.cStencilBits;      // stencil bits

        // support double buffer and stereo buffer
        if(pfd.dwFlags & PFD_DOUBLEBUFFER)  pf.doubleBuffer = true;
        else                                pf.doubleBuffer = false;

        if(pfd.dwFlags & PFD_STEREO)        pf.stereoBuffer = true;
        else                                pf.stereoBuffer = false;

        // number of aux buffers
        pf.auxBuffers = pfd.cAuxBuffers;

        // add it into container
        info.pixelFormats.push_back(pf);
    }

}



///////////////////////////////////////////////////////////////////////////////
// print OpenGL info to screen and save to a file
///////////////////////////////////////////////////////////////////////////////
void printGLInfo(glInfo& info)
{
    stringstream ss;
    int i;

    // copyright
    ss << endl;
    ss << COPYRIGHT << endl;

    ss << endl; // blank line
    ss << "OpenGL Driver Info" << endl;
    ss << "==================" << endl;
    ss << "Vendor: " << info.vendor << endl;
    ss << "Version: " << info.version << endl;
    ss << "Renderer: " << info.renderer << endl;

    ss << endl;
    ss << "Color Bits(R,G,B,A): (" << info.colorBits[0] << ", " << info.colorBits[1]
       << ", " << info.colorBits[2] << ", " << info.colorBits[3] << ")\n";
    ss << "Depth Bits: " << info.depthBits << endl;
    ss << "Stencil Bits: " << info.stencilBits << endl;

    ss << endl;
    ss << "Max Texture Size: " << info.maxTextureSize << "x" << info.maxTextureSize << endl;
    ss << "Max Lights: " << info.maxLights << endl;
    ss << "Max Clip Planes: " << info.maxClipPlanes << endl;
    ss << "Max Modelview Matrix Stacks: " << info.maxModelViewStacks << endl;
    ss << "Max Projection Matrix Stacks: " << info.maxProjectionStacks << endl;
    ss << "Max Attribute Stacks: " << info.maxAttribStacks << endl;
    ss << "Max Texture Stacks: " << info.maxTextureStacks << endl;

    ss << endl;
    ss << "Total Number of Extensions: " << info.extensions.size() << endl;
    ss << "==============================" << endl;
    for(i = 0; i < info.extensions.size(); ++i)
        ss << info.extensions.at(i) << endl;

    ss << endl;
    ss << "Pixel Format Info" << endl;
    ss << "=================" << endl;
    ss << "Current Index: " << info.pixelFormat << "\n\n";

    // table heading of pixel formats
    ss << "      Draw Pixel Color             Depth Stencil Double Stereo Aux    " << endl;
    ss << "Index To   Type  Bits  R  G  B  A  Bits  Bits    Buffer Buffer Buffers" << endl;
    ss << "======================================================================" << endl;

    ss.setf(std::ios::left);                    // left justification for all columns
    for(i = 0; i < info.pixelFormats.size(); ++i)
    {
        ss << std::setw(6) << info.pixelFormats[i].index;

        if(info.pixelFormats[i].drawTo == 1)        ss << "win  ";
        else if(info.pixelFormats[i].drawTo == 2)   ss << "bmp  ";
        else                                        ss << ".    ";

        if(info.pixelFormats[i].type == 1)          ss << "rgba  ";
        else if(info.pixelFormats[i].type == 2)     ss << "index ";
        else                                        ss << ".     ";

        if(info.pixelFormats[i].colorBits > 0)      ss << std::setw(6) << info.pixelFormats[i].colorBits;
        else                                        ss << ".     ";

        if(info.pixelFormats[i].redBits > 0)        ss << std::setw(3) << info.pixelFormats[i].redBits;
        else                                        ss << ".  ";

        if(info.pixelFormats[i].greenBits > 0)      ss << std::setw(3) << info.pixelFormats[i].greenBits;
        else                                        ss << ".  ";

        if(info.pixelFormats[i].blueBits > 0)       ss << std::setw(3) << info.pixelFormats[i].blueBits;
        else                                        ss << ".  ";

        if(info.pixelFormats[i].alphaBits > 0)      ss << std::setw(3) << info.pixelFormats[i].alphaBits;
        else                                        ss << ".  ";

        if(info.pixelFormats[i].depthBits > 0)      ss << std::setw(6) << info.pixelFormats[i].depthBits;
        else                                        ss << ".     ";

        if(info.pixelFormats[i].stencilBits > 0)    ss << std::setw(8) << info.pixelFormats[i].stencilBits;
        else                                        ss << ".       ";

        if(info.pixelFormats[i].doubleBuffer)       ss << "yes    ";
        else                                        ss << ".      ";

        if(info.pixelFormats[i].stereoBuffer)       ss << "yes    ";
        else                                        ss << ".      ";

        if(info.pixelFormats[i].auxBuffers > 0)     ss << std::setw(7) << info.pixelFormats[i].auxBuffers;
        else                                        ss << ".      ";

        ss << endl; // next line
    }
    ss.unsetf(std::ios::left);
    ss << "======================================================================" << endl;

    cout << ss.str() << endl;

    // to the file
    ofstream file(LOG_FILE, std::ios::out);
    file << ss.str();
    file.close();   // close the file explicitly

    cout << "...Saved info to " << LOG_FILE << "...\n";
}
