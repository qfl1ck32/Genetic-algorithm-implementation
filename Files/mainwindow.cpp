#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{

    ui -> setupUi(this);

    ui -> functionCoefficientA -> setText("-1");
    ui -> functionCoefficientB -> setText("1");
    ui -> functionCoefficientC -> setText("2");

    ui -> domainA -> setText("-1");
    ui -> domainB -> setText("2");

    ui -> populationSize -> setText("20");
    ui -> generationCount -> setText("100");

    ui -> precision -> setText("6");
    ui -> crossoverProbability -> setText("0.25");
    ui -> mutationProbability -> setText("0.05");

    ui -> individualsList -> verticalHeader() -> setVisible(false);

    elitistCriteriaBool = false;

    pixmap = QPixmap(64, 64);

    QStringList headerLabels;

    std :: vector <std :: string> labels = {"Index", "Chromosome", "X-Value", "F-Value", "Selection probability", "Is selected", "Crosses over", "Will be mutated", "Is elitist"};

    for (std :: string &s : labels)
        headerLabels.append(QString :: fromStdString(s));

    ui -> individualsList -> setColumnCount(labels.size());

    ui -> individualsList -> setHorizontalHeaderLabels(headerLabels);

    ui -> individualsList -> resizeColumnsToContents();

    ui -> individualsList -> horizontalHeader() -> setSectionResizeMode(QHeaderView :: Stretch);

    keepLogsBool = true;

    ui -> keepLogs -> setChecked(true);

    ui -> individualsList -> setStyleSheet("QHeaderView::section { color: white; background-color: #353535; }");
}

void MainWindow :: resizeColumn1() {
    ui -> individualsList -> setColumnWidth(1,
        std :: max(
                   (double) (6.6 * ui -> individualsList -> item(1, 1) -> text().length()),
                   (double) ui -> individualsList -> columnWidth(0)));
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_start_clicked()
{
    if (geneticAlgorithm != nullptr) {
        delete geneticAlgorithm;
        delete domain;
        delete function;
        delete coefficients;
    }

    domain = new Domain(ui -> domainA -> text().toInt(), ui -> domainB -> text().toInt());
    coefficients = new Coefficients(ui -> functionCoefficientA -> text().toInt(), ui -> functionCoefficientB -> text().toInt(), ui -> functionCoefficientC -> text().toInt());
    function = new QuadraticPolynomial(coefficients);

    populationSize = ui -> populationSize -> text().toInt();
    generationCount = ui -> generationCount -> text().toInt();

    precision = ui -> precision -> text().toDouble();
    crossoverProbability = ui -> crossoverProbability -> text().toDouble();
    mutationProbability = ui -> mutationProbability -> text().toDouble();

    geneticAlgorithm = new GeneticAlgorithm <QuadraticPolynomial> (function, domain, populationSize, generationCount, precision, crossoverProbability, mutationProbability, elitistCriteriaBool);

    ui -> nextStep -> setEnabled(true);
    ui -> nextCycle -> setEnabled(true);
    ui -> runAll -> setEnabled(true);
    ui -> currentGeneration -> setText("Generation #0");
    ui -> logs -> clear();

    clearLogAfterGen = false;
    ui -> clearLogAfterG -> setChecked(false);

    on_nextStep_clicked();

    ui -> individualsList -> horizontalHeader() -> setSectionResizeMode(QHeaderView :: Interactive);

    resizeColumn1();
}

void MainWindow :: updateIndividualsList() {
    ui -> individualsList -> setRowCount(0);
    ui -> individualsList -> setRowCount(populationSize);

    std :: string delimiter = " ";
    int row = 0;

    for (std :: string &s : geneticAlgorithm -> populationToString()) {
        auto start = 0U;
        auto end = s.find(delimiter);
        int column = -1;

        do {
            std :: string current = s.substr(start, end - start);
            start = end + delimiter.length();
            end = s.find(delimiter, start);

            QTableWidgetItem *item = new QTableWidgetItem();
            item -> setText(QString :: fromStdString(current));
            item -> setFlags(item -> flags() ^ Qt :: ItemIsEditable);
            item -> setForeground(QBrush("white"));

            ui -> individualsList -> setItem(row, ++column, item);
        } while (end != std :: string :: npos);

        ++row;
    }

    ui -> currentGeneration -> setText(QString :: fromStdString("Generation #" + std :: to_string(geneticAlgorithm -> currentGeneration)));
}

void MainWindow :: updateLog(bool clear = false) {
    if (!keepLogsBool)
        return;

    if ((clear || geneticAlgorithm -> step == 1) && clearLogAfterGen)
        ui -> logs -> clear();

    for (std :: pair <std :: string, std :: string> &s : geneticAlgorithm -> log -> log) {
        QListWidgetItem *item = new QListWidgetItem();

        item -> setText(QString :: fromStdString(s.first));

        pixmap.fill(QColor(QString :: fromStdString(s.second)));
        item -> setIcon(QIcon(pixmap));


        ui -> logs -> addItem(item);
    }

    ui -> logs -> scrollToBottom();
}

void MainWindow::on_nextStep_clicked()
{
    geneticAlgorithm -> nextStep();
    updateIndividualsList();
    updateLog();

    geneticAlgorithm -> log -> clear();
}

void MainWindow::on_nextCycle_clicked()
{
    geneticAlgorithm -> nextCycle();
    updateIndividualsList();
    updateLog(true);

    geneticAlgorithm -> log -> clear();
}

void MainWindow::on_clearLog_clicked()
{
    ui -> logs -> clear();
}

void MainWindow::on_clearLogAfterG_clicked()
{
    clearLogAfterGen = !clearLogAfterGen;
}

void MainWindow::on_runAll_clicked()
{
    on_start_clicked();
    geneticAlgorithm -> runAll();
    updateIndividualsList();
}

void MainWindow::on_keepLogs_clicked()
{
    keepLogsBool = !keepLogsBool;
    ui -> keepLogs -> setChecked(keepLogsBool);

    if (!keepLogsBool)
        ui -> logs -> clear();

    if (geneticAlgorithm)
        geneticAlgorithm -> log -> keepLog = !geneticAlgorithm -> log -> keepLog;
}

void MainWindow::on_elitistCriteria_clicked()
{
    elitistCriteriaBool = !elitistCriteriaBool;
    ui -> elitistCriteria -> setChecked(elitistCriteriaBool);

    if (geneticAlgorithm)
        geneticAlgorithm -> elitistCriteria = elitistCriteriaBool;
}
