#include "hiddevice.h"
#include <QDebug>
#include <QTimer>
#include <QDataStream>

#ifdef _WIN32
#include <setupapi.h>
#include <hidsdi.h>
#pragma comment(lib, "setupapi.lib")
#pragma comment(lib, "hid.lib")
#endif

#define HID_REPORT_ID 1U
#define HID_REPORT_SIZE 64

HIDDevice::HIDDevice(QObject *parent)
    : QObject(parent)
    , m_deviceHandle(nullptr)
    , m_connected(false)
    , m_vendorId(0)
    , m_productId(0)
{
}

HIDDevice::~HIDDevice()
{
    disconnect();
}

bool HIDDevice::connect(uint16_t vendorId, uint16_t productId)
{
    if (m_connected) {
        disconnect();
    }
    
    m_vendorId = vendorId;
    m_productId = productId;
    
#ifdef _WIN32
    GUID hidGuid;
    HidD_GetHidGuid(&hidGuid);
    
    HDEVINFO deviceInfo = SetupDiGetClassDevs(&hidGuid, nullptr, nullptr, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
    if (deviceInfo == INVALID_HANDLE_VALUE) {
        qDebug() << "Failed to get device information";
        return false;
    }
    
    SP_DEVICE_INTERFACE_DATA deviceInterfaceData;
    deviceInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
    
    DWORD deviceIndex = 0;
    bool deviceFound = false;
    
    while (SetupDiEnumDeviceInterfaces(deviceInfo, nullptr, &hidGuid, deviceIndex, &deviceInterfaceData)) {
        deviceIndex++;
        
        DWORD requiredSize = 0;
        SetupDiGetDeviceInterfaceDetail(deviceInfo, &deviceInterfaceData, nullptr, 0, &requiredSize, nullptr);
        
        PSP_DEVICE_INTERFACE_DETAIL_DATA deviceDetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc(requiredSize);
        if (!deviceDetailData) continue;
        
        deviceDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
        
        if (SetupDiGetDeviceInterfaceDetail(deviceInfo, &deviceInterfaceData, deviceDetailData, requiredSize, nullptr, nullptr)) {
            HANDLE deviceHandle = CreateFile(deviceDetailData->DevicePath, 
                                           GENERIC_READ | GENERIC_WRITE,
                                           FILE_SHARE_READ | FILE_SHARE_WRITE,
                                           nullptr, OPEN_EXISTING, 0, nullptr);
            
            if (deviceHandle != INVALID_HANDLE_VALUE) {
                PHIDP_PREPARSED_DATA preparsedData;
                if (HidD_GetPreparsedData(deviceHandle, &preparsedData)) {
                    HIDP_CAPS capabilities;
                    if (HidP_GetCaps(preparsedData, &capabilities) == HIDP_STATUS_SUCCESS) {
                        HIDD_ATTRIBUTES attributes;
                        attributes.Size = sizeof(HIDD_ATTRIBUTES);
                        
                        if (HidD_GetAttributes(deviceHandle, &attributes)) {
                            if (attributes.VendorID == vendorId && attributes.ProductID == productId) {
                                if((capabilities.UsagePage & HID_USAGE_PAGE_VENDOR_DEFINED_BEGIN) == HID_USAGE_PAGE_VENDOR_DEFINED_BEGIN)
                                {
                                    m_deviceHandle = deviceHandle;
                                    deviceFound = true;
                                    HidD_FreePreparsedData(preparsedData);
                                    break;
                                }
                            }
                        }
                    }
                    HidD_FreePreparsedData(preparsedData);
                }
                
                if (!deviceFound) {
                    CloseHandle(deviceHandle);
                }
            }
        }
        
        free(deviceDetailData);
    }
    
    SetupDiDestroyDeviceInfoList(deviceInfo);
    
    if (deviceFound) {
        m_connected = true;
        emit deviceStatusChanged(true);
        qDebug() << "Device connected successfully";
        return true;
    }
    
    qDebug() << "Device not found";
    return false;
#else
    qDebug() << "HID support not implemented for this platform";
    return false;
#endif
}

void HIDDevice::disconnect()
{
    if (m_deviceHandle) {
#ifdef _WIN32
        CloseHandle(m_deviceHandle);
#endif
        m_deviceHandle = nullptr;
    }
    
    if (m_connected) {
        m_connected = false;
        emit deviceStatusChanged(false);
    }
}

bool HIDDevice::isConnected() const
{
    return m_connected;
}

bool HIDDevice::sendData(const QByteArray &data)
{
    if (!m_connected || !m_deviceHandle) {
        return false;
    }
    
    QByteArray packet;
    QDataStream stream(&packet, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    
    stream << static_cast<quint32>(data.size());
    stream.writeRawData(data.constData(), data.size());
    
    QByteArray crc32 = calculateCRC32(data);
    stream.writeRawData(crc32.constData(), 4);
    
    return sendReport(packet);
}

QByteArray HIDDevice::getData()
{
    if (!m_connected || !m_deviceHandle) {
        return QByteArray();
    }
    
    QByteArray reportData;
    reportData.resize(64);
    
#ifdef _WIN32
    DWORD bytesRead = 0;
    if (ReadFile(m_deviceHandle, reportData.data(), reportData.size(), &bytesRead, nullptr)) {
        reportData.resize(bytesRead);
        
        if (bytesRead >= 8) {
            QDataStream stream(reportData);
            stream.setByteOrder(QDataStream::LittleEndian);
            
            quint32 length;
            stream >> length;
            
            if (bytesRead >= static_cast<DWORD>(8 + length)) {
                QByteArray data = reportData.mid(4, length);
                QByteArray receivedCrc32 = reportData.mid(4 + length, 4);
                QByteArray calculatedCrc32 = calculateCRC32(data);
                
                if (receivedCrc32 == calculatedCrc32) {
                    return data;
                }
            }
        }
    }
#endif
    
    return QByteArray();
}

bool HIDDevice::sendReport(QByteArray data)
{
    if (!m_deviceHandle) {
        return false;
    }

    for (int index = 1; data.size() > 0 ; index++)
    {
        int bytesToSend = min(data.size(), HID_REPORT_SIZE - 2);
        if(index > 256)
        {
            return false;
        }

        /*
         * 发送数据的格式：
         * 第一个字节：Report ID
         * 第二个字节：数据序号
         * 第三个字节：数据长度
         * 从第四个字节开始：数据内容
         */
        // 发送数据（第一个字节通常是 Report ID，0 表示无 ID）
        unsigned char outReport[HID_REPORT_SIZE + 1] = {0};
        outReport[0] = HID_REPORT_ID; // Report ID
        outReport[1] = index;  
        outReport[2] = bytesToSend;  

        memcpy(outReport + 3, data.constData(), bytesToSend);
        data.remove(0, bytesToSend);

    #ifdef _WIN32
        if (!HidD_SetOutputReport(m_deviceHandle, outReport, HID_REPORT_SIZE)) {
            qDebug("Failed to send output report.\n");
            return false;
        } else {
            qDebug("Output report sent.\n");
            return true;
        }
    #endif
    }
    
    return true;
    
}

QByteArray HIDDevice::calculateCRC32(const QByteArray &data)
{
    quint32 crc = 0xFFFFFFFF;
    
    for (int i = 0; i < data.size(); i++) {
        crc ^= static_cast<quint8>(data[i]);
        for (int j = 0; j < 8; j++) {
            if (crc & 1) {
                crc = (crc >> 1) ^ 0xEDB88320;
            } else {
                crc >>= 1;
            }
        }
    }
    
    crc ^= 0xFFFFFFFF;
    
    QByteArray result;
    QDataStream stream(&result, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << crc;
    
    return result;
}
