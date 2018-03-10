#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QString>
#include <QFileDialog>
#include <QStandardItemModel>

#include <vector>

#include "utils/StorageGraph/StorageGraph.h"

#include "datequerydialog.h"
#include "stringquerydialog.h"
#include "createlogrecorddialog.h"
#include "analysisdisplaydialog.h"


MainWindow::MainWindow(QWidget *parent) :
      QMainWindow(parent),
      ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  // this->setFixedSize(300, 150); // unresizable
  // init self-defined data fields
  this->storage = new Storage(5000, 300);
  this->query_result = std::vector<LogRecord *>();
  // TODO: init table view **PROPERLY**
  _render_recordDisplayArea();
  this->ui->statusBar->showMessage("Press <⌘-O> to load data from log dump.");
}

MainWindow::~MainWindow()
{
  delete ui;
  delete this->storage;
  delete this->analysis;
}

Storage &MainWindow::getStorage() { return *this->storage; }
FPTree &MainWindow::getAnalysis() { return *this->analysis; }
Ui::MainWindow *MainWindow::getUi() { return this->ui; }
std::vector<LogRecord *> &MainWindow::getQueryResult() { return this->query_result; }
MainWindow &MainWindow::setQueryResult(std::vector<LogRecord *> new_result)
{
  this->query_result = new_result;
  return *this;
}

/*
 * Re-render record display area
 *
 * Args:   None
 *
 * Return: None
 */
void MainWindow::_render_recordDisplayArea(vector<LogRecord *> data_to_display)
{
  this->setCursor(Qt::WaitCursor);
  // HACK: clean dirty table
  auto prev_model = this->ui->recordDisplayArea->model();
  if (prev_model != nullptr) {
    delete prev_model; // automatically deletes QStandardItems too!
  }
  auto *view_data = new QStandardItemModel();
  // set table headers
  view_data->setColumnCount(4);
  view_data->setHeaderData(0, Qt::Horizontal, "Date");
  view_data->setHeaderData(1, Qt::Horizontal, "Host");
  view_data->setHeaderData(2, Qt::Horizontal, "Sender");
  view_data->setHeaderData(3, Qt::Horizontal, "Message");
  this->ui->recordDisplayArea->setModel(view_data);
  // fill data into view_data
  // --- no data to show
  if (this->storage->messages->global_begin == nullptr) {
    this->ui->statusBar->showMessage("No data to show! Press <⌘-O> to load data from log dump.");
    this->setCursor(Qt::ArrowCursor);
    return;
  }
  // --- have something to show
  size_t total_count = 0;
  if (!data_to_display.empty()) { // --- show query results
    for (auto &each : data_to_display) {
      view_data->setItem(
            total_count, 0,
            new QStandardItem(QString(each->get_date().c_str())));
      view_data->setItem(
            total_count, 1,
            new QStandardItem(QString(each->get_host().c_str())));
      view_data->setItem(
            total_count, 2,
            new QStandardItem(QString(each->get_sender().c_str())));
      view_data->setItem(
            total_count, 3,
            new QStandardItem(QString(each->get_message().c_str())));
      total_count += 1;
    }
  } else { // --- show every thing
    for (auto &each : *this->storage->messages->global_begin) {
      view_data->setItem(
            total_count, 0,
            new QStandardItem(QString(each.get_date().c_str())));
      view_data->setItem(
            total_count, 1,
            new QStandardItem(QString(each.get_host().c_str())));
      view_data->setItem(
            total_count, 2,
            new QStandardItem(QString(each.get_sender().c_str())));
      view_data->setItem(
            total_count, 3,
            new QStandardItem(QString(each.get_message().c_str())));
      total_count += 1;
    }
  }
  this->ui->recordDisplayArea->resizeColumnsToContents();
  this->ui->recordDisplayArea->horizontalHeader()->setStretchLastSection(true);
  this->ui->recordDisplayArea->horizontalHeader()->adjustSize();
  this->ui->statusBar->showMessage(
        QString("%1 log(s) in total.").arg(total_count));
  this->setCursor(Qt::ArrowCursor);
  return;
}

