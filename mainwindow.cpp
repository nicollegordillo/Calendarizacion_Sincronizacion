#include "mainwindow.h"
#include "processes.h"
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
#include <QScrollArea>
#include <QCoreApplication>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    showSim(false)
{
    // setFixedSize(1000, 900);
    QWidget *central = new QWidget(this);

    QVBoxLayout *mainLayout = new QVBoxLayout(central);

    QScrollArea *mainScrollable = new QScrollArea;
    mainScrollable->setWidgetResizable(true);
    mainScrollable->setWidget(central);
    setCentralWidget(mainScrollable);

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
    // QCheckBox *modoSwitch = new QCheckBox;
    modoSwitch = new QCheckBox(this);
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
    QHBoxLayout *calBtnLayout = new QHBoxLayout;
    QPushButton *btnProcCal = new QPushButton("Seleccionar archivo de procesos");
    QPushButton *procViewCal = new QPushButton("Ver");
    lineProcCal = new QLineEdit(this);

    lineProcCal->setReadOnly(true);
    calLayout->addWidget(new QLabel("Procesos (.txt):"));
    calLayout->addWidget(lineProcCal);
    calBtnLayout->addWidget(btnProcCal,8);
    calBtnLayout->addWidget(procViewCal,2);
    calLayout->addLayout(calBtnLayout);
    configStack->addWidget(calPage);

    // --- Página Sincronización ---
    QWidget *syncPage = new QWidget;
    QVBoxLayout *syncLayout = new QVBoxLayout(syncPage);
    lineProcSync = new QLineEdit(this);
    lineProcSync->setReadOnly(true);
    lineRecSync = new QLineEdit(this);
    lineRecSync->setReadOnly(true);
    lineAccSync = new QLineEdit(this);
    lineAccSync->setReadOnly(true);

    QPushButton *btnProcSync = new QPushButton("Seleccionar archivo de procesos");
    QPushButton *procViewSync = new QPushButton("Ver");
    syncLayout->addWidget(new QLabel("Procesos (.txt):"));
    syncLayout->addWidget(lineProcSync);
    QHBoxLayout *syncBtn1 = new QHBoxLayout;
    syncBtn1->addWidget(btnProcSync,8);
    syncBtn1->addWidget(procViewSync,2);
    syncLayout->addLayout(syncBtn1);

    QPushButton *btnRecSync = new QPushButton("Seleccionar archivo de recursos");
    QPushButton *recViewSync = new QPushButton("Ver");
    syncLayout->addWidget(new QLabel("Recursos (.txt):"));
    syncLayout->addWidget(lineRecSync);
    QHBoxLayout *syncBtn2 = new QHBoxLayout;
    syncBtn2->addWidget(btnRecSync,8);
    syncBtn2->addWidget(recViewSync,2);
    syncLayout->addLayout(syncBtn2);

    QPushButton *btnAccSync = new QPushButton("Seleccionar archivo de acciones");
    QPushButton *accViewSync = new QPushButton("Ver");
    QHBoxLayout *syncBtn3 = new QHBoxLayout;
    syncLayout->addWidget(new QLabel("Acciones (.txt):"));
    syncLayout->addWidget(lineAccSync);
    syncBtn3->addWidget(btnAccSync,8);
    syncBtn3->addWidget(accViewSync,2);
    syncLayout->addLayout(syncBtn3);

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
    campoQuantum = new QLineEdit(this);
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

    // --- Simulación - Constantes ---
    QGroupBox *simGroup = new QGroupBox("Simulación");
    QVBoxLayout *simLayout = new QVBoxLayout(simGroup);
    simLayout->addWidget(new QLabel("Si la información ya fue seleccionada, presione generar simulador."));
    QPushButton *btnSimular = new QPushButton("Generar simulador");
    simLayout->addWidget(btnSimular);
    mainLayout->addWidget(simGroup);

    activeAlgorithms = QVector<QString>();
    QMetaObject::invokeMethod(this, [=]() {
        bool isSync = modoSwitch->isChecked();
        configStack->setCurrentIndex(isSync ? 1 : 0);

        activeAlgorithms.clear();

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


    // --- Simulacion - Variables ---
    errorMsg = new QLabel("Placeholder :)");
    errorMsg->setStyleSheet("QLabel { color : red; }");
    errorMsg->setVisible(false);
    simLayout->addWidget(errorMsg);
    // Timeline
    simContainer = new QWidget(this);
    QVBoxLayout *innerLayout = new QVBoxLayout(simContainer);
    QHBoxLayout *symBtnLayout = new QHBoxLayout;

    QPushButton *btnSimNext = new QPushButton();
    QIcon playIcon = style()->standardIcon(QStyle::SP_MediaPlay);
    btnSimNext->setIcon(playIcon);
    btnSimNext->setFixedSize(30, 30);

    QPushButton *btnSimFinish = new QPushButton();
    QIcon forwardIcon = style()->standardIcon(QStyle::SP_MediaSeekForward);
    btnSimFinish->setIcon(forwardIcon);
    btnSimFinish->setFixedSize(30, 30);

    QPushButton *btnSimRefresh = new QPushButton();
    QIcon refreshIcon = style()->standardIcon(QStyle::SP_BrowserReload);
    btnSimRefresh->setIcon(refreshIcon);
    btnSimRefresh->setFixedSize(30, 30);


    timeLabel = new QLabel("Placeholder :)");

    finishedMsg = new QLabel("Calendarizacion Finalizada!");
    finishedMsg->setStyleSheet("QLabel { color : green; }");
    finishedMsg->setVisible(false);


    symBtnLayout->addWidget(btnSimNext);
    symBtnLayout->addWidget(btnSimFinish);
    symBtnLayout->addWidget(btnSimRefresh);
    symBtnLayout->addWidget(timeLabel);
    symBtnLayout->addWidget(finishedMsg);

    QGroupBox *timelineGroup = new QGroupBox("Algoritmo");
    timelineGroup->setMinimumHeight(580);
    QWidget *scrollContent = new QWidget();
    procTimelineLayout = new QVBoxLayout(scrollContent);
    procTimelineLayout->setAlignment(Qt::AlignTop);

    // --- Métricas ---
    metricasBox = new QGroupBox("Métricas de simulación");
    metricasBox->setVisible(false);
    metricasLayout = new QVBoxLayout(metricasBox);
    innerLayout->addWidget(metricasBox);

    QScrollArea *scrollArea = new QScrollArea();

    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(scrollContent);

    timelineLayout = new QHBoxLayout(timelineGroup);
    timelineLayout->setAlignment(Qt::AlignTop);

    timelineLayout->addWidget(scrollArea);

    innerLayout->addLayout(symBtnLayout);
    innerLayout->addWidget(timelineGroup);
    // --- Secciones para sincronización ---
    QGroupBox *syncTimelineGroup = new QGroupBox("Sincronización");
    QVBoxLayout *syncTimelineLayout = new QVBoxLayout(syncTimelineGroup);

    // → Envolver cicloLabelsLayout + timelineLayout en verticalTimelineWrap
    QVBoxLayout* verticalTimelineWrap = new QVBoxLayout;

    // Layout superior con etiquetas de ciclos
    cicloLabelsLayout = new QHBoxLayout;
    cicloLabelsLayout->setAlignment(Qt::AlignLeft);
    verticalTimelineWrap->addLayout(cicloLabelsLayout);

    // Línea de tiempo única para sincronización
    timelineLayout = new QHBoxLayout();  // columnas de cada ciclo
    timelineLayout->setAlignment(Qt::AlignLeft);
    timelineLayout->setContentsMargins(10, 10, 10, 10);

    // Contenedor visual que tendrá tanto las etiquetas de ciclo como la línea de tiempo
    QWidget* syncContent = new QWidget();
    syncContent->setLayout(verticalTimelineWrap); // ← aquí está la clave
    verticalTimelineWrap->addLayout(timelineLayout);

    QScrollArea *scrollTimeline = new QScrollArea();
    scrollTimeline->setWidgetResizable(true);
    scrollTimeline->setWidget(syncContent);

    syncContent->setMinimumHeight(300);
    scrollTimeline->setMinimumHeight(300);

    syncTimelineLayout->addWidget(new QLabel("Línea de tiempo"));
    syncTimelineLayout->addWidget(scrollTimeline);

    syncTimelineGroup->setVisible(false);  // Solo se muestra en modo sincronización
    innerLayout->addWidget(syncTimelineGroup);

    simContainer->setVisible(false);
    simLayout->addWidget(simContainer);


    connect(btnSimular, &QPushButton::clicked, this, &MainWindow::generarSimulador);

    // --- Conexiones ---
    connect(modoSwitch, &QCheckBox::stateChanged, this, [=](int){
        bool isSync = modoSwitch->isChecked();
        configStack->setCurrentIndex(isSync ? 1 : 0);

        cbFIFO->setVisible(!isSync);
        cbSJF->setVisible(!isSync);
        cbSRT->setVisible(!isSync);
        cbRR->setVisible(!isSync);
        cbPriority->setVisible(!isSync);
        timelineGroup->setVisible(!isSync);

        bool mostrarQuantum = !isSync && cbRR->isChecked();
        quantumLabel->setVisible(mostrarQuantum);
        campoQuantum->setVisible(mostrarQuantum);

        syncSwitch->setVisible(isSync);
        labelSemaphore->setVisible(isSync);
        labelMutex->setVisible(isSync);

        syncTimelineGroup->setVisible(isSync); // Esto es clave si estás cambiando el modo
        finishedMsg->setText(isSync ? "Sincronización Finalizada!" : "Calendarización Finalizada!");
    });


    connect(cbRR, &QCheckBox::stateChanged, this, [=](int){
        bool isSync = modoSwitch->isChecked();
        bool mostrarQuantum = !isSync && cbRR->isChecked();
        campoQuantum->setVisible(mostrarQuantum);
        quantumLabel->setVisible(mostrarQuantum);
    });


    auto conectarBotonArchivo = [](QPushButton *boton, QLineEdit *destino) {
        QObject::connect(boton, &QPushButton::clicked, [=]() {
            QString file = QFileDialog::getOpenFileName(nullptr, "Seleccionar archivo", "", "Archivos de texto (*.txt)");
            if (!file.isEmpty()) destino->setText(file);
        });
    };

    connect(cbFIFO, &QCheckBox::checkStateChanged, this, [=](int state) {
        refreshAlgorithms(state, 0);  // 0 = ID for cbFIFO
    });
    connect(cbSJF, &QCheckBox::checkStateChanged, this, [=](int state) {
        refreshAlgorithms(state, 1);  // 1 = ID for cbSJF
    });
    connect(cbSRT, &QCheckBox::checkStateChanged, this, [=](int state) {
        refreshAlgorithms(state, 2);  // 2 = ID for cbSRT
    });
    connect(cbRR, &QCheckBox::checkStateChanged, this, [=](int state) {
        refreshAlgorithms(state, 3);  // 3 = ID for cbRR
    });
    connect(cbPriority, &QCheckBox::checkStateChanged, this, [=](int state) {
        refreshAlgorithms(state, 4);  // 4 = ID for cbPriority
    });


    conectarBotonArchivo(btnProcCal, lineProcCal);
    conectarBotonArchivo(btnProcSync, lineProcSync);
    conectarBotonArchivo(btnRecSync, lineRecSync);
    conectarBotonArchivo(btnAccSync, lineAccSync);

    // Sim Cal next
    connect(btnSimNext, &QPushButton::clicked, this, &MainWindow::calcularNextSim);
        // Sim Cal reset
    connect(btnSimRefresh, &QPushButton::clicked, this, &MainWindow::resetSim);
        // Sim Cal finish
    connect(btnSimFinish, &QPushButton::clicked, this, &MainWindow::skipSim);


    connect(helpButton, &QPushButton::clicked, this, &MainWindow::mostrarAyuda);
    connect(procViewCal, &QPushButton::clicked, this, [=]() {
        mostrarArchivo(0);  // 0 = Cal-Process
    });
    connect(procViewSync, &QPushButton::clicked, this, [=]() {
        mostrarArchivo(1);  // 1 = Sync-Process
    });
    connect(recViewSync, &QPushButton::clicked, this, [=]() {
        mostrarArchivo(2);  // 2 = Sync-Resources
    });
    connect(accViewSync, &QPushButton::clicked, this, [=]() {
        mostrarArchivo(3);  // 3 = Sync-Actions
    });
}

MainWindow::~MainWindow() {}

void MainWindow::mostrarAyuda() {
    QMessageBox::information(this, "Ayuda", "Esta aplicación permite simular algoritmos de calendarización y sincronización.");
}

void MainWindow::refreshAlgorithms(int state, int cbId){
    QString name = (cbId == 0) ? "FIFO" :
                       (cbId == 1) ? "SJF" :
                       (cbId == 2) ? "SRT" :
                       (cbId == 3) ? "RR" :
                       "Priority";

    if (state == Qt::Checked){
        activeAlgorithms.append(name);
        return;
    } else {
        activeAlgorithms.erase(std::remove_if(activeAlgorithms.begin(), activeAlgorithms.end(),
                                              [name](const QString &s) { return s == name; }),
                               activeAlgorithms.end());
        return;
    }
}

QString MainWindow::readFileContents(const QString &filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        return "No se pudo abrir el archivo";
    } else {
        QTextStream in(&file);
        return in.readAll();
    }
}
void MainWindow::mostrarArchivo(int view_case) {
    QLineEdit* targetLineEdit = nullptr;
    QString* targetPathVar = nullptr;
    QString tipoArchivo;

    switch (view_case) {
    case 0:
        targetLineEdit = lineProcCal;
        targetPathVar = &procFileCal;
        tipoArchivo = "Procesos (Calendarización)";
        break;
    case 1:
        targetLineEdit = lineProcSync;
        targetPathVar = &procFileSync;
        tipoArchivo = "Procesos (Sincronización)";
        break;
    case 2:
        targetLineEdit = lineRecSync;
        targetPathVar = &recFileSync;
        tipoArchivo = "Recursos";
        break;
    case 3:
        targetLineEdit = lineAccSync;
        targetPathVar = &accFileSync;
        tipoArchivo = "Acciones";
        break;
    default:
        QMessageBox::warning(this, "Error", "Selección inválida.");
        return;
    }

    if (!targetLineEdit || !targetPathVar) {
        QMessageBox::warning(this, "Error", "Campo no encontrado.");
        return;
    }

    QString path = targetLineEdit->text().trimmed();
    if (path.isEmpty()) {
        QMessageBox::information(this, "Archivo vacío", "La ruta del archivo está vacía.");
        return;
    }

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Error", "No se pudo abrir el archivo.");
        return;
    }

    QTextStream in(&file);
    QString rawContent = file.readAll();
    file.seek(0);  // Volver al inicio si queremos volver a leerlo

    *targetPathVar = path;  // Guardamos la ruta

    QStringList lines = rawContent.split('\n', Qt::SkipEmptyParts);
    qDebug() << "Archivo:" << tipoArchivo;

    if (view_case == 0 || view_case == 1) {
        Processes proc(path);
        for (int i = 0; i < proc.names.size(); ++i) {
            qDebug() << "PID:" << proc.names[i]
                     << " AT:" << proc.arrivalTime[i]
                     << " BT:" << proc.burstTime[i]
                     << " Priority:" << proc.priority[i]
                     << " Color:" << proc.hexColor[i];
        }
    }
    else if (view_case == 2) { // Recursos
        for (const QString &line : lines) {
            QStringList tokens = line.split(',', Qt::SkipEmptyParts);
            if (tokens.size() != 2) {
                qDebug() << "Línea inválida en archivo de recursos:" << line;
                continue;
            }
            QString nombre = tokens[0].trimmed();
            int contador = tokens[1].trimmed().toInt();
            qDebug() << "Recurso:" << nombre << "Contador:" << contador;
        }
    }
    else if (view_case == 3) { // Acciones
        for (const QString &line : lines) {
            QStringList tokens = line.split(',', Qt::SkipEmptyParts);
            if (tokens.size() != 4) {
                qDebug() << "Línea inválida en archivo de acciones:" << line;
                continue;
            }
            QString pid = tokens[0].trimmed();
            QString accion = tokens[1].trimmed();
            QString recurso = tokens[2].trimmed();
            int ciclo = tokens[3].trimmed().toInt();
            qDebug() << "PID:" << pid << "Acción:" << accion
                     << "Recurso:" << recurso << "Ciclo:" << ciclo;
        }
    }

    QMessageBox::information(this, tipoArchivo, rawContent);
}


