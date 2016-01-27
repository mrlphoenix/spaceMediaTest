/********************************************************************************
** Form generated from reading UI file 'iptvwidget.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_IPTVWIDGET_H
#define UI_IPTVWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <VLCQtWidgets/WidgetVideo.h>

QT_BEGIN_NAMESPACE

class Ui_IPTVWidget
{
public:
    QVBoxLayout *verticalLayout;
    VlcWidgetVideo *video;

    void setupUi(QWidget *IPTVWidget)
    {
        if (IPTVWidget->objectName().isEmpty())
            IPTVWidget->setObjectName(QStringLiteral("IPTVWidget"));
        IPTVWidget->resize(395, 264);
        verticalLayout = new QVBoxLayout(IPTVWidget);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        video = new VlcWidgetVideo(IPTVWidget);
        video->setObjectName(QStringLiteral("video"));

        verticalLayout->addWidget(video);


        retranslateUi(IPTVWidget);

        QMetaObject::connectSlotsByName(IPTVWidget);
    } // setupUi

    void retranslateUi(QWidget *IPTVWidget)
    {
        IPTVWidget->setWindowTitle(QApplication::translate("IPTVWidget", "Form", 0));
    } // retranslateUi

};

namespace Ui {
    class IPTVWidget: public Ui_IPTVWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_IPTVWIDGET_H
