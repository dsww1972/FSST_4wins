#include "mainwindow.h"
#include "QtDebug"
#include <QTextStream>
#include <QNetworkInterface>
#include <QProcess>
#include <QApplication>
#include <QProcess>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUi(this);
    socket = new QUdpSocket(this);
    socket->bind(QHostAddress::Any, 1234);
    connect(socket, &QUdpSocket::readyRead, this, &MainWindow::empfangen);
    // Zeige die eigene IP-Adresse an
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    for (int i = 0; i < ipAddressesList.size(); ++i) {
        if (ipAddressesList.at(i) != QHostAddress::LocalHost && ipAddressesList.at(i).toIPv4Address()) {
            ip_label->setText("Eigene IP-Adresse: " + ipAddressesList.at(i).toString());
            break;
        }
    }

    Player_Wait_Turn_label->setText("-");
    restart_button->setVisible(0);
    restart_button->setEnabled(0);

}

MainWindow::~MainWindow() {}

//Globale Variablen
int height_game = 500;
int width_game = 590;
int canvas_start_x = 10;
int canvas_start_y = 70;
int rows = 6;
int columns = 7;
int width_gaps = 50;
int height_gaps = 50;
int gaps_x_position = 40;
int gaps_y_position = 90;
int stone_x_position = 40;
int stone_y_position = 10;
int turn = 1;
int playfield[7][6]=
    {{0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0}};
int position_stone = 0;
int red_points=0;
int yellow_points=0;
bool down = false;
bool isset = false;
bool color = true;
bool wait= true;
QString ip_enemy;
QString package;
QString Player;

