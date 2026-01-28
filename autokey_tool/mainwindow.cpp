#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "hiddevice.h"
#include "keysequenceparser.h"
#include <QMessageBox>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_hidDevice(std::make_unique<HIDDevice>())
    , m_sequenceParser(std::make_unique<KeySequenceParser>())
{
    ui->setupUi(this);
    
    setWindowTitle("自动按键设置工具");
    
    connect(ui->connectButton, &QPushButton::clicked, this, &MainWindow::onConnectDevice);
    connect(ui->sendButton, &QPushButton::clicked, this, &MainWindow::onSendSequence);
    connect(ui->checkButton, &QPushButton::clicked, this, &MainWindow::onCheckSequence);
    
    connect(m_hidDevice.get(), &HIDDevice::deviceStatusChanged, 
            this, &MainWindow::onDeviceStatusChanged);
    
    updateUI();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onConnectDevice()
{
    if (m_hidDevice->isConnected()) {
        m_hidDevice->disconnect();
    } else {
        if (m_hidDevice->connect(0x1234, 0x5678)) {
            showStatusMessage("设备连接成功");
        } else {
            QMessageBox::warning(this, "连接失败", "无法连接到指定的HID设备");
            showStatusMessage("设备连接失败");
        }
    }
}

void MainWindow::onSendSequence()
{
    if (!m_hidDevice->isConnected()) {
        QMessageBox::warning(this, "错误", "请先连接HID设备");
        return;
    }
    
    QString sequence = ui->sequenceEdit->toPlainText().trimmed();
    if (sequence.isEmpty()) {
        QMessageBox::warning(this, "错误", "请输入按键序列");
        return;
    }
    
    QByteArray data = m_sequenceParser->parseSequence(sequence);
    if (data.isEmpty()) {
        QMessageBox::warning(this, "错误", "按键序列格式错误");
        return;
    }
    
    if (m_hidDevice->sendData(data)) {
        showStatusMessage("按键序列发送成功");
    } else {
        QMessageBox::warning(this, "错误", "发送按键序列失败");
        showStatusMessage("发送按键序列失败");
    }
}

void MainWindow::onCheckSequence()
{
    QString sequence = ui->sequenceEdit->toPlainText().trimmed();
    if (sequence.isEmpty()) {
        QMessageBox::warning(this, "错误", "请输入按键序列");
        return;
    }
    
    QByteArray data = m_sequenceParser->parseSequence(sequence);
    if (data.isEmpty()) {
        QMessageBox::warning(this, "错误", "按键序列格式错误");
    } else {
        QMessageBox::information(this, "检查结果", "按键序列格式正确");
    }
}

void MainWindow::onDeviceStatusChanged(bool connected)
{
    updateUI();
    if (connected) {
        showStatusMessage("设备已连接");
    } else {
        showStatusMessage("设备已断开");
    }
}

void MainWindow::updateUI()
{
    bool connected = m_hidDevice->isConnected();
    ui->connectButton->setText(connected ? "断开设备" : "连接设备");
    ui->sendButton->setEnabled(connected);
    ui->statusLabel->setText(connected ? "已连接" : "未连接");
}

void MainWindow::showStatusMessage(const QString &message)
{
    statusBar()->showMessage(message, 3000);
}