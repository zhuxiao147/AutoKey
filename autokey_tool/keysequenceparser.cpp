#include "keysequenceparser.h"
#include <QDebug>
#include <QRegularExpression>
#include <QDataStream>

KeySequenceParser::KeySequenceParser()
{
    initializeKeyMap();
}

QByteArray KeySequenceParser::parseSequence(const QString &sequence)
{
    if (!validateSequence(sequence)) {
        return QByteArray();
    }
    
    QVector<KeyAction> actions = parseActions(sequence);
    QByteArray result;
    QDataStream stream(&result, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    
    quint16 actionCount = actions.size();
    stream << actionCount;
    
    for (const KeyAction &action : actions) {
        QByteArray actionData = actionToBytes(action);
        stream.writeRawData(actionData.constData(), actionData.size());
    }
    
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
        
        if (trimmed.contains("ms") && trimmed.endsWith("ms")) {
            bool ok;
            int delay = trimmed.left(trimmed.length() - 2).toInt(&ok);
            if (!ok || delay <= 0) {
                return false;
            }
            continue;
        }
        
        if (trimmed.contains('(') && trimmed.endsWith(')')) {
            int openIndex = trimmed.indexOf('(');
            int closeIndex = trimmed.lastIndexOf(')');
            if (openIndex >= 0 && closeIndex > openIndex) {
                QString key = trimmed.left(openIndex);
                QString countStr = trimmed.mid(openIndex + 1, closeIndex - openIndex - 1);
                bool ok;
                int count = countStr.toInt(&ok);
                if (!ok || count <= 0) {
                    return false;
                }
                if (!m_keyMap.contains(key)) {
                    return false;
                }
                trimmed = key; // Update trimmed to be the key only, matching parseActions logic
                continue;
            }
        }
        
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
        }
        
        if (!m_keyMap.contains(trimmed)) {
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
        KeyAction action; // Initialize action for each part
        QString trimmed = part.trimmed();
        if (trimmed.isEmpty()) {
            continue;
        }
        
        if (trimmed.contains("ms") && trimmed.endsWith("ms")) {
            action.type = KeyAction::Delay;
            action.delay = trimmed.left(trimmed.length() - 2).toInt();
            continue;
        }else{
            action.type = KeyAction::KeyData;   //if not delay, set to KeyData first
        }
        
        if (trimmed.contains('(') && trimmed.endsWith(')')) {
            int openIndex = trimmed.indexOf('(');
            int closeIndex = trimmed.lastIndexOf(')');
            if (openIndex >= 0 && closeIndex > openIndex) {
                action.count = trimmed.mid(openIndex + 1, closeIndex - openIndex - 1).toInt();
                trimmed = trimmed.left(openIndex); // Update trimmed to be the key only
            }
        }else {
            action.count = 1;
        }
        
        if (trimmed.contains('+')) {
            QStringList comboKeys = trimmed.split('+', Qt::SkipEmptyParts);
            for (const QString &key : comboKeys) {
                action.data.append(key.trimmed());
            }
        }else{
            action.data.append(trimmed);    
        }
        actions.append(action);

        KeyAction pressAction;
        action.type = KeyAction::KeyPress;
        actions.append(pressAction);
        
        KeyAction releaseAction;
        releaseAction.type = KeyAction::KeyRelease;
        actions.append(releaseAction);
    }
    
    return actions;
}

QByteArray KeySequenceParser::actionToBytes(const KeyAction &action)
{
    QByteArray result;
    QDataStream stream(&result, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    
    stream << static_cast<quint8>(action.type);
    
    switch (action.type) {
    case KeyAction::KeyPress:
    case KeyAction::KeyRelease:
        stream << getKeyCode(action.key);
        break;
        
    case KeyAction::KeyData:
        stream << static_cast<quint8>(action.data.size());
        for (const QString &key : action.data) {
            stream << getKeyCode(key);
        }
        break;
        
    case KeyAction::KeyRepeat:
        stream << getKeyCode(action.key);
        stream << static_cast<quint16>(action.count);
        break;
        
    case KeyAction::Delay:
        stream << static_cast<quint16>(action.delay);
        break;
    }
    
    return result;
}

quint8 KeySequenceParser::getKeyCode(const QString &key)
{
    return m_keyMap.value(key, 0);
}

void KeySequenceParser::initializeKeyMap()
{
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