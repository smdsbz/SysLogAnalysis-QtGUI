#ifndef ANALYSISDISPLAYDIALOG_H
#define ANALYSISDISPLAYDIALOG_H

#include <QDialog>
#include <QTableView>
#include <QStandardItemModel>

#include <vector>

#include "utils/StorageGraph/StorageGraph.h"
#include "utils/FPTree/FPTree.h"

#include "mainwindow.h"

namespace Ui {
  class AnalysisDisplayDialog;
}

class AnalysisDisplayDialog : public QDialog
{
  Q_OBJECT

public:
  explicit AnalysisDisplayDialog(MainWindow *rootWin = 0,
                                 QWidget *parent = 0);
  ~AnalysisDisplayDialog();
  AnalysisDisplayDialog &_render_layer_panel();
  AnalysisDisplayDialog &_render_stack_panel();
  AnalysisDisplayDialog &_render_all();

private slots:
  void on_nextLayerChoice_doubleClicked(const QModelIndex &index);

  void on_inStackPattern_doubleClicked(const QModelIndex &index);

  void on_minOccur_valueChanged(int arg1);

private:
  Ui::AnalysisDisplayDialog *ui;
  MainWindow *mainWindow;
  FPTree *analysis = nullptr;
  std::vector<FPTree::_Node *>current_pattern;
  size_t min_occur = 10;
};

#endif // ANALYSISDISPLAYDIALOG_H
