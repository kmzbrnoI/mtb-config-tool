#ifndef WIN_DIAG_H
#define WIN_DIAG_H

#include <QDialog>
#include <functional>
#include <QVector>
#include "ui_diag.h"
#include "common.h"

using DVDescriptor = std::function<QString(const QJsonObject&)>;

struct DVDef {
    uint8_t dvi;
    QString dvName;
    DVDescriptor repr;

    //DVDef(unsigned dvi, const QString& dvName, DVDescriptor repr) : dvi(dvi), dvName(dvName), repr(repr) {}
};

class DiagDialog : public QDialog
{
    Q_OBJECT

public:
    DiagDialog(QWidget *parent = nullptr);
    ~DiagDialog();

    void retranslate();
    void moduleOpen(const QJsonObject& module);

private:
    Ui::DiagDialog ui;
    uint8_t moduleAddress;
    MtbModuleType moduleType;

    void twFill();
    void dvsFill(QTreeWidget&, const QVector<DVDef>&);
    void refresh();
    void refreshDV(unsigned line, const DVDef&);
    void diagReceived(QTreeWidgetItem&, const QJsonObject&, const DVDef&);

    enum TWDVColumn {
        cUpdate = 0,
        cDVI = 1,
        cUpdated = 2,
        cName = 3,
        cValue = 4,
    };

private slots:
    void ui_bRefreshHandle();

};

#endif // WIN_DIAG_H
