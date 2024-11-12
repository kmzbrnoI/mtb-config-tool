#include <QJsonDocument>
#include <QTreeWidgetItem>
#include <QFileDialog>
#include "qjsonsafe.h"
#include "win_diag.h"
#include "client.h"


/////////////////////////////////////////////////////////////////////////////////

DiagDialog::DiagDialog(QWidget *parent) :
    QDialog(parent) {
    this->ui.setupUi(this);

    QObject::connect(this->ui.b_refresh, SIGNAL(released()), this, SLOT(ui_bRefreshHandle()));
    QObject::connect(this->ui.chb_autorefresh, SIGNAL(stateChanged(int)), this, SLOT(ui_chbAutorefreshStateChanged()));
    QObject::connect(this->ui.sb_period, SIGNAL(valueChanged(int)), this, SLOT(ui_sbPeriodValueChanged()));
    QObject::connect(this->ui.b_browse, SIGNAL(released()), this, SLOT(ui_bBrowseHandle()));
    QObject::connect(this->ui.chb_log_file, SIGNAL(stateChanged(int)), this, SLOT(ui_chbLogFileStateChanged()));
    QObject::connect(&this->tUpdate, SIGNAL(timeout()), this, SLOT(tUpdateTimeout()));
    QObject::connect(this, SIGNAL(finished(int)), this, SLOT(ui_onFinished(int)));
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
    this->m_dvs = DVs::instance->dvs(this->moduleType);
    this->twFill();
    this->setWindowTitle(tr("Module Diagnostic – module ")+QString::number(this->moduleAddress));
    this->refresh();
    this->show();
}

void DiagDialog::ui_bRefreshHandle() {
    this->refresh();
}

void DiagDialog::twFill() {
    this->ui.tw_dvs->clear();
    this->dvsFill(*this->ui.tw_dvs, this->m_dvs);
}

void DiagDialog::dvsFill(QTreeWidget& tw, const QVector<DVDef>& dvs) {
    for (const DVDef& dvDef : dvs) {
        QTreeWidgetItem* newItem = new QTreeWidgetItem(&tw);
        newItem->setCheckState(TWDVColumn::cUpdate, Qt::Checked);
        newItem->setText(TWDVColumn::cDVI, QString::number(dvDef.dvi));
        newItem->setText(TWDVColumn::cUpdated, tr("never"));
        newItem->setText(TWDVColumn::cName, dvDef.dvName);
        newItem->setText(TWDVColumn::cValue, "?");
        tw.addTopLevelItem(newItem);
    }

    for (int i = 0; i < this->ui.tw_dvs->columnCount(); i++)
        this->ui.tw_dvs->resizeColumnToContents(i);
}

void DiagDialog::refresh() {
    for (int i = 0; i < this->m_dvs.size(); i++) {
        QTreeWidgetItem* item = this->ui.tw_dvs->topLevelItem(i);
        if ((item != nullptr) && (item->checkState(TWDVColumn::cUpdate)))
            this->refreshDV(i, this->m_dvs.at(i));
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
            QTreeWidgetItem* item = this->ui.tw_dvs->topLevelItem(line);
            if (item != nullptr) {
                setBacground(*item, QC_LIGHT_RED);
                item->setText(TWDVColumn::cUpdated, QTime::currentTime().toString("hh:mm:ss"));
                item->setText(TWDVColumn::cValue, errorMessage + " ("+QString::number(errorCode)+")");
            }
        }
    );
}

void DiagDialog::diagReceived(QTreeWidgetItem& item, const QJsonObject& json, const DVDef& dvdef) {
    setBacground(item, QColor(0xFF, 0xFF, 0xFF));
    item.setText(TWDVColumn::cUpdated, QTime::currentTime().toString("hh:mm:ss"));
    item.setText(TWDVColumn::cValue, dvdef.repr(json["DVvalue"].toObject()));
}

void DiagDialog::tUpdateTimeout() {
    this->refresh();
}

void DiagDialog::ui_chbAutorefreshStateChanged() {
    if (this->ui.chb_autorefresh->checkState() == Qt::CheckState::Checked) {
        this->refresh();
        this->tUpdate.start(1000*this->ui.sb_period->value());
    } else {
        this->tUpdate.stop();
    }
}

void DiagDialog::ui_onFinished(int result) {
    (void)result;
    this->tUpdate.stop();
    this->ui.chb_autorefresh->setChecked(false);
    this->ui.chb_log_file->setChecked(false);
    this->ui_chbLogFileStateChanged();
}

void DiagDialog::ui_sbPeriodValueChanged() {
    this->tUpdate.setInterval(1000*this->ui.sb_period->value());
}

void DiagDialog::ui_bBrowseHandle() {
    QString filename = QFileDialog::getSaveFileName(
        this,
        tr("Log filename"),
        ".",
        tr("Log file (*.log)")
    );

    if (filename != "")
        this->ui.le_log_filename->setText(filename);
}

void DiagDialog::ui_chbLogFileStateChanged() {
    bool checked = (this->ui.chb_log_file->checkState() == Qt::CheckState::Checked);
    this->ui.le_log_filename->setEnabled(!checked);
    this->ui.b_browse->setEnabled(!checked);
}
