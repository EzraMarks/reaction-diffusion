#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSettings>
#include <assert.h>
#include <QGridLayout>
#include <iostream>
#include "databinding.h"
#include "settings.h"
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::MainWindow)
{
    QGLFormat qglFormat;
    qglFormat.setVersion(4,0);
    qglFormat.setProfile(QGLFormat::CoreProfile);
    m_ui->setupUi(this);
    QGridLayout *gridLayout = new QGridLayout(m_ui->centralWidget);
    m_glWidget = new GLWidget(qglFormat, this);
    m_glWidget->setMinimumSize(100, 100);
    gridLayout->addWidget(m_glWidget, 0, 1);

    settings.loadSettingsOrDefaults();
    dataBind();

    // Restore the UI settings
    QSettings qtSettings("CS123", "ReactionDiffusion");
    restoreGeometry(qtSettings.value("geometry").toByteArray());
    restoreState(qtSettings.value("windowState").toByteArray());
}

MainWindow::~MainWindow()
{
    foreach (DataBinding *b, m_bindings) {
        delete b;
    }
    delete m_ui;
    delete m_glWidget;
}

void MainWindow::dataBind() {
#define BIND(b) { DataBinding *_b = (b); m_bindings.push_back(_b); assert(connect(_b, SIGNAL(dataChanged()), this, SLOT(settingsChanged()))); }

    BIND(FloatBinding::bindSliderAndTextbox(m_ui->dtSlider, m_ui->dtTextbox, settings.dt, 0.0, 2.0));
    BIND(FloatBinding::bindSliderAndTextbox(m_ui->diffusionRateASlider, m_ui->diffusionRateATextbox, settings.diffusionRateA, 0.0, 2.0));
    BIND(FloatBinding::bindSliderAndTextbox(m_ui->diffusionRateBSlider, m_ui->diffusionRateBTextbox, settings.diffusionRateB, 0.0, 2.0));
    BIND(FloatBinding::bindSliderAndTextbox(m_ui->feedRateSlider, m_ui->feedRateTextbox, settings.feedRate, 0.0, 2.0));
    BIND(FloatBinding::bindSliderAndTextbox(m_ui->killRateSlider, m_ui->killRateTextbox, settings.killRate, 0.0, 2.0));

#undef BIND
}

void MainWindow::settingsChanged() {
    m_glWidget->update();
}

void MainWindow::closeEvent(QCloseEvent *event) {
    // Save the settings before we quit
    settings.saveSettings();
    QSettings qtSettings("CS123", "ReactionDiffusion");
    qtSettings.setValue("geometry", saveGeometry());
    qtSettings.setValue("windowState", saveState());

    QMainWindow::closeEvent(event);
}

void MainWindow::resetSimulation() {
    settings.isFirstPass = true;
}

void MainWindow::resetParameters() {
    m_ui->dtTextbox->setText(QString::number(settings.defaultDt));
    m_ui->diffusionRateATextbox->setText(QString::number(settings.defaultDiffusionRateA));
    m_ui->diffusionRateBTextbox->setText(QString::number(settings.defaultDiffusionRateB));
    m_ui->feedRateTextbox->setText(QString::number(settings.defaultFeedRate));
    m_ui->killRateTextbox->setText(QString::number(settings.defaultKillRate));
}

void MainWindow::fileOpen() {
    QString file = QFileDialog::getOpenFileName(this, QString(), "");
    if (!file.isNull()) {
        settings.imagePath = file;
        settings.isFirstPass = true;
    }
}

void MainWindow::openColorDialog() {
    m_ui->colorDialog->open();
}

void MainWindow::selectColor(const QColor &color) {
    settings.color = color;
}

