#include "mainwindow.hpp"
#include "ui_mainwindow.h"

using json = nlohmann::json;

#include <stdio.h>
#include <fstream>
#include <ostream>
#include <iostream>

//json j = R"(

//   [
//      {
//         "name": "region1",
//         "area":
//         [
//            {
//               "name": "area1",
//               "anim":
//               [
//                  {
//                     "name": "anim1",
//                     "count": 0
//                  },
//                  {
//                     "name": "anim2",
//                     "count": 0
//                  }
//               ]
//            }
//         ]
//      }
//   ]
//)"_json;


MainWindow::MainWindow(QWidget *parent) :
   QMainWindow(parent),
   ui(new Ui::MainWindow)
{
   ui->setupUi(this);

   loadJson();

   tableData = new QStandardItemModel();
   searchData = new QStandardItemModel();

   ui->tableView->setModel(tableData);
   tableData->setHorizontalHeaderItem(0, new QStandardItem("Name"));
   tableData->setHorizontalHeaderItem(1, new QStandardItem("Count"));
   tableData->setHorizontalHeaderItem(2, new QStandardItem("Percent"));
   ui->tableView->setSortingEnabled(false);
   ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

   ui->tableView_2->setModel(searchData);
   searchData->setHorizontalHeaderItem(0, new QStandardItem("Area"));
   searchData->setHorizontalHeaderItem(1, new QStandardItem("Count"));
   searchData->setHorizontalHeaderItem(2, new QStandardItem("Percent"));
   ui->tableView_2->setSortingEnabled(true);
   ui->tableView_2->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

   connect(ui->comboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(switchcall(const QString&)));
   connect(ui->comboBox_2, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(searchcall(const QString&)));

   int i = 0;
   for(auto&element : j)
   {
      ui->comboBox->insertItem(i, QString::fromStdString(element["name"]));
      ++i;
   }
   ui->comboBox->setCurrentIndex(0);

   ui->horizontalLayout->setSpacing(50);
}

void
MainWindow::searchcall(const QString & name)
{
   for(auto&element : j)
   {
      if(QString::fromStdString(element["name"]) == ui->comboBox->currentText())
      {
         int i = 0;
         for( auto& area : element["area"] )
         {
            int amount = 0;
            for( auto& anim : area["anim"] )
            {
               amount+= anim["count"].get<int>();
            }

            for( auto& anim : area["anim"] )
            {
               if(QString::fromStdString(anim["name"]) == name)
               {
                  QStandardItem *item = new QStandardItem();
                  item->setData(QVariant(QString::fromStdString(area["name"])), Qt::DisplayRole);
                  searchData->setItem(i, 0, item);

                  QStandardItem *item2 = new QStandardItem();
                  item2->setData(QVariant(anim["count"].get<int>()), Qt::DisplayRole);
                  searchData->setItem(i, 1, item2);

                  QStandardItem *item3 = new QStandardItem();
                  float perc = ((float)anim["count"].get<int>() / (float)amount ) * 100;
                  QString s = QString::number(perc, 'f', 2);
                  s.append(QString(" %"));
                  item3->setData(QVariant(s), Qt::DisplayRole);
                  searchData->setItem(i, 2, item3);

                  ++i;
               }
            }
         }

         searchData->sort(1, Qt::DescendingOrder);
         ui->tableView_2->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

         if(searchData->rowCount() > i+1)
         {
            // clear
            searchData->removeRows(i+1, searchData->rowCount()-(i+1));
         }

         break;
      }
   }
}

void
MainWindow::switchcall(const QString & name)
{
   tableData->clear();
   searchData->clear();

   tableData->setHorizontalHeaderItem(0, new QStandardItem("Name"));
   tableData->setHorizontalHeaderItem(1, new QStandardItem("Count"));
   tableData->setHorizontalHeaderItem(2, new QStandardItem("Percent"));
   ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

   searchData->setHorizontalHeaderItem(0, new QStandardItem("Area"));
   searchData->setHorizontalHeaderItem(1, new QStandardItem("Count"));
   searchData->setHorizontalHeaderItem(2, new QStandardItem("Percent"));
   ui->tableView_2->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

   for(auto it = areaButtons.begin(); it != areaButtons.end(); )
   {
      (*it)->deleteLater();
      areaButtons.erase(it);
   }

   for(auto it = animButtons.begin(); it != animButtons.end(); )
   {
      (*it)->deleteLater();
      animButtons.erase(it);
   }

   for(auto&element : j)
   {
      if(QString::fromStdString(element["name"]) == name)
      {
         int i = 0;
         for( auto& area : element["area"] )
         {
            QString name = QString::fromStdString(area["name"]);
            QPushButton *btn = new QPushButton(name);
            btn->setEnabled( true );
            btn->setObjectName(name);
            btn->setMinimumHeight(30);
            btn->setStyleSheet(":disabled { color: black; background-color:#8080ff }");
            areaButtons.push_back(btn);
            ui->verticalLayout_2->addWidget(btn);
            btn->show();
            connect(areaButtons[i], SIGNAL(clicked()), this, SLOT(areaButtonClicked()));
            ++i;
         }

         i = 0;
         ui->comboBox_2->clear();
         for( auto& anim : element["area"][0]["anim"] )
         {
            QString name = QString::fromStdString(anim["name"]);
            QPushButton *btn = new QPushButton(name);
            btn->setEnabled( false );
            btn->setObjectName(name);
            btn->setMinimumHeight(30);

            if(anim["class"] == 0)
               btn->setStyleSheet(":enabled { color: black; background-color:#cccccc }");
            else if(anim["class"] == 1)
               btn->setStyleSheet(":enabled { color: black; background-color:#99ff99 }");
            else if(anim["class"] == 2)
               btn->setStyleSheet(":enabled { color: black; background-color:#ffff99 }");
            else if(anim["class"] == 3)
               btn->setStyleSheet(":enabled { color: black; background-color:#ff9999 }");
            else if(anim["class"] == 4)
               btn->setStyleSheet(":enabled { color: black; background-color:#ff99cc }");

            animButtons.push_back(btn);
            ui->verticalLayout->addWidget(btn);
            btn->show();
            connect(animButtons[i], SIGNAL(clicked()), this, SLOT(animButtonClicked()));

            ui->comboBox_2->insertItem(i, name);

            ++i;
         }

         ui->comboBox_2->setCurrentIndex(0);

         break;
      }
   }
}

