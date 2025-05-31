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

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    showSim(false)
{
    setFixedSize(600, 600);
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

    // --- Simulación ---
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
    symBtnLayout->addWidget(btnSimNext);
    symBtnLayout->addWidget(btnSimFinish);
    symBtnLayout->addWidget(btnSimRefresh);

    QGroupBox *timelineGroup = new QGroupBox("Algoritmo");

    QWidget *scrollContent = new QWidget();
    procTimelineLayout = new QHBoxLayout(scrollContent);
    procTimelineLayout->setAlignment(Qt::AlignLeft);

    QScrollArea *scrollArea = new QScrollArea();

    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(scrollContent);

    QVBoxLayout *timelineLayout = new QVBoxLayout(timelineGroup);
    timelineLayout->addWidget(scrollArea);

    innerLayout->addLayout(symBtnLayout);
    innerLayout->addWidget(timelineGroup);
    simContainer->setVisible(false);
    simLayout->addWidget(simContainer);

    connect(btnSimular, &QPushButton::clicked, this, &MainWindow::generarSimulador);




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
    if (view_case== 0){
        targetLineEdit = lineProcCal;
    } else if (view_case==1){
        targetLineEdit = lineProcSync;
    } else if (view_case==2){
        targetLineEdit = lineRecSync;
    } else if (view_case==3){
        targetLineEdit = lineAccSync;
    }
    if (!targetLineEdit) {
        QMessageBox::information(this, "Contenido de Archivo", "Direccion Vacia");
    } else {
        QString path = targetLineEdit->text();
        QString content = readFileContents(path);
        processes Myproc = processes(path);
        int len = Myproc.names.length();
        for (int i = 0; i<len;i++){
             qDebug() << "Process name:" << Myproc.names[i]
            << "AT:" << Myproc.arrivalTime[i]
            << "BT:" << Myproc.burstTime[i]
            << "Priority:" << Myproc.priority[i];
        }
        QMessageBox::information(this, "Contenido de Archivo", content);
    }
}

void MainWindow::cambiarModo(int) {}

void MainWindow::algoritmoSeleccionado() {}

void MainWindow::generarSimulador() {
    errorMsg->setVisible(false);
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


    if(activeAlgorithms.length()==1){ // Version con 1 algoritmo elegido
        if (activeAlgorithms.contains("Priority")){ // Priority
             qDebug() << "Starting Init";
            schedulerSim = new scheduler(
                 path,
                 4, // Algorithm 4 = Priority
                 0  // quantum (bien xd)
            );
            qDebug() << "Init Successful";
        }
    }
}

void MainWindow::calcularNextSim() {
    schedulerSim->nextPS();
    if (!schedulerSim->finished){
        QString name = schedulerSim->getExcecutedName();
        qDebug() << "T:"<< schedulerSim->t << name;
        QString col = schedulerSim->getColorByName(name);
        QLabel* tem = new QLabel(name);
        tem->setAlignment(Qt::AlignCenter);
        tem->setFixedSize(40, 40);  // Ensures it's square
        tem->setStyleSheet(
            "background-color: "+col+";"   // Any color
            "border-radius: 10px;"         // Rounded corners
            );
        procTimelineLayout->addWidget(tem);

    } else {
        qDebug()<<"Finalizo!!";
    }
    return;
}


void MainWindow::resetSim() {
    // Reset Widget
    QLayoutItem *child;
    while ((child = procTimelineLayout->takeAt(0)) != nullptr) {
        if (child->widget()) {
            delete child->widget();  // Destroys the widget
        }
        delete child;  // Deletes the layout item itself
    }
    QString path = lineProcCal->text();
    // Reset scheduler
    schedulerSim = new scheduler(
        path,
        4, // Algorithm 4 = Priority
        0  // quantum (bien xd)
    );
}


void MainWindow::skipSim() {
    schedulerSim->nextPS();
    while (!schedulerSim->finished){
        QString name = schedulerSim->getExcecutedName();
        qDebug() << "T:"<< schedulerSim->t << name;
        QString col = schedulerSim->getColorByName(name);
        QLabel* tem = new QLabel(name);
        tem->setAlignment(Qt::AlignCenter);
        tem->setFixedSize(40, 40);  // Ensures it's square
        tem->setStyleSheet(
            "background-color: "+col+";"   // Any color
                                         "border-radius: 10px;"         // Rounded corners
            );
        procTimelineLayout->addWidget(tem);

    }
    return;
}
