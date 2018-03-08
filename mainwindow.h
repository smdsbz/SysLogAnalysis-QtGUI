#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QCursor>

#include "utils/LogClass/LogClass.h"
#include "utils/StorageGraph/StorageGraph.h"
#include "utils/FPTree/FPTree.h"

namespace Ui {
  class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();
  void _render_recordDisplayArea(
      vector<LogRecord *> data_to_display=vector<LogRecord *>()); // empty vector ==> display all
  Storage &getStorage();
  FPTree &getAnalysis();
  Ui::MainWindow *getUi();
  std::vector<LogRecord *> &getQueryResult();
  MainWindow &setQueryResult(std::vector<LogRecord *> new_result);

private slots:
  void on_actionOpen_existing_log_dump_triggered();

  void on_actionStart_with_new_log_dump_triggered();

  void on_recordDisplayArea_clicked(const QModelIndex &index);

  void on_actionQuery_via_time_triggered();

  void on_actionShow_all_logs_in_buffer_triggered();

  void on_actionQuery_via_message_content_triggered();

  void on_actionQuery_via_sender_triggered();

  void on_actionDelete_selected_record_triggered();

  void on_actionAdd_record_after_selection_triggered();

  void on_actionRun_analysis_triggered();

public: // too fatigue to write getter / setter functions
        // let it be Pythonic!
  Ui::MainWindow *ui;
  Storage *storage  = nullptr; // app session
  FPTree *analysis  = nullptr;
  LogRecord *cursor = nullptr;
  std::vector<LogRecord *> query_result = std::vector<LogRecord *>();

};

#endif // MAINWINDOW_H