void MainWindow::cambiarModo(int) {}

void MainWindow::algoritmoSeleccionado() {}

void MainWindow::clearLayout(QLayout *layout) {
    while (QLayoutItem* item = layout->takeAt(0)) {
        if (QWidget* widget = item->widget()) {
            widget->deleteLater();
        }
        delete item;
    }
}


void MainWindow::generarSimulador() {
    bool isSync = modoSwitch->isChecked();
    if (isSync) {
        QString procPath = lineProcSync->text();
        QString resPath = lineRecSync->text();
        QString actPath = lineAccSync->text();

        if (procPath.isEmpty() || resPath.isEmpty() || actPath.isEmpty()) {
            errorMsg->setText("Todos los campos de archivo son requeridos para el modo de sincronización.");
            errorMsg->setVisible(true);
            showSim = false;
            simContainer->setVisible(false);
            return;
        }

        mutexSim = new MutexSync(procPath, resPath, actPath);
        showSim = true;
        simContainer->setVisible(true);
        timeLabel->setText("Ciclo Actual: 0");
        // Limpiar layouts de sincronización si existen
        QLayoutItem* child;
        while ((child = cicloLabelsLayout->takeAt(0)) != nullptr) {
            if (child->widget()) delete child->widget();
            delete child;
        }
        while ((child = timelineLayout->takeAt(0)) != nullptr) {
            if (child->layout()) {
                QLayoutItem* subchild;
                while ((subchild = child->layout()->takeAt(0)) != nullptr) {
                    if (subchild->widget()) delete subchild->widget();
                    delete subchild;
                }
                delete child->layout();
            }
            delete child;
        }

    } else {
        errorMsg->setVisible(false);
        finishedMsg->setVisible(false);
        QString path = lineProcCal->text();

        // Error handling
        if (path.isEmpty()) {
            errorMsg->setText("Campo \"Procesos (.txt)\" no puede ser vacio!");
            errorMsg->setVisible(true);
            showSim = false;
            simContainer->setVisible(false);
            return;
        }
        int quantum = 0;
        int len = activeAlgorithms.length();
        qDebug() << "Active Algorithms have length " << len;
        for (int i = 0; i<len;i++){
            qDebug() << "Chosen Algoritm:" << activeAlgorithms[i];
        }
        if (activeAlgorithms.isEmpty()){
            errorMsg->setText("Debe escoger AL MENOS algoritmo para generar el simulador!");
            errorMsg->setVisible(true);
            showSim = false;
            simContainer->setVisible(false);
            return;
        }

        if (activeAlgorithms.contains("RR")){
            if (campoQuantum->text().isEmpty()){
                errorMsg->setText("Campo \"Quantum\" es REQUERIDO para el algoritmo Round Robin");
                errorMsg->setVisible(true);
                showSim = false;
                simContainer->setVisible(false);
                return;
            }
            bool ok;
            quantum = campoQuantum->text().toInt(&ok);
            if(!ok){
                errorMsg->setText("Campo \"Quantum\" debe ser un ENTERO para el algoritmo Round Robin");
                errorMsg->setVisible(true);
                showSim = false;
                simContainer->setVisible(false);
                return;
            }
            if(quantum <=0){
                errorMsg->setText("Campo \"Quantum\" debe ser un ENTERO POSITIVO para el algoritmo Round Robin");
                errorMsg->setVisible(true);
                showSim = false;
                simContainer->setVisible(false);
                return;
            }

        }

        // No errors raised, show:
        showSim = true;
        simContainer->setVisible(true);
            // Process init
        qDebug() << "Shown";

        // Limpiar simuladores anteriores
        simuladores.clear();
        timelines.clear();
        QLayoutItem* child;
        while ((child = procTimelineLayout->takeAt(0)) != nullptr) {
            if (child->widget()) delete child->widget();
            delete child;
        }

        for (const QString& alg : activeAlgorithms) {
            int methodId = (alg == "FIFO") ? 0 :
                               (alg == "SJF") ? 1 :
                               (alg == "SRT") ? 2 :
                               (alg == "RR") ? 3 : 4;

            int q = 0;
            if (methodId == 3) q = campoQuantum->text().toInt();

            scheduler* s = new scheduler(path, methodId, q);
            s->nombre = alg;
            simuladores.append(s);

            // Crear contenedor de título + línea de tiempo
            QVBoxLayout* vbox = new QVBoxLayout;
            QLabel* title = new QLabel("Algoritmo: " + alg);
            QHBoxLayout* line = new QHBoxLayout;
            line->setAlignment(Qt::AlignLeft);
            timelines.append(line);
            QWidget* wrap = new QWidget;
            wrap->setLayout(line);
            vbox->addWidget(title);
            vbox->addWidget(wrap);
            // Envolver el vbox en un QWidget contenedor
            QWidget* container = new QWidget;
            container->setLayout(vbox);
            procTimelineLayout->addWidget(container);
        }

        timeLabel->setText("Ciclo Actual: 0");
    }

}

