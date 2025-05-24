#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QGroupBox>
#include <QFileDialog>
#include <QCheckBox>
#include <QLineEdit>
#include <QStackedWidget>
#include <QDebug>
#include <QMessageBox>
#include <QComboBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QWidget *central = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(central);
    setCentralWidget(central);

    // --- Header con botón de ayuda (?) ---
    QHBoxLayout *headerLayout = new QHBoxLayout;
    headerLayout->addStretch();
    QPushButton *helpButton = new QPushButton("?");
    helpButton->setFixedSize(30, 30);
    headerLayout->addWidget(helpButton);
    mainLayout->addLayout(headerLayout);

    // --- Configuración ---
    QGroupBox *configGroup = new QGroupBox("Configuración");
    QVBoxLayout *configLayout = new QVBoxLayout;

    QHBoxLayout *modoLayout = new QHBoxLayout;
    QLabel *labelCal = new QLabel("Calendarización");
    QCheckBox *modoSwitch = new QCheckBox;
    QLabel *labelSync = new QLabel("Sincronización");

    modoSwitch->setChecked(false);
    modoSwitch->setFixedSize(60, 30);
    modoSwitch->setStyleSheet(R"(
    QCheckBox {
        background-color: lightgray;
        border-radius: 15px;
        padding: 2px;
    }
    QCheckBox::indicator {
        width: 26px;
        height: 26px;
        border-radius: 13px;
        background-color: white;
        margin-left: 2px;
    }
    QCheckBox::indicator:checked {
        margin-left: 30px;
    }
)");

    modoLayout->addWidget(labelCal);
    modoLayout->addWidget(modoSwitch);
    modoLayout->addWidget(labelSync);
    modoLayout->addStretch();
    configLayout->addLayout(modoLayout);

    QStackedWidget *configStack = new QStackedWidget;
    configLayout->addWidget(configStack);

    configGroup->setLayout(configLayout);
    mainLayout->addWidget(configGroup);

    // --- Página Calendarización ---
    QWidget *calPage = new QWidget;
    QVBoxLayout *calLayout = new QVBoxLayout(calPage);
    QPushButton *btnProcCal = new QPushButton("Seleccionar archivo de procesos");
    QLineEdit *lineProcCal = new QLineEdit;
    lineProcCal->setReadOnly(true);
    calLayout->addWidget(new QLabel("Procesos (.txt):"));
    calLayout->addWidget(lineProcCal);
    calLayout->addWidget(btnProcCal);
    configStack->addWidget(calPage);

    // --- Página Sincronización ---
    QWidget *syncPage = new QWidget;
    QVBoxLayout *syncLayout = new QVBoxLayout(syncPage);
    QLineEdit *lineProcSync = new QLineEdit, *lineRecSync = new QLineEdit, *lineAccSync = new QLineEdit;
    QPushButton *btnProcSync = new QPushButton("Seleccionar archivo de procesos");
    QPushButton *btnRecSync = new QPushButton("Seleccionar archivo de recursos");
    QPushButton *btnAccSync = new QPushButton("Seleccionar archivo de acciones");
    lineProcSync->setReadOnly(true);
    lineRecSync->setReadOnly(true);
    lineAccSync->setReadOnly(true);
    syncLayout->addWidget(new QLabel("Procesos (.txt):"));
    syncLayout->addWidget(lineProcSync);
    syncLayout->addWidget(btnProcSync);
    syncLayout->addWidget(new QLabel("Recursos (.txt):"));
    syncLayout->addWidget(lineRecSync);
    syncLayout->addWidget(btnRecSync);
    syncLayout->addWidget(new QLabel("Acciones (.txt):"));
    syncLayout->addWidget(lineAccSync);
    syncLayout->addWidget(btnAccSync);
    configStack->addWidget(syncPage);

    // --- Algoritmo ---
    QGroupBox *algGroup = new QGroupBox("Algoritmo");
    QVBoxLayout *algLayout = new QVBoxLayout(algGroup);
    QHBoxLayout *algOptionsLayout = new QHBoxLayout;

    QCheckBox *cbFIFO = new QCheckBox("FIFO");
    QCheckBox *cbSJF = new QCheckBox("SJF");
    QCheckBox *cbSRT = new QCheckBox("SRT");
    QCheckBox *cbRR = new QCheckBox("Round Robin");
    QCheckBox *cbPriority = new QCheckBox("Priority");

    algOptionsLayout->addWidget(cbFIFO);
    algOptionsLayout->addWidget(cbSJF);
    algOptionsLayout->addWidget(cbSRT);
    algOptionsLayout->addWidget(cbRR);
    algOptionsLayout->addWidget(cbPriority);

    QHBoxLayout *quantumLayout = new QHBoxLayout;
    QLabel *quantumLabel = new QLabel("Quantum:");
    QLineEdit *campoQuantum = new QLineEdit;
    campoQuantum->setPlaceholderText("Quantum");

    algLayout->addLayout(algOptionsLayout);
    quantumLayout->addWidget(quantumLabel);
    quantumLayout->addWidget(campoQuantum);
    algLayout->addLayout(quantumLayout);

    cbFIFO->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    cbSJF->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    cbSRT->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    cbRR->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    cbPriority->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    QHBoxLayout *syncModeLayout = new QHBoxLayout;
    QLabel *labelMutex = new QLabel("Mutex");
    QCheckBox *syncSwitch = new QCheckBox;
    QLabel *labelSemaphore = new QLabel("Semaphore");

    syncSwitch->setFixedSize(60, 30);
    syncSwitch->setStyleSheet(modoSwitch->styleSheet());

    syncModeLayout->addWidget(labelMutex);
    syncModeLayout->addWidget(syncSwitch);
    syncModeLayout->addWidget(labelSemaphore);
    algLayout->addLayout(syncModeLayout);

    mainLayout->addWidget(algGroup);

    campoQuantum->setVisible(false);
    quantumLabel->setVisible(false);
    syncSwitch->setVisible(false);
    labelSemaphore->setVisible(false);
    labelMutex->setVisible(false);

    // --- Simulación ---
    QGroupBox *simGroup = new QGroupBox("Simulación");
    QVBoxLayout *simLayout = new QVBoxLayout(simGroup);
    simLayout->addWidget(new QLabel("Si la información ya fue seleccionada, presione generar simulador."));
    QPushButton *btnSimular = new QPushButton("Generar simulador");
    simLayout->addWidget(btnSimular);
    mainLayout->addWidget(simGroup);

    QMetaObject::invokeMethod(this, [=]() {
        bool isSync = modoSwitch->isChecked();
        configStack->setCurrentIndex(isSync ? 1 : 0);

        cbFIFO->setVisible(!isSync);
        cbSJF->setVisible(!isSync);
        cbSRT->setVisible(!isSync);
        cbRR->setVisible(!isSync);
        cbPriority->setVisible(!isSync);
        bool mostrarQuantum = !isSync && cbRR->isChecked();
        quantumLabel->setVisible(mostrarQuantum);
        campoQuantum->setVisible(mostrarQuantum);
        syncSwitch->setVisible(isSync);
        labelSemaphore->setVisible(isSync);
        labelMutex->setVisible(isSync);
    }, Qt::QueuedConnection);

    // --- Conexiones ---
    connect(modoSwitch, &QCheckBox::toggled, [=](bool checked) {
        configStack->setCurrentIndex(checked ? 1 : 0);

        cbFIFO->setVisible(!checked);
        cbSJF->setVisible(!checked);
        cbSRT->setVisible(!checked);
        cbRR->setVisible(!checked);
        cbPriority->setVisible(!checked);
        bool mostrarQuantum = !checked && cbRR->isChecked();
        quantumLabel->setVisible(mostrarQuantum);
        campoQuantum->setVisible(mostrarQuantum);
        syncSwitch->setVisible(checked);
        labelSemaphore->setVisible(checked);
        labelMutex->setVisible(checked);
    });

    connect(cbRR, &QCheckBox::toggled, [=](bool checked) {
        bool mostrarQuantum = checked && !modoSwitch->isChecked();
        campoQuantum->setVisible(mostrarQuantum);
        quantumLabel->setVisible(mostrarQuantum);
    });

    auto conectarBotonArchivo = [](QPushButton *boton, QLineEdit *destino) {
        QObject::connect(boton, &QPushButton::clicked, [=]() {
            QString file = QFileDialog::getOpenFileName(nullptr, "Seleccionar archivo", "", "Archivos de texto (*.txt)");
            if (!file.isEmpty()) destino->setText(file);
        });
    };

    conectarBotonArchivo(btnProcCal, lineProcCal);
    conectarBotonArchivo(btnProcSync, lineProcSync);
    conectarBotonArchivo(btnRecSync, lineRecSync);
    conectarBotonArchivo(btnAccSync, lineAccSync);

    connect(helpButton, &QPushButton::clicked, this, &MainWindow::mostrarAyuda);
}

MainWindow::~MainWindow() {}

void MainWindow::mostrarAyuda() {
    QMessageBox::information(this, "Ayuda", "Esta aplicación permite simular algoritmos de calendarización y sincronización.");
}

void MainWindow::cambiarModo(int) {}

void MainWindow::algoritmoSeleccionado() {}

void MainWindow::generarSimulador() {}






