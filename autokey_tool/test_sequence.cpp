#include <QCoreApplication>
#include <QDebug>
#include "keysequenceparser.h"
#include <iostream>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    
    KeySequenceParser parser;
    
    QStringList testSequences = {
        "A;",
        "Ctrl+A;",
        "A(5);",
        "100ms;",
        "A;Ctrl+A;A(5);100ms;",
        "F1;Enter;Esc;",
        "InvalidSequence"
    };
    
    std::cout << "按键序列解析测试:\n";
    std::cout << "===================\n\n";
    
    for (const QString &sequence : testSequences) {
        std::cout << "测试序列: " << sequence.toStdString() << std::endl;
        
        bool isValid = parser.validateSequence(sequence);
        std::cout << "有效性: " << (isValid ? "有效" : "无效") << std::endl;
        
        if (isValid) {
            QByteArray data = parser.parseSequence(sequence);
            std::cout << "解析数据长度: " << data.size() << " 字节" << std::endl;
            std::cout << "十六进制数据: " << data.toHex().toStdString() << std::endl;
        }
        
        std::cout << std::endl;
    }
    
    return 0;
}