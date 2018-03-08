#include "mainwindow.h"
#include <QApplication>
#include <QTableView>

#include "utils/StorageGraph/StorageGraph.h"
#include "utils/FPTree/FPTree.h"

int main(int argc, char *argv[]) {
  QApplication SysLogAnalyzerApp(argc, argv);

  // Root window
  MainWindow welcomeWindow;
  welcomeWindow.show();

  return SysLogAnalyzerApp.exec();
}
