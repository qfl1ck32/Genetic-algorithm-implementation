#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>

#include "headers/GeneticAlgorithm.h"
#include "headers/QuadraticPolynomial.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_start_clicked();

    void on_nextStep_clicked();

    void on_nextCycle_clicked();

    void on_clearLog_clicked();

    void on_clearLogAfterG_clicked();

    void on_runAll_clicked();

    void on_keepLogs_clicked();

    void on_elitistCriteria_clicked();

private:
    Ui::MainWindow *ui;
    void updateIndividualsList();
    void updateLog(bool);

    bool keepLogsBool, clearLogAfterGen, elitistCriteriaBool;
    void resizeColumn1();

    QPixmap pixmap;

    Coefficients *coefficients = nullptr;
    QuadraticPolynomial *function = nullptr;
    Domain *domain = nullptr;

    int populationSize, generationCount;
    double precision, crossoverProbability, mutationProbability;

    GeneticAlgorithm <QuadraticPolynomial> *geneticAlgorithm = nullptr;
};
#endif // MAINWINDOW_H