void MainWindow::calcularNextSim() {
    bool isSync = modoSwitch->isChecked();
    if (isSync) {
        if (mutexSim->finished()) {
            finishedMsg->setVisible(true);
            return;
        }
        mutexSim->simulateNext();
        int cicloActual = mutexSim->currentCycle();
        timeLabel->setText("Ciclo Actual: " + QString::number(cicloActual));

        // Crear columna nueva para el ciclo actual
        QVBoxLayout* columnaCiclo = new QVBoxLayout;
        columnaCiclo->setAlignment(Qt::AlignTop);
        // Crear etiqueta de ciclo alineada a la izquierda
        QLabel* cicloLabel = new QLabel(QString::number(cicloActual));
        cicloLabel->setFixedSize(60, 20);
        cicloLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop); // ← Aquí está la clave
        cicloLabelsLayout->addWidget(cicloLabel);
        // Obtener orden de procesos que participaron en el ciclo
        QVector<QString> ordenProceso = mutexSim->procesosEnCicloActual();
        for (const QString& pid : ordenProceso) {
            QString state = mutexSim->timelineStates[pid].value(cicloActual, "IDLE");
            QString resource = mutexSim->timelineResources[pid].value(cicloActual, "");
            QString action = mutexSim->timelineActions[pid].value(cicloActual, "");
            QString color = mutexSim->processes[pid];
            if (state == "WAITING") {
                color = "#B0B0B0";
            }

            QLabel* label = new QLabel(pid + "->" + resource + "\n" + action);
            label->setAlignment(Qt::AlignCenter);
            label->setFixedSize(60, 40);
            label->setStyleSheet("background-color: " + color + "; border-radius: 10px;");

            // Agregar el proceso a la columna del ciclo actual
            columnaCiclo->addWidget(label);
        }

        // Si ningún proceso fue afectado en el ciclo, mostrar "IDLE"
        if (ordenProceso.isEmpty()) {
            QLabel* label = new QLabel("IDLE");
            label->setAlignment(Qt::AlignCenter);
            label->setFixedSize(60, 40);
            columnaCiclo->addWidget(label);
        }

        // Agregar la columna a la línea de tiempo principal
        timelineLayout->addLayout(columnaCiclo);
        timelinePorCiclo.append(columnaCiclo);


        if (mutexSim->finished()) {
            finishedMsg->setVisible(true);
        }
        return;
    } else {
        for (int i = 0; i < simuladores.size(); ++i) {
            scheduler* s = simuladores[i];

            if (!s->finished) {
                s->calculateNext();

                //  Solo si aún no terminó, dibujamos
                if (!s->finished) {
                    QString name = s->getExcecutedName();
                    QString col = s->getColorByName(name);
                    QLabel* tem = new QLabel(name);
                    tem->setAlignment(Qt::AlignCenter);
                    tem->setFixedSize(40, 40);
                    tem->setStyleSheet("background-color: " + col + "; border-radius: 10px;");
                    timelines[i]->addWidget(tem);
                }
            }

        }

        if (!simuladores.isEmpty()) {
            timeLabel->setText("Ciclo Actual: " + QString::number(simuladores[0]->t));
        }

        // Revisa si TODOS están terminados después del paso
        bool allFinished = std::all_of(simuladores.begin(), simuladores.end(),
                                    [](scheduler* s) { return s->finished; });

        if (allFinished) {
            finishedMsg->setVisible(true);
            mostrarMetricasFinales();
        }
        return;
    }
}

