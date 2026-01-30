#include "keysequenceparser.h"
#include <QDebug>
#include <QRegularExpression>
#include <QDataStream>
#include <QCryptographicHash>

KeySequenceParser::KeySequenceParser()
{
    initializeKeyMaps();
}

QByteArray KeySequenceParser::parseSequence(const QString &sequence)
{
    if (!validateSequence(sequence)) {
        return QByteArray();
    }
    
    QVector<KeyAction> actions = parseActions(sequence);
    QByteArray data;
    QDataStream dataStream(&data, QIODevice::WriteOnly);
    dataStream.setByteOrder(QDataStream::LittleEndian);
    
    // 构建数据部分
    for (const KeyAction &action : actions) {
        QByteArray actionData = actionToBytes(action);
        dataStream.writeRawData(actionData.constData(), actionData.size());
    }
    
    // 构建完整协议：length(4字节) + data + crc32(4字节)
    QByteArray result;
    QDataStream resultStream(&result, QIODevice::WriteOnly);
    resultStream.setByteOrder(QDataStream::LittleEndian);
    
    // 写入数据长度（4字节）
    quint32 dataLength = data.size();
    resultStream << dataLength;
    
    // 写入数据
    resultStream.writeRawData(data.constData(), data.size());
    
    // 写入CRC32校验值（4字节）
    quint32 crc32 = calculateCRC32(data);
    resultStream << crc32;
    
    return result;
}

/**
 * @brief Validates a key sequence string.
 * 
 * @param sequence The key sequence string to validate.
 * @return true if the sequence is valid, false otherwise.
 */
bool KeySequenceParser::validateSequence(const QString &sequence)
{
    if (sequence.isEmpty()) {
        return false;
    }
    
    QStringList parts = sequence.split(';', Qt::SkipEmptyParts);
    
    for (const QString &part : parts) {
        QString trimmed = part.trimmed();
        if (trimmed.isEmpty()) {
            continue;
        }
        
        //判断延时格式
        if (trimmed.contains("ms") && trimmed.endsWith("ms")) {
            bool ok;
            int delay = trimmed.left(trimmed.length() - 2).toInt(&ok);
            if (!ok || delay <= 0) {
                return false;
            }
            continue;
        }
        
        //判断重复
        if (trimmed.contains('(') && trimmed.endsWith(')')) {
            int openIndex = trimmed.indexOf('(');
            int closeIndex = trimmed.lastIndexOf(')');
            if (openIndex >= 0 && closeIndex > openIndex) {
                QString countStr = trimmed.mid(openIndex + 1, closeIndex - openIndex - 1);
                bool ok;
                int count = countStr.toInt(&ok);
                if (!ok || count <= 0 || trimmed.length() != closeIndex + 1) {
                    return false;
                }
                QString key = trimmed.left(openIndex);
                trimmed = key; // Update trimmed to be the key only, matching parseActions logic
            }
        }
        
        //判断组合键或单个按键
        if (trimmed.contains('+')) {
            QStringList comboKeys = trimmed.split('+', Qt::SkipEmptyParts);
            if (comboKeys.size() < 2) {
                return false;
            }
            for (const QString &key : comboKeys) {
                if (!m_keyMap.contains(key.trimmed())) {
                    return false;
                }
            }
            continue;
        }else if (!m_keyMap.contains(trimmed)) {
            return false;
        }
    }
    
    return true;
}

/**
 * @brief Parses a key sequence string into a vector of KeyAction structs.
 * 
 * @param sequence The key sequence string to parse.
 * @return QVector<KeyAction> The vector of parsed KeyAction structs.
 */
QVector<KeyAction> KeySequenceParser::parseActions(const QString &sequence)
{
    QVector<KeyAction> actions;
    QStringList parts = sequence.split(';', Qt::SkipEmptyParts);
    
    for (const QString &part : parts) {
        QString trimmed = part.trimmed();
        if (trimmed.isEmpty()) {
            continue;
        }
        
        // 处理延时
        if (trimmed.contains("ms") && trimmed.endsWith("ms")) {
            KeyAction action;
            action.type = KeyAction::Delay;
            action.delay = trimmed.left(trimmed.length() - 2).toInt();
            actions.append(action);
            continue;
        }
        
        KeyAction action;
        action.type = KeyAction::KeyPress;
        
        // 处理重复按键
        int repeatCount = 1;
        if (trimmed.contains('(') && trimmed.endsWith(')')) {
            int openIndex = trimmed.indexOf('(');
            int closeIndex = trimmed.lastIndexOf(')');
            if (openIndex >= 0 && closeIndex > openIndex) {
                QString countStr = trimmed.mid(openIndex + 1, closeIndex - openIndex - 1);
                repeatCount = countStr.toInt();
                trimmed = trimmed.left(openIndex); // 提取按键名称
            }
        }
        
        // 处理组合键或单个按键
        if (trimmed.contains('+')) {
            QStringList comboKeys = trimmed.split('+', Qt::SkipEmptyParts);
            for (const QString &key : comboKeys) {
                action.keys.append(key.trimmed());
            }
        } else {
            action.keys.append(trimmed);
        }
        
        // 添加按键按下动作
        for (int i = 0; i < repeatCount; ++i) {
            KeyAction pressAction;
            pressAction.type = KeyAction::KeyPress;
            pressAction.keys = action.keys;
            actions.append(pressAction);
            
        }
    }
    
    return actions;
}

