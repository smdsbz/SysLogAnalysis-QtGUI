#ifndef DATEQUERYDIALOG_H
#define DATEQUERYDIALOG_H

#include <QDialog>
#include <QCursor>
#include <QMessageBox>
#include <QDebug>

#include "mainwindow.h"

namespace Ui {
  class DateQueryDialog;
}

class DateQueryDialog : public QDialog
{
  Q_OBJECT

public:
  explicit DateQueryDialog(MainWindow *rootWin = 0,
                           QWidget *parent = 0);
  ~DateQueryDialog();

private slots:
  void on_buttonBox_accepted();

private:
  Ui::DateQueryDialog *ui;
  MainWindow *mainWindow = nullptr;
};

#endif // DATEQUERYDIALOG_H
