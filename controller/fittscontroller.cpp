#include "fittscontroller.h"
#include "fittsview.h"
#include "fittsmodel.h"

#include <QApplication>
#include <QDebug>
#include <QtGlobal>
#include <QChart>
#include <QLineSeries>
#include <QChartView>
#include <QBarSeries>
#include <QBarSet>
#include <QLegend>
#include <QBarCategoryAxis>
#include <QHorizontalBarSeries>
#include <QCategoryAxis>
#include <QPieSeries>
#include <QPieSlice>
#include <QStringList>
#include <QElapsedTimer>

QT_CHARTS_USE_NAMESPACE

FittsController::FittsController(FittsView *fittsView, FittsModel *fittsModel) {
    this->fittsModel = fittsModel;
    this->fittsView = fittsView;
    this->start();
}

void FittsController::start() {
    this->fittsView->show();
}

void FittsController::startSimulation() {
    this->fittsView->mainStack->setCurrentIndex(1);
    this->fittsModel->cibleLeft = this->fittsModel->nbCible;
    this->fittsView->updateTestMsg();
    this->fittsView->resultBtn->setEnabled(false);
    this->fittsView->graphicView->setEnabled(true);
    this->fittsModel->cercleSize.clear();
    this->fittsModel->cercleCenter.clear();
    this->fittsModel->clickPoints.clear();
    this->fittsModel->times.clear();

    this->initGame();
}

void FittsController::quit() {
    QApplication::quit();
}

void FittsController::backToSettings() {
    this->fittsView->mainStack->setCurrentIndex(0);
    this->fittsView->resize(800, 400);
    this->fittsView->resultBtn->setStyleSheet("background-color: gray; color: white; min-height: 50px; font-size: 25px; font-weight: bold; margin: 5px; border-radius: 25px;");

}
void FittsController::resultClicked() {
    this->fittsView->mainStack->setCurrentIndex(2);
    this->fittsView->resize(1000, 700);


    this->fittsView->resultBtn->setStyleSheet("background-color: gray; color: white; min-height: 50px; font-size: 25px; font-weight: bold; margin: 5px; border-radius: 25px;");

    this->calculateResult();
    this->calculateSecondResult();
}

// a value
void FittsController::aValueChanged(double value) {
    this->fittsModel->a = value;
    value *= 100;
    int v = (value);
    this->fittsView->aSlider->setValue(v); //Changes value of Slider
    this->calculateResult();
}

void FittsController::aSliderChanged(int value) {
    double v = (double) value / 100.0;
    this->fittsModel->a = v;
    this->fittsView->aValue->setValue(v); //Changes value of SpinBox
}


// b value
void FittsController::bValueChanged(double value) {
    this->fittsModel->b = value;
    value *= 100;
    int v = (value);
    this->fittsView->bSlider->setValue(v);
    this->calculateResult();

}

void FittsController::bSliderChanged(int value) {
    double v = (double) value / 100.0;
    this->fittsModel->b = v;
    this->fittsView->bValue->setValue(v); //Changes value of SpinBox
}


void FittsController::nbCibleChanged(int value) {
    this->fittsModel->nbCible = value;
    this->fittsView->nbCibleSlider->setValue(value); //Changes value of Slider
}


void FittsController::nbSliderChanged(int value) {
    this->fittsModel->nbCible = value;
    this->fittsView->nbCible->setValue(value); //Changes value of SpinBox
}


void FittsController::minSizeChanged(int value) {
    this->fittsModel->minSize = value;
    this->fittsView->minSizeSlider->setValue(value); //Changes value of Slider
}

void FittsController::minSliderChanged(int value) {
    this->fittsModel->minSize = value;
    this->fittsView->minSize->setValue(value); //Changes value of SpinBox

    if (this->fittsView->minSizeSlider->value() > this->fittsView->maxSizeSlider->value()) { //Makes sure min < max
        this->fittsView->maxSizeSlider->setValue(value);
    }
}


void FittsController::maxSizeChanged(int value) {
    this->fittsModel->maxSize = value;
    this->fittsView->maxSizeSlider->setValue(value);
}

void FittsController::maxSliderChanged(int value) {
    this->fittsModel->maxSize = value;
    this->fittsView->maxSize->setValue(value); //Changes value of SpinBox

    if (this->fittsView->minSizeSlider->value() > this->fittsView->maxSizeSlider->value()) { //Makes sure max > min
        this->fittsView->minSizeSlider->setValue(value);
    }


}