void MainWindow::on_actionOpen_existing_log_dump_triggered()
{
  if (!this->storage->isEmpty()) {
    delete this->storage;
    delete this->analysis;
    this->storage = new Storage(5000, 300);
    this->analysis = nullptr;
  }
  // load from file
  QFileDialog fileDialog(this);
  QString filename = \
      fileDialog.getOpenFileName(this, "Locate log dump file",
                                 "/Users/smdsbz/tmp/SystemLogAnalysis/test_env/",
                                 "Text (*.txt)");
  qDebug() << "Reading from" << filename;
  try {
    this->storage->read_from_file(filename.toUtf8().constData());
  } catch (const std::runtime_error &e) {
    qDebug() << "Cannot read from " << filename;
  }
  _render_recordDisplayArea();
}

void MainWindow::on_actionStart_with_new_log_dump_triggered()
{
  // clean-up
  if (!this->storage->isEmpty()) {
    delete this->storage;
    delete this->analysis;
    this->storage = new Storage(5000, 300);
    this->analysis = nullptr;
  }
  QFileDialog fileDialog(this);
  QString tgtpath = \
      fileDialog.getSaveFileName(this, "Choose where to dump logs",
                                 "/Users/smdsbz/tmp/SystemLogAnalysis/"
                                 "test_env/logdump.txt",
                                 "Text (*.txt)");
  if (tgtpath.isEmpty()) {
    // TODO: Pop-up toast message
    return;
  }
  system((QString("touch ") + tgtpath).toUtf8().constData());
  system((QString("syslog -F bsd > ") + tgtpath).toUtf8().constData());
  try {
    this->storage->read_from_file(tgtpath.toUtf8().constData());
  } catch (const std::runtime_error &e) {
    qDebug() << "Cannot read from " << tgtpath;
  }
  _render_recordDisplayArea();
}

void MainWindow::on_recordDisplayArea_clicked(const QModelIndex &index)
{
  // set cursor as shown on screen
  if (this->query_result.empty()) { // --- not query
    this->cursor = this->storage->messages->global_begin;
    for (auto cur = index.row(); cur != 0; --cur) {
      this->cursor = this->cursor->time_suc;
    }
  } else {  // --- is query
    this->cursor = this->query_result[index.row()];
  }
  // qDebug() << "Cursor set to ==> No." << index;
}

void MainWindow::on_actionQuery_via_time_triggered()
{
  auto dialog = new DateQueryDialog(this);
  dialog->show();
}

void MainWindow::on_actionShow_all_logs_in_buffer_triggered()
{
  _render_recordDisplayArea();
}

void MainWindow::on_actionQuery_via_message_content_triggered()
{
  auto dialog = new StringQueryDialog(this, "message");
  dialog->show();
}

void MainWindow::on_actionQuery_via_sender_triggered()
{
  auto dialog = new StringQueryDialog(this, "sender");
  dialog->show();
}

void MainWindow::on_actionDelete_selected_record_triggered()
{
  if (this->cursor == nullptr) {
    QMessageBox toast_box;
    toast_box.setText("Failed while deleting record!");
    toast_box.setDetailedText("Click on the record you want to delete first!");
    toast_box.exec();
    return;
  }
  try {
    this->storage->delete_rec(this->cursor);
    this->cursor = nullptr;
    QMessageBox toast_box;
    toast_box.setText("Successfully deleted record!");
    toast_box.exec();
  } catch (const std::runtime_error &e) {
    QMessageBox toast_box;
    toast_box.setText("Failed while deleting record!");
    toast_box.setDetailedText(e.what());
    toast_box.exec();
  }
  _render_recordDisplayArea();
}

void MainWindow::on_actionAdd_record_after_selection_triggered()
{
  if (this->cursor == nullptr) {
    QMessageBox toast_box;
    toast_box.setText("Failed while inserting record!");
    toast_box.setDetailedText("Click on the record you want to insert after first!");
    toast_box.exec();
    return;
  }
  auto dialog = new CreateLogRecordDialog(this);
  dialog->exec();
  _render_recordDisplayArea();
}

void MainWindow::on_actionRun_analysis_triggered()
{
  auto dialog = new AnalysisDisplayDialog(this);
  dialog->show();
}
