#ifndef KEYSEQUENCEPARSER_H
#define KEYSEQUENCEPARSER_H

#include <QByteArray>
#include <QString>
#include <QVector>
#include <QHash>

typedef enum{
    CMD_NONE = 0x00,
    CMD_KEYBOARD = 0x01,
    CMD_MOUSE = 0x02,
    CMD_DELAY = 0x03,
}CmdType;

// HID键盘数据结构体
typedef struct {
    uint8_t modifier; // 修饰键状态
    uint8_t reserved; // 保留字节
    uint8_t keycode[6]; // 普通按键的键值
} HIDKeyboardReport;

struct KeyAction {
    enum Type {
        KeyPress,
        MouseOpreat,
        Delay
    };
    Type type;
    uint16_t repeat;  // 重复次数
    
    QVector<QString> keys;  // 按键列表（支持组合键）
    uint32_t delay;  // 延时（毫秒）
};

class KeySequenceParser
{
public:
    KeySequenceParser();
    
    QByteArray parseSequence(const QString &sequence);
    bool validateSequence(const QString &sequence);

private:
    QVector<KeyAction> parseActions(const QString &sequence);
    QByteArray actionToBytes(const KeyAction &action);
    quint8 getModifierBits(const QString& key);
    quint8 getKeyCode(const QString &key);
    quint32 calculateCRC32(const QByteArray& data);
    
    QHash<QString, quint8> m_keyMap;
    QHash<QString, quint8> m_modifierMap;
    void initializeKeyMaps();
};

#endif // KEYSEQUENCEPARSER_H