void FittsController::cibleClicked(int x, int y) {
    if(this->fittsModel->cercleCenter.isEmpty()) {
        // Si vide alors premier click, on demarre le timer
        this->timer = new QElapsedTimer;
        timer->start();

        // On d??marre avec la premi??re cible
        this->fittsModel->clickPoints.append(QPoint(x,y));
        this->nextCible();
    }
    else {
        QPointF coords = this->fittsView->graphicView->mapToScene(x,y);
        if(sqrt(pow(coords.x() - this->fittsModel->cercleCenter.last().x(),2) + pow(coords.y() - this->fittsModel->cercleCenter.last().y(),2)) <= this->fittsModel->cercleSize.last() / 2) {
            // On stock le temps de click
            this->fittsModel->times.append(timer->elapsed());
            // On restart le chrono
            timer->restart();

            // On stock la position du click
            this->fittsModel->clickPoints.append(QPoint(x,y));
            this->nextCible();
        }
    }
}

void FittsController::nextCible() {
    if(!this->fittsModel->cercleCenter.isEmpty())
        this->fittsModel->cibleLeft--;
    this->fittsView->updateTestMsg();

    QGraphicsScene *scene = this->fittsView->scene;
    scene->clear();

    // On stop si c'est finis
    if(this->fittsModel->cibleLeft == 0) {
        this->finish();
        return;
    }

    // On g??n??re la taille du cercle rouge
    // qrand() % ((high + 1) - low) + low;
    int size = qrand() % ((this->fittsModel->maxSize + 1) - this->fittsModel->minSize) + this->fittsModel->minSize;
    // Car on veut le rayon
    // On place le cercle dans la scene (Attention faut pas qu'il soit en dehors du cadre)
    int sceneW = int(this->fittsView->scene->width());
    int sceneH = int(this->fittsView->scene->height());

    qreal posX = qrand() % ((sceneW - size) - size) + size;
    qreal posY = qrand() % ((sceneH - size) - size) + size;

    // On stock les infos sur le cercle
    this->fittsModel->cercleCenter.append(QPoint(int(posX),int(posY)));
    this->fittsModel->cercleSize.append(size);

    // On place le cercle
    scene->addEllipse(posX - (size / 2), posY - (size / 2), size, size, QPen(QColor("red")),QBrush(QColor("red")));
}


void FittsController::finish() {
    this->fittsView->graphicView->setEnabled(false);
    this->fittsView->resultBtn->setEnabled(true);
    this->fittsView->resultBtn->setStyleSheet("background-color: #339DFF; color: white; min-height: 50px; font-size: 25px; font-weight: bold; margin: 5px; border-radius: 25px;");
}

void FittsController::initGame() {
    QGraphicsScene *scene = this->fittsView->scene;
    scene->clear();

    if(this->fittsModel->maxSize >= this->fittsView->graphicView->width() / 2)
        this->fittsModel->maxSize = this->fittsView->graphicView->width() / 2;

    if(this->fittsModel->maxSize >= this->fittsView->graphicView->height() / 2)
        this->fittsModel->maxSize = this->fittsView->graphicView->height() / 2;

    qreal posX = scene->width() / 2;
    qreal posY = scene->height() / 2;
    int size = 100;

    scene->addEllipse(posX - (size / 2), posY - (size / 2), size, size, QPen(QColor("blue")),QBrush(QColor("blue")));
}