void
MainWindow::areaButtonClicked()
{
   QObject *senderObj = sender();
   QString senderObjName = senderObj->objectName();

   for(auto it = areaButtons.begin(); it != areaButtons.end(); ++it)
   {
      if( (*it)->objectName() == senderObjName)
      {
         (*it)->setEnabled(false);

         tableData->clear();
         tableData->setHorizontalHeaderItem(0, new QStandardItem("Name"));
         tableData->setHorizontalHeaderItem(1, new QStandardItem("Count"));
         tableData->setHorizontalHeaderItem(2, new QStandardItem("Percent"));
         updateTableData(senderObjName);
      }
      else
      {
         (*it)->setEnabled(true);
      }
   }

   for(auto it = animButtons.begin(); it != animButtons.end(); ++it)
   {
      (*it)->setEnabled( true );
   }

}

void
MainWindow::animButtonClicked()
{
   QObject *senderObj = sender();
   QString senderObjName = senderObj->objectName();


   for(auto it = areaButtons.begin(); it != areaButtons.end(); ++it)
   {
      if( !(*it)->isEnabled() )
      {
         for(auto&element : j)
         {
            if(QString::fromStdString(element["name"]) == ui->comboBox->currentText())
            {
               for( auto& area : element["area"] )
               {
                  if( QString::fromStdString(area["name"]) == (*it)->objectName())
                  {
                     for( auto& anim : area["anim"] )
                     {
                        if(QString::fromStdString(anim["name"]) == senderObjName )
                        {
                           anim["count"] = anim["count"].get<int>() + 1;
                           updateTableData((*it)->objectName());

                           break;
                        }
                     }
                  }
               }
            }
         }
      }
   }
}

void
MainWindow::updateTableData(const QString &name)
{
   for(auto&element : j)
   {
      if(QString::fromStdString(element["name"]) == ui->comboBox->currentText())
      {
         for( auto& area : element["area"] )
         {
            if( QString::fromStdString(area["name"]) == name)
            {
               int i = 0;
               int amount = 0;
               for( auto& anim : area["anim"] )
               {
                  amount+= anim["count"].get<int>();
               }

               for( auto& anim : area["anim"] )
               {
                  QStandardItem *item = new QStandardItem();
                  item->setData(QVariant(QString::fromStdString(anim["name"])), Qt::DisplayRole);
                  tableData->setItem(i, 0, item);

                  QStandardItem *item2 = new QStandardItem();
                  item2->setData(QVariant(anim["count"].get<int>()), Qt::DisplayRole);
                  tableData->setItem(i, 1, item2);

                  if(amount > 0)
                  {
                     QStandardItem *item3 = new QStandardItem();
                     float perc = ((float)anim["count"].get<int>() / (float)amount ) * 100;
                     QString s = QString::number(perc, 'f', 2);
                     s.append(QString(" %"));
                     item3->setData(QVariant(s), Qt::DisplayRole);
                     tableData->setItem(i, 2, item3);
                  }

                  ++i;
               }

               tableData->sort(1, Qt::DescendingOrder);
               ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

               if(tableData->rowCount() > i+1)
               {
                  // clear
                  tableData->removeRows(i+1, tableData->rowCount()-(i+1));
               }

               searchcall(ui->comboBox_2->currentText());

               saveJson();
               break;
            }
         }
      }
   }
}

void
MainWindow::loadJson()
{
   std::ifstream i("config.json");
   try
   {
      i >> j;
   }
   catch(...)
   {
      QMessageBox messageBox;
      messageBox.critical(0,"Error","No config file found");
      messageBox.setFixedSize(500,200);
   }

   i.close();
}

void
MainWindow::saveJson()
{
   std::ofstream o("config.json.tmp");
   try
   {
      o << std::setw(4) << j << std::endl;
      o.close();

      // TODO corrupt check
      std::remove("config.json");
      std::rename("config.json.tmp", "config.json");
   }

   catch(...)
   {
      o.close();
   }
}

MainWindow::~MainWindow()
{
   delete ui;
}
