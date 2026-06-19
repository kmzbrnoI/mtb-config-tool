#ifndef WIN_MTBLEDCONFIG_H
#define WIN_MTBLEDCONFIG_H

#include <array>
#include <QLabel>
#include <QComboBox>
#include <QCheckBox>
#include <QSpinBox>
#include "win_config.h"
#include "ui_mtbledconfig.h"
#include "common.h"

struct LedConfigGuiOutput {
    QLabel name;
    QComboBox safeState;
    QSpinBox brightness;
};

class MtbLedConfigWindow : public MtbModuleConfigDialog {
    Q_OBJECT

public:
    MtbLedConfigWindow(QWidget *parent = nullptr);

    void editModule(const QJsonObject& module) override;
    void newModule(unsigned addr, MtbModuleType) override;
    void update(const QJsonObject& module) override;
    void retranslate() override;

private:
    Ui::MtbLedConfigWindow ui;
    std::array<LedConfigGuiOutput, LED_IO_COUNT> m_guiOutputs;
    QLabel lOutSafeState[2];
    QLabel lOutBrightness[2];

    void createGuiOutputs();
    void apply();

private slots:
    void ui_bClicked(QAbstractButton *button);

};

#endif // WIN_MTBLEDCONFIG_H