void FittsController::calculateResult() {
    QChart *chart = new QChart;
    this->fittsView->plot->setChart(chart);
    this->fittsView->plot->setRenderHint(QPainter::Antialiasing);
    chart->setTitle("R??sultats loi Fitts");
    chart->setAnimationOptions(QChart::AllAnimations);
    chart->createDefaultAxes();
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);

    QLineSeries *expSeries = new QLineSeries;
    expSeries->setName("Courbe exp??rimentale");
    QLineSeries *fittsSeries = new QLineSeries;
    fittsSeries->setName("Courbe th??orique");
    QCategoryAxis *axis = new QCategoryAxis;

    QList<double> fittsValues;

    for(int i = 0; i < this->fittsModel->nbCible; ++i) {
        double T = this->fittsModel->times[i];
        expSeries->append(i,T);
        double D = sqrt(pow(this->fittsModel->clickPoints[i].x() - this->fittsModel->cercleCenter[i].x(),2) + pow(this->fittsModel->clickPoints[i].y() - this->fittsModel->cercleCenter[i].y(),2));

        // On multiplie par 100 pour ??tre en ms
        double value = (this->fittsModel->a * 1000) + ((this->fittsModel->b * 1000) * log2((D / this->fittsModel->cercleSize[i]) + 1));
        fittsValues.append(value);
        fittsSeries->append(i,value);

        axis->append(QString::number(i + 1) + "<br />T: "+QString::number(T)+"<br />D: " + QString::number(D),i);
    }
    axis->setLabelsPosition(QCategoryAxis::AxisLabelsPositionOnValue);

    chart->addSeries(expSeries);
    chart->addSeries(fittsSeries);

    chart->setAxisX(axis,expSeries);
    chart->setAxisX(axis,fittsSeries);

    QValueAxis *axisY = new QValueAxis;
    axisY->setTitleText("temps (en ms)");
    chart->setAxisY(axisY,expSeries);



    // Calcul des valeurs
    // Moyennes
    QList<double> diffValues;
    double diffMoy = 0;

    for (int i = 0; i < fittsValues.size(); ++i) {
        diffValues.append(fabs(fittsValues[i] - this->fittsModel->times[i]));
        diffMoy += fabs(fittsValues[i] - this->fittsModel->times[i]);
    }
    diffMoy /= fittsValues.size();

    // On stock la difference de moyenne
    this->fittsModel->diffMoy = fabs(diffMoy);


    // Ecart type
    double variance = 0;

    for (int i = 0; i < fittsValues.size(); ++i) {
        variance += pow(diffValues[i] - diffMoy,2);
    }
    variance /= fittsValues.size();

    double ecartType = sqrt(variance);

    // On stock l'ecart type
    this->fittsModel->ecartType = ecartType;
    // On stock l'erreur type
    this->fittsModel->erreurType = fabs(ecartType / sqrt(fittsValues.size()));

    // On stock itc 95%
    this->fittsModel->itc95 = 2 * this->fittsModel->erreurType;

    this->fittsView->displayResults();
}

void FittsController::calculateSecondResult(){
    QChart *chart = new QChart;
    this->fittsView->graphicResult->setChart(chart);
    this->fittsView->graphicResult->setRenderHint(QPainter::Antialiasing);

    chart->setTitle("Temps d'execution (T) en fonction de log(2D/L)");
    chart->setAnimationOptions(QChart::AllAnimations);
    chart->createDefaultAxes();
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);


    QLineSeries *lineSeries = new QLineSeries;
    lineSeries->setName("Mod??le obtenu ?? partir des donn??es exp??rimentales");

    QList<double> fittsValues;

    double moyenne = 0.0;


    for(int i = 0; i < this->fittsModel->nbCible; ++i) {
        double T = this->fittsModel->times[i];
        double D = sqrt(pow(this->fittsModel->clickPoints[i].x() - this->fittsModel->cercleCenter[i].x(),2) + pow(this->fittsModel->clickPoints[i].y() - this->fittsModel->cercleCenter[i].y(),2));

        double value = log((2*D / this->fittsModel->cercleSize[i]));
        fittsValues.append(value);

        moyenne += (T / value);
    }


    moyenne = moyenne / this->fittsModel->nbCible;

    lineSeries->append(0,0);
    lineSeries->append(20, moyenne * 20); //Creation de la droite

    chart->addSeries(lineSeries);

    QValueAxis *axisX = new QValueAxis;
    axisX->setTitleText("log(2D/L) ");
    chart->setAxisX(axisX, lineSeries);

    QValueAxis *axisY = new QValueAxis;
    axisY->setTitleText("temps (en ms)");
    chart->setAxisY(axisY,lineSeries);




    // Calcul des valeurs
    // Moyennes
    QList<double> diffValues;
    double diffMoy = 0;

    for (int i = 0; i < fittsValues.size(); ++i) {
        diffValues.append(fabs(fittsValues[i] - this->fittsModel->times[i]));
        diffMoy += fabs(fittsValues[i] - this->fittsModel->times[i]);
    }
    diffMoy /= fittsValues.size();

    // On stock la difference de moyenne
    this->fittsView->diffMoy2->setText(QString::number(diffMoy));


    // Ecart type
    double variance = 0;

    for (int i = 0; i < fittsValues.size(); ++i) {
        variance += pow(diffValues[i] - diffMoy,2);
    }
    variance /= fittsValues.size();

    double ecartType = sqrt(variance);

    // On stock l'ecart type
    this->fittsView->ecartType2->setText(QString::number(ecartType));

    // On stock l'erreur type
    this->fittsView->erreurType2->setText(QString::number(fabs(ecartType / sqrt(fittsValues.size()))));

    // On stock itc 95%
    this->fittsView->itc952->setText(QString::number(2*fabs(ecartType / sqrt(fittsValues.size()))));
}

void FittsController::showQuitWindow(){
    this->fittsView->quitWindow->show();
    this->fittsView->hide();
}

void FittsController::escape(){
    this->fittsView->quitWindow->hide();
    this->fittsView->show();
}
