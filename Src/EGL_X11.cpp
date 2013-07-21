#include "common.hpp"
#include "App.hpp"
#include "EGL.hpp"

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

#include <memory.h>
#define TRUE 1
#define FALSE 0

using namespace std;
Display * x_display;

namespace Encom13 {

    bool App::processEvents() {
        XEvent xev;
        KeySym key;
        bool exit = false;
        char text;

        // Pump all messages from X server. Keypresses are directed to keyfunc (if defined)
        while (XPending(x_display)) {
            XNextEvent(x_display, &xev);
            if (xev.type == KeyPress) {
                if (XLookupString(&xev.xkey, &text, 1, &key, 0) == 1) {
                    if (!keys(text, 0, 0)) {
                        exit = true;
                    }
                }
            }
            if (xev.type == ConfigureNotify) {
                XConfigureEvent xce = xev.xconfigure;
                if (xce.width != window.width || xce.height != window.height) {
                    resize(xce.width, xce.height);
                }
            }
            if (xev.type == DestroyNotify)
                exit = true;
        }
        return !exit;
    }





    //note the struct is declared elsewhere, is here just for clarity.
    //code is from [http://tonyobryan.com/index.php?article=9][1]
    typedef struct MotifHints
    {
        unsigned long   flags;
        unsigned long   functions;
        unsigned long   decorations;
        long            inputMode;
        unsigned long   status;
    } Hints;



    namespace GL {

        EGLNativePixmapType EGL::createNativePixmap() {
             Window window = createNativeWindow();
             int d = XDefaultDepth(x_display, 0);
             return XCreatePixmap(x_display, window, width * 10, height * 10, 32);
        }

        EGLNativeWindowType EGL::createNativeWindow() {
            if (x_display == NULL) {
                x_display = XOpenDisplay(NULL);
            }
            if (x_display == NULL) {
                throw exception();
            }

            Window root = DefaultRootWindow(x_display);
            XSetWindowAttributes swa;
            swa.event_mask = ExposureMask | StructureNotifyMask | PointerMotionMask | KeyPressMask;
            Window win = XCreateWindow(x_display, root, 100, 100, width, height, 0,
            CopyFromParent, InputOutput, CopyFromParent, CWEventMask, &swa);

            XSetWindowAttributes xattr;
            xattr.override_redirect = FALSE;
            XChangeWindowAttributes(x_display, win, CWOverrideRedirect, &xattr);

            {
                XWMHints hints;
                hints.input = TRUE;
                hints.flags = InputHint;
                XSetWMHints(x_display, win, &hints);
            }

            {
                //code to remove decoration
                MotifHints hints;
                hints.flags = 2;
                hints.decorations = 0;
                Atom property = XInternAtom(x_display, "_MOTIF_WM_HINTS", true);
                XChangeProperty(x_display,win,property,property,32,PropModeReplace,(unsigned char *)&hints,5);
            }

            // make the window visible on the screen
            XMapWindow(x_display, win);
            XStoreName(x_display, win, "TronHorrible");
            XMoveWindow(x_display, win, x, y);

            // get identifiers for the provided atom name strings
            Atom wm_state = XInternAtom(x_display, "_NET_WM_STATE", FALSE);

            XEvent xev;
            memset(&xev, 0, sizeof(xev));
            xev.type = ClientMessage;
            xev.xclient.window = win;
            xev.xclient.message_type = wm_state;
            xev.xclient.format = 32;
            xev.xclient.data.l[0] = 1;
            xev.xclient.data.l[1] = FALSE;
            XSendEvent(x_display, DefaultRootWindow ( x_display ),
            FALSE,
            SubstructureNotifyMask, &xev);

            return (EGLNativeWindowType) win;
        }

    } // GL namespace

}
