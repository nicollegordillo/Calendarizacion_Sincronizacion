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

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QWidget *central = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(central);

    // --- Header con botón de ayuda (?) ---
    QHBoxLayout *headerLayout = new QHBoxLayout;
    headerLayout->addStretch();
    QPushButton *helpButton = new QPushButton("?");
    helpButton->setFixedSize(30, 30);
    headerLayout->addWidget(helpButton);
    mainLayout->addLayout(headerLayout);

    // --- Configuración ---
    QGroupBox *configGroup = new QGroupBox("Configuración");
    QVBoxLayout *configLayout = new QVBoxLayout(configGroup);

    // Switch visual: Calendarización <switch> Sincronización
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
            transition: margin 0.2s ease;
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

    // --- Página Calendarización ---
    QWidget *calPage = new QWidget;
    QVBoxLayout *calLayout = new QVBoxLayout(calPage);
    QPushButton *btnProcCal = new QPushButton("Seleccionar archivo de procesos");
    QLineEdit *lineProcCal = new QLineEdit;
    lineProcCal->setReadOnly(true);
    calLayout->addWidget(new QLabel("Procesos (.txt):"));
    calLayout->addWidget(btnProcCal);
    calLayout->addWidget(lineProcCal);
    configStack->addWidget(calPage);

    // --- Página Sincronización ---
    QWidget *syncPage = new QWidget;
    QVBoxLayout *syncLayout = new QVBoxLayout(syncPage);
    QPushButton *btnProcSync = new QPushButton("Seleccionar archivo de procesos");
    QPushButton *btnRecSync = new QPushButton("Seleccionar archivo de recursos");
    QPushButton *btnAccSync = new QPushButton("Seleccionar archivo de acciones");
    QLineEdit *lineProcSync = new QLineEdit;
    QLineEdit *lineRecSync = new QLineEdit;
    QLineEdit *lineAccSync = new QLineEdit;
    lineProcSync->setReadOnly(true);
    lineRecSync->setReadOnly(true);
    lineAccSync->setReadOnly(true);

    syncLayout->addWidget(new QLabel("Procesos (.txt):"));
    syncLayout->addWidget(btnProcSync);
    syncLayout->addWidget(lineProcSync);
    syncLayout->addWidget(new QLabel("Recursos (.txt):"));
    syncLayout->addWidget(btnRecSync);
    syncLayout->addWidget(lineRecSync);
    syncLayout->addWidget(new QLabel("Acciones (.txt):"));
    syncLayout->addWidget(btnAccSync);
    syncLayout->addWidget(lineAccSync);
    configStack->addWidget(syncPage);

    configLayout->addWidget(configStack);
    mainLayout->addWidget(configGroup);

    // --- Algoritmo ---
    QGroupBox *algGroup = new QGroupBox("Algoritmo");
    QVBoxLayout *algLayout = new QVBoxLayout(algGroup);
    // Aquí se añadirán los widgets dinámicos según el modo...
    mainLayout->addWidget(algGroup);

    // --- Simulación ---
    QGroupBox *simGroup = new QGroupBox("Simulación");
    QVBoxLayout *simLayout = new QVBoxLayout(simGroup);
    simLayout->addWidget(new QLabel("Si la información ya fue seleccionada, presione Generar simulador."));
    QPushButton *btnSimular = new QPushButton("Generar simulador");
    simLayout->addWidget(btnSimular);
    mainLayout->addWidget(simGroup);

    setCentralWidget(central);

    // --- Conexiones ---
    connect(modoSwitch, &QCheckBox::toggled, [=](bool checked) {
        configStack->setCurrentIndex(checked ? 1 : 0);
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

void MainWindow::cambiarModo(int) {
    // Lógica futura: cambiar entre modos calendarización / sincronización
}

void MainWindow::algoritmoSeleccionado() {
    // Lógica futura: mostrar campo de quantum si el algoritmo seleccionado es RR
}

void MainWindow::generarSimulador() {
    // Lógica futura: validación de archivos + ejecución de simulación
}