void MainWindow::resetSim() {
    QLayoutItem* item;
    while ((item = procTimelineLayout->takeAt(0)) != nullptr) {
        if (QWidget* w = item->widget()) {
            delete w;  //  usa delete, NO deleteLater
        }
        delete item;
    }

    simuladores.clear();
    timelines.clear();
    metricasBox->setVisible(false);
    generarSimulador();
}


void MainWindow::skipSim() {
    bool anyRemaining = true;
    while (anyRemaining) {
        anyRemaining = false;
        for (int i = 0; i < simuladores.size(); ++i) {
            scheduler* s = simuladores[i];
            if (!s->finished) {
                s->calculateNext();

                // Después de avanzar, verificar si sigue activo
                if (!s->finished) {
                    QString name = s->getExcecutedName();
                    QString col = s->getColorByName(name);
                    QLabel* tem = new QLabel(name);
                    tem->setAlignment(Qt::AlignCenter);
                    tem->setFixedSize(40, 40);
                    tem->setStyleSheet("background-color: " + col + "; border-radius: 10px;");
                    timelines[i]->addWidget(tem);
                    anyRemaining = true;
                }
            }
        }
    }

    if (!simuladores.isEmpty()) {
        timeLabel->setText("Ciclo Actual: " + QString::number(simuladores[0]->t));
    }
    finishedMsg->setVisible(true);
    mostrarMetricasFinales();
}