void MainWindow::paintEvent(QPaintEvent *){

    QPainter painter(this);

    //Canvas Pen
    QPen pen_canvas;
    pen_canvas.setColor(Qt::blue);
    pen_canvas.setWidth(5);

    //Canvas Brush
    QBrush brush_canvas;
    brush_canvas.setColor(Qt::blue);
    brush_canvas.setStyle(Qt::SolidPattern);

    //Playfield Painter
    painter.setPen(pen_canvas);
    painter.setBrush(brush_canvas);
    painter.drawRect(QRect(canvas_start_x, canvas_start_y, width_game, height_game));

    //Gaps Pen
    QPen pen_gaps;
    pen_gaps.setColor(Qt::gray);
    pen_gaps.setWidth(2);

    //Gaps Brush
    QBrush brush_gaps;
    brush_gaps.setColor(Qt::gray);
    brush_gaps.setStyle(Qt::SolidPattern);

    //Stone Pen Red
    QPen pen_stone_red;
    pen_stone_red.setColor(Qt::red);
    pen_stone_red.setWidth(2);

    //Stone Pen Yellow
    QPen pen_stone_yellow;
    pen_stone_yellow.setColor(Qt::yellow);
    pen_stone_yellow.setWidth(2);

    //Stone Brush Red
    QBrush brush_stone_red;
    brush_stone_red.setColor(Qt::red);
    brush_stone_red.setStyle(Qt::SolidPattern);

    //Stone Brush Yellow
    QBrush brush_stone_yellow;
    brush_stone_yellow.setColor(Qt::yellow);
    brush_stone_yellow.setStyle(Qt::SolidPattern);

    //Playfield Gaps Painter
    for(rows = 0; rows < 6; rows++){
        for(columns = 0; columns < 7; columns++){
            painter.setPen(pen_gaps);
            painter.setBrush(brush_gaps);
            painter.drawEllipse(QRect(gaps_x_position, gaps_y_position, width_gaps, height_gaps));
            gaps_x_position += 80;
        }
        gaps_y_position += 80;
        gaps_x_position = 40;
    }

    gaps_y_position = 90;

    //Playfield Set Stone Color
    if(down){
        down=false;
        for(int row = 5;row>=0&&isset==false;row--){
            if(playfield[position_stone][row]==0){
                if(Player=="RED"){
                    playfield[position_stone][row]=1;
                    package = Player+"."+QString::number(position_stone)+"."+QString::number(row);
                }
                else if(Player=="YELLOW"){
                    playfield[position_stone][row]=2;
                    package = Player+"."+QString::number(position_stone)+"."+QString::number(row);
                }
                //qDebug() << QString::number(position_stone)+QString::number(row)+QString::number(playfield[position_stone][row]);
                isset=true;
                color = !color;
            }
        }
        isset=false;
    }

    //Stone Playfield Painter
    for (rows = 0; rows < 6; rows++) {
        for (columns = 0; columns < 7; columns++) {
            if(playfield[columns][rows]==1){
                painter.setPen(pen_stone_red);
                painter.setBrush(brush_stone_red);
                painter.drawEllipse(QRect((80*columns)+40,(80*rows)+90,width_gaps,height_gaps));
            }
            else if(playfield[columns][rows]==2){
                painter.setPen(pen_stone_yellow);
                painter.setBrush(brush_stone_yellow);
                painter.drawEllipse(QRect((80*columns)+40,(80*rows)+90,width_gaps,height_gaps));
            }
        }
    }

    //Select Stone Painter
    if(Player=="RED"){
        painter.setPen(pen_stone_red);
        painter.setBrush(brush_stone_red);
        painter.drawEllipse(QRect(stone_x_position,stone_y_position,width_gaps,height_gaps));
    }

    if(Player=="YELLOW"){
        painter.setPen(pen_stone_yellow);
        painter.setBrush(brush_stone_yellow);
        painter.drawEllipse(QRect(stone_x_position,stone_y_position,width_gaps,height_gaps));
    }

    //Determine Winner
    for (int row = 0; row < 6; row++)
    {
        for (int col = 0; col < 7; col++)
        {
            int player = playfield[row][col];
            if (player != 0 && playfield[row][col + 1] == player && playfield[row][ col + 2] == player && playfield[row][col + 3] == player)
            {
                if(player==1){
                    winner_text->setText("Winner: RED");
                    //restart_button->setVisible(1);
                    //restart_button->setEnabled(1);
                    red_points++;
                }
                else if(player==2){
                    winner_text->setText("Winner: YELLOW");
                    //restart_button->setVisible(1);
                    //restart_button->setEnabled(1);
                    yellow_points++;
                }
                return;
            }
        }
    }

    for (int row = 0; row < 6; row++)
    {
        for (int col = 0; col < 7; col++)
        {
            int player = playfield[row][col];
            if (player != 0 && playfield[row+1][col] == player && playfield[row+2][col] == player && playfield[row+3][col] == player)
            {
                if(player==1){
                    winner_text->setText("Winner: RED");
                    //restart_button->setVisible(1);
                    //restart_button->setEnabled(1);
                    red_points++;
                }
                else if(player==2){
                    winner_text->setText("Winner: YELLOW");
                    //restart_button->setVisible(1);
                    //restart_button->setEnabled(1);
                    yellow_points++;
                }
                return;
            }
        }
    }

    for (int row = 0; row < 6; row++)
    {
        for (int col = 0; col < 7; col++)
        {
            int player = playfield[row][col];
            if (player != 0 &&playfield[row - 1][ col + 1] == player &&playfield[row - 2][ col + 2] == player &&playfield[row - 3][ col + 3] == player)
            {
                if(player==1){
                    winner_text->setText("Winner: RED");
                    //restart_button->setVisible(1);
                    //restart_button->setEnabled(1);
                    red_points++;
                }
                else if(player==2){
                    winner_text->setText("Winner: YELLOW");
                    //restart_button->setVisible(1);
                    //restart_button->setEnabled(1);
                    yellow_points++;
                }
                return;
            }
        }
    }

    for (int row = 0; row < 6; row++)
    {
        for (int col = 0; col < 7; col++)
        {
            int player = playfield[row][col];
            if (player != 0 &&playfield[row + 1][ col + 1] == player &&playfield[row + 2][ col + 2] == player &&playfield[row + 3][ col + 3] == player)
            {
                if(player==1){
                    winner_text->setText("Winner: RED");
                    //restart_button->setVisible(1);
                    //restart_button->setEnabled(1);
                    red_points++;
                }
                else if(player==2){
                    winner_text->setText("Winner: YELLOW");
                    //restart_button->setVisible(1);
                    //restart_button->setEnabled(1);
                    yellow_points++;
                }
                return;
            }
        }
    }

    turn_text->setText("Turn: "+QString::number(turn));
    red_points_text->setText("RED: "+QString::number(red_points));
    yellow_points_text->setText("YELLOW: "+QString::number(yellow_points));

}

