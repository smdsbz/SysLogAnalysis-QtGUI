#include "datequerydialog.h"
#include "ui_datequerydialog.h"

#include "utils/LogClass/LogClass.h"

DateQueryDialog::DateQueryDialog(MainWindow *rootWin, QWidget *parent) :
  QDialog(parent),
  ui(new Ui::DateQueryDialog)
{
  ui->setupUi(this);
  this->mainWindow = rootWin;
}

DateQueryDialog::~DateQueryDialog()
{
  delete ui;
}

void DateQueryDialog::on_buttonBox_accepted() {
  std::string str_tgtdate = \
      this->ui->targetDate->dateTime()
      .toString("MMM dd hh:mm:ss").toUtf8().constData();
  // qDebug() << "Querying on time:";
  // qDebug() << qt_tgtdate.toString("MMM dd hh:mm:ss").toUtf8().constData();
  // QDate to LogDate convertion check
  try {
    LogDate(str_tgtdate);
  } catch (const std::runtime_error &e) {
    qDebug() << e.what();
  }
  this->setCursor(Qt::WaitCursor);
  auto result = this->mainWindow->getStorage().query(str_tgtdate, false, "time");
  if (result.empty()) {
    QMessageBox toast_box;
    toast_box.setText("No result!");
    toast_box.exec();
    return;
  }
  this->mainWindow->setQueryResult(result);
  this->mainWindow->_render_recordDisplayArea(result);
}
