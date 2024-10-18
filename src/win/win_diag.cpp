#include <QJsonDocument>
#include <QTreeWidgetItem>
#include "qjsonsafe.h"
#include "win_diag.h"

QString reprId(const QJsonObject&);

QVector<DVDef> dvsCommon {
    {0, "version", reprId},
    {1, "state", reprId},
    {2, "uptime", reprId},
    {10, "errors", reprId},
    {11, "warnings", reprId},
    {12, "mcu voltage", reprId},
    {13, "mcu temperature", reprId},
    {16, "mtbbus received", reprId},
    {17, "mtbbus received invalid crc", reprId},
    {18, "mtbbus sent", reprId},
    {19, "mtbbus unsent", reprId},
};

QString reprId(const QJsonObject &json) {
    return QString(QJsonDocument(json).toJson(QJsonDocument::Compact));
}

/////////////////////////////////////////////////////////////////////////////////

DiagDialog::DiagDialog(QWidget *parent) :
    QDialog(parent)
{
    this->ui.setupUi(this);

    QObject::connect(ui.b_refresh, SIGNAL(released()), this, SLOT(ui_bRefreshHandle()));
}

DiagDialog::~DiagDialog()
{
    QObject::disconnect(ui.b_refresh, SIGNAL(released()), this, SLOT(ui_bRefreshHandle()));
}

void DiagDialog::retranslate() {
    this->ui.retranslateUi(this);
}

void DiagDialog::moduleOpen(const QJsonObject& module) {
    this->moduleAddress = QJsonSafe::safeUInt(module["address"]);
    this->moduleType = static_cast<MtbModuleType>(QJsonSafe::safeUInt(module["type_code"]));
    this->twFill();
    this->show();
}

void DiagDialog::ui_bRefreshHandle() {

}

void DiagDialog::twFill() {
    this->ui.tw_dvs->clear();
    this->dvsFill(*this->ui.tw_dvs, dvsCommon);
}

void DiagDialog::dvsFill(QTreeWidget& tw, const QVector<DVDef>& dvs) {
    for (const DVDef& dvDef : dvs) {
        QTreeWidgetItem* newItem = new QTreeWidgetItem(&tw);
        newItem->setText(0, QString::number(dvDef.dvi));
        newItem->setText(1, dvDef.dvName);
        newItem->setText(2, "?");
        tw.addTopLevelItem(newItem);
    }
}
