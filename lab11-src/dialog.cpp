/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:

**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/




#include <QtWidgets>
#include "dialog.h"
#include "talk_client.h"
#define MAX_RESPONSE (20 * 1024)
Talk_client * talk_client;
void Dialog::sendAction()
{
    printf("Send Button\n");
    QString input = currentRoomName + " " + inputMessage->toPlainText();
    QByteArray msg = input.toLocal8Bit();
    char * cmsg = msg.data();
    talk_client->send_message(cmsg);
    inputMessage->clear();
}


void Dialog::newUserAction()
{
    QString user = userInput->text();
    QString pass = passInput->text();
    QByteArray userN = user.toLocal8Bit();
    QByteArray passW = pass.toLocal8Bit();
    char * usernam = userN.data();
    char * passwor = passW.data();
    allMessages->append(talk_client->getUserName());
    talk_client = new Talk_client("localhost", usernam, passwor, 8979);
    userInput->clear();
    passInput->clear();
}

void Dialog::newRoomAction(){
    char * roomname = roomInput->text().toLocal8Bit().data();
    talk_client->create_room(roomname);

}

void Dialog::timerAction()
{
    roomsList->clear();
    char * responsee;
    responsee = talk_client->list_rooms();
    char * e = responsee;
    int count = 0;
    char s[50];
    printf("%s", responsee);
    while(*e != '\0'){
        while(*e != '\n'){
            s[count] = *e;
            count++;
            e++;
        }
        e++;
        s[count] = '\0';
        count = 0;
        roomsList->addItem(s);
    }
    printf("Timer wakeup\n");

    QString ar = "0 " + currentRoomName;
    QByteArray xr = ar.toLocal8Bit();
    char * temp = xr.data();
    char * response = talk_client->get_messages(temp);

    if(strlen(response) > 4)allMessages->append(response);
}

void Dialog::changeRoom(){
    usersList->clear();
    QByteArray xr = currentRoomName.toLocal8Bit();
    char * temp = xr.data();
    talk_client->leave_room(temp);
    currentRoomName = roomsList->currentItem()->text();
    QByteArray  cr = roomsList->currentItem()->text().toLocal8Bit();
    char * currentRoom = cr.data();
    talk_client->enter_room(currentRoom);
    char * response;
    response = talk_client->print_users_in_room(currentRoom);
    char * e = response;
    int count = 0;
    char s[50];
    printf("%s", response);
    while(*e != '\0'){
        while(*e != '\n'){
            s[count] = *e;
            count++;
            e++;
        }
        e++;
        s[count] = '\0';
        count = 0;
        usersList->addItem(s);
    }
    refreshMessage();
    QString username = talk_client->getUserName();
    talk_client->numReset();
    timerAction();
}

void Dialog::refreshMessage(){
    allMessages->clear();
}

