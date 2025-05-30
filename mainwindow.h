#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <processes.h>

class QStackedWidget;
class QComboBox;
class QPushButton;
class QLineEdit;
class QCheckBox;
class QGroupBox;
class QRadioButton;
class QButtonGroup;
class QLabel;

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

private:
    void crearInterfaz();
    QVector<QLabel*> labelList;
    processes globalProcs;
    QWidget *central;
    QCheckBox *modoSwitch;
    QStackedWidget *stackConfiguracion;
    QGroupBox *grupoAlgoritmo;
    QButtonGroup *algoritmoButtons;
    QLineEdit *campoQuantum;
    QLineEdit *lineProcCal;
    QLineEdit *lineProcSync;
    QLineEdit *lineRecSync;
    QLineEdit *lineAccSync;
    QComboBox *comboAlgoritmos;
    QPushButton *botonGenerar;
    QWidget *simContainer;
    bool showSim;
};

#endif // MAINWINDOW_H

