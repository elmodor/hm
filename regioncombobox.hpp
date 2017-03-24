#ifndef REGIONCOMBOBOX_HPP
#define REGIONCOMBOBOX_HPP

#include <QtGui>

//Derived Class from QComboBox
class RegionComboBox: public QComboBox
{
  Q_OBJECT
  public:
   RegionComboBox(QWidget* parent):QComboBox(parent)
   {
     this->setParent(parent);
     connect(this , SIGNAL(currentIndexChanged(int)),this,SLOT(handleSelectionChanged(int)));
   }

   ~RegionComboBox(){}


  public slots:
   //Slot that is called when QComboBox selection is changed
   void handleSelectionChanged(int index)
   {
       QMessageBox* msg = new QMessageBox();
       msg->setWindowTitle("Hello !");
       msg->setText("Selected Index is :"+QString::number(index));
       msg->show();
   }

};
#endif // REGIONCOMBOBOX_HPP
