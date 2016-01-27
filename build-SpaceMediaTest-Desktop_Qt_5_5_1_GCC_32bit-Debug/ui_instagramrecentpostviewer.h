/********************************************************************************
** Form generated from reading UI file 'instagramrecentpostviewer.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_INSTAGRAMRECENTPOSTVIEWER_H
#define UI_INSTAGRAMRECENTPOSTVIEWER_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_InstagramRecentPostViewer
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *postTextLabel;
    QLabel *imageLabel;

    void setupUi(QWidget *InstagramRecentPostViewer)
    {
        if (InstagramRecentPostViewer->objectName().isEmpty())
            InstagramRecentPostViewer->setObjectName(QStringLiteral("InstagramRecentPostViewer"));
        InstagramRecentPostViewer->resize(380, 290);
        verticalLayout = new QVBoxLayout(InstagramRecentPostViewer);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        postTextLabel = new QLabel(InstagramRecentPostViewer);
        postTextLabel->setObjectName(QStringLiteral("postTextLabel"));
        QFont font;
        font.setPointSize(12);
        postTextLabel->setFont(font);
        postTextLabel->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(postTextLabel);

        imageLabel = new QLabel(InstagramRecentPostViewer);
        imageLabel->setObjectName(QStringLiteral("imageLabel"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(imageLabel->sizePolicy().hasHeightForWidth());
        imageLabel->setSizePolicy(sizePolicy);
        imageLabel->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(imageLabel);


        retranslateUi(InstagramRecentPostViewer);

        QMetaObject::connectSlotsByName(InstagramRecentPostViewer);
    } // setupUi

    void retranslateUi(QWidget *InstagramRecentPostViewer)
    {
        InstagramRecentPostViewer->setWindowTitle(QApplication::translate("InstagramRecentPostViewer", "Form", 0));
        postTextLabel->setText(QString());
        imageLabel->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class InstagramRecentPostViewer: public Ui_InstagramRecentPostViewer {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_INSTAGRAMRECENTPOSTVIEWER_H