void MainWindow::keyPressEvent(QKeyEvent *event){

    //Key Press Event
    if(event->key()==Qt::Key_Right && stone_x_position<500&&!wait){
        stone_x_position+=80;
        position_stone++;
        update();
    }else if(event->key()==Qt::Key_Left && stone_x_position > 50&&!wait){
        stone_x_position-=80;
        position_stone--;
        update();
    }else if(event->key()==Qt::Key_Down&&!wait){
        down = true;
        nachricht();
        update();
    }


}

void MainWindow::nachricht(){
    QByteArray datagram = package.toUtf8();
    QHostAddress hostAddress(ip_enemy);
    quint16 port = 1234;
    if(!package.isEmpty()){
        socket->writeDatagram(datagram, hostAddress, port);
        qDebug() << hostAddress.toString() + ":" + datagram;
    }
    wait=true;
    turn++;

    Player_Wait_Turn_label->setText("Enemys Turn");
}

void MainWindow::empfangen(){
    int get_player = 0;
    int get_position = 0;
    int get_row = 0;
    QByteArray buffer;
    buffer.resize(socket->pendingDatagramSize());
    quint16 senderPort;
    QHostAddress sender;
    socket->readDatagram(buffer.data(), buffer.size(), &sender, &senderPort);
    ip_enemy = sender.toString();
    ip_enemy.remove(QString("::ffff:"));
    if(!buffer.isEmpty()){
        status_label->setText("Bereit");
    }
    wait=false;
    Player_Wait_Turn_label->setText("Your Turn");
    QString nachricht = buffer;
    QStringList pieces = nachricht.split('.');
    qDebug() << nachricht;
    if(pieces[0]=="RED"){
        get_player=1;
    }
    if(pieces[0]=="YELLOW"){
        get_player=2;
    }
    get_position = pieces[1].toInt();
    get_row = pieces[2].toInt();
    playfield[get_position][get_row]=get_player;
    turn++;
    turn_button->setEnabled(1);
    update();
}

void MainWindow::on_ip_set_button_clicked()
{
    ip_enemy = enemy_ip_text->text();
    ip_set_button->setEnabled(0);
    enemy_ip_text->setEnabled(0);
    if(set_player_radio->isChecked()){
        Player="YELLOW";
        wait=true;
        Player_Wait_Turn_label->setText("Enemys Turn");
        qDebug() << "Ich bin Spieler 2";
        update();
    }
    else{
        Player="RED";
        wait=false;
        Player_Wait_Turn_label->setText("Your Turn");
        qDebug() << "Ich bin Spieler 1";
        update();
    }
}

void MainWindow::fct_restart(){
    qDebug() << "Programm neustarten";
    QString program = qApp->arguments()[0];
    QStringList arguments = qApp->arguments().mid(1);
    qApp->quit();
    QProcess::startDetached(program, arguments);
}

void MainWindow::on_turn_button_clicked()
{
    nachricht();
    turn_button->setEnabled(0);
}
/*
void MainWindow::on_restart_button_clicked()
{
    turn = 1;
    for (int row = 0; row < 6; row++)
    {
        for (int col = 0; col < 7; col++)
        {
            playfield[columns][rows]=0;
        }
    }
    turn_text->setText("Turn: "+QString::number(turn));
    red_points_text->setText("RED: "+QString::number(red_points));
    yellow_points_text->setText("YELLOW: "+QString::number(yellow_points));
    restart_button->setVisible(0);
    restart_button->setEnabled(0);
    update();
}
*/
