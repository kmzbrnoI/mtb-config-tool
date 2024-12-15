#ifndef WIN_MTBRCIO_H
#define WIN_MTBRCIO_H

#include <array>
#include <QLabel>
#include <QTextEdit>
#include "win_io.h"
#include "ui_mtbrcio.h"
#include "common.h"

struct RCIOGuiInput {
    QLabel lName;
    QTextEdit teAddresses;
};

class MtbRCIOWindow : public MtbModuleIODialog {
    Q_OBJECT

public:
    MtbRCIOWindow(QWidget *parent = nullptr);

    void openModule(const QJsonObject& module) override;
    void moduleChanged(const QJsonObject& module) override;
    void inputsChanged(const QJsonObject& module_inputs_changed) override;
    void outputsChanged(const QJsonObject& module_outputs_changed) override;
    void retranslate() override;

private:
    Ui::MtbRCIOWindow ui;
    std::array<RCIOGuiInput, RC_INPUTS_COUNT> m_guiInputs;

    void update(const QJsonObject& module);
    void createGuiInputs();
    void updateInputs(const QJsonObject& inputs);
    void disableAll() override;

    QLabel lHeadInput;
    QLabel lHeadAddresses;

};

#endif // WIN_MTBUNICONFIG_H
