#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDateTime>



struct uzel
{
    QString objectName;
    QString IP_addr_ICPCON;

}uzels[]=
{


    "LELAKI","172.16.223.2",

    "GGPZ_GNED","172.16.223.18",
    "GGPZ_LELAKI_VHOD","172.16.223.19",
    //"GGPZ_LELAKI_VIHOD","172.16.57.72",

    "GGPZ_Z_OS","172.16.57.75",
    "GGPZ_Z_UPS","172.16.57.76",
    "GGPZ_Z_KSU","172.16.57.78",
    "GZU1_GNED","172.16.223.34",
    "GGPZ_4VOV","172.16.57.74",
    "YAROSHIV","172.16.48.100",
    "TALAL_Z_YAROSHIV","172.16.223.42",
    "GGPZ_GAZ","172.16.57.72",
    "VON_MALODIV","172.16.55.34",
    "GGPZ_3SHFLU","172.16.57.79",
};
//==========================================================================================

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->pushButtonClose,SIGNAL(clicked()),this,SLOT(close()));
    //connect(modbusPollThread,SIGNAL(textchange(int,QString)),this,SLOT(TextChanged(int,QString)));
    connect(ui->pushButtonReadOne,SIGNAL(clicked()),this,SLOT(PushButtonReadOne()));
    connect(ui->pushButtonReadAll,SIGNAL(clicked()),this,SLOT(PushButtonReadAll()));
    connect(ui->pushButtonSetOne,SIGNAL(clicked()),this,SLOT(PushButtonSetOne()));
    connect(ui->pushButtonSetAll,SIGNAL(clicked()),this,SLOT(PushButtonSetAll()));

    for(uint i=0;i<sizeof(uzels)/sizeof(uzel);++i)
    {
        ui->listWidget->addItem(uzels[i].objectName+" ("+uzels[i].IP_addr_ICPCON+")");
    }

    connect(&timer200ms,SIGNAL(timeout()),this,SLOT(TimerEvent200ms()));
    timer200ms.start(200);
}
//==========================================================================================
MainWindow::~MainWindow()
{
    delete ui;
}

