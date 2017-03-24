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

   areaLay = new QVBoxLayout();
   animLay = new QVBoxLayout();
   tableData = new QStandardItemModel();
   ui->horizontalLayout->addLayout(areaLay);
   ui->horizontalLayout->addLayout(animLay);
   ui->tableView->setModel(tableData);
   tableData->setHorizontalHeaderItem(0, new QStandardItem("Name"));
   tableData->setHorizontalHeaderItem(1, new QStandardItem("Count"));
   tableData->setHorizontalHeaderItem(2, new QStandardItem("Percent"));
   ui->tableView->setSortingEnabled(false);

   connect(ui->comboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(switchcall(const QString&)));

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
MainWindow::switchcall(const QString & name)
{
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
            areaButtons.push_back(btn);
            areaLay->addWidget(btn);
            btn->show();
            connect(areaButtons[i], SIGNAL(clicked()), this, SLOT(areaButtonClicked()));
            ++i;
         }

         i = 0;
         for( auto& anim : element["area"][0]["anim"] )
         {
            QString name = QString::fromStdString(anim["name"]);
            QPushButton *btn = new QPushButton(name);
            btn->setEnabled( false );
            btn->setObjectName(name);
            animButtons.push_back(btn);
            animLay->addWidget(btn);
            btn->show();
            connect(animButtons[i], SIGNAL(clicked()), this, SLOT(animButtonClicked()));
            ++i;
         }

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
   int i = 0;
   for(auto&element : j)
   {
      if(QString::fromStdString(element["name"]) == ui->comboBox->currentText())
      {
         for( auto& area : element["area"] )
         {
            if( QString::fromStdString(area["name"]) == name)
            {
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

                  QStandardItem *item3 = new QStandardItem();
                  float perc = ((float)anim["count"].get<int>() / (float)amount ) * 100;
                  QString s = QString::number(perc, 'f', 2);
                  s.append(QString(" %"));
                  item3->setData(QVariant(s), Qt::DisplayRole);
                  tableData->setItem(i, 2, item3);

                  ++i;
               }

               saveJson();
               break;
            }
         }
      }
   }

   tableData->sort(1, Qt::DescendingOrder);
   ui->tableView->resizeColumnsToContents();
   ui->tableView->resizeRowsToContents();
}

void
MainWindow::loadJson()
{
   std::ifstream i("config.json");
   i >> j;
}

void
MainWindow::saveJson()
{
   std::ofstream o("config.json.tmp");
   o << std::setw(4) << j << std::endl;

   // TODO
   std::remove("config.json");
   std::rename("config.json.tmp", "config.json");
}

MainWindow::~MainWindow()
{
   delete ui;
}
