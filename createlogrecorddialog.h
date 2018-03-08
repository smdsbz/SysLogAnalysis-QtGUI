#ifndef CREATELOGRECORDDIALOG_H
#define CREATELOGRECORDDIALOG_H

#include <QDialog>
#include <QSpinBox>

#include "mainwindow.h"

namespace Ui {
  class CreateLogRecordDialog;
}

class CreateLogRecordDialog : public QDialog
{
  Q_OBJECT

public:
  explicit CreateLogRecordDialog(MainWindow *rootWin = 0,
                                 QWidget *parent = 0);
  ~CreateLogRecordDialog();

private slots:
  void on_buttonBox_accepted();

private:
  Ui::CreateLogRecordDialog *ui;
  MainWindow *mainWindow = nullptr;
};

#endif // CREATELOGRECORDDIALOG_H
