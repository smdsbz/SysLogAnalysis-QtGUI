#include "analysisdisplaydialog.h"
#include "ui_analysisdisplaydialog.h"

AnalysisDisplayDialog::AnalysisDisplayDialog(MainWindow *rootWin, QWidget *parent) :
  QDialog(parent),
  ui(new Ui::AnalysisDisplayDialog)
{
  ui->setupUi(this);
  this->layout()->setSizeConstraint(QLayout::SetNoConstraint);
  this->setSizeGripEnabled(true);
  this->mainWindow = rootWin;
  // init fp-tree and build analysis cache
  if (this->mainWindow->storage != nullptr) {
    this->analysis = new FPTree(this->mainWindow->storage);
    // TODO: Dialog to determine hyper-parameters
    this->analysis->run(/*hp_min_freq=*/10, /*hp_max_delay=*/1);
  } else { /* TODO: Error message and Close */ ; }
  // render displays
  _render_all();
}

AnalysisDisplayDialog::~AnalysisDisplayDialog()
{
  delete ui;
  if (this->analysis) { delete this->analysis; }
}

AnalysisDisplayDialog &
AnalysisDisplayDialog::_render_layer_panel()
{
  // clean previous display
  auto prev_model = this->ui->nextLayerChoice->model();
  if (prev_model != nullptr) { delete prev_model; }
  // linking current display to data model
  auto *layer_nodes = new QStandardItemModel();
  layer_nodes->setColumnCount(3);
  layer_nodes->setHeaderData(0, Qt::Horizontal, "Host");
  layer_nodes->setHeaderData(1, Qt::Horizontal, "Sender");
  layer_nodes->setHeaderData(2, Qt::Horizontal, "Message");
  this->ui->nextLayerChoice->setModel(layer_nodes);
  // buffering model with data
  // --- nothing to show
  if (this->analysis->headers.size() == 0) {
    layer_nodes->setItem(0, 2, new QStandardItem(QString("Nothing to show!")));
  } else if (this->analysis->nodes) {  // --- something to show
    size_t count = 0;
    FPTree::_Node *each = nullptr;
    if (this->current_pattern.empty()) {  // --- but at 0-th layer
      each = this->analysis->nodes;
    } else {  // --- not on first layer, get current layer
      each = this->current_pattern.back()->child;
    }
    for (/* pass */;
         each && each->occur >= this->min_occur;
         each = each->brother, ++count) {
      // qDebug() << "Adding No." << count + 1 << each << "to left panel";
      layer_nodes->setItem(
            count, 0,
            new QStandardItem(QString(each->entity->data.get_host().c_str())));
      layer_nodes->setItem(
            count, 1,
            new QStandardItem(QString(each->entity->data.get_sender().c_str())));
      layer_nodes->setItem(
            count, 2,
            new QStandardItem(QString(each->entity->data.get_message().c_str())));
    }
    this->ui->layerLogCount->setText(QString("%1 log(s)").arg(QString().setNum(count)));
  } else {  // --- there is headers, but no nodes??
    // not possible
    /* pass */ ;
  }
  this->ui->nextLayerChoice->resizeColumnsToContents();
  this->ui->nextLayerChoice->horizontalHeader()->setStretchLastSection(true);
  // this->ui->nextLayerChoice->horizontalHeader()->adjustSize();
  return *this;
}

AnalysisDisplayDialog &
AnalysisDisplayDialog::_render_stack_panel()
{
  // clean previous display
  auto prev_model = this->ui->inStackPattern->model();
  if (prev_model != nullptr) { delete prev_model; }
  // linking current display to data model
  auto *layer_nodes = new QStandardItemModel();
  layer_nodes->setColumnCount(3);
  layer_nodes->setHeaderData(0, Qt::Horizontal, "Host");
  layer_nodes->setHeaderData(1, Qt::Horizontal, "Sender");
  layer_nodes->setHeaderData(2, Qt::Horizontal, "Message");
  this->ui->inStackPattern->setModel(layer_nodes);
  // buffering model with data
  // --- nothing to show
  if (this->analysis->headers.size() == 0) {
    // layer_nodes->setItem(0, 2, new QStandardItem(QString("Nothing to show!")));
  } else {  // --- something to show
    size_t count = 0;
    for (auto &each : this->current_pattern) {
      layer_nodes->setItem(
            count, 0,
            new QStandardItem(QString(each->entity->data.get_host().c_str())));
      layer_nodes->setItem(
            count, 1,
            new QStandardItem(QString(each->entity->data.get_sender().c_str())));
      layer_nodes->setItem(
            count, 2,
            new QStandardItem(QString(each->entity->data.get_message().c_str())));
      ++count;
    }
    if (this->current_pattern.size() == 0) {
      this->ui->patternOccurCount->setText(QString("(Double click on the left panel "
                                                   "to add log into pattern)"));
    } else {
      this->ui->patternOccurCount->setText(QString("Pattern occured %1 time(s)")
                                           .arg(QString().setNum(this->current_pattern.back()->occur)));
    }
  }
  this->ui->inStackPattern->resizeColumnsToContents();
  this->ui->inStackPattern->horizontalHeader()->setStretchLastSection(true);
  // this->ui->inStackPattern->horizontalHeader()->adjustSize();
  return *this;
}

AnalysisDisplayDialog &
AnalysisDisplayDialog::_render_all()
{
  _render_layer_panel();
  _render_stack_panel();
  return *this;
}

void AnalysisDisplayDialog::on_nextLayerChoice_doubleClicked(const QModelIndex &index)
{
  // get LogRecord* clicked
  FPTree::_Node *cur = nullptr;
  if (this->current_pattern.empty()) {  // --- entering first layer
    cur = this->analysis->nodes;
  } else {  // --- venturing forth into pattern
    cur = this->current_pattern.back()->child;
  }
  for (auto cnt = index.row(); cnt != 0; --cnt) {
    cur = cur->brother;
  }
  this->current_pattern.push_back(cur);
  _render_all();
}

void AnalysisDisplayDialog::on_inStackPattern_doubleClicked(const QModelIndex &index)
{
  // get LogRecord* clicked, whilst clear trailing patterns
  FPTree::_Node *cur = nullptr;
  while (this->current_pattern.size() != static_cast<size_t>(index.row() + 1)) {
    this->current_pattern.pop_back();
  }
  cur = this->current_pattern.back();
  this->current_pattern.pop_back();
  _render_all();
}

void AnalysisDisplayDialog::on_minOccur_valueChanged(int arg1)
{
  this->min_occur = arg1;
  this->current_pattern.clear();
  _render_all();
}
