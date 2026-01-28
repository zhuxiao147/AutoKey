#ifndef HIDDEVICE_H
#define HIDDEVICE_H

#include <QObject>
#include <QByteArray>
#include <memory>

#ifdef _WIN32
#include <windows.h>
#endif

class HIDDevice : public QObject
{
    Q_OBJECT

public:
    explicit HIDDevice(QObject *parent = nullptr);
    ~HIDDevice();

    bool connect(uint16_t vendorId, uint16_t productId);
    void disconnect();
    bool isConnected() const;
    
    bool sendData(const QByteArray &data);
    QByteArray getData();

signals:
    void deviceStatusChanged(bool connected);

private:
    bool sendReport(const QByteArray &data);
    QByteArray calculateCRC32(const QByteArray &data);
    
#ifdef _WIN32
    HANDLE m_deviceHandle;
#else
    void* m_deviceHandle;
#endif
    
    bool m_connected;
    uint16_t m_vendorId;
    uint16_t m_productId;
};

#endif // HIDDEVICE_H