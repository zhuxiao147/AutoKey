#ifndef KEYSEQUENCEPARSER_H
#define KEYSEQUENCEPARSER_H

#include <QByteArray>
#include <QString>
#include <QVector>
#include <QHash>

struct KeyAction {
    enum Type {
        KeyData,
        KeyPress,
        KeyRelease,
        Delay
    };
    Type type;
    int count;  // for repeat actions
    int delay;  // for delay actions in milliseconds
    QVector<QString> data;  // for data actions
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
    quint8 getKeyCode(const QString &key);
    
    QHash<QString, quint8> m_keyMap;
    void initializeKeyMap();
};

#endif // KEYSEQUENCEPARSER_H