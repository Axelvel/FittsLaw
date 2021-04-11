#include "FittsView.h"
#include "graphicwidget.h"
#include "../Controller/fittscontroller.h"
#include "../Model/fittsmodel.h"

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QPushButton>

#include <QStackedLayout>
#include <QGraphicsView>
#include <QChart>
#include <QPolarChart>
#include <QLineSeries>
#include <QImage>

QT_CHARTS_USE_NAMESPACE

FittsView::FittsView(FittsModel *fittsModel) : QMainWindow() {
    this->fittsModel = fittsModel;

    this->initWindows();
    this->fittsController = new FittsController(this, this->fittsModel);

    // Btn clicked
    connect(leaveBtn,SIGNAL(clicked()),fittsController,SLOT(showQuitWindow()));
    connect(startBtn,SIGNAL(clicked()),fittsController,SLOT(startSimulation()));
    connect(quitButton,SIGNAL(clicked()),fittsController,SLOT(quit()));
    connect(escapeButton,SIGNAL(clicked()),fittsController,SLOT(escape()));

    connect(backBtn,SIGNAL(clicked()),fittsController,SLOT(backToSettings()));
    connect(resultBtn,SIGNAL(clicked()),fittsController,SLOT(resultClicked()));

    connect(resultLeaveBtn,SIGNAL(clicked()),fittsController,SLOT(showQuitWindow()));
    connect(restartBtn,SIGNAL(clicked()),fittsController,SLOT(backToSettings()));

    connect(graphicView, SIGNAL(mouseClicked(int,int)), fittsController, SLOT(cibleClicked(int,int)));

    // SpinBox values update
    connect(aValue,SIGNAL(valueChanged(double)),fittsController,SLOT(aValueChanged(double)));
    connect(bValue,SIGNAL(valueChanged(double)),fittsController,SLOT(bValueChanged(double)));
    connect(nbCible,SIGNAL(valueChanged(int)),fittsController,SLOT(nbCibleChanged(int)));
    connect(minSize,SIGNAL(valueChanged(int)),fittsController,SLOT(minSizeChanged(int)));
    connect(maxSize,SIGNAL(valueChanged(int)),fittsController,SLOT(maxSizeChanged(int)));


    // Sliders value updated
    //connect(aSlider, SIGNAL(valueChanged(double)), fittsController, SLOT(aSliderChanged(double)));
    connect(aSlider, SIGNAL(valueChanged(int)), fittsController, SLOT(aSliderChanged(int)));
    connect(bSlider, SIGNAL(valueChanged(int)), fittsController, SLOT(bSliderChanged(int)));
    connect(nbCibleSlider, SIGNAL(valueChanged(int)), fittsController, SLOT(nbSliderChanged(int)));
    connect(minSizeSlider, SIGNAL(valueChanged(int)), fittsController, SLOT(minSliderChanged(int)));
    connect(maxSizeSlider, SIGNAL(valueChanged(int)), fittsController, SLOT(maxSliderChanged(int)));
}

FittsView::~FittsView() {}