Dialog::Dialog()
{
    talk_client = new Talk_client("localhost", "Andrew", "andrew", 8979);
    talk_client->create_room("Room1");
    //talk_client->create_room("Room2");
    //talk_client->create_room("Room3");

    Talk_client * temp1 = new Talk_client("localhost", "Andrew", "andrew", 8979);
    Talk_client * temp2 = new Talk_client("localhost", "Emma", "emma", 8979);
    Talk_client * temp3 = new Talk_client("localhost", "Edison", "edison", 8979);

    talk_client->enter_room("Room1");
    currentRoomName = "Room1";
    //temp1->enter_room("Room1");
    //temp2->enter_room("Room1");
    //temp3->enter_room("Room1");



    Dialog::createMenu();

    QVBoxLayout *mainLayout = new QVBoxLayout;

    // Rooms List
    QVBoxLayout * roomsLayout = new QVBoxLayout();
    QLabel * roomsLabel = new QLabel("Rooms");
    roomsList = new QListWidget();
    roomsLayout->addWidget(roomsLabel);
    roomsLayout->addWidget(roomsList);

    // Users List
    QVBoxLayout * usersLayout = new QVBoxLayout();
    QLabel * usersLabel = new QLabel("Users");
    usersList = new QListWidget();
    usersLayout->addWidget(usersLabel);
    usersLayout->addWidget(usersList);

    // Layout for rooms and users
    QHBoxLayout *layoutRoomsUsers = new QHBoxLayout;
    layoutRoomsUsers->addLayout(roomsLayout);
    //layoutRoomsUsers->addLayout(roomssLayout);
    layoutRoomsUsers->addLayout(usersLayout);

    // Textbox for all messages
    QVBoxLayout * allMessagesLayout = new QVBoxLayout();
    QLabel * allMessagesLabel = new QLabel("Messages");
    allMessages = new QTextEdit;
    allMessagesLayout->addWidget(allMessagesLabel);
    allMessagesLayout->addWidget(allMessages);

    // Textbox for input message
    QVBoxLayout * inputMessagesLayout = new QVBoxLayout();
    QLabel * inputMessagesLabel = new QLabel("Type your message:");
    inputMessage = new QTextEdit;
    inputMessagesLayout->addWidget(inputMessagesLabel);
    inputMessagesLayout->addWidget(inputMessage);

    // Send and new account buttons
    QHBoxLayout *layoutButtons = new QHBoxLayout;
    QPushButton * sendButton = new QPushButton("Send Message");
    QPushButton * newUserButton = new QPushButton("Create Account");
    layoutButtons->addWidget(sendButton);
    layoutButtons->addWidget(newUserButton);

    // Setup actions for buttons
    connect(sendButton, SIGNAL (released()), this, SLOT (sendAction()));
    connect(newUserButton, SIGNAL (released()), this, SLOT (newUserAction()));
    connect(roomsList, SIGNAL(itemSelectionChanged()), this, SLOT(changeRoom()));
    // Add all widgets to window
    mainLayout->addLayout(layoutRoomsUsers);
    mainLayout->addLayout(allMessagesLayout);
    mainLayout->addLayout(inputMessagesLayout);
    mainLayout->addLayout(layoutButtons);

    QHBoxLayout * loginLayout = new QHBoxLayout();
    QPushButton * loginButton = new QPushButton("Login");
    userInput = new QLineEdit;
    passInput = new QLineEdit;
    userLabel = new QLabel("User");
    passLabel = new QLabel("Password");
    loginLayout->addWidget(loginButton);
    loginLayout->addWidget(userLabel);
    loginLayout->addWidget(userInput);
    loginLayout->addWidget(passLabel);
    loginLayout->addWidget(passInput);



    QHBoxLayout * newRoomLayout = new QHBoxLayout();
    QPushButton * newRoomButton = new QPushButton("Create Room");
    roomInput = new QLineEdit;
    QLabel * newRoomLabel = new QLabel("Room Name");
    newRoomLayout->addWidget(newRoomButton);
    newRoomLayout->addWidget(newRoomLabel);
    newRoomLayout->addWidget(roomInput);
    // Setup actions for buttons
    connect(loginButton, SIGNAL(released()),this,SLOT(newUserAction()));
    connect(newRoomButton, SIGNAL(released()),this,SLOT(newRoomAction()));
    // Add all widgets to window

    mainLayout->addLayout(layoutRoomsUsers);
    mainLayout->addLayout(allMessagesLayout);
    mainLayout->addLayout(inputMessagesLayout);
    mainLayout->addLayout(layoutButtons);
    mainLayout->addLayout(loginLayout);
    mainLayout->addLayout(newRoomLayout);

    QPalette palet = newUserButton->palette();
    palet.setColor(QPalette::Button, QColor(Qt::cyan));
    newUserButton->setAutoFillBackground(true);
    newUserButton->setPalette(palet);
    newUserButton->update();

    QPalette palet1 = sendButton->palette();
    palet.setColor(QPalette::Button, QColor(Qt::cyan));
    sendButton->setAutoFillBackground(true);
    sendButton->setPalette(palet);
    sendButton->update();

    QPalette palet2 = loginButton->palette();
    palet.setColor(QPalette::Button, QColor(Qt::red));
    loginButton->setAutoFillBackground(true);
    loginButton->setPalette(palet);
    loginButton->update();

    QPalette palet3 = newRoomButton->palette();
    palet.setColor(QPalette::Button, QColor(Qt::red));
    newRoomButton->setAutoFillBackground(true);
    newRoomButton->setPalette(palet);
    newRoomButton->update();
    // Add layout to main window
    setLayout(mainLayout);

    setWindowTitle(tr("CS240 IRC Client"));
    //timer->setInterval(5000);

    messageCount = 0;userInput = new QLineEdit;
    passInput = new QLineEdit;
    userLabel = new QLabel;

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Dialog::timerAction);
    timer->start(5000);

}


void Dialog::createMenu()

{
    menuBar = new QMenuBar;
    fileMenu = new QMenu(tr("&File"), this);
    exitAction = fileMenu->addAction(tr("&Exit"));
    menuBar->addMenu(fileMenu);

    connect(exitAction, SIGNAL(triggered()), this, SLOT(accept()));
}