QByteArray KeySequenceParser::actionToBytes(const KeyAction &action)
{
    QByteArray result;
    QDataStream stream(&result, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    
    switch (action.type) {
    case KeyAction::KeyPress:
        {
            // 创建HID键盘报告（8字节）
            HIDKeyboardReport report;
            report.modifier = 0;
            report.reserved = 0;
            for (int i = 0; i < 6; ++i) {
                report.keycode[i] = 0;
            }
            
            // 设置修饰键和普通按键
            int keyIndex = 0;
            for (const QString &key : action.keys) {
                quint8 keyCode = getKeyCode(key);
                if (keyCode >= 0xE0 && keyCode <= 0xE7) {
                    // 修饰键（Ctrl, Shift, Alt, Win等）
                    report.modifier |= getModifierBits(key);
                } else if (keyIndex < 6) {
                    // 普通按键
                    report.keycode[keyIndex++] = keyCode;
                }
            }
            
            // 写入8字节HID报告
            stream << report.modifier;
            stream << report.reserved;
            for (int i = 0; i < 6; ++i) {
                stream << report.keycode[i];
            }
        }
        break;
        
    case KeyAction::Delay:
        // 延时数据占用2字节
        stream << static_cast<quint16>(action.delay);
        break;
    }
    
    return result;
}

quint8 KeySequenceParser::getKeyCode(const QString &key)
{
    return m_keyMap.value(key, 0);
}

quint8 KeySequenceParser::getModifierBits(const QString& key)
{
    quint8 modifier = 0;
    if (key == "Ctrl") modifier = 0x01;      // 左Ctrl
    else if (key == "Shift") modifier = 0x02;  // 左Shift
    else if (key == "Alt") modifier = 0x04;    // 左Alt
    else if (key == "Win") modifier = 0x08;    // 左Win
    return modifier;
}

quint32 KeySequenceParser::calculateCRC32(const QByteArray& data)
{
    return qChecksum(data.constData(), data.size());
}

void KeySequenceParser::initializeKeyMaps()
{
    // 初始化按键映射
    m_keyMap["A"] = 0x04;
    m_keyMap["B"] = 0x05;
    m_keyMap["C"] = 0x06;
    m_keyMap["D"] = 0x07;
    m_keyMap["E"] = 0x08;
    m_keyMap["F"] = 0x09;
    m_keyMap["G"] = 0x0A;
    m_keyMap["H"] = 0x0B;
    m_keyMap["I"] = 0x0C;
    m_keyMap["J"] = 0x0D;
    m_keyMap["K"] = 0x0E;
    m_keyMap["L"] = 0x0F;
    m_keyMap["M"] = 0x10;
    m_keyMap["N"] = 0x11;
    m_keyMap["O"] = 0x12;
    m_keyMap["P"] = 0x13;
    m_keyMap["Q"] = 0x14;
    m_keyMap["R"] = 0x15;
    m_keyMap["S"] = 0x16;
    m_keyMap["T"] = 0x17;
    m_keyMap["U"] = 0x18;
    m_keyMap["V"] = 0x19;
    m_keyMap["W"] = 0x1A;
    m_keyMap["X"] = 0x1B;
    m_keyMap["Y"] = 0x1C;
    m_keyMap["Z"] = 0x1D;
    
    m_keyMap["1"] = 0x1E;
    m_keyMap["2"] = 0x1F;
    m_keyMap["3"] = 0x20;
    m_keyMap["4"] = 0x21;
    m_keyMap["5"] = 0x22;
    m_keyMap["6"] = 0x23;
    m_keyMap["7"] = 0x24;
    m_keyMap["8"] = 0x25;
    m_keyMap["9"] = 0x26;
    m_keyMap["0"] = 0x27;
    
    m_keyMap["Enter"] = 0x28;
    m_keyMap["Esc"] = 0x29;
    m_keyMap["Backspace"] = 0x2A;
    m_keyMap["Tab"] = 0x2B;
    m_keyMap["Space"] = 0x2C;
    
    m_keyMap["Ctrl"] = 0xE0;
    m_keyMap["Shift"] = 0xE1;
    m_keyMap["Alt"] = 0xE2;
    m_keyMap["Win"] = 0xE3;

    m_keyMap["F1"] = 0x3A;
    m_keyMap["F2"] = 0x3B;
    m_keyMap["F3"] = 0x3C;
    m_keyMap["F4"] = 0x3D;
    m_keyMap["F5"] = 0x3E;
    m_keyMap["F6"] = 0x3F;
    m_keyMap["F7"] = 0x40;
    m_keyMap["F8"] = 0x41;
    m_keyMap["F9"] = 0x42;
    m_keyMap["F10"] = 0x43;
    m_keyMap["F11"] = 0x44;
    m_keyMap["F12"] = 0x45;
}
