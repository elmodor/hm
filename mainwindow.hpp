#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QtWidgets>

#include "json.hpp"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
   Q_OBJECT

public:
   explicit MainWindow(QWidget *parent = 0);
   ~MainWindow();

public slots:
   void switchcall(const QString & name);
   void searchcall(const QString & name);
   void areaButtonClicked();
   void animButtonClicked();
   void updateTableData(const QString & name);
   void loadJson();
   void saveJson();

private:
   Ui::MainWindow *ui;
   QVBoxLayout *areaLay;
   QVBoxLayout *animLay;
   QStandardItemModel *tableData;
   QStandardItemModel *searchData;
   std::vector<QPushButton*> areaButtons;
   std::vector<QPushButton*> animButtons;
   nlohmann::json j;
};

#endif // MAINWINDOW_HPP