//==========================================================================================
int MainWindow::ReadTimeFromICPDAS(QString IPaddr, QDateTime *icpDT)
{
    int res;

    modbus_t *mb;
    uint16_t tab_reg[200];
    const uint number_readed_registers=6;
    uint year=0;
    uint month=0;
    uint day=0;
    uint hour=0;
    uint minute=0;
    uint second=0;

                //inialized context
                mb = modbus_new_tcp(IPaddr.toStdString().c_str(), 502);
                if (mb==NULL) {return -1;}

                res=modbus_connect(mb);
                modbus_set_slave(mb, 1);


                if (res==-1) {modbus_free(mb); return -2;}


                res=modbus_read_registers(mb, 1, number_readed_registers, tab_reg);// 6 reg from 40002

                if (res!=number_readed_registers){modbus_close(mb);modbus_free(mb); return -3;}

        year=tab_reg[0];
        month=tab_reg[1];
        day=tab_reg[2];
        hour=tab_reg[3];
        minute=tab_reg[4];
        second=tab_reg[5];


        QDate dt;
        dt.setDate(year,month,day);
        QTime tm;
        tm.setHMS(hour,minute,second);

        icpDT->setDate(dt);
        icpDT->setTime(tm);

        modbus_close(mb);
        modbus_free(mb);


return 1;
}
//==========================================================================================
int MainWindow::SetTimeToICPDAS(QString IPaddr,QDateTime icpNewDT)
{
    int res;

    modbus_t *mb;
    uint16_t tab_reg[200];
    const uint number_write_registers=7;


    tab_reg[0]=icpNewDT.date().year();
    tab_reg[1]=icpNewDT.date().month();
    tab_reg[2]=icpNewDT.date().day();
    tab_reg[3]=icpNewDT.time().hour();
    tab_reg[4]=icpNewDT.time().minute();
    tab_reg[5]=icpNewDT.time().second();
    tab_reg[6]=1;          //flag

                //inialized context
                mb = modbus_new_tcp(IPaddr.toStdString().c_str(), 502);
                if (mb==NULL) {return -1;}

                res=modbus_connect(mb);
                modbus_set_slave(mb, 1);


                if (res==-1) {modbus_free(mb); return -2;}


                res=modbus_write_registers(mb, 11, number_write_registers, tab_reg);// 7 reg from 40012

                if (res!=number_write_registers){modbus_close(mb);modbus_free(mb); return -3;}

        modbus_close(mb);
        modbus_free(mb);


return 1;
}
//==========================================================================================
void MainWindow::PushButtonReadOne()
{


    QDateTime icpDT;
    int res=0;
    int diff_in_msec=0;

    int number_in_uzel_array=ui->listWidget->currentRow();
    QString sign="";


        res=ReadTimeFromICPDAS(uzels[number_in_uzel_array].IP_addr_ICPCON,&icpDT);
        if (res==1)
        {

            diff_in_msec=QDateTime::currentDateTime().msecsTo(icpDT);
            if (diff_in_msec>0) sign="+";


            ui->listWidget->item(number_in_uzel_array)->setText(uzels[number_in_uzel_array].objectName+" ("+uzels[number_in_uzel_array].IP_addr_ICPCON+")" + " === "+
                                                                icpDT.toString("dd.MM.yyyy HH:mm:ss")+ " (" + sign + QString::number(diff_in_msec/1000.0,'f',0)+" s.)");
        }
        else
        {
            if (res==-1) ui->listWidget->item(number_in_uzel_array)->setText(uzels[number_in_uzel_array].objectName+" ("+uzels[number_in_uzel_array].IP_addr_ICPCON+")"+ " === "+"error init modbus context!!!");
            if (res==-2) ui->listWidget->item(number_in_uzel_array)->setText(uzels[number_in_uzel_array].objectName+" ("+uzels[number_in_uzel_array].IP_addr_ICPCON+")"+ " === "+"error connect!!!");
            if (res==-3) ui->listWidget->item(number_in_uzel_array)->setText(uzels[number_in_uzel_array].objectName+" ("+uzels[number_in_uzel_array].IP_addr_ICPCON+")"+ " === "+"error read!!!");

        }




}
//==========================================================================================
void MainWindow::PushButtonReadAll()
{

    QDateTime icpDT;
    int res=0;
    int diff_in_msec=0;




    for(uint i=0;i<sizeof(uzels)/sizeof(uzel);++i)
    {

        res=ReadTimeFromICPDAS(uzels[i].IP_addr_ICPCON,&icpDT);
        if (res==1)
        {
            QString sign="";
            diff_in_msec=QDateTime::currentDateTime().msecsTo(icpDT);
            if (diff_in_msec>0) sign="+";


            ui->listWidget->item(i)->setText(uzels[i].objectName+" ("+uzels[i].IP_addr_ICPCON+")" + " === "+
                                                                icpDT.toString("dd.MM.yyyy HH:mm:ss")+ " (" + sign + QString::number(diff_in_msec/1000.0,'f',0)+" s.)");
        }
        else
        {
            if (res==-1) ui->listWidget->item(i)->setText(uzels[i].objectName+" ("+uzels[i].IP_addr_ICPCON+")"+ " === "+"error init modbus context!!!");
            if (res==-2) ui->listWidget->item(i)->setText(uzels[i].objectName+" ("+uzels[i].IP_addr_ICPCON+")"+ " === "+"error connect!!!");
            if (res==-3) ui->listWidget->item(i)->setText(uzels[i].objectName+" ("+uzels[i].IP_addr_ICPCON+")"+ " === "+"error read!!!");

        }
    }



}
//==========================================================================================
void MainWindow::PushButtonSetOne()
{



    QDateTime icpDT=QDateTime::currentDateTime();
    if (ui->radioButtonAuto->isChecked())
    {
        /* if (icpDT.time().msec()>=500)*/ icpDT=icpDT.addMSecs(1500);
    }
    else
    {
        icpDT.setTime(ui->timeEditManual->time());
    }

    int res=0;

    int number_in_uzel_array=ui->listWidget->currentRow();


        res=SetTimeToICPDAS(uzels[number_in_uzel_array].IP_addr_ICPCON,icpDT);
        if (res==1)
        {

            ui->listWidget->item(number_in_uzel_array)->setText(uzels[number_in_uzel_array].objectName+" ("+uzels[number_in_uzel_array].IP_addr_ICPCON+")" + " === "+
                                                                "set time OK");
        }
        else
        {
            if (res==-1) ui->listWidget->item(number_in_uzel_array)->setText(uzels[number_in_uzel_array].objectName+" ("+uzels[number_in_uzel_array].IP_addr_ICPCON+")"+ " === "+"error init modbus context!!!");
            if (res==-2) ui->listWidget->item(number_in_uzel_array)->setText(uzels[number_in_uzel_array].objectName+" ("+uzels[number_in_uzel_array].IP_addr_ICPCON+")"+ " === "+"error connect!!!");
            if (res==-3) ui->listWidget->item(number_in_uzel_array)->setText(uzels[number_in_uzel_array].objectName+" ("+uzels[number_in_uzel_array].IP_addr_ICPCON+")"+ " === "+"error read!!!");

        }
}
//==========================================================================================
void MainWindow::PushButtonSetAll()
{

    for(uint i=0;i<sizeof(uzels)/sizeof(uzel);++i)
    {

        QDateTime icpDT=QDateTime::currentDateTime();
        if (ui->radioButtonAuto->isChecked())
        {
            /* if (icpDT.time().msec()>=500)*/ icpDT=icpDT.addMSecs(1500);
        }
        else
        {
            icpDT.setTime(ui->timeEditManual->time());
        }

        int res=0;

        int number_in_uzel_array=i;


        res=SetTimeToICPDAS(uzels[number_in_uzel_array].IP_addr_ICPCON,icpDT);
        if (res==1)
        {

            ui->listWidget->item(number_in_uzel_array)->setText(uzels[number_in_uzel_array].objectName+" ("+uzels[number_in_uzel_array].IP_addr_ICPCON+")" + " === "+
                                                                "set time OK");
        }
        else
        {
            if (res==-1) ui->listWidget->item(number_in_uzel_array)->setText(uzels[number_in_uzel_array].objectName+" ("+uzels[number_in_uzel_array].IP_addr_ICPCON+")"+ " === "+"error init modbus context!!!");
            if (res==-2) ui->listWidget->item(number_in_uzel_array)->setText(uzels[number_in_uzel_array].objectName+" ("+uzels[number_in_uzel_array].IP_addr_ICPCON+")"+ " === "+"error connect!!!");
            if (res==-3) ui->listWidget->item(number_in_uzel_array)->setText(uzels[number_in_uzel_array].objectName+" ("+uzels[number_in_uzel_array].IP_addr_ICPCON+")"+ " === "+"error read!!!");

        }
    }

}
//==========================================================================================
void MainWindow::TimerEvent200ms()
{

    static bool firstCycle=true;
    static bool currTimeLessAutoSyncTime_prev=true;
    bool currTimeLessAutoSyncTime=true;
    int diff_in_msec=0;

    //autosynchronize
    if (firstCycle)
    {
        diff_in_msec=QTime::currentTime().msecsTo(ui->timeEditAutosync->time());

        if (diff_in_msec>=0)
        {
            currTimeLessAutoSyncTime_prev=true;
        }
        else
        {
            currTimeLessAutoSyncTime_prev=false;
        }

        firstCycle=false;

        //ui->listWidgetHistory->addItem("sfzxf");

        //ui->listWidgetHistory->addItem(QString::number(currTimeLessAutoSyncTime_prev)+" === " + QString::number(currTimeLessAutoSyncTime));

    }
    else
    {

        diff_in_msec=QTime::currentTime().msecsTo(ui->timeEditAutosync->time());

        if (diff_in_msec>=0)
        {
            currTimeLessAutoSyncTime=true;
        }
        else
        {
            currTimeLessAutoSyncTime=false;
        }

        if (currTimeLessAutoSyncTime==false && currTimeLessAutoSyncTime_prev==true)
        {
            ui->listWidgetHistory->addItem("<<<<<<< Autosynchronization started at " + QDateTime::currentDateTime().toString("dd.MM.yyyy HH:mm:ss")+" >>>>>>>");
            ui->listWidgetHistory->item(ui->listWidgetHistory->count()-1)->setTextColor(Qt::darkGreen);

            //get time form all
            PushButtonReadAll();
            ui->listWidgetHistory->addItem("<<<<<<< Time in PLCs before sychronize >>>>>>>");
            ui->listWidgetHistory->item(ui->listWidgetHistory->count()-1)->setTextColor(Qt::darkGreen);

            AddListToHistory();
            //set time to all
            PushButtonSetAll();
            ui->listWidgetHistory->addItem("<<<<<<< Synchronize result >>>>>>>");
            ui->listWidgetHistory->item(ui->listWidgetHistory->count()-1)->setTextColor(Qt::darkGreen);

            AddListToHistory();

            //sleep to get time PLC update his time and update modbus
            Sleep(2000);
            //get time from all
            PushButtonReadAll();
            ui->listWidgetHistory->addItem("<<<<<<< Time in PLCs after sychronize >>>>>>>");
            ui->listWidgetHistory->item(ui->listWidgetHistory->count()-1)->setTextColor(Qt::darkGreen);

            AddListToHistory();
            //end
            ui->listWidgetHistory->addItem("<<<<<<< Autosynchronization ended at " + QDateTime::currentDateTime().toString("dd.MM.yyyy HH:mm:ss")+" >>>>>>>");
            ui->listWidgetHistory->item(ui->listWidgetHistory->count()-1)->setTextColor(Qt::darkGreen);
            ui->listWidgetHistory->scrollToBottom();


        }

        currTimeLessAutoSyncTime_prev = currTimeLessAutoSyncTime;
    }

    //set datetime
    ui->dateTimeEdit->setDateTime(QDateTime::currentDateTime());

}
//==========================================================================================
void MainWindow::AddListToHistory()
{


    for(uint i=0;i < ui->listWidget->count();++i)
    {
        ui->listWidgetHistory->addItem(ui->listWidget->item(i)->text());
        ui->listWidgetHistory->scrollToBottom();


    }



}
//==========================================================================================
