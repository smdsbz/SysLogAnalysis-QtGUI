#ifndef STRINGQUERYDIALOG_H
#define STRINGQUERYDIALOG_H

#include <QDialog>
#include <QCursor>
#include <QString>
#include <QPlainTextEdit>
#include <QMessageBox>

#include "mainwindow.h"

namespace Ui {
  class StringQueryDialog;
}

class StringQueryDialog : public QDialog
{
  Q_OBJECT

public:
  explicit StringQueryDialog(MainWindow *rootWin = 0,
                             const QString &axis = "message",
                             QWidget *parent = 0);
  ~StringQueryDialog();

private slots:
  void on_buttonBox_accepted();

private:
  Ui::StringQueryDialog *ui;
  MainWindow *mainWindow = nullptr;
  QString axis = "message";
};

#endif // STRINGQUERYDIALOG_H
