#ifndef WIN_DIAG_H
#define WIN_DIAG_H

#include <QDialog>
#include <functional>
#include <QVector>
#include <QTimer>
#include <QSet>
#include "ui_diag.h"
#include "common.h"
#include "dvs.h"

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
    QTimer tUpdate;
    QVector<DVDef> m_dvs;
    QSet<uint8_t> m_remaining; // dvs remaining to be read

    void twFill();
    void dvsFill(QTreeWidget&, const QVector<DVDef>&);
    void refresh();
    void refreshDV(unsigned line, const DVDef&);
    void diagReceived(QTreeWidgetItem&, const QJsonObject&, const DVDef&);

    void logWriteHeader(const QString& filename) const;
    void logWriteTable(const QString& filename) const;

    enum TWDVColumn {
        cUpdate = 0,
        cDVI = 1,
        cUpdated = 2,
        cName = 3,
        cValue = 4,
    };

private slots:
    void ui_bRefreshHandle();
    void tUpdateTimeout();
    void ui_chbAutorefreshStateChanged();
    void ui_onFinished(int result);
    void ui_sbPeriodValueChanged();
    void ui_bBrowseHandle();
    void ui_chbLogFileStateChanged();
    void ui_twDVsItemChanged(QTreeWidgetItem *item, int column);

};

#endif // WIN_DIAG_H
