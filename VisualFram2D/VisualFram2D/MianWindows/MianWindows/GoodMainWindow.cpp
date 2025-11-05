//#include "GoodMainWindow.h"
//
//GoodMainWindow::GoodMainWindow(QWidget* parent) : QGoodWindow(parent)
//{
//    initUI();
//}
//
//
//GoodMainWindow::~GoodMainWindow()
//{
//}
//
//void GoodMainWindow::initUI()
//{
//    mCentralWidget = new MianWindows(this);
//    mCentralWidget->setWindowFlags(Qt::Widget);
//    mGoodCentralWidget = new QGoodCentralWidget(this);
//    mGoodCentralWidget->setCentralWidget(mCentralWidget);
//    setCentralWidget(mGoodCentralWidget);
//   // this->
//    this->setWindowTitle("");//设置窗口名
//    this->setWindowIcon(QIcon(":/MianWindows/Image/Logo.png"));
//
//    QLabel* label = new QLabel("行为识别2.0");
//    label->setAttribute(Qt::WA_TransparentForMouseEvents);
//    mGoodCentralWidget->setRightTitleBarWidget(label, true);
//
//    mGoodCentralWidget->setTitleBarColor(Qt::red );
//
//    mGoodCentralWidget->setTitleVisible(false);
//
//    move(qApp->primaryScreen()->availableGeometry().center() - rect().center());
//
//
//#ifdef QGOODWINDOW
//    mGoodCentralWidget->setTitleAlignment(Qt::AlignCenter);
//#endif
//}
//
//bool GoodMainWindow::nativeEvent(const QByteArray& eventType, void* message, qgoodintptr* result)
//{
//#ifdef QGOODWINDOW
//#ifdef Q_OS_WIN
//#ifdef QT_VERSION_QT5
//    MSG* msg = static_cast<MSG*>(message);
//
//    switch (msg->message)
//    {
//    case WM_THEMECHANGED:
//    case WM_DWMCOMPOSITIONCHANGED:
//    {
//        //Keep window theme on Windows theme change events.
//        QTimer::singleShot(100, this, [=] {
//            if (qGoodStateHolder->isCurrentThemeDark())
//                QGoodWindow::setAppDarkTheme();
//            else
//                QGoodWindow::setAppLightTheme();
//            });
//
//        break;
//    }
//    default:
//        break;
//    }
//#endif
//#endif
//#endif
//    return QGoodWindow::nativeEvent(eventType, message, result);
//}
//
//void GoodMainWindow::closeEvent(QCloseEvent* event)
//{
//    QMessageBox msgbox(this);
//    msgbox.setIcon(QMessageBox::Question);
//    msgbox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
//    msgbox.setDefaultButton(QMessageBox::No);
//    msgbox.setText("Are you sure to close?");
//
//    int result = QGoodCentralWidget::execDialogWithWindow(&msgbox, this, mGoodCentralWidget);
//
//    if (result != QMessageBox::Yes)  {
//        event->ignore();
//        return;
//    }
//}