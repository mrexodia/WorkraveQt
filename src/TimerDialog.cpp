#include "TimerDialog.h"
#include "ui_TimerDialog.h"
#include "Helpers.h"

#include <QSettings>
#include <QCloseEvent>
#include <QShowEvent>

#if defined(Q_OS_LINUX)
#include <xcb/xcb.h>
#include <QGuiApplication>
#if QT_VERSION >= QT_VERSION_CHECK(6, 2, 0)
#include <QtGui/qguiapplication_platform.h>
#else
#include <QX11Info>
#endif
#endif

TimerDialog::TimerDialog(QWidget* parent) :
    QTrayDialog(parent),
    ui(new Ui::TimerDialog)
{
    ui->setupUi(this);
    setWindowFlag(Qt::WindowStaysOnTopHint);
    setWindowFlag(Qt::WindowFullscreenButtonHint, false);
    setFixedSize(size());

    restoreDialogGeometry();
    QSettings settings;
    if(settings.value("TimerDialog/visible").toBool())
        setVisible(true);
}

TimerDialog::~TimerDialog()
{
    delete ui;
}

static void setTimerProgress(QProgressBar* progressBar, QLabel* labelTimer, int progress)
{
    progressBar->setValue(progress);
    progressBar->update();
    labelTimer->setText(Helpers::timeFormat(progressBar->maximum() - progress));
}

void TimerDialog::setMicroBreakMaximum(int microBreakMaximum)
{
    ui->progressBarMicroBreak->setMaximum(microBreakMaximum);
    setMicroBreakProgress(0);
}

void TimerDialog::setMicroBreakProgress(int microBreakValue)
{
    setTimerProgress(ui->progressBarMicroBreak, ui->labelMicroBreakTimer, microBreakValue);;
}

void TimerDialog::setRestBreakMaximum(int restBreakMaximum)
{
    ui->progressBarRestBreak->setMaximum(restBreakMaximum);
    setRestBreakProgress(0);
}

void TimerDialog::setRestBreakProgress(int restBreakValue)
{
    setTimerProgress(ui->progressBarRestBreak, ui->labelRestBreakTimer, restBreakValue);
}

void TimerDialog::setIdleMaximum(int idleMaximum)
{
    ui->progressBarIdle->setMaximum(idleMaximum);
    setIdleProgress(0);
}

void TimerDialog::setIdleProgress(int idleValue)
{
	setTimerProgress(ui->progressBarIdle, ui->labelIdleTimer, idleValue);
}

void TimerDialog::closeEvent(QCloseEvent* event)
{
    QSettings settings;
    settings.setValue("TimerDialog/visible", isVisible());
    QTrayDialog::closeEvent(event);
}

void TimerDialog::showEvent(QShowEvent* event)
{
    QTrayDialog::showEvent(event);
    applySkipTaskbarHint();
}

void TimerDialog::applySkipTaskbarHint()
{
#if defined(Q_OS_LINUX)
    xcb_connection_t *conn = nullptr;
    
#if QT_VERSION >= QT_VERSION_CHECK(6, 2, 0)
    auto *x11App = qGuiApp->nativeInterface<QNativeInterface::QX11Application>();
    if (x11App)
        conn = x11App->connection();
#else
    conn = QX11Info::connection();
#endif

    if (!conn)
        return;

    xcb_window_t wid = static_cast<xcb_window_t>(winId());

    // Get the atoms we need
    auto getAtom = [conn](const char *name) -> xcb_atom_t {
        xcb_intern_atom_cookie_t cookie = xcb_intern_atom(conn, 0, strlen(name), name);
        xcb_intern_atom_reply_t *reply = xcb_intern_atom_reply(conn, cookie, nullptr);
        xcb_atom_t atom = reply ? reply->atom : static_cast<xcb_atom_t>(XCB_ATOM_NONE);
        free(reply);
        return atom;
    };

    xcb_atom_t netWmState = getAtom("_NET_WM_STATE");
    xcb_atom_t skipTaskbar = getAtom("_NET_WM_STATE_SKIP_TASKBAR");
    xcb_atom_t skipPager = getAtom("_NET_WM_STATE_SKIP_PAGER");

    if (netWmState == XCB_ATOM_NONE || skipTaskbar == XCB_ATOM_NONE)
        return;

    // Set the property directly on the window
    xcb_atom_t atoms[2] = { skipTaskbar, skipPager };
    xcb_change_property(conn, XCB_PROP_MODE_APPEND, wid, netWmState, XCB_ATOM_ATOM, 32, 2, atoms);

    // Send client message to root window (EWMH-compliant way)
    xcb_screen_t *screen = xcb_setup_roots_iterator(xcb_get_setup(conn)).data;
    
    xcb_client_message_event_t evt = {};
    evt.response_type = XCB_CLIENT_MESSAGE;
    evt.format = 32;
    evt.window = wid;
    evt.type = netWmState;
    evt.data.data32[0] = 1;  // _NET_WM_STATE_ADD
    evt.data.data32[1] = skipTaskbar;
    evt.data.data32[2] = skipPager;
    evt.data.data32[3] = 1;  // Source indication: normal application

    xcb_send_event(conn, 0, screen->root,
                   XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT | XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY,
                   reinterpret_cast<const char*>(&evt));

    xcb_flush(conn);
#endif
}
