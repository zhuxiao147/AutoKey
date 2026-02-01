/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QGroupBox *groupBox;
    QHBoxLayout *horizontalLayout;
    QLabel *statusLabel;
    QSpacerItem *horizontalSpacer;
    QPushButton *connectButton;
    QGroupBox *groupBox_2;
    QVBoxLayout *verticalLayout_2;
    QLabel *label;
    QTextEdit *sequenceEdit;
    QLabel *label_2;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *checkButton;
    QPushButton *sendButton;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(600, 600);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        verticalLayout = new QVBoxLayout(centralwidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        groupBox = new QGroupBox(centralwidget);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(groupBox->sizePolicy().hasHeightForWidth());
        groupBox->setSizePolicy(sizePolicy);
        groupBox->setMinimumSize(QSize(0, 140));
        groupBox->setMaximumSize(QSize(16777215, 60));
        horizontalLayout = new QHBoxLayout(groupBox);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        statusLabel = new QLabel(groupBox);
        statusLabel->setObjectName(QString::fromUtf8("statusLabel"));

        horizontalLayout->addWidget(statusLabel);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        connectButton = new QPushButton(groupBox);
        connectButton->setObjectName(QString::fromUtf8("connectButton"));

        horizontalLayout->addWidget(connectButton);


        verticalLayout->addWidget(groupBox);

        groupBox_2 = new QGroupBox(centralwidget);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        verticalLayout_2 = new QVBoxLayout(groupBox_2);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        label = new QLabel(groupBox_2);
        label->setObjectName(QString::fromUtf8("label"));

        verticalLayout_2->addWidget(label);

        sequenceEdit = new QTextEdit(groupBox_2);
        sequenceEdit->setObjectName(QString::fromUtf8("sequenceEdit"));
        sequenceEdit->setMinimumSize(QSize(0, 200));

        verticalLayout_2->addWidget(sequenceEdit);

        label_2 = new QLabel(groupBox_2);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setWordWrap(true);

        verticalLayout_2->addWidget(label_2);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        checkButton = new QPushButton(groupBox_2);
        checkButton->setObjectName(QString::fromUtf8("checkButton"));

        horizontalLayout_2->addWidget(checkButton);

        sendButton = new QPushButton(groupBox_2);
        sendButton->setObjectName(QString::fromUtf8("sendButton"));

        horizontalLayout_2->addWidget(sendButton);


        verticalLayout_2->addLayout(horizontalLayout_2);


        verticalLayout->addWidget(groupBox_2);

        MainWindow->setCentralWidget(centralwidget);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "\350\207\252\345\212\250\346\214\211\351\224\256\350\256\276\347\275\256\345\267\245\345\205\267", nullptr));
        groupBox->setTitle(QCoreApplication::translate("MainWindow", "\350\256\276\345\244\207\347\212\266\346\200\201", nullptr));
        statusLabel->setText(QCoreApplication::translate("MainWindow", "\346\234\252\350\277\236\346\216\245", nullptr));
        connectButton->setText(QCoreApplication::translate("MainWindow", "\350\277\236\346\216\245\350\256\276\345\244\207", nullptr));
        groupBox_2->setTitle(QCoreApplication::translate("MainWindow", "\346\214\211\351\224\256\345\272\217\345\210\227\350\256\276\347\275\256", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "\346\214\211\351\224\256\345\272\217\345\210\227\357\274\232", nullptr));
        sequenceEdit->setPlaceholderText(QCoreApplication::translate("MainWindow", "\344\276\213\345\246\202\357\274\232\n"
"A;Ctrl+A;A(5);100ms;\n"
"F1;Enter;Esc;\n"
"Ctrl+C;500ms;Ctrl+V;", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "\350\257\255\346\263\225\350\257\264\346\230\216\357\274\232\n"
"- \"A;\"\357\274\232\346\214\211\344\270\213\345\271\266\346\235\276\345\274\200A\351\224\256\n"
"- \"Ctrl+A;\"\357\274\232\346\214\211\344\270\213Ctrl\351\224\256\345\222\214A\351\224\256\357\274\214\346\235\276\345\274\200\346\227\266\345\205\210\346\235\276\345\274\200A\351\224\256\345\206\215\346\235\276\345\274\200Ctrl\351\224\256\n"
"- \"A(5);\"\357\274\232\346\214\211\344\270\213\345\271\266\346\235\276\345\274\200A\351\224\2565\346\254\241\n"
"- \"100ms;\"\357\274\232\347\255\211\345\276\205100\346\257\253\347\247\222", nullptr));
        checkButton->setText(QCoreApplication::translate("MainWindow", "\346\243\200\346\237\245\345\272\217\345\210\227", nullptr));
        sendButton->setText(QCoreApplication::translate("MainWindow", "\345\217\221\351\200\201\345\272\217\345\210\227", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
