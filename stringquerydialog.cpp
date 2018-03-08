#include "stringquerydialog.h"
#include "ui_stringquerydialog.h"

StringQueryDialog::StringQueryDialog(MainWindow *rootWin,
                                     const QString &queryAxis,
                                     QWidget *parent) :
  QDialog(parent),
  ui(new Ui::StringQueryDialog),
  mainWindow(rootWin),
  axis(queryAxis)
{
  ui->setupUi(this);
}

StringQueryDialog::~StringQueryDialog()
{
  delete ui;
}

void StringQueryDialog::on_buttonBox_accepted() {
  std::string search_content = \
      this->ui->plainTextEdit->toPlainText()
      .replace("\n", "\t\n")
      .toUtf8().constData();
  bool use_fuzzy = this->ui->checkBox->isChecked();
  this->setCursor(Qt::WaitCursor);
  auto result = this->mainWindow->getStorage()
                .query(search_content, use_fuzzy,
                       this->axis.toUtf8().constData());
  if (result.empty()) {
    QMessageBox toast_box;
    toast_box.setText("No result!");
    toast_box.exec();
    return;
  }
  this->mainWindow->setQueryResult(result);
  this->mainWindow->_render_recordDisplayArea(result);
}
