#ifndef WIN_MTBUNICONFIG_H
#define WIN_MTBUNICONFIG_H

#include <array>
#include <QLabel>
#include <QComboBox>
#include <QCheckBox>
#include "win_config.h"
#include "ui_mtbuniconfig.h"
#include "common.h"

struct UniGuiInput {
    QLabel name;
    QComboBox type;
    QComboBox delay;
};

struct UniGuiOutput {
    QLabel name;
    QLabel typeLabel;
    QComboBox type;
    QLabel safeStateLabel;
    QComboBox safeState;
};

class MtbUniConfigWindow : public MtbModuleConfigDialog {
    Q_OBJECT

public:
    MtbUniConfigWindow(QWidget *parent = nullptr);

    virtual void editModule(const QJsonObject& module) override;
    virtual void newModule(unsigned addr, MtbModuleType) override;
    virtual void update(const QJsonObject& module) override;
    virtual void retranslate() override;

private:
    Ui::MtbUniConfigWindow ui;
    std::array<UniGuiInput, UNI_INPUTS_COUNT> m_guiInputs;
    std::array<UniGuiOutput, UNI_INPUTS_COUNT> m_guiOutputs;
    bool updateInProgress = false;

    void createGuiInputs();
    void createGuiOutputs();
    void updateUiType(MtbModuleType);
    void jsonParseError(const QString& err);
    void apply();

    static void fillOutputSafeState(QComboBox&, unsigned value, const QString& type);
    static int outputCbToValue(const QString& type, unsigned index);

private slots:
    void ui_bClicked(QAbstractButton *button);
    void ui_cbOutputTypeCurrentIndexChanged(int);

};

#endif // WIN_MTBUNICONFIG_H
