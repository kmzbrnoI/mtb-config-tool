#ifndef WIN_IO_H
#define WIN_IO_H

#include <QDialog>
#include <QJsonObject>
#include <QStyleOption>
#include <QPainter>
#include "client.h"

class MtbModuleIODialog : public QDialog {
    Q_OBJECT

    virtual void disableAll() = 0;

public:
    MtbModuleIODialog(QWidget *parent = nullptr) : QDialog(parent) {}

    virtual void openModule(const QJsonObject& module) = 0;
    virtual void moduleChanged(const QJsonObject& module) = 0;
    virtual void inputsChanged(const QJsonObject& module) = 0;
    virtual void outputsChanged(const QJsonObject& module) = 0;
    virtual void retranslate() = 0;

protected:
    uint8_t address;

    void sendModuleRequest(ResponseOkEvent&& onOk = [](const QJsonObject&){});

protected slots:
    void refresh();

};


class QClickableWidget : public QWidget {
    Q_OBJECT

    void mouseReleaseEvent(QMouseEvent *event) override {
        QWidget::mouseReleaseEvent(event);
        emit onClicked();
    }

public:
    QClickableWidget(QWidget *parent = nullptr) : QWidget(parent) {}

    void paintEvent(QPaintEvent *paintEvent) override {
        Q_UNUSED(paintEvent);
        QStyleOption option;
        option.initFrom(this);
        QPainter painter(this);
        style()->drawPrimitive(QStyle::PE_Widget, &option, &painter, this);
    }

signals:
    void onClicked();

};

#endif // WIN_MTBUNICONFIG_H
