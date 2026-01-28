#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class HIDDevice;
class KeySequenceParser;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onConnectDevice();
    void onSendSequence();
    void onCheckSequence();
    void onDeviceStatusChanged(bool connected);

private:
    void updateUI();
    void showStatusMessage(const QString &message);

    Ui::MainWindow *ui;
    std::unique_ptr<HIDDevice> m_hidDevice;
    std::unique_ptr<KeySequenceParser> m_sequenceParser;
};

#endif // MAINWINDOW_H
