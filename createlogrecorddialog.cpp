#include "createlogrecorddialog.h"
#include "ui_createlogrecorddialog.h"

CreateLogRecordDialog::CreateLogRecordDialog(MainWindow *rootWin, QWidget *parent) :
  QDialog(parent),
  ui(new Ui::CreateLogRecordDialog)
{
  ui->setupUi(this);
  this->mainWindow = rootWin;
  this->ui->newDate->setDateTime( // NOTE: `cursor` promised legal!
        QDateTime().fromString(this->mainWindow->cursor->get_date().c_str(),
                               "MMM dd hh:mm:ss"));
}

CreateLogRecordDialog::~CreateLogRecordDialog()
{
  delete ui;
}

void CreateLogRecordDialog::on_buttonBox_accepted() {
  auto qstr_log = QString("%1 %2 %3[%4]: %5").arg(
                    this->ui->newDate->dateTime().toString("MMM dd hh:mm:ss"),
                    this->ui->newHost->text(),
                    this->ui->newSender->text(),
                    QString().setNum(this->ui->newThread->value()),
                    this->ui->newMessage->toPlainText().replace("\n", "\t\n"));
  try {
    this->mainWindow->getStorage().add_after_rec(this->mainWindow->cursor,
                                                 qstr_log.toUtf8().constData());
    QMessageBox toast_box;
    toast_box.setText("Successfully inserted record!");
    toast_box.exec();
  } catch (const std::runtime_error &e) {
    QMessageBox toast_box;
    toast_box.setText("Failed while inserting record!");
    toast_box.setDetailedText(e.what());
    toast_box.exec();
    return;
  }
}
