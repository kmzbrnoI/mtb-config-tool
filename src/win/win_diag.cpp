#include <QJsonDocument>
#include <QTreeWidgetItem>
#include "qjsonsafe.h"
#include "win_diag.h"
#include "client.h"

QString reprId(const QJsonObject&);
QString reprSingleValue(const QJsonObject&);

QVector<DVDef> dvsCommon {
    {0, "version", reprSingleValue},
    {1, "state", reprId},
    {2, "uptime", reprSingleValue},
    {10, "errors", reprId},
    {11, "warnings", reprId},
    {12, "mcu voltage", reprId},
    {13, "mcu temperature", reprId},
    {16, "mtbbus received", reprSingleValue},
    {17, "mtbbus received invalid crc", reprSingleValue},
    {18, "mtbbus sent", reprSingleValue},
    {19, "mtbbus unsent", reprSingleValue},
};

QString reprId(const QJsonObject &json) {
    return QString(QJsonDocument(json).toJson(QJsonDocument::Compact));
}

QString reprSingleValue(const QJsonObject &json) {
    const QStringList& keys = json.keys();
    return (keys.count() == 1) ? json[keys[0]].toVariant().toString() : "?";
}

/////////////////////////////////////////////////////////////////////////////////

DiagDialog::DiagDialog(QWidget *parent) :
    QDialog(parent) {
    this->ui.setupUi(this);

    QObject::connect(ui.b_refresh, SIGNAL(released()), this, SLOT(ui_bRefreshHandle()));
}

DiagDialog::~DiagDialog() {
    QObject::disconnect(ui.b_refresh, SIGNAL(released()), this, SLOT(ui_bRefreshHandle()));
}

void DiagDialog::retranslate() {
    this->ui.retranslateUi(this);
}

void DiagDialog::moduleOpen(const QJsonObject& module) {
    this->moduleAddress = QJsonSafe::safeUInt(module["address"]);
    this->moduleType = static_cast<MtbModuleType>(QJsonSafe::safeUInt(module["type_code"]));
    this->twFill();
    this->setWindowTitle(tr("Module Diagnostic – module ")+QString::number(this->moduleAddress));
    this->show();
}

void DiagDialog::ui_bRefreshHandle() {
    this->refresh();
}

void DiagDialog::twFill() {
    this->ui.tw_dvs->clear();
    this->dvsFill(*this->ui.tw_dvs, dvsCommon);
}

void DiagDialog::dvsFill(QTreeWidget& tw, const QVector<DVDef>& dvs) {
    for (const DVDef& dvDef : dvs) {
        QTreeWidgetItem* newItem = new QTreeWidgetItem(&tw);
        newItem->setText(0, QString::number(dvDef.dvi));
        newItem->setText(1, tr("never"));
        newItem->setText(2, dvDef.dvName);
        newItem->setText(3, "?");
        tw.addTopLevelItem(newItem);
    }

    for (int i = 0; i < this->ui.tw_dvs->columnCount(); i++)
        this->ui.tw_dvs->resizeColumnToContents(i);
}

void DiagDialog::refresh() {
    for (int i = 0; i < dvsCommon.size(); i++) {
        if (i >= this->ui.tw_dvs->topLevelItemCount())
            return;
        this->refreshDV(i, dvsCommon[i]);
    }
}

void DiagDialog::refreshDV(unsigned line, const DVDef& dv) {
    DaemonClient::instance->sendNoExc(
        {
            {"command", "module_diag"},
            {"address", this->moduleAddress},
            {"DVnum", static_cast<int>(dv.dvi)},
        },
        [this, line, dv](const QJsonObject& content) {
            QTreeWidgetItem* item = this->ui.tw_dvs->topLevelItem(line);
            if (item != nullptr)
                this->diagReceived(*item, content, dv);
        },
        [this, line](unsigned errorCode, QString errorMessage) {
            (void)errorCode;
            (void)errorMessage;
            QTreeWidgetItem* item = this->ui.tw_dvs->topLevelItem(line);
            if (item != nullptr) {
                setBacground(*item, QC_LIGHT_RED);
                item->setText(1, QTime::currentTime().toString("hh:mm:ss"));
            }
        }
    );
}

void DiagDialog::diagReceived(QTreeWidgetItem& item, const QJsonObject& json, const DVDef& dvdef) {
    setBacground(item, QColor(0xFF, 0xFF, 0xFF));
    item.setText(1, QTime::currentTime().toString("hh:mm:ss"));
    item.setText(3, dvdef.repr(json["DVvalue"].toObject()));
}
