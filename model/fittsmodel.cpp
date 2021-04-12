#include "fittsmodel.h"
#include "fittsview.h"

FittsModel::FittsModel() {
    this->fittsView = new FittsView(this);
}