#include <QFile>
#include <QTextStream>
#include <QMessageBox>

void MainWindow::mostrarMetricasFinales() {
    QLayoutItem* item;
    while ((item = metricasLayout->takeAt(0)) != nullptr) {
        if (item->widget()) delete item->widget();
        delete item;
    }

    // Mostrar nuevas métricas
    for (int i = 0; i < simuladores.size(); ++i) {
        scheduler* s = simuladores[i];
        QVector<int> arrival = s->snapshot.arrivalTime;
        QVector<int> burst = s->originalBurstTime;
        QVector<int> completion(arrival.size(), -1);
        int n = arrival.size();

        for (int t = 0; t < s->timeline.size(); ++t) {
            int pid = s->timeline[t];
            if (pid != -1) {
                completion[pid] = t + 1;
            }
        }

        double totalWT = 0;
        for (int j = 0; j < n; ++j) {
            int wt = 0;
            if (s->nombre == "FIFO" || s->nombre == "SJF" || s->nombre == "Priority") {
                wt = completion[j] - burst[j];
            } else {
                wt = completion[j] - arrival[j] - burst[j];
            }
            totalWT += wt;
        }

        double avgWT = totalWT / n;

        QLabel* metricLabel = new QLabel(
            QString("Algoritmo: %1 | Avg. WT: %2")
                .arg(s->nombre)
                .arg(QString::number(avgWT, 'f', 2))
            );
        metricasLayout->addWidget(metricLabel);
    }
    metricasBox->setVisible(true);

}

