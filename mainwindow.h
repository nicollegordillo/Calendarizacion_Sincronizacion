#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QStackedWidget;
class QComboBox;
class QPushButton;
class QLineEdit;
class QCheckBox;
class QGroupBox;
class QRadioButton;
class QButtonGroup;

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
};

#endif // MAINWINDOW_H