void FittsView::initWindows() {


    mainWidget = new QWidget;
    this->setCentralWidget(mainWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(mainWidget);

    mainStack = new QStackedLayout;
    mainLayout->addLayout(mainStack);

    QWidget *settingsWidget = new QWidget;
    mainStack->addWidget(settingsWidget);

    QVBoxLayout *settingsLayout = new QVBoxLayout(settingsWidget);
    settingsLayout->setSpacing(50);


    QLabel *label;

    label = new QLabel("Programme pour vérifier la loi Fitts");
    label->setAlignment(Qt::AlignCenter);
    label->setFont(QFont("Arial",35, QFont::Bold));
    settingsLayout->addWidget(label);


    QHBoxLayout * firstLayout = new QHBoxLayout;
    settingsLayout->addLayout(firstLayout);

    QLabel * explanationsLabel = new QLabel("Ce programme a pour but de vérifier la loi de Fitts, un modèle du mouvement humain, indicatif de la difficulté d'une tâche. Vous allez devoir passer un petit test. À la fin, nous comparerons vos résultats avec une approximation généré par le modèle.");
    explanationsLabel->setFont(QFont("Arial", 17));
    explanationsLabel->setMaximumWidth(300);
    explanationsLabel->setWordWrap(true);
    firstLayout->addWidget(explanationsLabel);






    //Config Box

    QGroupBox *configBox = new QGroupBox("Configuration du test :");
    configBox->setMaximumWidth(1000);
    //settingsLayout->addWidget(configBox);
    firstLayout->addWidget(configBox);
    QGridLayout *configLayout = new QGridLayout(configBox);
    configLayout->setVerticalSpacing(40);
    //configLayout->setSpacing(50);

    configBox->setStyleSheet("font-size: 15px"); //

    configLayout->addWidget(new QLabel("Nombre de cibles:"),1,0);
    configLayout->addWidget(new QLabel("Taille minimum cible:"),2,0);
    configLayout->addWidget(new QLabel("Taille maximal cible:"),3,0);

    nbCible = new QSpinBox;
    nbCible->setValue(this->fittsModel->nbCible);
    //nbCible->setMinimumSize(QSize(100,30)); //
    nbCible->setMinimumWidth(10);
    nbCible->setMaximum(100);
    configLayout->addWidget(nbCible,1,1);

    //Slider 1 nbCible
    nbCibleSlider = new QSlider(Qt::Horizontal);
    nbCibleSlider->setRange(0,99);
    nbCibleSlider->setValue(10);
    nbCibleSlider->setMinimumWidth(200);
    //nbCibleSlider->setMaximumWidth(600);
    configLayout->addWidget(nbCibleSlider, 1, 2);


    minSize = new QSpinBox;
    minSize->setMinimum(5);
    minSize->setMaximum(300);
    minSize->setValue(this->fittsModel->minSize);
    configLayout->addWidget(minSize,2,1);

    //Slider 2 minSize
    minSizeSlider = new QSlider(Qt::Horizontal);
    minSizeSlider->setRange(5,300);
    minSizeSlider->setValue(10);
    nbCibleSlider->setMinimumWidth(200);
    configLayout->addWidget(minSizeSlider, 2, 2);

    maxSize = new QSpinBox;
    maxSize->setMinimum(5);
    maxSize->setMaximum(300);
    maxSize->setValue(this->fittsModel->maxSize);
    configLayout->addWidget(maxSize,3,1);

    //Slider 3 maxSize
    maxSizeSlider = new QSlider(Qt::Horizontal);
    maxSizeSlider->setRange(5,300);
    maxSizeSlider->setValue(150);
    maxSizeSlider->setMinimumWidth(200);
    configLayout->addWidget(maxSizeSlider, 3, 2);




    QHBoxLayout *btnLayout = new QHBoxLayout;
    settingsLayout->addLayout(btnLayout);

    leaveBtn = new QPushButton("Quitter");
    leaveBtn->setStyleSheet("QPushButton { background-color: #B22222; color: white; min-height: 50px; font-size: 25px; font-weight: bold; margin: 5px; border-radius: 25px;}");
    //leaveBtn->setMinimumHeight(50);
    btnLayout->addWidget(leaveBtn);



    startBtn = new QPushButton("Démarrer");
    //startBtn->setMinimumHeight(50); //
    startBtn->setStyleSheet("QPushButton { background-color: #339DFF; color: white; min-height: 50px; font-size: 25px; font-weight: bold; margin: 5px; border-radius: 25px;}");

/*
    QPalette palette = startBtn->palette();
    palette.setColor(QPalette::Background, QColor("#339DFF"));
    startBtn->setPalette(palette);
*/
    btnLayout->addWidget(startBtn);









    // Test part

    QWidget *testWidget = new QWidget;
    mainStack->addWidget(testWidget);

    QVBoxLayout *testLayout = new QVBoxLayout(testWidget);

    testLabel = new QLabel;
    testLayout->addWidget(testLabel);

    testLabel2 = new QLabel("Cliquez sur les cibles qui apparaissent en <font color='#B22222'><strong>rouge</strong></font>");
    testLabel2->setStyleSheet("font-size: 15px");
    testLayout->addWidget(testLabel2);

    graphicView = new GraphicWidget;
    testLayout->addWidget(graphicView);
    graphicView->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff);
    graphicView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    scene = new QGraphicsScene;
    graphicView->fitInView(scene->sceneRect(),Qt::KeepAspectRatio);
    graphicView->setScene(scene);
    scene->setSceneRect(0,0,graphicView->width(),300);

    btnLayout = new QHBoxLayout;
    testLayout->addLayout(btnLayout);

    backBtn = new QPushButton("Retour");
    backBtn->setStyleSheet("background-color: #B22222; color: white; min-height: 50px; font-size: 25px; font-weight: bold; margin: 5px; border-radius: 25px;");
    backBtn->setMinimumHeight(50); //
    btnLayout->addWidget(backBtn);

    resultBtn = new QPushButton("Résultats");
    resultBtn->setStyleSheet("background-color: gray; color: white; min-height: 50px; font-size: 25px; font-weight: bold; margin: 5px; border-radius: 25px;");
    resultBtn->setMinimumHeight(50); //
    btnLayout->addWidget(resultBtn);
    resultBtn->setEnabled(false);















    //Results Window


    QWidget *resultWidget = new QWidget;


    mainStack->addWidget(resultWidget);
    QVBoxLayout *resultLayout = new QVBoxLayout(resultWidget);


    //Tab Widget
    QTabWidget * tabWidget = new QTabWidget;

    resultLayout->addWidget(tabWidget);

    QWidget * graph1 = new QWidget;
    graph1->setFont(QFont("Arial", 20));

    QWidget * graph2 = new QWidget;
    graph2->setFont(QFont("Arial", 20));

    QVBoxLayout * graph1Layout = new QVBoxLayout(graph1);
    QVBoxLayout * graph2Layout = new QVBoxLayout(graph2);

    tabWidget->addTab(graph1, "Graphique 1");
    tabWidget->addTab(graph2, "Graphique 2");



    //Results box

    QGroupBox *resultBox =  new QGroupBox("Statistiques");
    QGridLayout *resultBoxLayout = new QGridLayout(resultBox);

    label = new QLabel("Ecart-Type : ");
    resultBoxLayout->addWidget(label,0,0);
    ecartType = new QLabel;
    resultBoxLayout->addWidget(ecartType,0,1);

    label = new QLabel("Erreur-Type : ");
    resultBoxLayout->addWidget(label,1,0);
    erreurType = new QLabel;
    resultBoxLayout->addWidget(erreurType,1,1);

    label = new QLabel("Différence moyenne : ");
    resultBoxLayout->addWidget(label,0,2);
    diffMoy = new QLabel;
    resultBoxLayout->addWidget(diffMoy,0,3);

    label = new QLabel("Intervalle de confiance à 95% : ");
    resultBoxLayout->addWidget(label,1,2);
    itc95 = new QLabel;
    resultBoxLayout->addWidget(itc95,1,3);

    resultBoxLayout->setColumnStretch(1,10);
    resultBoxLayout->setColumnStretch(3,10);



    //Rappel box

    QGroupBox *rappelBox = new QGroupBox("Rappel :");

    QHBoxLayout *rappelLayout = new QHBoxLayout(rappelBox);

    QVBoxLayout *rappelLeftLayout = new QVBoxLayout();
    rappelLayout->addLayout(rappelLeftLayout,2);

    label = new QLabel("Formule utilisée:");
    rappelLeftLayout->addWidget(label);


    label = new QLabel;
    label->setPixmap(QPixmap(":/data/formule.png").scaled(200,100,Qt::KeepAspectRatio));
    rappelLeftLayout->addWidget(label);


    QGridLayout *rappelRightLayout = new QGridLayout();

    rappelLayout->addLayout(rappelRightLayout,1);

    label = new QLabel("Choix de a et b");
    rappelRightLayout->addWidget(label,0,0,1,2);

    rappelRightLayout->addWidget(new QLabel("Variable a "),1,0);
    rappelRightLayout->addWidget(new QLabel("Variable b "),2,0);

    aValue = new QDoubleSpinBox;
    aValue->setRange(0,10);
    aValue->setValue(this->fittsModel->a); //TODO: Remove ?
    rappelRightLayout->addWidget(aValue,1,1);

    //Slider a

    aSlider = new QSlider(Qt::Horizontal);
    aSlider->setRange(0,1000);
    rappelRightLayout->addWidget(aSlider, 1, 2);


    bValue = new QDoubleSpinBox;
    bValue->setRange(0,10);
    bValue->setValue(this->fittsModel->b);
    bValue->setMinimumHeight(30); //TODO: Remove
    rappelRightLayout->addWidget(bValue,2,1);

    //Slider b

    bSlider = new QSlider(Qt::Horizontal);
    bSlider->setRange(0,1000);
    rappelRightLayout->addWidget(bSlider, 2, 2);



    //Graph 1 Tab
    plot = new QChartView;
    graph1Layout->addWidget(plot); //TODO:Change name of graph1
    graph1Layout->addWidget(resultBox);
    graph1Layout->addWidget(rappelBox);

    //Graph2 Tab
    graphicResult = new QChartView;
    graph2Layout->addWidget(graphicResult); //TODO: Change name of graph2

    QGroupBox *secondRBox = new QGroupBox("Statistiques");
    graph2Layout->addWidget(secondRBox);
    QGridLayout *secondRBoxLayout = new QGridLayout(secondRBox);

    label = new QLabel("Ecart-Type : ");
    secondRBoxLayout->addWidget(label,0,0);
    ecartType2 = new QLabel;
    secondRBoxLayout->addWidget(ecartType2,0,1);

    label = new QLabel("Erreur-Type : ");
    secondRBoxLayout->addWidget(label,1,0);
    erreurType2 = new QLabel;
    secondRBoxLayout->addWidget(erreurType2,1,1);

    label = new QLabel("Différence moyenne : ");
    secondRBoxLayout->addWidget(label,0,2);
    diffMoy2 = new QLabel;
    secondRBoxLayout->addWidget(diffMoy2,0,3);

    label = new QLabel("Intervalle de confiance à 95% : ");
    secondRBoxLayout->addWidget(label,1,2);
    itc952 = new QLabel;
    secondRBoxLayout->addWidget(itc952,1,3);


    btnLayout = new QHBoxLayout;
    resultLayout->addLayout(btnLayout);

    resultLeaveBtn = new QPushButton("Quitter");
    resultLeaveBtn->setStyleSheet("background-color: #B22222; color: white; min-height: 50px; font-size: 25px; font-weight: bold; margin: 5px; border-radius: 25px;");
    resultLeaveBtn->setMinimumHeight(50);
    btnLayout->addWidget(resultLeaveBtn);

    restartBtn = new QPushButton("Recommencer");
    restartBtn->setStyleSheet("background-color: #339DFF; color: white; min-height: 50px; font-size: 25px; font-weight: bold; margin: 5px; border-radius: 25px;");
    restartBtn->setMinimumHeight(50);
    btnLayout->addWidget(restartBtn);




    //Confirmation Window

    quitWindow = new QWidget;
    quitWindow->setMinimumSize(QSize(500,300));

    QSize screenSize = qApp->screens()[0]->size();
    quitWindow->move((screenSize.width()/2) - (250), (screenSize.height()/2) - ((150)));


    QVBoxLayout *quitParent = new QVBoxLayout(quitWindow);

    quitLabel = new QLabel("Ceci fermera l'application, êtes vous sûr de vouloir quitter l'application ?");
    quitLabel->setFont(QFont("Arial", 15));
    quitParent->addWidget(quitLabel);

    QHBoxLayout *quitLayout = new QHBoxLayout();
    quitParent->addLayout(quitLayout);

    quitButton = new QPushButton("Quitter");
    quitButton->setStyleSheet("background-color: #B22222; color: white; min-height: 50px; font-size: 25px; font-weight: bold; margin: 5px; border-radius: 25px;");
    quitButton->setMinimumHeight(50);
    quitLayout->addWidget(quitButton);

    escapeButton = new QPushButton("Annuler");
    escapeButton->setStyleSheet("background-color: gray; color: white; min-height: 50px; font-size: 25px; font-weight: bold; margin: 5px; border-radius: 25px;");
    escapeButton->setMinimumHeight(50);
    quitLayout->addWidget(escapeButton);
}


void FittsView::updateTestMsg() {

    if (this->fittsModel->cibleLeft == 0) {
        this->testLabel->setText("Félicitation, vous avez terminé le test !");
        this->testLabel2->hide();
    } else {
        this->testLabel->setText("<strong>Le test commencera après avoir appuyé sur la cible <font color='#339DFF'>bleue</font>.</strong> Nombre de cibles restantes : " + QString::number(this->fittsModel->cibleLeft));
        this->testLabel->setFont(QFont("Arial", 18));
        this->testLabel2->show();
    }
}


void FittsView::displayResults() {
    this->diffMoy->setText(QString::number(this->fittsModel->diffMoy));
    this->ecartType->setText(QString::number(this->fittsModel->ecartType));
    this->erreurType->setText(QString::number(this->fittsModel->erreurType));
    this->itc95->setText(QString::number(this->fittsModel->itc95));
}
