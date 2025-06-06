#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <processes.h>
#include <scheduler.h>
#include <mutexsync.h>
#include <semaphore.h>

class QStackedWidget;
class QComboBox;
class QPushButton;
class QLineEdit;
class QCheckBox;
class QGroupBox;
class QRadioButton;
class QButtonGroup;
class QLabel;
class QHBoxLayout;
class QVBoxLayout;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    QString readFileContents(const QString &);
    void mostrarAyuda();
    void mostrarArchivo(int);
    void cambiarModo(int state);
    void algoritmoSeleccionado();
    void generarSimulador();
    void refreshAlgorithms(int, int);
    void calcularNextSim();
    void skipSim();
    void resetSim();
    void mostrarMetricasFinales();
    void clearLayout(QLayout *layout);
    void limpiarTimelinesSincronizacion();

private:
    void crearInterfaz();
    MutexSync* mutexSim = nullptr;
    Semaphore* semaphoreSim = nullptr;
    QVector<QString> activeAlgorithms;
    QVector<QLabel*> labelList;
    //scheduler *schedulerSim;
    QVector<scheduler*> simuladores;
    QVector<QHBoxLayout*> timelines;
    QWidget *central;
    QCheckBox *modoSwitch;
    QCheckBox *syncSwitch;
    QVBoxLayout *procTimelineLayout;
    QStackedWidget *stackConfiguracion;
    QGroupBox *grupoAlgoritmo;
    QButtonGroup *algoritmoButtons;
    QLineEdit *campoQuantum;
    QLineEdit *lineProcCal;
    QLineEdit *lineProcSync;
    QLineEdit *lineRecSync;
    QLineEdit *lineAccSync;
    QLabel *errorMsg;
    QLabel *finishedMsg;
    QLabel *timeLabel;
    QComboBox *comboAlgoritmos;
    QPushButton *botonGenerar;
    QWidget *simContainer;
    bool showSim;
    bool isSync;
    QHBoxLayout *accesedTimelineLayout;
    QHBoxLayout *waitingTimelineLayout;
    QWidget *accesedContent;
    QWidget *waitingContent;
    QGroupBox *timelineGroup;
    QGroupBox *syncTimelineGroup;
    QGroupBox *metricasBox;
    QVBoxLayout *metricasLayout;
    QString procFileCal, procFileSync, recFileSync, accFileSync;
    QMap<QString, QHBoxLayout*> timelineMap;
    QMap<QString, QHBoxLayout*> timelineMapAccessed;
    QMap<QString, QHBoxLayout*> timelineMapWaiting;
    QHBoxLayout *timelineLayout;
    QVector<QVBoxLayout*> timelinePorCiclo;
    QWidget *syncContent;
    QHBoxLayout* cicloLabelsLayout;
};

#endif // MAINWINDOW_H

